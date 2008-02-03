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

// message types
enum MessageType { MESSAGE_CRITICAL=0, MESSAGE_WARNING };

// whether or not debugging is on
extern bool g_DebugOn;

// general filesystem functions for cross-platform compatability
namespace FS {

// get the current working directory
ustring cwd();

// move a file on the filesystem
void move(const ustring &from, const ustring &to);

// check if a directory exists
bool dirExists(const ustring &path);

// create a directory
void makeDir(const ustring &path);

// remove a directory
void removeDir(const ustring &path);

} // namespace FS

// display an alert message to the user
// this will show a message box on Windows
void alert(const ustring &text, const MessageType &type=MESSAGE_CRITICAL);

// convert a court camera script string to animation limits
// string should be in the form of: "location1,location2"
void scriptToLimits(const ustring &str, UI::Limit &start, UI::Limit &end);

// print a debug message
void debugMessage(const ustring &msg);

// see if a point is in a rectangle
bool pointInRect(const Point &p, const Rect &rect);

// calculate a random offset point (used during shake animations)
Point calculateShakePoint(int highestRadius);

// get a random number in the provided range
int randomRange(int min, int max);

// get the location of the mouse pointer
Point getMouseLocation(Uint8 *modState=NULL);

// create a blank surface
SDL_Surface* createSurface(int width, int height);

// convert a uchar to a string
ustring ucharToStr(uchar ch);

// convert an integer to string
ustring itoa(int num);

// break a string apart based on a delimiting string
StringVector explodeString(const ustring &delimiter, const ustring &str);

// break a string apart based on a delimiting character
StringVector explodeString(uchar delimiter, const ustring &str);

// count occurrences of a substring in a string
int amountInString(const ustring &sub, const ustring &str);

}; // namespace UI
