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
// block_extract.cpp: block extraction tool

#include <iostream>

#include "common.h"

int main(int argc, char *argv[]) {
	// check the amount of arguments
	if (argc!=3) {
		std::cout << "Phoenix Wright Case Editor Tools\n";
		std::cout << "Block Extractor by Mike Polan (kanadakid@gmail.com)\n\n";
		std::cout << "Usage: block_extract <CASE_FILE> <OUTPUT_DIRECTORY>\n";
		return 0;
	}
	
	// format the root path
	std::string root=argv[2];
	if (root[root.size()-1]!='/')
		root+='/';
	
	// open the case file first, and check to see if it was opened successfully
	FILE *f=fopen(argv[1], "rb");
	if (!f) {
		std::cout << "Unable to open case file: '" << argv[1] << "'.\n";
		return 0;
	}
	
	// read the header
	PWTHeader header;
	fread(&header, sizeof(PWTHeader), 1, f);
	
	// compare magic number
	if (header.ident!=MAGIC_NUM) {
		std::cout << "This is not a valid case file. Expected magic number '" << MAGIC_NUM << "', read '" << header.ident << "'.\n";
		fclose(f);
		return 0;
	}
	
	// check version
	if (header.version!=VERSION) {
		std::cout << "Unsupported file version. Expected '" << VERSION << "', read '" << header.version << "'.\n";
		fclose(f);
		return 0;
	}
	
	// seek to the block offset
	fseek(f, header.blockOffset, SEEK_SET);
	
	// read amount of blocks
	int amount;
	fread(&amount, sizeof(int), 1, f);
	
	// read in each block
	for (int i=0; i<amount; i++) {
		// read id
		std::string id=readString(f);
		
		// read text
		std::string txt=readString(f);
		
		// now create a new file with this id in the provided directory
		std::string path=root;
		path+=id;
		path+=".txt";
		
		// open a new file in the directory
		FILE *block=fopen(path.c_str(), "w");
		if (!block) {
			std::cout << "Warning: couldn't open file '" << path << "' to dump block text.\n";
			continue;
		}
		
		// and dump the text
		fputs(txt.c_str(), block);
		fclose(block);
	}
	
	std::cout << "Extracted " << amount << " block(s) from case file.\n";
	
	// we're done
	fclose(f);
	return 0;
}

