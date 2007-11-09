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
// texture.h: SDL texture functions

#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <map>
#include "SDL.h"

namespace Textures {

struct _Texture {
	int w;
	int h;
	short bpp;
	char *pixels;
	SDL_Surface *surface;
};
typedef struct _Texture Texture;

// map of all allocated textures
static std::map<std::string, SDL_Surface*> g_TextureMap;

// get a texture from the map
SDL_Surface* queryTexture(const std::string &id);

// add a new texture
void pushTexture(const std::string &id, SDL_Surface *texture);

// remove a texture from the stack
void popTexture(const std::string &id);

// clear the texture stack
void clearStack();

// create a texture after loading an image from file
SDL_Surface* createTexture(const std::string &id, const std::string &file);

// create a texture from data
SDL_Surface* createTexture(const std::string &id, const Texture &tex);

}; // namespace Textures

#endif
