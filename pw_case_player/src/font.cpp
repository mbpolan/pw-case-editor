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
#include "texture.h"
#include "utilities.h"

namespace Fonts {

std::map<int, Font*> g_Fonts;

const Color COLOR_BLACK(0, 0, 0);
const Color COLOR_WHITE(255, 255, 255);
const Color COLOR_GREEN(0, 247, 0);
const Color COLOR_ORANGE(247, 115, 57);
const Color COLOR_BLUE(107, 198, 247);
const Color COLOR_YELLOW(229, 204, 148);

}

// get a surface with a rendered glyph
SDL_Surface* Fonts::renderGlyph(char ch, int size, const Color &color, const Quality &quality) {
	Font *f=queryFont(size);
	if (!f)
		return NULL;
	
	switch(quality) {
		case QUALITY_SOLID: return TTF_RenderGlyph_Solid(f, ch, color.toSDLColor());
		case QUALITY_BLEND: return TTF_RenderGlyph_Blended(f, ch, color.toSDLColor());
	}
}

// calculate the y coordinate for a glyph to render correctly on baseline
int Fonts::glyphBase(int y, char ch, int size) {
	Font *f=queryFont(size);
	if (!f)
		return 0;
	
	int maxy;
	TTF_GlyphMetrics(f, ch, NULL, NULL, NULL, &maxy, NULL);
	
	// calculate where to begin drawing this rectangle
	return y+TTF_FontAscent(f)-maxy;
}

// see if a character should not be drawn
bool Fonts::discardChar(char ch) {
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
bool Fonts::lineWillBreak(const Point &p, int rightClamp, const std::string &str, int size) {
	int breakCount=0;
	int ex=p.x();
	
	// get the requested font
	if (!queryFont(size)) {
		std::cout << "Font: font '" << size << "' not found\n";
		return -1;
	}
	Font *font=queryFont(size);
	
	for (int i=0; i<str.size(); i++) {
		char ch=(char) str[i];
		
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
int Fonts::drawString(const Point &p, const std::string &str, int size, const Color &color) {
	drawString(p, str.size(), SDL_GetVideoSurface()->w, str, size, color);
}

// draw a string with clamped restrictions
int Fonts::drawString(const Point &p, int limit, int rightClamp, const std::string &str, int size, const Color &color) {
	// also, make sure the default video surface exists
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen) {
		std::cout << "Fonts: unable to get pointer to video surface.\n";
		return -1;
	}
	
	// get the requested font
	if (!queryFont(size)) {
		std::cout << "Font: font '" << size << "' not found\n";
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
		char ch=(char) str[i];
		
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
			
			// modify the y coordinate
			SDL_Rect r={ drect.x, drect.y };
			r.y=glyphBase(drect.y, ch, size);
			
			// blit the character that was supposed to be here
			SDL_Surface *glyph=renderGlyph(ch, size, color, QUALITY_SOLID);
			SDL_BlitSurface(glyph, NULL, screen, &r);
			SDL_FreeSurface(glyph);
			
			// move to next one
			drect.x+=getGlyphWidth(ch, size)+SIZE_CHAR_SPACE;
			
			breakCount++;
			continue;
		}
		
		// draw the glyph
		// modify the y coordinate
		SDL_Rect r={ drect.x, drect.y };
		r.y=glyphBase(drect.y, ch, size);
		
		SDL_Surface *glyph=renderGlyph(ch, size, color, QUALITY_SOLID);
		SDL_BlitSurface(glyph, NULL, screen, &r);
		SDL_FreeSurface(glyph);
		
		// move over to the next glyph
		drect.x+=getGlyphWidth(ch, size)+SIZE_CHAR_SPACE;
	}
	
	return -1;
}

// draw a string centered on the screen
int Fonts::drawStringCentered(int y, int delimiter, const std::string &str, int size, const Color &color) {
	std::string nstr=str;
	
	// cache the right clamp value
	int clamp=SDL_GetVideoSurface()->w;
	
	// explode the string
	StringVector vec=Utils::explodeString("\\n", str);
	
	// draw first string, since there's always at least one
	int x=128-(Fonts::getWidth(vec[0], size)/2);
	drawString(Point(x, y), delimiter, clamp, vec[0], size, color);
	
	// second line
	if (vec.size()>1 && delimiter>=vec[0].size()) {
		// calculate another new x and draw the string
		x=128-(Fonts::getWidth(vec[1], size)/2);
		drawString(Point(x, y+Fonts::SIZE_LINE_BREAK), delimiter-vec[0].size(), clamp, vec[1], size, color);
		
		// third line
		if (vec.size()>2 && delimiter>=(vec[0].size()+vec[1].size())) {
			// calculate last new x, and draw string
			x=128-(Fonts::getWidth(vec[2], size)/2);
			drawString(Point(x, y+Fonts::SIZE_LINE_BREAK*2), delimiter-(vec[0].size()+vec[1].size()), clamp, vec[2], size, color);
		}
	}
}

// draw a ttf font string
void Fonts::drawStringBlended(const Point &p, const std::string &str, int size, const Color &color) {
	// get the font
	TTF_Font *font=queryFont(size);
	if (!font) {
		std::cout << "Fonts: requested TTF font size '" << size << "' doesn't exist.\n";
		return;
	}
	
	// SDL_ttf doesn't support newline characters, so we need to handle
	// those ourselves
	StringVector lines=Utils::explodeString("\\n", str);
	
	// draw the lines
	for (int i=0; i<lines.size(); i++) {
		// render a solid text line, calculate its position, and blit it
		SDL_Surface *rendered=TTF_RenderText_Blended(font, lines[i].c_str(), color.toSDLColor());
		SDL_Rect drect={ p.x(), p.y()+TTF_FontLineSkip(font)*i+3 };
		SDL_BlitSurface(rendered, NULL, SDL_GetVideoSurface(), &drect);
		SDL_FreeSurface(rendered);
	}
}

// get the width of a string
int Fonts::getWidth(const std::string &str, int size) {
	// get the requested font
	if (!queryFont(size)) {
		std::cout << "Font: font '" << size << "' not found\n";
		return -1;
	}
	Font *font=queryFont(size);
	
	// iterate over string
	int width=0;
	for (int i=0; i<str.size(); i++) {
		// new line characters end the string
		if (str[i]=='\n')
			break;
		
		// spaces are 5 pixels wide
		else if (str[i]==' ')
			width+=SIZE_WHITESPACE;
		
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

// get the width of a glyph
int Fonts::getGlyphWidth(char ch, int size) {
	Font *font=queryFont(size);
	if (font) {
		int min, max;
		TTF_GlyphMetrics(font, ch, &min, &max, NULL, NULL, NULL);
		return max-min;
	}
	
	return -1;
}

// get the height of a ttf font
int Fonts::getHeight(int size) {
	Font *font=queryFont(size);
	if (!font) {
		std::cout << "Fonts: requested TTF font size '" << size << "' not found.\n";
		return 0;
	}
	
	return TTF_FontHeight(font);
}

// return a ttf font from the map
Fonts::Font* Fonts::queryFont(int size) {
	return g_Fonts[size];
}

// add a font to the map
void Fonts::pushFont(int size,Font *font) {
	g_Fonts[size]=font;
}

// clear the font map
void Fonts::clearFontStack() {
	for (std::map<int, Font*>::iterator it=g_Fonts.begin(); it!=g_Fonts.end(); ++it) {
		if ((*it).second)
			TTF_CloseFont((*it).second);
	}
}
