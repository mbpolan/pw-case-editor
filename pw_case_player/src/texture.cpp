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
// texture.cpp: implementation of Textures namespace

#include <cmath>
#include <sstream>
#include "SDL_image.h"

#include "texture.h"
#include "utilities.h"

namespace Textures {

std::map<ustring, Texture> g_TextureMap;

Texture g_NullTexture;

}

// test if a texture is null
bool Textures::isNull(const GLuint &id) {
	return (g_NullTexture.id==id);
}

// get a texture from the map
Textures::Texture Textures::queryTexture(const ustring &id) {
	// automatically return the null texture if requested
	if (id=="no_texture")
		return g_NullTexture;
	
	// see if this texture exists
	if (g_TextureMap.find(id)==g_TextureMap.end())
		return g_NullTexture;
	
	return g_TextureMap[id];
}

Textures::Texture Textures::queryTexture(const GLuint &id) {
	for (std::map<ustring, Texture>::iterator it=g_TextureMap.begin(); it!=g_TextureMap.end(); ++it) {
		if ((*it).second.id==id)
			return queryTexture((*it).first);
	}
	
	return queryTexture("no_texture");
}

// add a new texture
void Textures::pushTexture(const ustring &id, const Texture &tex) {
	if (g_TextureMap.find(id)!=g_TextureMap.end())
		popTexture(id);
	g_TextureMap[id]=tex;
}

// remove a texture from the stack
void Textures::popTexture(const ustring &id) {
	glDeleteTextures(1, &queryTexture(id).id);
	g_TextureMap.erase(id);
}

// clear the texture stack
void Textures::clearStack() {
	for (std::map<ustring, Texture>::iterator it=g_TextureMap.begin(); it!=g_TextureMap.end(); ++it)
		glDeleteTextures(1, &(*it).second.id);
	
	glDeleteTextures(1, &g_NullTexture.id);
	
	g_TextureMap.clear();
}

// create a texture after loading an image from file
GLuint Textures::createTexture(const ustring &id, const ustring &str, int alpha) {
	ustring file=str;
	
	// get the file extension
	ustring ext=file.substr(file.size()-3, file.size()-1);
	
	// see if this image needs to have its red/blue values flipped
	bool rgbFix=false;
	if (file[0]=='*') {
		rgbFix=true;
		file.erase(0, 1);
	}
	
	// load the image
	SDL_Surface *surface=IMG_Load(file.c_str());
	if (!surface)
		return 0;
	
	return createTexture(id, surface, alpha);
}

// create a texture from data
GLuint Textures::createTexture(const ustring &id, SDL_Surface *surface, int alpha) {
	// our new texture struct
	Texture tex;
	
	// set the width and height
	int w=tex.w=surface->w;
	int h=tex.h=surface->h;
	
	// default texture coordinates
	tex.u=tex.v=1.0f;
	
	// just in case we need to expand the surface
	SDL_Surface *expanded=NULL;
	
	// test to see if the texture's width and height aren't powers of 2
	if ((tex.w & (tex.w-1))!=0 || (tex.h & (tex.h-1))!=0) {
		// find new dimensions
		w=Utils::nearestPower2(surface->w);
		h=Utils::nearestPower2(surface->h);
		
		// now calculate the texture coordinates
		tex.u=(float) surface->w/w;
		tex.v=(float) surface->h/h;
		
		// get our rgba color masks
		Uint32 rm, gm, bm, am;
		Utils::setRGBAMasks(rm, gm, bm, am);
		
		// create a new surface for this expanded texture
		expanded=SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, rm, gm, bm, am);
		
		// clear the alpha if it is enabled
		SDL_SetAlpha(surface, 0, 0);
		
		// only blit the actual surface onto the new one
		SDL_Rect rect={ 0, 0, surface->w, surface->h };
		SDL_BlitSurface(surface, &rect, expanded, &rect);
	}
	
	else {
		// make sure to convert surfaces which aren't 32 bit
		if (surface->format->BytesPerPixel!=4) {
			SDL_Surface *tmp=SDL_DisplayFormat(surface);
			SDL_FreeSurface(surface);
			surface=tmp;
		}
	}
	
	// set a pointer to our active surface
	SDL_Surface *ptr=(expanded ? expanded : surface);
	
	SDL_LockSurface(ptr);
	
	// iterate over pixels, and modify the pixel data
	// all surfaces will have 4 bytes per pixel, since they will either be recreated
	// through a new size, or converted to display format
	char *pixel=(char*) ptr->pixels;
	for (int i=0; i<ptr->w*ptr->h*4; i+=4, pixel+=4) {
			// set new alpha if not fully opaque
		if (alpha!=255)
			*(pixel+3)=alpha;
			
			// our color key is green (0,255,0), so if any color matches that, set the alpha
			// component to be fully transparent
		if (*pixel==(char) 0 && *(pixel+1)==(char) 255 && *(pixel+2)==(char) 0)
			*(pixel+3)=0;
	}
		
	SDL_UnlockSurface(ptr);
	
	// generate a new texture
	glGenTextures(1, &tex.id);
	glBindTexture(GL_TEXTURE_2D, tex.id);
	
	// set parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	// now create the actual texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr->pixels);
	
	// free used surfaces
	if (expanded)
		SDL_FreeSurface(expanded);
	SDL_FreeSurface(surface);
	
	// null texture gets its own variable
	if (id=="no_texture")
		g_NullTexture=tex;
	
	// add this texture to the map
	else if (id==STR_NULL) {
		// use next available name
		std::stringstream ss;
		ss << "UNKNOWN_TEXTURE_" << g_TextureMap.size()+20;
		
		pushTexture(ss.str(), tex);
	}
	
	else if (id!="")
		pushTexture(id, tex);
	
	// return this texture
	return tex.id;
}
