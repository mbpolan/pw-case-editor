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
#include <map>

#include "case.h"
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
const QString SPR_MAGIC_NUM="SPR";

/// Supported SPR file version
const int SPR_VERSION=10;

/** Save a case and its associated data to file
  * \param path The path to save to
  * \param pcase The case to save
  * \param buffers The buffers in this case
  * \param bufferDescriptions Descriptions of buffers
  * \return IO::CODE_OK if successful, other codes if an error occurred.
*/
IO::Code saveCaseToFile(const QString &path, const Case::Case &pcase, const BufferMap &buffers,
		       std::map<QString, QString> &bufferDescriptions);

/** Export a case to file
  * \param path The path to export to
  * \param pcase The case to export
  * \param buffers The buffers in this case
  * \return IO::CODE_OK if successful, other codes if an error occurred.
*/
IO::Code exportCaseToFile(const QString &path, const Case::Case &pcase, const BufferMap &buffers);

/** Load a case from file
  * \param path The path to the case file to load
  * \param pcase Empty Case object to populate
  * \param buffers Empty buffer map to populate
  * \param bufferDescriptions Empty map of descriptions to populate
  * \return IO::CODE_OK if successful, other codes if an error occurred.
*/
IO::Code loadCaseFromFile(const QString &path, Case::Case &pcase, BufferMap &buffers,
		         std::map<QString, QString> &bufferDescriptions);

/** Save a sprite to file
  * \param path The path to save to
  * \param spr The Sprite object to save
  * \return IO::CODE_OK if successful, other codes if an error occurred.
*/
IO::Code saveSpriteToFile(const QString &path, const Sprite &spr);

/** Export a sprite to file
  * \param path The path to export to
  * \param spr The Sprite to export
  * \return IO::CODE_OK if successful, other codes if an error occurred.
*/
IO::Code exportSpriteToFile(const QString &path, const Sprite &spr);

/** Load a sprite from file
  * \param path The path to the sprite to load
  * \param spr Empty Sprite object to populate
  * \return IO::CODE_OK if successful, other codes if an error occurred.
*/
IO::Code loadSpriteFromFile(const QString &path, Sprite &spr);

/** Load default blocks for the case
  * \param lang The language to load
  * \return IO::CODE_OK if successful, other codes if an error occurred.
*/
IO::Code loadDefaultBlocks(const QString &lang);

/** Write a string to file
  * \param f Pointer to open FILE handle
  * \param str The string to write
*/
void writeString(FILE *f, const QString &str);

/** Read a string from file
  * \param f Pointer to open FILE handle
  * \return The resulting string
*/
QString readString(FILE *f);

/** Write a pixbuf to file
  * \param f Pointer to open FILE handle
  * \param pixbuf The actual image data to write
*/
void writeImage(FILE *f, const QPixmap &pixbuf);

/** Read pixbuf data from file
  * \param f Pointer to open FILE handle
*/
QPixmap readImage(FILE *f);

}; // namespace IO

#endif
