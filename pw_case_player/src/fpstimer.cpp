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
// fpstimer.h: implementation of FPSTimer class

#include "SDL.h"

#include "fpstimer.h"

// constructor
FPSTimer::FPSTimer() {
	m_LastFrame=0;
	m_FPSLock=65.0;
}

// calculate delta in seconds between two sets of ticks
double FPSTimer::deltaSeconds(int ticks1, int ticks2) {
	return ((ticks2-ticks1)/1000.0);
}

// delay the engine until the fps lock rate is reached
void FPSTimer::delay() {
	int now=SDL_GetTicks();
	
	// loop until delta time has reached the appropriate frame rate
	while(deltaSeconds(m_LastFrame, now)<(1.0/m_FPSLock))
		now=SDL_GetTicks();
	
	m_LastFrame=now;
}
