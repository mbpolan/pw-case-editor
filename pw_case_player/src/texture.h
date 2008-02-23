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

#include "common.h"

/// Namespace for management of individual images used in the player
namespace Textures {

/// Intermediate struct used for loading an image
struct _Texture {
	int w;		///< The width
	int h;		///< The height
	short bpp;	///< The bits per pixel
	char *pixels;	///< Pixel data
	SDL_Surface *surface;
};
typedef struct _Texture Texture;

/// Map of all allocated images
extern std::map<ustring, SDL_Surface*> g_TextureMap;

/** Get an image from the map
  * \param id The ID of the image
  * \return Pointer to a surface of the image
*/
SDL_Surface* queryTexture(const ustring &id);

/** Add a new image to the internal map
  * \param id The ID of the image to use
  * \param texture Pointer to allocated surface of the image
*/
void pushTexture(const ustring &id, SDL_Surface *texture);

/** Remove an image from the stack and free allocated memory
  * \param id The ID of the image
*/
void popTexture(const ustring &id);

/// Clear the image stack
void clearStack();

/** Create a usable surface after loading an image from file
  * \param id The ID of the image
  * \param file The path to the image
  * \return Pointer to a new image surface
*/
SDL_Surface* createTexture(const ustring &id, const ustring &file);

/** Create a usable surface after loading a texture from memory
  * \param id The ID of the image
  * \param tex Texture struct prepared from memory
  * \return Pointer to a new image surface
*/
SDL_Surface* createTexture(const ustring &id, const Texture &tex);

}; // namespace Textures

#endif
