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
// common.h: common definitions for all tools

#ifndef COMMON_H
#define COMMON_H

#include <iostream>

// file information
const int MAGIC_NUM=(('T' << 16) + ('W' << 8) + 'P');
const int VERSION=10;

// the pwt file header
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

// read a string from file
static std::string readString(FILE *f) {
	// read string length
	int len;
	fread(&len, sizeof(int), 1, f);
	
	// read the string
	std::string str="";
	for (int i=0; i<len; i++)
		str+=(char) fgetc(f);
	
	return str;
}

#endif

