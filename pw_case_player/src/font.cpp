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

#include "iohandler.h"
#include "font.h"
#include "texture.h"

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
	
	// read amount of characters
	int amount;
	fread(&amount, sizeof(int), 1, f);
	
	// iterate over glyphs
	for (int i=0; i<amount; i++) {
		// read character
		char ch=fgetc(f);
		
		// read buffer size
		unsigned int size;
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
bool Fonts::willBreak(int x, int y, int rightClamp, const std::string &str, const std::string &fontId) {
	Font font=g_Fonts[fontId];
	int breakCount=0;
	int ex=x;
	
	for (int i=0; i<str.size(); i++) {
		char ch=(char) str[i];
		
		// see if we should force a new line
		if (ch=='\n') {
			// check if we already have three lines
			if (breakCount==3)
				return true;
			
			ex=x;
			
			breakCount++;
			continue;
		}
		
		// see if this is a literal new line character
		if (ch=='\\' && str[i+1]=='n') {
			// make sure we don't make any extra lines
			if (breakCount==3)
				return true;
			
			ex=x;
			
			breakCount++;
			
			// we skip the next character
			i++;
			
			continue;
		}
		
		// if a space is needed, skip some pixels
		else if (ch==' ') {
			ex+=5;
			
			continue;
		}
		
		// force a new line if needed
		int width=font.glyphs[ch].w+font.glyphs[str[i+1]].w+2;
		if ((ex+width)>=rightClamp) {
			// check if we already have three lines
			if (breakCount==3)
				return true;
			
			// reset for next row
			ex=x;
			
			// move to next one
			ex+=font.glyphs[ch].w+1;
			
			breakCount++;
			continue;
		}
		
		// move over to the next glyph
		ex+=font.glyphs[ch].w+1;
	}
	
	return false;
}

// draw a string on the screen
int Fonts::drawString(int x, int y, const std::string &str, const std::string &fontId) {
	drawString(x, y, str.size(), SDL_GetVideoSurface()->w, str, fontId);
}

// draw a string with clamped restrictions
int Fonts::drawString(int x, int y, int limit, int rightClamp, const std::string &str, const std::string &fontId) {
	// check if the font exists
	if (g_Fonts.find(fontId)==g_Fonts.end()) {
		std::cout << "Fonts: font '" << fontId << "' not found in stack.\n";
		return -1;
	}
	
	// also, make sure the default video surface exists
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen) {
		std::cout << "Fonts: unable to get pointer to video surface.\n";
		return -1;
	}
	
	// get the requested font
	Font font=g_Fonts[fontId];
	
	// set initial position
	SDL_Rect drect;
	drect.x=x;
	drect.y=y;
	
	// count the amount of times we started a new line
	int breakCount=0;
	
	// bounds check
	if (limit>str.size())
		limit=str.size();
	
	// iterate over string and draw each character
	for (int i=0; i<limit; i++) {
		char ch=(char) str[i];
		
		// see if we should force a new line
		if (ch=='\n') {
			// check if we already have three lines
			if (breakCount==3)
				return i;
			
			drect.x=x;
			drect.y+=15;
			
			breakCount++;
			continue;
		}
		
		// see if this is a literal new line character
		if (ch=='\\' && str[i+1]=='n') {
			// make sure we don't make any extra lines
			if (breakCount==3)
				return i;
			
			drect.x=x;
			drect.y+=15;
			
			breakCount++;
			
			// we skip the next character
			i++;
			
			continue;
		}
		
		// if a space is needed, skip some pixels
		else if (ch==' ') {
			drect.x+=5;
			
			continue;
		}
		
		// force a new line if needed
		int width=font.glyphs[ch].w+font.glyphs[str[i+1]].w+2;
		if ((drect.x+width)>=rightClamp) {
			// check if we already have three lines
			if (breakCount==3)
				return i;
			
			// blit a hyphen, if needed
			if (i>0 && str[i-1]!=' ')
				SDL_BlitSurface(font.glyphs['-'].surface, NULL, screen, &drect);
			
			// reset for next row
			drect.x=x;
			drect.y+=15;
			
			// blit the character that was supposed to be here
			SDL_BlitSurface(font.glyphs[ch].surface, NULL, screen, &drect);
			
			// move to next one
			drect.x+=font.glyphs[ch].w+1;
			
			breakCount++;
			continue;
		}
		
		// draw the glyph
		SDL_BlitSurface(font.glyphs[ch].surface, NULL, screen, &drect);
		
		// move over to the next glyph
		drect.x+=font.glyphs[ch].w+1;
	}
	
	return -1;
}

// get the width of a string
int Fonts::getWidth(const std::string &id, const std::string &str) {
	Font font=queryFont(id);
	
	// iterate over string
	int width=0;
	for (int i=0; i<str.size(); i++) {
		// new line characters end the string
		if (str[i]=='\n')
			break;
		
		// spaces are 5 pixels wide
		else if (str[i]==' ')
			width+=5;
		
		else {
			// increment width
			width+=font.glyphs[str[i]].w;
		}
	}
	
	return width;
}

// return a font from the map
Fonts::Font Fonts::queryFont(const std::string &id) {
	return g_Fonts[id];
}

// add a font to the map
void Fonts::pushFont(const std::string &id, const Font &font) {
	g_Fonts[id]=font;
}

// remove a font from the map
void Fonts::popFont(const std::string &id) {
	Font &f=g_Fonts[id];
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
