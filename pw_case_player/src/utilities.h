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
// utilities.h: various extra functions

#include <iostream>

#include "case.h"
#include "uimanager.h"

// utilities namespace
namespace Utils {

// whether or not debugging is on
extern bool g_DebugOn;

// convert a court camera script string to animation limits
// string should be in the form of: "location1,location2"
void scriptToLimits(const std::string &str, UI::Limit &start, UI::Limit &end);

// print a debug message
void debugMessage(const std::string &object, const std::string &msg);

// see if a point is in a rectangle
bool pointInRect(const Point &p, const Rect &rect);

// get a random number in the provided range
int randomRange(int min, int max);

// get the location of the mouse pointer
Point getMouseLocation(Uint8 *modState=NULL);

// create a blank surface
SDL_Surface* createSurface(int width, int height);

// convert a char to a string
std::string ucharToStr(char ch);

// break a string apart based on a delimiting string
StringVector explodeString(const std::string &delimiter, const std::string &str);

// break a string apart based on a delimiting character
StringVector explodeString(char delimiter, const std::string &str);

// count occurrences of a substring in a string
int amountInString(const std::string &sub, const std::string &str);

}; // namespace UI
