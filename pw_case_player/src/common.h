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
// common.h: various, shared structs

#ifndef COMMON_H
#define COMMON_H

#include <iostream>

// a value,key string pair
typedef std::pair<std::string, std::string> StringPair;

// a point
class Point {
	public:
		// constructor
		Point(int x=0, int y=0) {
			m_X=x;
			m_Y=y;
		}
		
		// set coordinates
		void setX(int x) { m_X=x; }
		void setY(int y) { m_Y=y; }
		
		// get coordinates
		int x() const { return m_X; }
		int y() const { return m_Y; }
		
	private:
		// x,y pair
		int m_X;
		int m_Y;
};

#endif
