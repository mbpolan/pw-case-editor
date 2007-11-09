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

#include <sstream>
#include "SDL/SDL_image.h"

#include "texture.h"

// get a texture from the map
SDL_Surface* Textures::queryTexture(const std::string &id) {
	return g_TextureMap[id];
}

// add a new texture
void Textures::pushTexture(const std::string &id, SDL_Surface *texture) {
	g_TextureMap[id]=texture;
}

// remove a texture from the stack
void Textures::popTexture(const std::string &id) {
	SDL_FreeSurface(g_TextureMap[id]);
	g_TextureMap.erase(id);
}

// clear the texture stack
void Textures::clearStack() {
	for (std::map<std::string, SDL_Surface*>::iterator it=g_TextureMap.begin(); it!=g_TextureMap.end(); ++it) {
		SDL_FreeSurface((*it).second);
		g_TextureMap.erase(it);
	}
}

// create a texture after loading an image from file
SDL_Surface* Textures::createTexture(const std::string &id, const std::string &str) {
	std::string file=str;
	
	// get the file extension
	std::string ext=file.substr(file.size()-3, file.size()-1);
	
	// see if this image needs to have its red/blue values flipped
	bool rgbFix=false;
	if (file[0]=='*') {
		rgbFix=true;
		file.erase(0, 1);
	}
	
	// load the image
	SDL_Surface *surface=IMG_Load(file.c_str());
	if (surface) {
		// set transparent pixel
		SDL_SetColorKey(surface, SDL_SRCCOLORKEY, SDL_MapRGB(SDL_GetVideoSurface()->format, 0, 255, 0));
		
		// add this texture to the map
		if (id=="null") {
			// use next available name
			std::stringstream ss;
			ss << "UNKNOWN_TEXTURE_" << g_TextureMap.size()+20;
			
			pushTexture(ss.str(), surface);
		}
		
		else if (id!="")
			pushTexture(id, surface);
		
		// return this texture
		return surface;
	}
	
	return NULL;
}

// create a texture from data
SDL_Surface* Textures::createTexture(const std::string &id, const Textures::Texture &tex) {
	SDL_Surface *surface;
	
	if (!tex.surface) {
		if (!tex.pixels)
			return NULL;
		
		// create the surface
		surface=SDL_CreateRGBSurface(SDL_SWSURFACE, tex.w, tex.h, tex.bpp, 255U << 16, 255 << 8, 255 << 0, 0);
		
		SDL_LockSurface(surface);
		
		// copy the texture's pixels to the usable surface
		char *pixels=(char*) surface->pixels;
		memcpy(pixels, tex.pixels, tex.w*tex.h*(tex.bpp/8));
		surface->pixels=(void*) pixels;
		
		// delete the unused pixels
		delete [] tex.pixels;
		
		SDL_UnlockSurface(surface);
		
	}
	
	else
		surface=tex.surface;
	
	// set the transparent key
	SDL_SetColorKey(surface, SDL_SRCCOLORKEY, SDL_MapRGB(surface->format, 0, 255, 0));
	
	// add this texture to the map
	if (id=="null") {
		// use next available id
		std::stringstream ss;
		ss << "UNKNOWN_TEXTURE_" << g_TextureMap.size()+20;
		
		pushTexture(ss.str(), surface);
	}
	
	else if (id!="")
		pushTexture(id, surface);
	
	return surface;
}
