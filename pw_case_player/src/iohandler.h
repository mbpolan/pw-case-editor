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
// iohandler.h: various I/O functions

#ifndef IOHANDLER_H
#define IOHANDLER_H

#include <iostream>

#include "case.h"
#include "sprite.h"
#include "texture.h"
#include "theme.h"

/// Namespace for file loading and parsing functions
namespace IO {

/// The header found in each case file
struct _PWTHeader {
	int ident; // magic number
	int version; // file version

	// byte offsets in data (relative to start of file)
	int overviewOffset;
	int overridesOffset;
	int charOffset;
	int imgOffset;
	int bgOffset;
	int evidenceOffset;
	int locationOffset;
	int audioOffset;
	int testimonyOffset;
	int blockOffset;
};
typedef struct _PWTHeader PWTHeader;

/// Magic number for PWT case file
const int FILE_MAGIC_NUM=(('T' << 16) + ('W' << 8) + 'P');

/// Supported version of the PWT case file
const int FILE_VERSION=10;

/// Magic number for the sprite file
const ustring SPR_MAGIC_NUM="SPR";

/// Supported sprite file version
const int SPR_VERSION=10;

/** Unpack and extract the resource file
  * \param path Path to the resource file
  * \return <b>true</b> if no errors occurred, <b>false</b> otherwise
*/
bool unpackResourceFile(const ustring &path);

/** Load a case from file
  * \param path The path to the file
  * \param pcase Case::Case object to load the data into
  * \return <b>true</b> if no errors occurred, <b>false</b> otherwise
*/
bool loadCaseFromFile(const ustring &path, Case::Case &pcase);

/** Load a sprite from file
  * \param path Path to the sprite file
  * \param sprite Sprite object to load the dat into
  * \return <b>true</b> if no errors occurred, <b>false</b> otherwise
*/
bool loadSpriteFromFile(const ustring &path, Sprite &sprite);

/** Load the stock assets config file
  * \param path Path to the config file
  * \param pcase Pointer to Case::Case object to load the config options into
  * \return <b>true</b> if no errors occurred, <b>false</b> otherwise
*/
bool loadStockFile(const ustring &path, Case::Case *pcase);

/** Load the theme from XML
  * \param path The path to the config file
  * \param map A Theme::ColorMap object to load theme data into
  * \return <b>true</b> if no errors occurred, <b>false</b> otherwise
*/
bool loadThemeXML(const ustring &path, Theme::ColorMap &map);

/** Read image data from the file
  * \param f FILE handle with read pointer set to image
  * \return A Textures::Texture object with data
*/
Textures::Texture readImage(FILE *f);

/** Read a string from the file
  * \param f FILE handle with read pointer set to string
  * \return The read string
*/
ustring readString(FILE *f);

}; // namespace IO

#endif
