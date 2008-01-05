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
#include "SDL/SDL_rotozoom.h"

#include "iohandler.h"
#include "font.h"
#include "texture.h"
#include "utilities.h"

namespace Fonts {
	TTF_Font *g_Arial=NULL;
}

// load a font from file
bool Fonts::loadFont(const std::string &path, Fonts::Font &font) {
	// open the file
	FILE *f=fopen(path.c_str(), "rb");
	if (!f)
		return false;
	
	// read magic number
	if (fgetc(f)!='P' || fgetc(f)!='W' || fgetc(f)!='F') {
		std::cout << "This is not a font file\n";
		return false;
	}
	
	// read version
	int version;
	fread(&version, sizeof(int), 1, f);
	if (version!=10) {
		std::cout << "Wrong version\n";
		return false;
	}
	
	// read amount of ucharacters
	int amount;
	fread(&amount, sizeof(int), 1, f);
	
	// iterate over glyphs
	for (int i=0; i<amount; i++) {
		// read ucharacter
		char ch=fgetc(f);
		
		// read buffer size
		int size;
		fread(&size, sizeof(unsigned int), 1, f);
		
		// allocate space
		char *buffer=new char[size];
		
		// read data
		fread(buffer, sizeof(char), size, f);
		
		// create a surface
		SDL_Surface *surface=createSurface(buffer);
		if (surface) {
			font.glyphs[ch].w=surface->w;
			font.glyphs[ch].surface=surface;
		}
		
		delete [] buffer;
	}
	
	// wrap up
	fclose(f);
	return true;
}

// create a font glyph
SDL_Surface* Fonts::createSurface(char *pbuf) {
	// bmp data
	int w, h;
	short planes, bpp;
	
	pbuf+=18;

	// read width
	w=*(int*) pbuf;
	pbuf+=sizeof(int);
	
	// read height
	h=*(int*) pbuf;
	pbuf+=sizeof(int);
	
	// read planes
	planes=*(short*) pbuf;
	pbuf+=sizeof(short);
	
	// read bpp
	bpp=*(short*) pbuf;
	pbuf+=sizeof(short);
	
	// seek past the header
	pbuf+=24;
	
	// calculate image size
	int realSize=w*h*bpp;
	
	// allocate image buffer
	char *pixels=(char*) malloc(realSize);
		
	// copy pixels from buffer
	memcpy(pixels, pbuf, realSize);
	
	// create a surface for this glyph
	SDL_Surface *surface=SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, bpp, 255U << 16, 255 << 8, 255 << 0, 0);
	
	// now set the pixels
	SDL_LockSurface(surface);
	
	// delete old pixels first
	char *sPixels=(char*) surface->pixels;
	delete [] sPixels;
	
	// set these pixels in the surface
	surface->pixels=pixels;
	
	SDL_UnlockSurface(surface);
	
	// set transparent pixel
	SDL_SetColorKey(surface, SDL_SRCCOLORKEY, SDL_MapRGB(surface->format, 0, 255, 0));
	
	return surface;	
}

// see if this string is too long and needs to be broken
bool Fonts::lineWillBreak(int x, int y, int rightClamp, const std::string &str, const std::string &fontId) {
	int breakCount=0;
	int ex=x;
	
	// get the requested font
	if (!queryFont(fontId)) {
		std::cout << "Font: font '" << fontId << "' not found\n";
		return -1;
	}
	Font font=*queryFont(fontId);
	
	for (int i=0; i<str.size(); i++) {
		char ch=(char) str[i];
		
		// see if we should force a new line
		if (ch=='\n') {
			// check if we already have three lines
			if (breakCount==2)
				return true;
			
			ex=x;
			
			breakCount++;
			continue;
		}
		
		// see if this is a literal new line ucharacter
		if (ch=='\\' && str[i+1]=='n') {
			// make sure we don't make any extra lines
			if (breakCount==3)
				return true;
			
			ex=x;
			
			breakCount++;
			
			// we skip the next ucharacter
			i++;
			
			continue;
		}
		
		// if a space is needed, skip some pixels
		else if (ch==' ') {
			ex+=g_SpaceSize;
			
			continue;
		}
		
		// force a new line if needed
		int width=font.glyphs[ch].w+font.glyphs[str[i+1]].w+2;
		if ((ex+width)>=rightClamp) {
			// check if we already have three lines
			if (breakCount==2)
				return true;
			
			// reset for next row
			ex=x;
			
			// move to next one
			ex+=font.glyphs[ch].w+g_CharSpace;
			
			breakCount++;
			continue;
		}
		
		// move over to the next glyph
		ex+=font.glyphs[ch].w+g_CharSpace;
	}
	
	return false;
}

