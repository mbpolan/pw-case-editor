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
// utilities.h: various utility functions

#ifndef UTILITIES_H
#define UTILITIES_H

#include <glibmm/ustring.h>

#include "iohandler.h"

/// Namespace for all various functions that don't belong to any class
namespace Utils {

/// Namespace for general filesystem functions for cross-platform compatability
namespace FS {

/** Get the current working directory
  * \return The current working directory
*/
Glib::ustring cwd();

/** Move a file on the filesystem
  * \param from The original file path
  * \param to The destination path
*/
void move(const Glib::ustring &from, Glib::ustring &to);

/** Check if a directory exists
  * \param path The path to the directory in question
  * \return <b>true</b> if it exists, <b>false</b> otherwise
*/
bool dir_exists(const Glib::ustring &path);

/** Create a directory
  * \param path The path of the new directory
*/
void make_dir(const Glib::ustring &path);

/** Recursively remove a directory
  * \param path The directory's path
*/
void remove_dir(const Glib::ustring &path);

} // namespace FS

/// Flush GUI events that may still be pending in the main loop
void flush_events();

/** Calculate center of screen coordinates for a window
  * \param width The width of the window
  * \param height The height of the window
  * \param x The calculated x-coordinate
  * \param y The calculated y-coordinate
*/
void calculate_center(int width, int height, int &x, int &y);

/** Map an I/O error code into a string error message
  * \param code the IO::Code to translate
  * \return The string error message
*/
Glib::ustring io_error_to_str(const IO::Code &code);

/** Format an exception string for a file and line
  * \param reason The exception's reason for being raised
  * \param file The file where this exception originated from
  * \param line The line where this exception occurred
  * \return The formatted string to display
*/
Glib::ustring exception_string(const Glib::ustring &reason, const Glib::ustring &file, int line);

/** Convert an int to string
  * \param val The int value
  * \return String containing the value
*/
Glib::ustring to_string(int val);

/** Extract a text block's ID from a full string
  * \param str The full string
  * \return The extracted ID
*/
Glib::ustring extract_block_id(const Glib::ustring str);

/** Extract a text block's description from a full string
  * \param str The full string
  * \return The extracted description
*/
Glib::ustring extract_block_description(const Glib::ustring &str);

}; // namespace Utils

#endif
