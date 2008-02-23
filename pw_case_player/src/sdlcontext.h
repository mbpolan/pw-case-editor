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

/** Context used for rendering using SDL.
  * Since the game engine is backed by SDL, a context needs to first
  * be allocated before any drawing can take place. This context then
  * handles swapping drawing buffers, and sets up all of SDL's subsystems.
  * It also makes sure the window is correctly set up and updated.
*/
class SDLContext {
	public:
		/// Create a rendering context
		static std::auto_ptr<SDLContext> create();
		
		/// Destructor
		~SDLContext();
		
		/** Get the context's drawing area width
		  * \return The width of the context
		*/
		int getWidth() const { return m_Width; }
		
		/** Get the context's drawing area height
		  * \return The height of the context
		*/
		int getHeight() const { return m_Height; }
		
		/** Initialize basic functionality
		  * \return <b>true</b> if no errors occurred, <b>false</b> otherwise
		*/
		bool init();
		
		/** Initialize video output
		  * \param width The width of the video context
		  * \param height The height of the video context
		  * \param fullscreen Whether or not to enable fullscreen by default
		  * \return <b>true</b> if no errors occurred, <b>false</b> otherwise
		*/
		bool initVideo(int width, int height, bool fullscreen);
		
		/** Initialize audio output
		  * \return <b>true</b> if no errors occurred, <b>false</b> otherwise
		*/
		bool initAudio();
		
		/** Initialize the game
		  * \param pathToCase Path to the case file
		  * \return <b>true</b> if no errors occurred, <b>false</b> otherwise
		*/
		bool initGame(const ustring &pathToCase);
		
		/// Render the scene
		void render();
		
		/** Handle keyboard events
		  * \param e SDL struct representing the keyboard event
		*/
		void onKeyboardEvent(SDL_KeyboardEvent *e);
		
		/** Handle mouse click events
		  * \param e SDL struct representing the mouse event
		*/
		void onMouseEvent(SDL_MouseButtonEvent *e);
		
	private:
		/// Constructor
		SDLContext();
		
		//@{
		/** Dimensions of context */
		static int m_Width, m_Height;
		//@}
		
		/// Flags passed to set the video mode
		int m_VFlags;
		
		/// The Game engine
		Game *m_Game;
		
		/// Screen surface
		SDL_Surface *m_Screen;
};

#endif