// see if a word is too long to fit in this line
bool Fonts::wordWillBreak(int x, int rightClamp, const std::string &word, const std::string &fontId) {
	// get the requested font
	if (!queryFont(fontId)) {
		std::cout << "Font: font '" << fontId << "' not found\n";
		return false;
	}
	Font font=*queryFont(fontId);
	
	// calculate the length of this word
	int length=getWidth(fontId, word);
	
	// combine given x with length and see if we crossed the right most limit
	return (x+length>rightClamp);
}

// draw a string on the screen
int Fonts::drawString(int x, int y, const std::string &str, const std::string &fontId) {
	drawString(x, y, str.size(), SDL_GetVideoSurface()->w, str, fontId);
}

// draw a string with clamped restrictions
int Fonts::drawString(int x, int y, int limit, int rightClamp, const std::string &str, const std::string &fontId) {
	// also, make sure the default video surface exists
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen) {
		std::cout << "Fonts: unable to get pointer to video surface.\n";
		return -1;
	}
	
	// get the requested font
	if (!queryFont(fontId)) {
		std::cout << "Font: font '" << fontId << "' not found\n";
		return -1;
	}
	Font font=*queryFont(fontId);
	
	// set initial position
	SDL_Rect drect;
	drect.x=x;
	drect.y=y;
	
	// count the amount of times we started a new line
	int breakCount=0;
	
	// bounds check
	if (limit>str.size())
		limit=str.size();
	
	// iterate over string and draw each ucharacter
	for (int i=0; i<limit; i++) {
		char ch=(char) str[i];
		
		// don't bother drawing unknown ucharacters
		if (font.glyphs.find(ch)==font.glyphs.end())
			continue;
		
		// see if we should force a new line
		if (ch=='\n') {
			// check if we already have three lines
			if (breakCount==2)
				return i;
			
			drect.x=x;
			drect.y+=g_LineBreakSize;
			
			breakCount++;
			continue;
		}
		
		// see if this is a literal new line ucharacter
		if (ch=='\\' && str[i+1]=='n') {
			// make sure we don't make any extra lines
			if (breakCount==2)
				return i;
			
			drect.x=x;
			drect.y+=g_LineBreakSize;
			
			breakCount++;
			
			// we skip the next ucharacter
			i++;
			
			continue;
		}
		
		// if a space is needed, skip some pixels
		else if (ch==' ') {
			drect.x+=g_SpaceSize;
			
			continue;
		}
		
		// force a new line if needed
		int width=font.glyphs[ch].w+font.glyphs[str[i+1]].w+g_CharSpace;
		if ((drect.x+width)>=rightClamp) {
			// check if we already have three lines
			if (breakCount==2)
				return i;
			
			// blit a hyphen, if needed
			if (i>0 && str[i-1]!=' ')
				SDL_BlitSurface(font.glyphs['-'].surface, NULL, screen, &drect);
			
			// reset for next row
			drect.x=x;
			drect.y+=g_LineBreakSize;
			
			// blit the ucharacter that was supposed to be here
			SDL_BlitSurface(font.glyphs[ch].surface, NULL, screen, &drect);
			
			// move to next one
			drect.x+=font.glyphs[ch].w+g_CharSpace;
			
			breakCount++;
			continue;
		}
		
		// draw the glyph
		SDL_BlitSurface(font.glyphs[ch].surface, NULL, screen, &drect);
		
		// move over to the next glyph
		drect.x+=font.glyphs[ch].w+g_CharSpace;
	}
	
	return -1;
}

