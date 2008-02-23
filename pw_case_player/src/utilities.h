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

/// Namespace for all various functions that don't belong to any class
namespace Utils {

/// Error message types
enum MessageType { MESSAGE_CRITICAL=0, MESSAGE_WARNING };

/// Whether or not debugging is enabled in the player
extern bool g_DebugOn;

/// Namespace for general filesystem functions for cross-platform compatability
namespace FS {

/** Get the current working directory
  * \return The current working directory
*/
ustring cwd();

/** Move a file on the filesystem
  * \param from The original file path
  * \param to The destination path
*/
void move(const ustring &from, const ustring &to);

/** Check if a directory exists
  * \param path The path to the directory in question
  * \return <b>true</b> if it exists, <b>false</b> otherwise
*/
bool dirExists(const ustring &path);

/** Create a directory
  * \param path The path of the new directory
*/
void makeDir(const ustring &path);

/** Recursively remove a directory
  * \param path The directory's path
*/
void removeDir(const ustring &path);

} // namespace FS

/** Display an alert message to the user.
  * \note This will show a message box on Windows, and console output on Linux
  * \param text The text to display
  * \param type The type of message
*/
void alert(const ustring &text, const MessageType &type=MESSAGE_CRITICAL);

/** Convert a court camera script string to animation limits.
  * The provided string should be in the form of: "location1,location2" or the
  * returned limits won't be correct
  * \param str The string
  * \param start The returned beginning location
  * \param end The returned end location
*/
void scriptToLimits(const ustring &str, UI::Limit &start, UI::Limit &end);

/** Map a char representing a text color to the actual color
  * \param ch The color code character
  * \return Completed Color object based on provided color code
*/
Color keyToColor(char ch);

/** Print a debug message
  * \note: On Windows, this will display a message box
  * \param msg The string to display
*/
void debugMessage(const ustring &msg);

/** See if a point is in a rectangle
  * \param p The point to test
  * \param rect The rect to test against
  * \return <b>true</b> if the point is in range, <b>false</b> otherwise
*/
bool pointInRect(const Point &p, const Rect &rect);

/** Calculate a random offset point (used during shake animations)
  * \param highestRadius The largest radius to use in the calculation
  * \return The computed point
*/
Point calculateShakePoint(int highestRadius);

/** Get a random number in the provided range
  * \param min The lower value in the range
  * \param max The upper value in the range
  * \return A random number in this range
*/
int randomRange(int min, int max);

/** Get the location of the mouse pointer
  * \param modState Optional SDL mod button state
  * \return The current mouse location
*/
Point getMouseLocation(Uint8 *modState=NULL);

/** Create a blank surface
  * \param width The width
  * \param height The height
  * \return A new surface
*/
SDL_Surface* createSurface(int width, int height);

/** Convert a Unicode char to a string
  * \param ch The character
  * \return A string with this character
*/
ustring ucharToStr(uchar ch);

/** Convert an integer to string
  * \param num The integer value
  * \return A string with this value
*/
ustring itoa(int num);

/** Break a string apart based on a delimiting substring
  * \param delimiter The substring to use as a separator
  * \param str The string to break apart
  * \return A string vector with the pieces of the broken string
*/
StringVector explodeString(const ustring &delimiter, const ustring &str);

/** Break a string apart based on a delimiting character
  * \param delimiter The character to use as a separator
  * \param str The string to break apart
  * \return A string vector with the pieces of the broken string
*/
StringVector explodeString(uchar delimiter, const ustring &str);

/** Count occurrences of a substring in a string
  * \param sub The substring to test
  * \param str The string to test
  * \return The amount of times <i>sub</i> is found in <i>str</i>
*/
int amountInString(const ustring &sub, const ustring &str);

}; // namespace UI
