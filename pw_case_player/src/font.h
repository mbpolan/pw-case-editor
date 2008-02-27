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

/// Namespace for all font and string drawing related API
namespace Fonts {

/// The amount of pixels to skip following a line break
const int SIZE_LINE_BREAK=17;

/// The amount of pixels to skip following a character
const int SIZE_CHAR_SPACE=2;

/// The amount of pixels that define the size of a whitespace
const int SIZE_WHITESPACE=10-SIZE_CHAR_SPACE;

/// Size of font for button text
static const int FONT_BUTTON_TEXT=16;

/// Size of font for regular dialogue rendering
static const int FONT_STANDARD=13;

/// Size of font in the Court Record info pages
static const int FONT_INFO_PAGE=11;

/// Predefined black font color
extern const Color COLOR_BLACK;

/// Predefined white font color
extern const Color COLOR_WHITE;

/// Predefined blue font color
extern const Color COLOR_BLUE;

/// Predefined orange font color
extern const Color COLOR_ORANGE;

/// Predefined green font color
extern const Color COLOR_GREEN;

/// Predefined yellow font color
extern const Color COLOR_YELLOW;

/// Quality of font glyph rendering
enum Quality { QUALITY_SOLID=0, QUALITY_BLEND };

// ttf fonts
typedef TTF_Font Font;
extern std::map<int, Font*> g_Fonts;

/** Load a TrueType font
  * \param path Path to font file
  * \param size Size of the font
  * \return <b>true</b> if successful, <b>false</b> otherwise
*/
bool loadFont(const ustring &path, int size);

/** Draw a single glyph onto a new surface ready for further blitting
  * \param ch The Unicode character to render
  * \param size The size of the source font
  * \param color The glyph color
  * \param quality The quality of the rendering
  * \return A new SDL_Surface with the glyph drawn
*/
SDL_Surface* renderGlyph(Uint16 ch, int size, const Color &color, const Quality &quality);

/** Calculates the y-coordinate for a glyph to render correctly on the baseline.
  * Since certain glyphs are bigger than others, this function computes the correct 
  * y-coordinte (based on the provided one) that this glyph should be drawn on, in 
  * order for the line of text, as a whole, to be drawn correctly and smoothly.
  * \param y The y-coordinate of the line of text
  * \param ch The Unicode character
  * \param size The source font size
  * \return The calculated y-coordinate for this glyph
*/
int glyphBase(int y, Uint16 ch, int size);

/** Check to see if a character should not be drawn.
  * Certain characters are reserved for internal player operation, and therefore should 
  * be culled from a visible string. This function will check the provided character, and 
  * then decide if it is indeed one of the reserved characters.
  * \param ch The Unicode character to check
  * \return <b>true</b> if it should be discarded, <b>false</b> otherwise
*/
bool discardChar(Uint16 ch);

/** Check to see if a string is too long for one line
  * \param p The point at which the string is to be drawn
  * \param rightClamp The limit at which the line breaks
  * \param str The string to check
  * \param size The size of the source font
  * \return <b>true</b> if this string exceeds the provided limit, <b>false</b> otherwise
*/
bool lineWillBreak(const Point &p, int rightClamp, const ustring &str, int size);

/** Draw a string on the screen
  * \param p The point at which the string should be drawn
  * \param str The string to draw
  * \param size The size of the source font
  * \param color The string color
  * \return Index into the string where drawing stopped
*/
int drawString(const Point &p, const ustring &str, int size, const Color &color);

/** Draw a string with clamped restrictions and delimiter
  * \param p The point at which the string should be drawn
  * \param delimiter Index into string at which to stop drawing
  * \param rightClamp X-coordinate to limit string drawing
  * \param str The string to draw
  * \param size The size of the source font
  * \param color The color of the string
  * \return Index into the string where drawing stopped
*/
int drawString(const Point &p, int delimiter, int rightClamp, const ustring &str, int size, const Color &color);

/** Draw a multicolor string
  * \param p The point at which to start drawing
  * \param delimiter Index into string at which to stop drawing
  * \param rightClamp X-coordinate to limit string drawing
  * \param str The string to draw
  * \param vec A ColorRangeVector containing color offsets
  * \return Index into string where drawing stopped
*/
int drawStringMulticolor(const Point &p, int delimiter, int rightClamp, const ustring &str, int size, const ColorRangeVector &vec);

/** Draw a string centered on the screen
  * \param y The y-coordinate of the string
  * \param delimiter Index into string at which to stop drawing
  * \param str The string to draw
  * \param size The size of the source font
  * \param color The color of the string
  * \return Index into string at which drawing stopped
*/
int drawStringCentered(int y, int delimiter, const ustring &str, int size, const Color &color);

/** Draw a string with nicer quality
  * \param p The point at which to draw the string
  * \param str The string to draw
  * \param size The size of the source font
  * \param color The color of the string
*/
void drawStringBlended(const Point &p, const ustring &str, int size, const Color &color);

/** Calculate the width of a string
  * \param str The string to test
  * \param size The size of the source font
  * \return The calculated pixel string width as if it were drawn on the screen
*/
int getWidth(const ustring &str, int size);

/** Calculate the width of a string drawn using the SDL_TTF library functions.
  * This function differs from the regular Fonts::getWidth() in that strings
  * which were drawn using functions such as TTF_RenderUNICODE_* don't follow
  * the engine's whitespace and glyph spacings sizes, and so using the aforementioned
  * function will cause the width to be incorrectly calculated
  * \param str The string to test
  * \param size The size of the source font
  * \return The calculated pixel string width as if it were drawn on the screen
*/
int getWidthTTF(const ustring &str, int size);

/** Calculate the width of a glyph
  * \param ch The Unicode character to test
  * \param size The size of the source font
  * \return The pixel width of the glyph
*/
int getGlyphWidth(Uint16 ch, int size);

/** Calculate the pixel height of a string
  * \param size The size of the font to test
  * \return The pixel height of the font
*/
int getHeight(int size);

/** Get a font object from the internal map
   * \param size The size of the font to get
   * \return Pointer to requested font object
*/
Font* queryFont(int size);

/** Add a font to the internal map
  * \param size The size of this font
  * \param font Pointer to allocated font object to add
*/
void pushFont(int size, Font *font);

/// Clear the font map and free allocated memory
void clearFontStack();

};

#endif
