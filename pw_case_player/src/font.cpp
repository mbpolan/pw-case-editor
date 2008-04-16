/***************************************************************************
 *   Copyright (C) 2007 by Mike Polan                                      *
 *   kanadakid@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
// font.cpp: implementation of Font engine

#include <cstdio>
#include <iostream>
#include "SDL_rotozoom.h"
#include "SDL_ttf.h"

#include "iohandler.h"
#include "font.h"
#include "renderer.h"
#include "texture.h"
#include "utilities.h"

namespace Fonts {

std::map<int, Font> g_Fonts;

const Color COLOR_BLACK(0, 0, 0);
const Color COLOR_WHITE(255, 255, 255);
const Color COLOR_GREEN(0, 247, 0);
const Color COLOR_ORANGE(247, 115, 57);
const Color COLOR_BLUE(107, 198, 247);
const Color COLOR_YELLOW(229, 204, 148);

}

// load a font
bool Fonts::loadFont(const ustring &str, int size) {
	TTF_Font *font=TTF_OpenFontRW(SDL_RWFromFile(str.c_str(), "rb"), 1, size);
	if (font) {
		Font f;
		f.font=font;
		
		// once the font is opened, generate 256 characters for the cache
		for (uchar i=20; i<(uchar) 128; ++i)
			f.glyphs[i]=renderGlyph(&f, i, (size==FONT_STANDARD ? QUALITY_SOLID : QUALITY_BLEND));
		
		g_Fonts[size]=f;
		
		return true;
	}
	
	return false;
}

// get a texture with a rendered glyph
Textures::Texture Fonts::renderGlyph(const Font *f, Uint16 ch, const Quality &quality) {
	Textures::Texture tex;
	
	SDL_Surface *surface;
	switch(quality) {
		//case QUALITY_SOLID: surface=TTF_RenderGlyph_Solid(f->font, ch, COLOR_WHITE.toSDLColor()); break;
		default:
		case QUALITY_BLEND: surface=TTF_RenderGlyph_Blended(f->font, ch, COLOR_WHITE.toSDLColor()); break;
	}
	
	return Textures::queryTexture(Textures::createTexture("null", surface));
}

// calculate the y coordinate for a glyph to render correctly on baseline
int Fonts::glyphBase(int y, Uint16 ch, int size) {
	Font *f=queryFont(size);
	if (!f)
		return 0;
	
	int maxy;
	TTF_GlyphMetrics(f->font, ch, NULL, NULL, NULL, &maxy, NULL);
	
	// calculate where to begin drawing this rectangle
	return y+TTF_FontAscent(f->font)-maxy;
}

// see if a character should not be drawn
bool Fonts::discardChar(Uint16 ch) {
	switch(ch) {
		case '[':
		case ']':
		case '{':
		case '}':
		case '^':
		case '#':
		case '|': return true;
		
		default: return false;
	}
}

// see if this string is too long and needs to be broken
bool Fonts::lineWillBreak(const Point &p, int rightClamp, const ustring &str, int size) {
	int breakCount=0;
	int ex=p.x();
	
	// get the requested font
	if (!queryFont(size)) {
		Utils::debugMessage("Font: font size '"+Utils::itoa(size)+"' not found");
		return -1;
	}
	Font *font=queryFont(size);
	
	for (int i=0; i<str.size(); i++) {
		Uint16 ch=(Uint16) str[i];
		
		// see if we should force a new line
		if (ch=='\n') {
			// check if we already have three lines
			if (breakCount==2)
				return true;
			
			ex=p.x();
			
			breakCount++;
			continue;
		}
		
		// see if this is a literal new line character
		if (ch=='\\' && str[i+1]=='n') {
			// make sure we don't make any extra lines
			if (breakCount==3)
				return true;
			
			ex=p.x();
			
			breakCount++;
			
			// we skip the next character
			i++;
			
			continue;
		}
		
		// if a space is needed, skip some pixels
		else if (ch==' ') {
			ex+=SIZE_WHITESPACE;
			
			continue;
		}
		
		// ignore certain characters
		else if (discardChar(ch))
			continue;
		
		// force a new line if needed
		int width=getGlyphWidth(ch, size)+getGlyphWidth(str[i+1], size)+2;
		if ((ex+width)>=rightClamp) {
			// check if we already have three lines
			if (breakCount==2)
				return true;
			
			// reset for next row
			ex=p.x();
			
			// move to next one
			ex+=getGlyphWidth(ch, size);
			
			breakCount++;
			continue;
		}
		
		// move over to the next glyph
		ex+=getGlyphWidth(ch, size);
	}
	
	return false;
}

// draw a string on the screen
int Fonts::drawString(const Point &p, const ustring &str, int size, const Color &color) {
	drawString(p, str.size(), SDL_GetVideoSurface()->w, str, size, color);
}

// draw a string with clamped restrictions
int Fonts::drawString(const Point &p, int limit, int rightClamp, const ustring &str, int size, const Color &color) {
	ColorRangeVector vec;
	vec.push_back(std::make_pair<ValueRange, Color> (ValueRange(0, 256), color));
	
	// draw the string using just this color
	drawStringMulticolor(p, limit, rightClamp, str, size, vec);
}

// draw a multicolor string
int Fonts::drawStringMulticolor(const Point &p, int limit, int rightClamp, const ustring &str, int size, const ColorRangeVector &vec) {
	// also, make sure the default video surface exists
	SDL_Surface *screen=SDL_GetVideoSurface();
	
	// get the requested font
	if (!queryFont(size)) {
		Utils::debugMessage("Font: font size '"+Utils::itoa(size)+"' not found");
		return -1;
	}
	Font *font=queryFont(size);
	
	// set initial position
	SDL_Rect drect;
	drect.x=p.x();
	drect.y=p.y();
	
	// count the amount of times we started a new line
	int breakCount=0;
	
	// bounds check
	if (limit>str.size())
		limit=str.size();
	
	// iterate over string and draw each character
	for (int i=0; i<limit; i++) {
		Uint16 ch=(Uint16) str[i];
		
		// don't bother drawing unknown characters
		if (getGlyphWidth(ch, size)==-1)
			continue;
		
		// see if we should force a new line
		if (ch=='\n') {
			// check if we already have three lines
			if (breakCount==2)
				return i;
			
			drect.x=p.x();
			drect.y+=SIZE_LINE_BREAK;
			
			breakCount++;
			continue;
		}
		
		// see if this is a literal new line character
		if (ch=='\\' && str[i+1]=='n') {
			// make sure we don't make any extra lines
			if (breakCount==2)
				return i;
			
			drect.x=p.x();
			drect.y+=SIZE_LINE_BREAK;
			
			breakCount++;
			
			// we skip the next character
			i++;
			
			continue;
		}
		
		// if a space is needed, skip some pixels
		else if (ch==' ') {
			drect.x+=SIZE_WHITESPACE;
			
			continue;
		}
		
		// ignore certain characters
		else if (discardChar(ch))
			continue;
		
		// force a new line if needed
		int width=getGlyphWidth(ch, size)+getGlyphWidth(str[i+1], size)+SIZE_CHAR_SPACE;
		if ((drect.x+width)>=rightClamp) {
			// check if we already have three lines
			if (breakCount==2)
				return i;
			
			// reset for next row
			drect.x=p.x();
			drect.y+=SIZE_LINE_BREAK;
			
			breakCount++;
		}
		
		// draw the glyph
		// modify the y coordinate
		SDL_Rect r={ drect.x, drect.y };
		r.y=glyphBase(drect.y, ch, size);
		
		// see if we should apply a color to this glyph
		Color color=COLOR_WHITE;
		for (int c=0; c<vec.size(); c++) {
			if (vec[c].first.inRange(i)) {
				color=vec[c].second;
				break;
			}
		}
		
		glColor3ub(color.r(), color.g(), color.b());
		
		// draw this glyph
		Textures::Texture g=font->glyphs[ch];
		Renderer::drawImage(Point(r.x, r.y, Z_TEXT), g);
		
		glColor3ub(255, 255, 255);
		
		// move over to the next glyph
		drect.x+=getGlyphWidth(ch, size)+SIZE_CHAR_SPACE;
	}
	
	return -1;
}

// draw a string centered on the screen
int Fonts::drawStringCentered(int y, int delimiter, const ustring &str, int size, const Color &color) {
	ustring nstr=str;
	
	// cache the right clamp value
	int clamp=SDL_GetVideoSurface()->w;
	
	// explode the string
	StringVector vec=Utils::explodeString("\\n", str);
	
	// draw first string, since there's always at least one
	int x=128-(Fonts::getWidth(vec[0], size)/2);
	drawString(Point(x, y, Z_TEXT), delimiter, clamp, vec[0], size, color);
	
	// second line
	if (vec.size()>1 && delimiter>=vec[0].size()) {
		// calculate another new x and draw the string
		x=128-(Fonts::getWidth(vec[1], size)/2);
		drawString(Point(x, y+Fonts::SIZE_LINE_BREAK, Z_TEXT), delimiter-vec[0].size(), clamp, vec[1], size, color);
		
		// third line
		if (vec.size()>2 && delimiter>=(vec[0].size()+vec[1].size())) {
			// calculate last new x, and draw string
			x=128-(Fonts::getWidth(vec[2], size)/2);
			drawString(Point(x, y+Fonts::SIZE_LINE_BREAK*2, Z_TEXT), delimiter-(vec[0].size()+vec[1].size()), clamp, vec[2], size, color);
		}
	}
}

// draw a ttf font string
void Fonts::drawStringBlended(const Point &p, const ustring &str, int size, const Color &color) {
	// get the font
	Font *f=queryFont(size);
	if (!f) {
		Utils::debugMessage("Fonts: font size '"+Utils::itoa(size)+"' not found.");
		return;
	}
	
	// SDL_ttf doesn't support newline characters, so we need to handle
	// those ourselves
	StringVector lines=Utils::explodeString("\\n", str);
	
	// draw the lines
	for (int i=0; i<lines.size(); i++)
		drawString(Point(p.x(), p.y()+TTF_FontLineSkip(f->font)*i+3, Z_TEXT), lines[i], size, color);
}

// get the width of a string
int Fonts::getWidth(const ustring &str, int size) {
	// get the requested font
	if (!queryFont(size)) {
		Utils::debugMessage("Font: font size '"+Utils::itoa(size)+"' not found");
		return -1;
	}
	Font *font=queryFont(size);
	
	// iterate over string
	int width=0;
	for (int i=0; i<str.size(); i++) {
		// new line characters end the string
		if (str[i]=='\n')
			break;
		
		// spaces have different widths according to size
		else if (str[i]==' ') {
			if (size==FONT_STANDARD)
				width+=SIZE_WHITESPACE;
			
			else
				width+=getGlyphWidth(' ', size);
		}
		
		else {
			if (str[i]=='\\')
				i+=2;
			
			else {
				// increment width
				width+=getGlyphWidth(str[i], size)+SIZE_CHAR_SPACE;
			}
		}
	}
	
	return width;
}

// get the width of a string drawn using SDL_ttf functions
int Fonts::getWidthTTF(const ustring &str, int size) {
	Font *font=queryFont(size);
	if (font) {
		int w;
		Uint16 *text=Utils::ustringToArray(str);
		TTF_SizeUNICODE(font->font, text, &w, NULL);
		delete [] text;
		return w;
	}
	
	return -1;
}

// get the width of a glyph
int Fonts::getGlyphWidth(Uint16 ch, int size) {
	Font *font=queryFont(size);
	if (font && !(ch>=0x01 && ch<=0x05) && ch!=0x00) {
		int min, max;
		TTF_GlyphMetrics(font->font, ch, &min, &max, NULL, NULL, NULL);
		return max-min;
	}
	
	return -1;
}

// get the height of a ttf font
int Fonts::getHeight(int size) {
	Font *font=queryFont(size);
	if (!font) {
		Utils::debugMessage("Fonts: font size '"+Utils::itoa(size)+"' not found.");
		return 0;
	}
	
	return TTF_FontHeight(font->font);
}

// return a ttf font from the map
Fonts::Font* Fonts::queryFont(int size) {
	return &g_Fonts[size];
}

// add a font to the map
void Fonts::pushFont(int size, Font *font) {
	g_Fonts[size]=*font;
}

// clear the font map
void Fonts::clearFontStack() {
	for (std::map<int, Font>::iterator it=g_Fonts.begin(); it!=g_Fonts.end(); ++it) {
		if ((*it).second.font)
			TTF_CloseFont((*it).second.font);
	}
}
