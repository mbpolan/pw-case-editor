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
// application.cpp: implementation of Application class

#include <sstream>
#include "SDL.h"

#include "application.h"
#include "iohandler.h"

// constructor
Application::Application(int argc, char *argv[]) {
	// second argument is path to case file
	m_CasePath=argv[1];
}

// run the application
void Application::run() {
	// create a new gl context
	m_SDLContext=SDLContext::create();
	
	// initialize it
	if (!m_SDLContext->init())
		return;
	
	// initialize audio
	if (!m_SDLContext->initAudio())
		return;
	
	// initialize video
	if (!m_SDLContext->initVideo(256, 389))
		return;
	
	// initialize game data
	if (!m_SDLContext->initGame(m_CasePath))
		return;
	
	// set the window manager title
	SDL_WM_SetCaption("PW Case Player", 0);
	
	// start main loop
	bool loop=true;
	while(loop) {
		// process pending events in the loop
		loop=processEvents();
		
		// render the scene
		m_SDLContext->render();
		
		// calculate and display FPS
		calculateFPS();
	}
}

// process any events
bool Application::processEvents() {
	SDL_Event e;
	
	// see if we have any events
	while(SDL_PollEvent(&e)) {
		switch(e.type) {
			// quit event
			case SDL_QUIT: return false;
			
			// keyboard event
			case SDL_KEYDOWN: {
				// process this event first
				bool ret=keyboardEvent(e.key);
				if (!ret)
					return ret;
				
				// if it wasn't a critical event, pass it to the context
				m_SDLContext->onKeyboardEvent(&e.key);
			}; break;
			
			// mouse button event
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN: {
				// pass this event to the context
				m_SDLContext->onMouseEvent(&e.button);
			}; break;
			
			default: break;
		}
	}
	
	return true;
}

// calculate and display fps
void Application::calculateFPS() {
	static Uint32 last=0;
	static int ff=0;
	
	// if a second has passed, see how many frames were drawn
	Uint32 now=SDL_GetTicks();
	if (now-last>1000) {
		last=now;
		
		// set the wm title to include fps
		std::stringstream ss;
		ss << "PW Case Player - " << ff << " FPS";
		SDL_WM_SetCaption(ss.str().c_str(), 0);
		
		// reset frame counter
		ff=0;
	}
	
	ff++;
}

// keyboard event handler
bool Application::keyboardEvent(SDL_KeyboardEvent &e) {
	int sym=e.keysym.sym;
	
	// see the key that was pressed
	switch(sym) {
		case SDLK_ESCAPE: return false;
		
		default: return true;
	}
}
