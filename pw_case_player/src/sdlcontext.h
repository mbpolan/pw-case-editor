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
// glcontext.h: the SDLContext class

#ifndef SDLCONTEXT_H
#define SDLCONTEXT_H

#include <iostream>
#include "SDL.h"

#include "case.h"
#include "game.h"

// SDL rendering context
class SDLContext {
	public:
		// create a rendering context
		static std::auto_ptr<SDLContext> create();
		
		// destructor
		~SDLContext();
		
		// return the context's drawing area width
		int getWidth() const { return m_Width; }
		
		// return the context's drawing area height
		int getHeight() const { return m_Height; }
		
		// initialize basic functionality
		bool init();
		
		// initialize video output
		bool initVideo(int width, int height);
		
		// initialize audio output
		bool initAudio();
		
		// initialize the game
		bool initGame(const std::string &pathToCase);
		
		// render the scene
		void render();
		
		// handle keyboard event
		void onKeyboardEvent(SDL_KeyboardEvent *e);
		
		// handle mouse click event
		void onMouseEvent(SDL_MouseButtonEvent *e);
		
	private:
		// hidden constructor
		SDLContext();
		
		// dimensions of drawing area
		static int m_Width, m_Height;
		
		// flags passed to set the video mode
		int m_VFlags;
		
		// game engine
		Game *m_Game;
		
		// screen surface
		SDL_Surface *m_Screen;
};

#endif