// draw a string centered on the screen
int Fonts::drawStringCentered(int y, int delimiter, const std::string &str, const std::string &fontId) {
	std::string nstr=str;
	
	// cache the right clamp value
	int clamp=SDL_GetVideoSurface()->w;
	
	// explode the string
	StringVector vec=Utils::explodeString("\\n", str);
	
	// draw first string, since there's always at least one
	int x=128-(Fonts::getWidth(fontId, vec[0])/2);
	drawString(x, y, delimiter, clamp, vec[0], fontId);
	
	// second line
	if (vec.size()>1 && delimiter>=vec[0].size()) {
		// calculate another new x and draw the string
		x=128-(Fonts::getWidth(fontId, vec[1])/2);
		drawString(x, y+Fonts::g_LineBreakSize, delimiter-vec[0].size(), clamp, vec[1], fontId);
		
		// third line
		if (vec.size()>2 && delimiter>=(vec[0].size()+vec[1].size())) {
			// calculate last new x, and draw string
			x=128-(Fonts::getWidth(fontId, vec[2])/2);
			drawString(x, y+Fonts::g_LineBreakSize*2, delimiter-(vec[0].size()+vec[1].size()), clamp, vec[2], fontId);
		}
	}
}

// draw a ttf font string
void Fonts::drawTTF(const Point &p, const std::string &str) {
	SDL_Color black={ 0, 0, 0 };
	
	// SDL_ttf doesn't support newline ucharacters, so we need to handle
	// those ourselves
	StringVector lines=Utils::explodeString("\\n", str);
	
	// draw the lines
	for (int i=0; i<lines.size(); i++) {
		// render a solid text line, calculate its position, and blit it
		SDL_Surface *rendered=TTF_RenderText_Solid(g_Arial, lines[i].c_str(), black);
		SDL_Rect drect={ p.x(), p.y()+TTF_FontLineSkip(g_Arial)*i+3 };
		SDL_BlitSurface(rendered, NULL, SDL_GetVideoSurface(), &drect);
		SDL_FreeSurface(rendered);
	}
}

// get the width of a string
int Fonts::getWidth(const std::string &id, const std::string &str) {
	// get the requested font
	if (!queryFont(id)) {
		std::cout << "Font: font '" << id << "' not found\n";
		return -1;
	}
	Font font=*queryFont(id);
	
	// iterate over string
	int width=0;
	for (int i=0; i<str.size(); i++) {
		// new line ucharacters end the string
		if (str[i]=='\n')
			break;
		
		// spaces are 5 pixels wide
		else if (str[i]==' ')
			width+=g_SpaceSize;
		
		else {
			if (str[i]=='\\')
				i+=2;
			
			else {
				// increment width
				width+=font.glyphs[str[i]].w+g_CharSpace;
			}
		}
	}
	
	return width;
}

// get the width of a ttf string
int Fonts::getTTFWidth(const std::string &str) {
	int width=0;
	
	for (int i=0; i<str.size(); i++) {
		if (i==' ')
			width+=g_SpaceSize;
		
		else {
			// get the amount of pixels this glyph needs, and add it
			int advance=0;
			if (TTF_GlyphMetrics(g_Arial, str[i], NULL, NULL, NULL, NULL, &advance)!=-1)
				width+=advance;
		}
	}
	
	return width;
}

// return a font from the map
Fonts::Font* Fonts::queryFont(const std::string &id) {
	return &g_Fonts[id];
}

// add a font to the map
void Fonts::pushFont(const std::string &id, const Font &font) {
	g_Fonts[id]=font;
}

// remove a font from the map
void Fonts::popFont(const std::string &id) {
	Font &f=g_Fonts[id];
	
	// free normal size glyphs
	for (std::map<char, Glyph>::iterator it=f.glyphs.begin(); it!=f.glyphs.end(); ++it)
		SDL_FreeSurface((*it).second.surface);
	
	g_Fonts.erase(id);
}

// clear the font map
void Fonts::clearFontStack() {
	for (FontMap::iterator it=g_Fonts.begin(); it!=g_Fonts.end(); ++it) {
		for (std::map<char, Glyph>::iterator g=(*it).second.glyphs.begin(); g!=(*it).second.glyphs.end(); ++g)
			SDL_FreeSurface((*g).second.surface);
	}
}
