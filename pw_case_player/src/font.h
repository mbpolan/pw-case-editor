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
// font.h: the Font engine

#ifndef FONT_H
#define FONT_H

#include <iostream>
#include <SDL/SDL.h>
#include <map>

#include "case.h"

namespace Fonts {

// amount of pixels to skip following a line break
const int g_LineBreakSize=17;

// a single glyph
struct _Glyph {
	int w;
	SDL_Surface *surface;
};
typedef struct _Glyph Glyph;

// font containing glyphs
struct _Font {
	std::string id;
	std::map<char, Glyph> glyphs;
};
typedef struct _Font Font;

// global font map
typedef std::map<std::string, Font> FontMap;
static FontMap g_Fonts;

// load a font from file
bool loadFont(const std::string &path, Fonts::Font &font);

// create a font glyph
SDL_Surface* createSurface(char *buffer);

// break a string apart based on a delimiting character
StringVector explodeString(char delimiter, const std::string &str);

// see if this string is too long and needs to be broken
bool lineWillBreak(int x, int y, int rightClamp, const std::string &str, const std::string &fontId);

// see if a word is too long to fit in this line
bool wordWillBreak(int x, int rightClamp, const std::string &word, const std::string &fontId);

// draw a string on the screen
int drawString(int x, int y, const std::string &str, const std::string &fontId);

// draw a string with clamped restrictions and delimiter
int drawString(int x, int y, int delimiter, int rightClamp, const std::string &str, const std::string &fontId);

// get the width of a string
int getWidth(const std::string &id, const std::string &str);

// return a font from the map
Font queryFont(const std::string &id);

// add a font to the map
void pushFont(const std::string &id, const Font &font);

// remove a font from the map
void popFont(const std::string &id);

// clear the font map
void clearFontStack();

};

#endif
