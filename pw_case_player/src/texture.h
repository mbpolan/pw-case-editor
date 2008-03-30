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
#include <GL/gl.h>
#include <GL/glu.h>
#include <map>
#include "SDL.h"

#include "common.h"

/// Namespace for management of individual images used in the player
namespace Textures {

/// Struct used to represent a texture
struct _Texture {
	int w;		///< The width
	int h;		///< The height
	float u;
	float v;
	GLuint id;	///< The ID of the GL texture
};
typedef struct _Texture Texture;

/// The null texture to use if a texture is not found but still requested
extern Texture g_NullTexture;

/// Map of all allocated images
extern std::map<ustring, Texture> g_TextureMap;

/** Test if a texture does not exist
  * \param id The OpenGL texture ID to test
  * \return <b>true</b> if it does not exist, <b>false</b> otherwise
*/
bool isNull(const GLuint &id);

/** Get a texture from the map
  * \param id The ID of the image
  * \return The requested texture
*/
Texture queryTexture(const ustring &id);

/** Get a texture based on GL ID from the map
  * \param id The GL texture ID
  * \return The requested texture
*/
Texture queryTexture(const GLuint &id);

/** Add a new image to the internal map
  * \param id The ID of the image to use
  * \param texture A completed Texture struct
*/
void pushTexture(const ustring &id, const Texture &tex);

/** Remove an image from the stack and free allocated memory
  * \param id The ID of the image
*/
void popTexture(const ustring &id);

/// Clear the image stack
void clearStack();

/** Create a usable surface after loading an image from file
  * \param id The ID of the image
  * \param file The path to the image
  * \param alpha The requested alpha value to apply
  * \return ID of this texture
*/
GLuint createTexture(const ustring &id, const ustring &file, int alpha=255);

/** Create a usable surface after loading a texture from memory
  * \param id The ID of the image
  * \param tex Pointer to an SDL_Surface
  * \param alpha The requested alpha value to apply
  * \return ID of this texture
*/
GLuint createTexture(const ustring &id, SDL_Surface *surface, int alpha=255);

}; // namespace Textures

#endif
