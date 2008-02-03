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

namespace IO {

// the pwt file header
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

// case file information
const int FILE_MAGIC_NUM=(('T' << 16) + ('W' << 8) + 'P');
const int FILE_VERSION=10;

// sprite file information
const ustring SPR_MAGIC_NUM="SPR";
const int SPR_VERSION=10;

// unpack the resource file
bool unpackResourceFile(const ustring &path);

// load a case from file
bool loadCaseFromFile(const ustring &path, Case::Case &pcase);

// load a sprite from file
bool loadSpriteFromFile(const ustring &path, Sprite &sprite);

// load stock assets config file
bool loadStockFile(const ustring &path, Case::Case *pcase);

// load theme from xml
bool loadThemeXML(const ustring &path, Theme::ColorMap &map);

// read image data from file
Textures::Texture readImage(FILE *f);

// read a string
ustring readString(FILE *f);

}; // namespace IO

#endif
