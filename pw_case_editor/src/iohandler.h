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
#include <gdkmm/pixbuf.h>
#include <glibmm/ustring.h>
#include <gtkmm/textbuffer.h>
#include <map>

#include "case.h"
#include "config.h"
#include "iconmanager.h"
#include "sprite.h"

/// Namespace for all file loading/saving functions
namespace IO {

/// Various error codes that are used to identify if something went wrong
enum Code { CODE_OK=		 	 1,
	    CODE_OPEN_FAILED=	 	 0,
	    CODE_WRONG_MAGIC_NUM=	-1,
	    CODE_WRONG_VERSION=		-2,
	    CODE_VALIDATE_FAILED=	-3
};

/** The header for the PWT file format.
  * This is the header that is saved with each exported case file
*/
struct _PWTHeader {
	int ident; // magic number
	int version; // file version
	
	// byte offsets in data (relative to start of file)
	int overviewOffset;
	int overridesOffset;
	int charOffset;
	int imgOffset;
	int bgOffset;
	int evidenceOffset;
	int locationOffset;
	int audioOffset;
	int testimonyOffset;
	int blockOffset;
};
typedef struct _PWTHeader PWTHeader;

/// Magic number for PWT file format
const int FILE_MAGIC_NUM=(('T' << 16) + ('W' << 8) + 'P');

/// Supported version for PWT file
const int FILE_VERSION=10;

/// Magic number for SPR file format
const Glib::ustring SPR_MAGIC_NUM="SPR";

/// Supported SPR file version
const int SPR_VERSION=10;

/// The expected size in bytes of the resource file
const long RESOURCE_FILE_SIZE=192434;

/** Save a case and its associated data to file
  * \param path The path to save to
  * \param pcase The case to save
  * \param buffers The buffers in this case
  * \param bufferDescriptions Descriptions of buffers
  * \return IO::CODE_OK if successful, other codes if an error occurred.
*/
IO::Code save_case_to_file(const Glib::ustring &path, const Case::Case &pcase,
		       const BufferMap &buffers,
		       std::map<Glib::ustring, Glib::ustring> &bufferDescriptions);

/** Export a case to file
  * \param path The path to export to
  * \param pcase The case to export
  * \param buffers The buffers in this case
  * \return IO::CODE_OK if successful, other codes if an error occurred.
*/
IO::Code export_case_to_file(const Glib::ustring &path, const Case::Case &pcase, const BufferMap &buffers);

/** Load a case from file
  * \param path The path to the case file to load
  * \param pcase Empty Case object to populate
  * \param buffers Empty buffer map to populate
  * \param bufferDescriptions Empty map of descriptions to populate
  * \return IO::CODE_OK if successful, other codes if an error occurred.
*/
IO::Code load_case_from_file(const Glib::ustring &path, Case::Case &pcase,
			 BufferMap &buffers,
		         std::map<Glib::ustring, Glib::ustring> &bufferDescriptions);

/** Save a sprite to file
  * \param path The path to save to
  * \param spr The Sprite object to save
  * \return IO::CODE_OK if successful, other codes if an error occurred.
*/
IO::Code save_sprite_to_file(const Glib::ustring &path, const Sprite &spr);

/** Export a sprite to file
  * \param path The path to export to
  * \param spr The Sprite to export
  * \return IO::CODE_OK if successful, other codes if an error occurred.
*/
IO::Code export_sprite_to_file(const Glib::ustring &path, const Sprite &spr);

/** Load a sprite from file
  * \param path The path to the sprite to load
  * \param spr Empty Sprite object to populate
  * \return IO::CODE_OK if successful, other codes if an error occurred.
*/
IO::Code load_sprite_from_file(const Glib::ustring &path, Sprite &spr);

/** Load a translation file
  * \param path The path to the file
  * \param map The translation string map to load data into
  * \return IO::CODE_OK if successful, other codes if an error occurred
*/
IO::Code load_translation_file(const Glib::ustring &path, std::map<Glib::ustring, Glib::ustring> &map);

/** Save the configuration file
  * \param path The path to the file
  * \param config A completed Config::File struct representing values to save
  * \return IO::CODE_OK if successful, other codes if an error occurred
*/
IO::Code save_config_file(const Glib::ustring &path, const Config::File &file);

/** Load the configuration file
  * \param path The path to the file
  * \param file The Config::File struct to load data into
  * \return IO::CODE_OK if successful, other codes if an error occurred
*/
IO::Code load_config_file(const Glib::ustring &path, Config::File &file);

/** Write a string to file
  * \param f Pointer to open FILE handle
  * \param str The string to write
*/
void write_string(FILE *f, const Glib::ustring &str);

/** Read a string from file
  * \param f Pointer to open FILE handle
  * \return The resulting string
*/
Glib::ustring read_string(FILE *f);

/** Write a pixbuf to compressed, internal format
  * \param f Pointer to open FILE handle
  * \param pixbuf The actual image data to write
*/
void write_export_image(FILE *f, const Glib::RefPtr<Gdk::Pixbuf> &pixbuf);

/** Write a pixbuf in uncompressed format to file
  * \param f Pointer to open FILE handle
  * \param pixbuf The image data to write
*/
void write_pixbuf(FILE *f, const Glib::RefPtr<Gdk::Pixbuf> &pixbuf);

/** Read an uncompressed pixbuf from file
  * \param f Pointer to open FILE handle
  * \return The resulting image
*/
Glib::RefPtr<Gdk::Pixbuf> read_pixbuf(FILE *f);

/** Add a file to the recent files record
  * \param uri The path to the recent file
  * \param display The display string
*/
void add_recent_file(const Glib::ustring &uri, const Glib::ustring &display);

/** Read the recent files record
  * \param vec Empty vector to populate with files
  * \return IO::CODE_OK if successful, other codes if an error occurred.
*/
IO::Code read_recent_files(std::vector<StringPair> &vec);

/** Unpack the resource file
  * \param file The path to the resource file
  * \return IO::CODE_OK if successful, other codes if an error occurred.
*/
IO::Code unpack_resource_file(const Glib::ustring &file);

/** Read icons from a theme file
  * \param icons Empty map of icons to populate
  * \return IO::CODE_OK if successful, other codes if an error occurred.
*/
IO::Code read_icons_from_file(IconMap &icons);

}; // namespace IO

#endif
