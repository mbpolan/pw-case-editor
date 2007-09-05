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

namespace IO {

// case file information
const std::string FILE_MAGIC_NUM="PWT";
const int FILE_VERSION=10;

// sprite file information
const std::string SPR_MAGIC_NUM="SPR";
const int SPR_VERSION=10;

// load a case from file
bool loadCaseFromFile(const std::string &path, Case::Case &pcase);

// load a sprite from file
bool loadSpriteFromFile(const std::string &path, Sprite &sprite);

// load stock texture config file
bool loadTextureFile(const std::string &path);

// read image data from file
Textures::Texture readImage(FILE *f);

// read a string
std::string readString(FILE *f);

}; // namespace IO

#endif
