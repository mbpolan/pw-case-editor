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
#include <SDL/SDL_ttf.h>
#include <map>

#include "case.h"

namespace Fonts {

// amount of pixels to skip following a line break, space, and in between characters
const int SIZE_LINE_BREAK=17;
const int SIZE_CHAR_SPACE=2;
const int SIZE_WHITESPACE=10-SIZE_CHAR_SPACE;

// default font sizes for elements
static const int FONT_BUTTON_TEXT=16;
static const int FONT_STANDARD=13;
static const int FONT_INFO_PAGE=11;

// predefined font colors
extern const Color COLOR_BLACK;
extern const Color COLOR_WHITE;
extern const Color COLOR_BLUE;
extern const Color COLOR_ORANGE;
extern const Color COLOR_GREEN;
extern const Color COLOR_YELLOW;

// quality of rendering
enum Quality { QUALITY_SOLID=0, QUALITY_BLEND };

// ttf fonts
typedef TTF_Font Font;
extern std::map<int, Font*> g_Fonts;

// load a font
bool loadFont(const ustring &path, int size);

// get a surface with a rendered glyph
SDL_Surface* renderGlyph(Uint16 ch, int size, const Color &color, const Quality &quality);

// calculate the y coordinate for a glyph to render correctly on baseline
int glyphBase(int y, Uint16 ch, int size);

// see if a character should not be drawn
bool discardChar(Uint16 ch);

// see if this string is too long and needs to be broken
bool lineWillBreak(const Point &p, int rightClamp, const ustring &str, int size);

// draw a string on the screen
int drawString(const Point &p, const ustring &str, int size, const Color &color);

// draw a string with clamped restrictions and delimiter
int drawString(const Point &p, int delimiter, int rightClamp, const ustring &str, int size, const Color &color);

// draw a multicolor string
int drawStringMulticolor(const Point &p, int delimiter, int rightClamp, const ustring &str, int size, const ColorRangeVector &vec);

// draw a string centered on the screen
int drawStringCentered(int y, int delimiter, const ustring &str, int size, const Color &color);

// draw a ttf font string
void drawStringBlended(const Point &p, const ustring &str, int size, const Color &color);

// get the width of a ttf string
int getWidth(const ustring &str, int size);

// get the width of a glyph
int getGlyphWidth(Uint16 ch, int size);

// get the height of a ttf font
int getHeight(int size);

// return a ttf font from the map
Font* queryFont(int size);

// add a font to the map
void pushFont(int size, Font *font);

// clear the font map
void clearFontStack();

};

#endif
