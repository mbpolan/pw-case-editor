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
// fpstimer.h: the Timer class

#ifndef FPSTIMER_H
#define FPSTIMER_H

// timer class used for game engine fps consistency purposes
class FPSTimer {
	public:
		// constructor
		FPSTimer();
		
		// set the fps to lock; used in delay()
		void setFPSLock(double fps) { m_FPSLock=fps; }
		
		// set the time that the last frame was drawn
		void setLastFrame(int ticks) { m_LastFrame=ticks; }
		
		// calculate delta in seconds between two sets of ticks
		double deltaSeconds(int ticks1, int ticks2);
		
		// delay the engine until the locked fps rate is reached
		void delay();
		
	private:
		// last time a frame was drawn
		int m_LastFrame;
		
		// rate of fps to lock
		double m_FPSLock;
};

#endif
