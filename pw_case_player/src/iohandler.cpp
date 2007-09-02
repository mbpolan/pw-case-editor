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
// iohandler.cpp: implementation of I/O functions

#include <cstdio>
#include <iostream>
#include "SDL_rotozoom.h"

#include "iohandler.h"

// load a case from file
bool IO::loadCaseFromFile(const std::string &path, Case::Case &pcase) {
	// open requested file
	FILE *f=fopen(path.c_str(), "rb");
	if (!f)
		return false;
	
	// read magic number and verify it
	std::string magicNum=readString(f);
	if (magicNum!=FILE_MAGIC_NUM) {
		printf("IO: Incorrect magic number. Expected '%s', read '%s'.", FILE_MAGIC_NUM.c_str(), magicNum.c_str());
		return false;
	}
	
	// read file version and verify it
	int version;
	fread(&version, sizeof(int), 1, f);
	if (version!=FILE_VERSION)
		return false;
	
	// create a new overview struct
	Case::Overview overview;
	
	// read in data
	overview.name=readString(f);
	overview.author=readString(f);
	fread(&overview.lawSys, sizeof(int), 1, f);
	
	// set the overview
	pcase.setOverview(overview);
	
	// read initial block id
	std::string initialBlock=readString(f);
	pcase.setInitialBlockId(initialBlock);
	
	// read amount of characters
	int charCount;
	fread(&charCount, sizeof(int), 1, f);
	
	// read each character
	for (int i=0; i<charCount; i++) {
		Character character;
		std::string str;
		
		// read internal name
		str=readString(f);
		character.setInternalName(str);
		
		// read displayed name
		str=readString(f);
		character.setName(str);
		
		// read description
		str=readString(f);
		character.setDescription(str);
		
		// read sprite name
		str=readString(f);
		character.setSpriteName(str);
		
		// see if this character has a text box tag
		bool tag;
		fread(&tag, sizeof(bool), 1, f);
		character.setHasTextBoxTag(tag);
		
		// if the tag exists, read the image
		if (tag) {
			SDL_Surface *texTag=Textures::createTexture("null", readImage(f));
			
			// set alpha to match that of the text box
			SDL_SetAlpha(texTag, SDL_SRCALPHA, 165);
			
			character.setTextBoxTag(texTag);
		}
		
		// see if this character has a headshot image
		bool headshot;
		fread(&headshot, sizeof(bool), 1, f);
		character.setHasHeadshot(headshot);
		
		// if the headshot exists, read the image
		if (headshot) {
			// read full image
			SDL_Surface *headshot=Textures::createTexture("null", readImage(f));
			
			// read scaled thumbnail
			SDL_Surface *thumb=Textures::createTexture("null", readImage(f));
			
			character.setHeadshot(headshot, thumb);
		}
		
		// include this character
		pcase.addCharacter(character);
	}
	
	// read amount of backgrounds
	int bgCount;
	fread(&bgCount, sizeof(int), 1, f);
	
	// iterate over backgrounds
	for (int i=0; i<bgCount; i++) {
		Case::Background bg;
		
		// read id
		bg.id=readString(f);
		
		// read type
		int bgType;
		fread(&bgType, sizeof(int), 1, f);
		bg.type=(bgType==0 ? Case::BG_SINGLE_SCREEN : Case::BG_DOUBLE_SCREEN);
		
		// read pixbuf data
		bg.texture=Textures::createTexture(bg.id, readImage(f));
		
		// add this background
		pcase.addBackground(bg);
	}
	
	// read amount of evidence
	int evidenceCount;
	fread(&evidenceCount, sizeof(int), 1, f);
	
	// iterate over evidence
	for (int i=0; i<evidenceCount; i++) {
		Case::Evidence evidence;
	
		// read id
		evidence.id=readString(f);
		
		// read name
		evidence.name=readString(f);
		
		// read caption
		evidence.caption=readString(f);
		
		// read description
		evidence.description=readString(f);
		
		// read pixbuf data
		evidence.texture=Textures::createTexture("null", readImage(f));
		
		// read thumbnail data
		evidence.thumb=Textures::createTexture("null", readImage(f));
		
		// add this evidence
		pcase.addEvidence(evidence);
	}
	
	// read amount of locations
	int locationCount;
	fread(&locationCount, sizeof(int), 1, f);
	
	// iterate over locations
	for (int i=0; i<locationCount; i++) {
		Case::Location location;
		location.triggerBlock="null";
		
		// read id
		location.id=readString(f);
		
		// read name
		location.name=readString(f);
		
		// read bg id
		location.bg=readString(f);
		
		// get the background image in question
		if (pcase.getBackground(location.bg)) {
			Case::Background *bg=pcase.getBackground(location.bg);
			
			// scale it
			location.bgScaled=zoomSurface(bg->texture, 0.3125, 0.3125, SMOOTHING_ON);
			Textures::pushTexture("null", location.bgScaled);
		}
		
		// read amount of hotspots
		int hcount;
		fread(&hcount, sizeof(int), 1, f);
		
		// iterate over hotspots
		for (int i=0; i<hcount; i++) {
			Case::Hotspot hspot;
			
			// read area
			fread(&hspot.x, sizeof(int), 1, f);
			fread(&hspot.y, sizeof(int), 1, f);
			fread(&hspot.w, sizeof(int), 1, f);
			fread(&hspot.h, sizeof(int), 1, f);
			
			// read target block
			hspot.block=readString(f);
			
			// add this hotspot
			location.hotspots.push_back(hspot);
		}
		
		// add this location
		pcase.addLocation(location);
	}
	
	// read amount of text blocks
	int bufferCount;
	fread(&bufferCount, sizeof(int), 1, f);
	
	// iterate over text blocks
	for (int i=0; i<bufferCount; i++) {
		// read id
		std::string bufferId=readString(f);
		
		// read text contents
		std::string contents=readString(f);
		
		// append this text buffer to the map
		pcase.addBuffer(bufferId, contents);
	}
	
	// wrap up
	fclose(f);
	std::cout << "Done loading case\n";
	return true;
}

// read image data from file
Textures::Texture IO::readImage(FILE *f) {
	Textures::Texture tex;
	
	// read buffer size
	unsigned int bsize;
	fread(&bsize, sizeof(unsigned int), 1, f);
	
	// read buffer
	char *buffer=new char[bsize];
	fread(buffer, sizeof(char), bsize, f);
	char *pbuf=buffer;
	
	// skip to width, height
	pbuf+=18;
	
	// read width
	tex.w=*(int*) pbuf;
	pbuf+=sizeof(int);
	
	// read height
	tex.h=*(int*) pbuf;
	pbuf+=sizeof(int);
	
	// skip to bpp
	pbuf+=sizeof(short);
	
	// read bpp
	tex.bpp=*(short*) pbuf;
	pbuf+=sizeof(short);
	
	// skip to pixels
	pbuf+=24;
	
	// calculate remaining size of buffer
	int size=bsize-54;
	
	// allocate buffer
	tex.pixels=new char[size];
	
	// copy pixels to new buffer
	memcpy(tex.pixels, pbuf, size);
	
	// delete the buffer
	delete [] buffer;
	
	return tex;
}

// read a string from file
std::string IO::readString(FILE *f) {
	// read string length
	int len;
	fread(&len, sizeof(int), 1, f);
	
	// read the string
	std::string str="";
	for (int i=0; i<len; i++)
		str+=(char) fgetc(f);
	
	return str;
}
