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

#include <glibmm/ustring.h>
#include <iostream>
#include <signal.h>
#include <vector>
#include "SDL.h"

class Color;
class ValueRange;

// ustring for internationalization
typedef Glib::ustring ustring;
typedef gunichar uchar;

// null strings
const ustring STR_NULL="null";

// a value,key string pair
typedef std::pair<ustring, ustring> StringPair;

// vector used for mapping a range of characters to a color
typedef std::vector<std::pair<ValueRange, Color> > ColorRangeVector;

// function prototypes
static void onSigSegv(int sig) { };

// an rgba color
class Color {
	public:
		// constructor
		Color(char r=255, char g=255, char b=255, char a=255) {
			m_R=r;
			m_G=g;
			m_B=b;
			m_A=a;
		}
		
		// convert this color to an SDL color
		SDL_Color toSDLColor() const {SDL_Color color={ m_R, m_G, m_B }; return color; }
		
		// accessors
		char r() const { return m_R; }
		char g() const { return m_G; }
		char b() const { return m_B; }
		char a() const { return m_A; }
		
	private:
		char m_R, m_G, m_B, m_A;
};

// a range between two values
class ValueRange {
	public:
		// constructor
		ValueRange(int low, int high) {
			m_Low=low;
			m_High=high;
		}
		
		// compare this range to another
		bool operator<(ValueRange r) const {
			return (m_High!=r.getHighValue() || m_Low!=r.getLowValue());
		}
		
		// compute the difference between the two values
		int difference() const { return m_High-m_Low; }
		
		// see if a value is in this range
		bool inRange(int val, bool inclusive=true) const {
			if (inclusive)
				return (val<=m_High && val>=m_Low);
			else
				return (m_High>val && val<m_Low);
		}
		
		// accessors
		int getLowValue() const { return m_Low; }
		int getHighValue() const { return m_High; }
		
	private:
		int m_Low;
		int m_High;
};


// overloaded stream operator for range value output
static std::ostream& operator<<(std::ostream &s, const ValueRange &range) {
	s << range.getLowValue() << " to " << range.getHighValue() << std::endl;
	return s;
}

// a point
class Point {
	public:
		// constructor
		Point(int x=0, int y=0) {
			m_X=x;
			m_Y=y;
		}
		
		// add two points together
		Point operator+(const Point &p) {
			return Point(m_X+p.x(), m_Y+p.y());
		}
		
		// reverse the signs on the coordinates
		void invert() {
			m_X=-m_X;
			m_Y=-m_Y;
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

// overloaded stream operator for point value output
static std::ostream& operator<<(std::ostream &s, const Point &p) {
	s << "(" << p.x() << "," << p.y() << ")\n";
	return s;
}

// a rectangle
class Rect {
	public:
		// default constructor
		Rect() {
			m_Corner=Point(0, 0);
			m_Width=m_Height=0;
		}
		
		// constructor
		Rect(const Point &p, int w, int h) {
			m_Corner=p;
			m_Width=w;
			m_Height=h;
		}
		
		// get the top left corner
		Point getPoint() const { return m_Corner; }
		
		// get the dimensions
		int getWidth() const { return m_Width; }
		int getHeight() const { return m_Height; }
		
		// get the geometry of the rectangle
		void getGeometry(Point &p, int &w, int &h) const {
			p=m_Corner;
			w=m_Width;
			h=m_Height;
		}
		
	private:
		// top left corner
		Point m_Corner;
		
		// dimensions
		int m_Width;
		int m_Height;
};

// overloaded stream operator for rectangle value output
static std::ostream& operator<<(std::ostream &s, const Rect &rect) {
	Point p;
	int w, h;
	rect.getGeometry(p, w, h);
	
	s << "(" << p.x() << "," << p.y() << "), " << w << "x" << h << std::endl;
	return s;
}

#endif
