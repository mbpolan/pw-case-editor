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
// iohandler.h: I/O functionality

#ifndef IOHANDLER_H
#define IOHANDLER_H

#include <cstdio>
#include <glibmm/ustring.h>
#include <gtkmm/textbuffer.h>
#include <map>

#include "case.h"
#include "sprite.h"

namespace IO {

// case file information
const Glib::ustring FILE_MAGIC_NUM="PWT";
const int FILE_VERSION=10;

// sprite file information
const Glib::ustring SPR_MAGIC_NUM="SPR";
const int SPR_VERSION=10;

// save a case and its associated data to file
bool save_case_to_file(const Glib::ustring &path, const Case::Case &pcase,
		       const BufferMap &buffers,
		       std::map<Glib::ustring, Glib::ustring> &bufferDescriptions);

// export a case to file
bool export_case_to_file(const Glib::ustring &path, const Case::Case &pcase, const BufferMap &buffers);

// load a case from file
bool load_case_from_file(const Glib::ustring &path, Case::Case &pcase,
			 BufferMap &buffers,
		         std::map<Glib::ustring, Glib::ustring> &bufferDescriptions);

// save a sprite to file
bool save_sprite_to_file(const Glib::ustring &path, const Sprite &spr);

// export a sprite to file
bool export_sprite_to_file(const Glib::ustring &path, const Sprite &spr);

// load a sprite from file
bool load_sprite_from_file(const Glib::ustring &path, Sprite &spr);

// write a string to file
void write_string(FILE *f, const Glib::ustring &str);

// read a string from file
Glib::ustring read_string(FILE *f);

// write a pixbuf to compressed, internal format
void write_export_image(FILE *f, const Glib::RefPtr<Gdk::Pixbuf> &pixbuf);

// write a pixbuf to file
void write_pixbuf(FILE *f, const Glib::RefPtr<Gdk::Pixbuf> &pixbuf);

// read a pixbuf from file
Glib::RefPtr<Gdk::Pixbuf> read_pixbuf(FILE *f);

}; // namespace IO

#endif
