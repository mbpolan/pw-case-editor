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

/// Typedef'd ustring for internationalization
typedef Glib::ustring ustring;

/// Typedef'd Unicode character
typedef gunichar uchar;

/// Typedef for a null string
const ustring STR_NULL="null";

/// A value,key string pair
typedef std::pair<ustring, ustring> StringPair;

/// Vector used for mapping a range of characters to a color
typedef std::vector<std::pair<ValueRange, Color> > ColorRangeVector;

//@{
/** Constants to simplify layering on Z axis.
  * Since everything in the game is basically layered, it's easier 
  * to define some global constants which will be used to draw
  * the elements accordingly. This way, if something needs to be
  * changed, there's no need to go hunting for the values in the
  * entire code. Also, floats are used since they're more versatile
  * than just regular ints, and they allow for miniscule elements to be
  * layered, such as in many Renderer namespace functions, where a rectangle
  * needs to be drawn over another.
*/
const float Z_FADE=		20.0f;
const float Z_ANIM_SPRITE=	14.5f;
const float Z_TEXT=		14.0f;
const float Z_ANSWER_BAR=	13.5f;
const float Z_TEXT_BOX_TAG=	13.0f;
const float Z_IFC_BTN=		13.0f;
const float Z_TEXT_BOX=		12.0f;
const float Z_SCREEN_BARS=	11.5f;
const float Z_SPRITE=		11.0f;
const float Z_BARS=		10.5f;
const float Z_SCANLINES=	10.0f;
const float Z_GUI_BTN=		5.0f;
const float Z_BTN_ARROWS=	6.0f;
//@}

// function prototypes
static void onSigSegv(int sig) { };

/** An RGBA color.
  * Each color can have four color attributes, including the optional
  * alpha value. However, only the first three are actually used by
  * the engine at this time, though that may change in the future
*/
class Color {
	public:
		/** Constructor
		  * \param r The red component
		  * \param g The green component
		  * \param b The blue component
		  * \param a The alpha component
		*/
		Color(char r=255, char g=255, char b=255, char a=255) {
			m_R=r;
			m_G=g;
			m_B=b;
			m_A=a;
		}
		
		/** Convert this color to an SDL_Color struct
		  * \return A completed SDL_Color struct
		*/
		SDL_Color toSDLColor() const {SDL_Color color={ m_R, m_G, m_B }; return color; }
		
		//@{
		/** Get a color component from this object */
		char r() const { return m_R; }
		char g() const { return m_G; }
		char b() const { return m_B; }
		char a() const { return m_A; }
		//@}
		
	private:
		char m_R, m_G, m_B, m_A;
};

/** A range consisting of two values
*/
class ValueRange {
	public:
		/** Constructor
		  * \param low The lesser value
		  * \param high The greater value
		*/
		ValueRange(int low, int high) {
			m_Low=low;
			m_High=high;
		}
		
		/** Compare this range to another
		  * \return <b>true</b> if this range is greater than the other, <b>false</b> otherwise
		*/
		bool operator<(ValueRange r) const {
			return (m_High!=r.getHighValue() || m_Low!=r.getLowValue());
		}
		
		/** Compute the difference between the two range values
		  * \return The difference between the values in the range
		*/
		int difference() const { return m_High-m_Low; }
		
		/** See if a value is in this range
		  * \param val The value to test
		  * \param inclusive Flag whether or not to include the range values themselves
		  * \return <b>true</b> if the value is in range, <b>false</b> otherwise
		*/
		bool inRange(int val, bool inclusive=true) const {
			if (inclusive)
				return (val<=m_High && val>=m_Low);
			else
				return (m_High>val && val<m_Low);
		}
		
		/** Get the lower value
		  * \return The range's lower value
		*/
		int getLowValue() const { return m_Low; }
		
		/** Get the higher value
		  * \return The range's upper value
		*/
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

/** A point in 3D space.
  * Since many functions in the player require a coordinate, 
  * this class simplifies passing the different parameters as one,
  * unified class object
*/
class Point {
	public:
		/** Constructor
		  * \param x The x-coordinate
		  * \param y The y-coordinate
		  * \param z The z-coordinate
		*/
		Point(int x=0, int y=0, float z=0.0f) {
			m_X=x;
			m_Y=y;
			m_Z=z;
		}
		
		/** Add two points together
		  * \param p The second point to add
		  * \return New Point representing the sum of the two
		*/
		Point operator+(const Point &p) {
			return Point(m_X+p.x(), m_Y+p.y(), m_Z+p.z());
		}
		
		/// Reverse the signs on the coordinates
		void invert() {
			m_X=-m_X;
			m_Y=-m_Y;
			m_Z=-m_Z;
		}
		
		/** Set the x-coordinate in this point
		  * \param x The coordinate
		*/
		void setX(int x) { m_X=x; }
		
		/** Set the y-coordinate in this point
		  * \param y The coordinate
		*/
		void setY(int y) { m_Y=y; }
		
		/** Set the z-coordinate in this point
		  * \param z The coordinate
		*/
		void setZ(float z) { m_Z=z; }
		
		//@{
		/** Get a coordinate in this point */
		int x() const { return m_X; }
		int y() const { return m_Y; }
		float z() const { return m_Z; }
		//@}
		
	private:
		// coordinates
		int m_X;
		int m_Y;
		float m_Z;
};

// overloaded stream operator for point value output
static std::ostream& operator<<(std::ostream &s, const Point &p) {
	s << "(" << p.x() << "," << p.y() << "," << p.z() << ")\n";
	return s;
}

/** A rectangle in 2D space.
  * The Rectangle class basically consists of a Point object, which
  * represents the top-left corner of the rectangle, along with width
  * and height members that define the dimensions of the rectangle
*/
class Rect {
	public:
		/// Default constructor
		Rect() {
			m_Corner=Point(0, 0);
			m_Width=m_Height=0;
		}
		
		/** Constructor taking parameters
		  * \param p The top-left corner
		  * \param w The width
		  * \param h The height
		*/
		Rect(const Point &p, int w, int h) {
			m_Corner=p;
			m_Width=w;
			m_Height=h;
		}
		
		/** Get the point representing the top left corner
		  * \return The top-left corner
		*/
		Point getPoint() const { return m_Corner; }
		
		//@{
		/** Get the dimensions of the rectangle */
		int getWidth() const { return m_Width; }
		int getHeight() const { return m_Height; }
		//@}
		
		/** Get the geometry of the rectangle
		  * \param p The top-left corner point
		  * \param w The width
		  * \param h The height
		*/
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
