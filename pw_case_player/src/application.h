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
// application.h: the Application class

#ifndef APPLICATION_H
#define APPLICATION_H

#include <iostream>

#include "sdlcontext.h"

// class that controls toplevel functions
class Application {
	public:
		// constructor
		Application(int argc, char *argv[]);
		
		// run the application
		void run();
		
	private:
		// process any events
		bool processEvents();
		
		// calculate and display fps
		void calculateFPS();
		
		// keyboard event handler
		bool keyboardEvent(SDL_KeyboardEvent &e);
		
		// opengl rendering context
		std::auto_ptr<SDLContext> m_SDLContext;
		
		// path to case file
		std::string m_CasePath;
};

#endif
