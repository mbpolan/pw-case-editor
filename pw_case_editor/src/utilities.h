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

namespace Utils {

// general filesystem functions for cross-platform compatability
namespace FS {

// get the current working directory
Glib::ustring cwd();

// move a file on the filesystem
void move(const Glib::ustring &from, Glib::ustring &to);

// check if a directory exists
bool dir_exists(const Glib::ustring &path);

// remove a directory
void remove_dir(const Glib::ustring &path);

} // namespace FS

// flush gui events that may still be pending in the main loop
void flush_events();

// calculate center of screen coordinates for a window
void calculate_center(int width, int height, int &x, int &y);

// convert an I/O error code into a string error message
Glib::ustring io_error_to_str(const IO::Code &code);

// format an exception string
Glib::ustring exception_string(const Glib::ustring &reason, const Glib::ustring &file, int line);

// convert int to string
Glib::ustring to_string(int val);

// extract a text block's id from a full string
Glib::ustring extract_block_id(const Glib::ustring str);

// extract a text block's description from a full string
Glib::ustring extract_block_description(const Glib::ustring &str);

}; // namespace Utils

#endif
