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
// glcontext.cpp: implementation of SDLContext class

#include <sstream>
#include "SDL_ttf.h"
#include "SDL_mixer.h"

#include "audio.h"
#include "font.h"
#include "sdlcontext.h"
#include "iohandler.h"
#include "renderer.h"
#include "theme.h"
#include "utilities.h"

int SDLContext::m_Width=640;
int SDLContext::m_Height=480;

// create a rendering context
std::auto_ptr<SDLContext> SDLContext::create() {
	std::auto_ptr<SDLContext> glc(new SDLContext);
	return glc;
}

// destructor
SDLContext::~SDLContext() {
	// delete the game engine
	delete m_Game;
	
	// close audio channel
	if (Audio::g_Output)
		Mix_CloseAudio();
	
	// free the screen
	SDL_FreeSurface(m_Screen);	
}

// initialize basic functionality
bool SDLContext::init() {
	// initialize SDL
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER)<0) {
		std::cout << "SDLContext: unable to intialize video: " << SDL_GetError() << std::endl;
		return false;
	}
	atexit(SDL_Quit);
	
	// seed the random number sequence
	srand(time(NULL));
	
	return true;
}

// initialize video output
bool SDLContext::initVideo(int width, int height) {
	m_VFlags=SDL_HWPALETTE;
	m_VFlags |= SDL_DOUBLEBUF;
	
	// get video hardware information
	const SDL_VideoInfo *vInfo=SDL_GetVideoInfo();
	if (!vInfo)
		std::cout << "SDLContext: unable to get video card information: " << SDL_GetError() << std::endl;
	else {
		// hardware acceleration
		if (vInfo->blit_hw)
			m_VFlags |= SDL_HWACCEL;
		
		// surface storage location
		if (vInfo->hw_available)
			m_VFlags |= SDL_HWSURFACE;
		else
			m_VFlags |= SDL_SWSURFACE;
	}
	
	// try to initialize video
	m_Screen=SDL_SetVideoMode(width, height, 32, m_VFlags);
	if (!m_Screen) {
		std::cout << "SDLContext: unable to set " << width << "x" << height << " video mode: " << SDL_GetError() << std::endl;
		return false;
	}
	
	// copy values to static variables
	SDLContext::m_Width=width;
	SDLContext::m_Height=height;
	
	// set tentative window manager title
	SDL_WM_SetCaption("Loading case...", 0);
	
	return true;
}

// initialize audio output
bool SDLContext::initAudio() {
	// open an audio channel
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)==-1) {
		std::cout << "Unable to open audio! Reason: " << Mix_GetError() << std::endl;
		
		// normally, we should quit if audio wasn't opened, but the game is 
		// still playable without sound
		return true;
	}
	
	return true;
}

// initialize the game
bool SDLContext::initGame(const std::string &pathToCase) {
	// allocate new case
	Case::Case *pcase=new Case::Case;
	
	// load the case
	if (!IO::loadCaseFromFile(pathToCase, *pcase)) {
		std::cout << "SDLContext: unable to load case\n";
		return false;
	}
	
	// find the root path based on path to case
	int npos;
#ifndef __WIN32__
	npos=pathToCase.rfind('/');
#else
	npos=pathToCase.rfind('\\');
#endif
	std::string rootPath=pathToCase.substr(0, npos);
	
	// create game engine
	m_Game=new Game(rootPath, pcase);
	
	// load stock textures
	if (!m_Game->loadStockTextures())
		return false;
	
	// load ttf fonts
	Fonts::g_Fonts[Fonts::FONT_INFO_PAGE]=TTF_OpenFont(".temp/data/fonts/arial.ttf", Fonts::FONT_INFO_PAGE);
	Fonts::g_Fonts[Fonts::FONT_STANDARD]=TTF_OpenFont(".temp/data/fonts/arial.ttf", Fonts::FONT_STANDARD);
	Fonts::g_Fonts[Fonts::FONT_BUTTON_TEXT]=TTF_OpenFont(".temp/data/fonts/arial.ttf", Fonts::FONT_BUTTON_TEXT);
	
	// load our theme
	if (!IO::loadThemeXML(".temp/data/theme.xml", Theme::g_Theme))
		return false;
	
	return true;
}

// render the scene
void SDLContext::render() {
	// render the current scene
	m_Game->render();
	
	// swap buffers and draw our scene
	SDL_Flip(m_Screen);
}

// handle keyboard event
void SDLContext::onKeyboardEvent(SDL_KeyboardEvent *e) {
	// handle video specific keys
	if (e->keysym.sym==SDLK_F1) {
		// see if we're already in fullscreen mode
		if (m_VFlags & SDL_FULLSCREEN)
			m_VFlags &= ~SDL_FULLSCREEN;
		else
			m_VFlags |= SDL_FULLSCREEN;
		
		// set full screen mode
		m_Screen=SDL_SetVideoMode(m_Width, m_Height, 32, m_VFlags);
	}
	
	// toggle audio mute
	else if (e->keysym.sym==SDLK_F2) {
		static bool once=false;
		
		// mute all sound effects
		if (!once) {
			Mix_Volume(-1, 0);
			Mix_VolumeMusic(0);
			
			std::cout << "Audio muted\n";
		}
		
		// resume all sound effects
		else {
			Mix_Volume(-1, MIX_MAX_VOLUME);
			Mix_VolumeMusic(MIX_MAX_VOLUME);
			
			std::cout << "Audio resumed\n";
		}
		
		once=!once;
	}
	
	// take a screenshot
	else if (e->keysym.sym==SDLK_F3) {
		// format a unique file name, and save the screen as a bmp
		std::stringstream ss;
		ss << "screenshot_" << time(NULL) << ".bmp";
		SDL_SaveBMP(m_Screen, ss.str().c_str());
	}
	
	m_Game->onKeyboardEvent(e);
}

// handle mouse click event
void SDLContext::onMouseEvent(SDL_MouseButtonEvent *e) {
	m_Game->onMouseEvent(e);
}

// constructor
SDLContext::SDLContext() {
	m_Screen=NULL;
}
