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

#include "fpstimer.h"
#include "sdlcontext.h"

/** Class that controls toplevel functions.
  * The Application class handles many functions that relate to the
  * functionality of the player as a whole. This includes window manager
  * control, video and audio output, and keyboard and mouse input.
*/
class Application {
	public:
		/// The version of this program
		static ustring VERSION;
		
		/// Possible command line arguments
		enum ArgFlags { ARG_NONE=0x00, ARG_NO_SOUND=0x01, ARG_FULLSCREEN=0x02 };
		
		/** Constructor
		  * \param argc Amount of arguments
		  * \param argv Array of arguments
		*/
		Application(int argc, char *argv[]);
		
		/// Run the application
		void run();
		
	private:
		/** Process any events
		  * \return <b>true</b> if the event loop should continue, <b>false</b> otherwise
		*/
		bool processEvents();
		
		/// Calculate and display the FPS
		void calculateFPS();
		
		/** Keyboard event handler
		  * \param The SDL_KeyboardEvent object
		  * \return <b>true</b> if the event loop should continue, <b>false</b> otherwise
		*/
		bool keyboardEvent(SDL_KeyboardEvent &e);
		
		/// SDL rendering context
		std::auto_ptr<SDLContext> m_SDLContext;
		
		/// Path to case file
		ustring m_CasePath;
		
		/// Game timer
		FPSTimer m_Timer;
		
		/// Arguments from the command line
		int m_ArgFlags;
};

#endif
