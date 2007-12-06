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

namespace Utils {

// flush gui events that may still be pending in the main loop
void flush_events();

// convert int to string
Glib::ustring to_string(int val);

// compress a buffer
char* compress_buffer(const char *buffer, int size, int &newSize, bool autoFree);

// extract a text block's id from a full string
Glib::ustring extract_block_id(const Glib::ustring str);

// extract a text block's description from a full string
Glib::ustring extract_block_description(const Glib::ustring &str);

}; // namespace Utils

#endif
