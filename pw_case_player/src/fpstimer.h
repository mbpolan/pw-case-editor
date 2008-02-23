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

/** Timer class used for game engine FPS consistency purposes.
  * Since not all computers run graphics games at equal speeds, this
  * class is responsible for keeping the frame rate at a reasonable level
*/
class FPSTimer {
	public:
		/// Constructor
		FPSTimer();
		
		/** Set the fps to lock the frame rate to; used in delay()
		  * \param fps The frame rate
		*/
		void setFPSLock(double fps) { m_FPSLock=fps; }
		
		/** Set the time that the last frame was drawn
		  * \param ticks Last frame's drawing time
		*/
		void setLastFrame(int ticks) { m_LastFrame=ticks; }
		
		/** Calculate delta in seconds between two sets of ticks
		  * \param ticks1 The first tick count
		  * \param ticks2 The second tick count
		  * \return The difference between the two ticks, in seconds
		*/
		double deltaSeconds(int ticks1, int ticks2);
		
		/// Delay the engine until the locked FPS rate is reached
		void delay();
		
	private:
		/// The last time a frame was drawn
		int m_LastFrame;
		
		/// Rate of FPS to lock
		double m_FPSLock;
};

#endif
