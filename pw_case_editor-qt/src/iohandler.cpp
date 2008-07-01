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
#include <dirent.h>
#include <QBuffer>

#include "clistview.h"
#include "iohandler.h"

// save a case and its associated data to file
IO::Code IO::saveCaseToFile(const QString &path, const Case::Case &pcase, const BufferMap &buffers,
			          std::map<QString, QString> &bufferDescriptions) {
	// open the requested file
	FILE *f=fopen(path.toStdString().c_str(), "wb");
	if (!f)
		return IO::CODE_OPEN_FAILED;
	
	// write the header
	fputc('C', f);
	fputc('P', f);
	fputc('R', f);
	fputc('J', f);
	fputc('T', f);
	
	fwrite(&FILE_VERSION, sizeof(int), 1, f);
	
	// get case overview
	Case::Overview overview=pcase.getOverview();
	
	// write overview details
	writeString(f, overview.name);
	writeString(f, overview.author);
	fwrite(&overview.days, sizeof(int), 1, f);
	
	// iterate over core blocks and write them
	for (int i=0; i<Case::Case::CORE_BLOCK_COUNT; i++)
		writeString(f, pcase.getCoreBlock(i));
	
	// get overrides
	Case::Overrides ov=pcase.getOverrides();
	
	// write override details
	fwrite(&ov.textboxAlpha, sizeof(int), 1, f);
	writeString(f, ov.titleScreen);
	
	// write initial block id
	writeString(f, pcase.getInitialBlockID());
	
	// get character data and write the amount of objects
	std::map<QString, Character> characters=pcase.getCharacters();
	int charCount=characters.size();
	fwrite(&charCount, sizeof(int), 1, f);
	
	// iterate over character data
	for (CharacterMap::iterator it=characters.begin(); it!=characters.end(); ++it) {
		// write internal name
		writeString(f, (*it).second.getInternalName());
		
		// write displayed name
		writeString(f, (*it).second.getName());
		
		// write gender
		int gender=(*it).second.getGender();
		fwrite(&gender, sizeof(int), 1, f);
		
		// write caption
		writeString(f, (*it).second.getCaption());
		
		// write description
		writeString(f, (*it).second.getDescription());
		
		// write sprite name
		writeString(f, (*it).second.getSpriteName());
		
		// write text box tag existance
		bool hasTag=(*it).second.hasTextBoxTag();
		fwrite(&hasTag, sizeof(bool), 1, f);
		
		// write text box tag
		if (hasTag)
			writeImage(f, (*it).second.getTextBoxTag());
		
		// write headshot existance
		bool hasHeadshot=(*it).second.hasHeadshot();
		fwrite(&hasHeadshot, sizeof(bool), 1, f);
		
		// write headshot
		if (hasHeadshot)
			writeImage(f, (*it).second.getHeadshot());
	}
	
	// get background map and write the amount of objects
	BackgroundMap backgrounds=pcase.getBackgrounds();
	int bgCount=backgrounds.size();
	fwrite(&bgCount, sizeof(int), 1, f);
	
	// iterate over backgrounds
	for (BackgroundMap::iterator it=backgrounds.begin(); it!=backgrounds.end(); ++it) {
		// write id
		writeString(f, (*it).second.id);
		
		// write type
		fwrite(&(*it).second.type, sizeof(int), 1, f);
		
		// write pixbuf data
		writeImage(f, (*it).second.pixbuf);
	}
	
	// get evidence map and write the amount of objects
	EvidenceMap evidence=pcase.getEvidence();
	int evidenceCount=evidence.size();
	fwrite(&evidenceCount, sizeof(int), 1, f);
	
	// iterate over evidence
	for (EvidenceMap::iterator it=evidence.begin(); it!=evidence.end(); ++it) {
		// write id
		writeString(f, (*it).second.id);
		
		// write name
		writeString(f, (*it).second.name);
		
		// write caption
		writeString(f, (*it).second.caption);
		
		// write description
		writeString(f, (*it).second.description);
		
		// write check id
		writeString(f, (*it).second.checkID);
		
		// write pixbuf
		writeImage(f, (*it).second.pixbuf);
	}
	
	// get image map and write amount of objects
	ImageMap images=pcase.getImages();
	int imageCount=images.size();
	fwrite(&imageCount, sizeof(int), 1, f);
	
	// iterate over images
	for (ImageMap::iterator it=images.begin(); it!=images.end(); ++it) {
		// write id
		writeString(f, (*it).second.id);
		
		// write image data
		writeImage(f, (*it).second.pixbuf);
	}
	
	// get location map and write amount of objects
	LocationMap locations=pcase.getLocations();
	int locationCount=locations.size();
	fwrite(&locationCount, sizeof(int), 1, f);
	
	// iterate over locations
	for (LocationMap::iterator it=locations.begin(); it!=locations.end(); ++it) {
		// write id
		writeString(f, (*it).second.id);
		
		// write name
		writeString(f, (*it).second.name);
		
		// write amount of hotspots
		int hcount=(*it).second.hotspots.size();
		fwrite(&hcount, sizeof(int), 1, f);
		
		// iterate over hotspots
		for (int i=0; i<hcount; i++) {
			Case::Hotspot hspot=(*it).second.hotspots[i];
			
			// write x,y; width and height
			fwrite(&hspot.rect.x, sizeof(int), 1, f);
			fwrite(&hspot.rect.y, sizeof(int), 1, f);
			fwrite(&hspot.rect.w, sizeof(int), 1, f);
			fwrite(&hspot.rect.h, sizeof(int), 1, f);
			
			// write target block
			writeString(f, hspot.block);
		}
		
		// write amount of states
		int scount=(*it).second.states.size();
		fwrite(&scount, sizeof(int), 1, f);
		
		// iterate over states
		for (std::map<QString, QString>::iterator t=(*it).second.states.begin(); 
				   t!=(*it).second.states.end(); ++t) {
			// write the id and bg id
			writeString(f, (*t).first);
			writeString(f, (*t).second);
		}
	}
	
	// get audio map and write count of samples
	AudioMap amap=pcase.getAudio();
	int audioCount=amap.size();
	fwrite(&audioCount, sizeof(int), 1, f);
	
	// iterate over audio
	for (AudioMap::iterator it=amap.begin(); it!=amap.end(); ++it) {
		Case::Audio audio=(*it).second;
		
		// write id
		writeString(f, audio.id);
		
		// write file name
		writeString(f, audio.name);
	}
	
	// write count of testimonies
	TestimonyMap tmap=pcase.getTestimonies();
	int testimonyCount=tmap.size();
	fwrite(&testimonyCount, sizeof(int), 1, f);
	
	// iterate over testimonies
	for (TestimonyMap::iterator it=tmap.begin(); it!=tmap.end(); ++it) {
		// write testimony id
		writeString(f, (*it).first);
		
		// write title
		writeString(f, (*it).second.title);
		
		// write speaker
		writeString(f, (*it).second.speaker);
		
		// write next block
		writeString(f, (*it).second.nextBlock);
		
		// write follow location
		writeString(f, (*it).second.followLoc);
		
		// write cross examine end block
		writeString(f, (*it).second.xExamineEndBlock);
		
		// write amount of pieces
		int tpieceCount=(*it).second.pieces.size();
		fwrite(&tpieceCount, sizeof(int), 1, f);
		
		// iterate over pieces
		for (int i=0; i<tpieceCount; i++) {
			Case::TestimonyPiece piece=(*it).second.pieces[i];
			
			// write contents
			writeString(f, piece.text);
			
			// write present evidence id
			writeString(f, piece.presentId);
			
			// write present target
			writeString(f, piece.presentBlock);
			
			// write press target
			writeString(f, piece.pressBlock);
			
			// write hidden value
			fwrite(&piece.hidden, sizeof(bool), 1, f);
		}
	}
	
	// write count of blocks
	int bufferCount=buffers.size();
	fwrite(&bufferCount, sizeof(int), 1, f);
	
	// iterate over text blocks
	int i=0;
	for (BufferMap::const_iterator it=buffers.begin(); it!=buffers.end(); ++it) {
		// write buffer id
		writeString(f, (*it).first);
		
		// find the real id
		QString realId=(*it).first.mid(0, (*it).first.lastIndexOf("_"));
		
		// write mapped buffer description
		QString bd=bufferDescriptions[realId];
		writeString(f, bd);
		
		// get text and write it to file
		QString bufText=(*it).second;
		writeString(f, bufText);
	}
	
	// wrap up
	fclose(f);
	return IO::CODE_OK;
}

// export a case to file
IO::Code IO::exportCaseToFile(const QString &path, const Case::Case &pcase, const BufferMap &buffers) {
	// open the requested file
	FILE *f=fopen(path.toStdString().c_str(), "wb");
	if (!f)
		return IO::CODE_OPEN_FAILED;
	
	// write the header
	PWTHeader header;
	fwrite(&header, sizeof(PWTHeader), 1, f);
	
	// get case overview
	Case::Overview overview=pcase.getOverview();
	
	// write overview details
	header.overviewOffset=ftell(f);
	writeString(f, overview.name);
	writeString(f, overview.author);
	fwrite(&overview.days, sizeof(int), 1, f);
	
	// iterate over core blocks and write them
	for (int i=0; i<Case::Case::CORE_BLOCK_COUNT; i++)
		writeString(f, pcase.getCoreBlock(i));
	
	// get overrides
	Case::Overrides ov=pcase.getOverrides();
	
	// write override details
	header.overridesOffset=ftell(f);
	fwrite(&ov.textboxAlpha, sizeof(int), 1, f);
	writeString(f, ov.titleScreen);
	
	// write initial block id
	writeString(f, pcase.getInitialBlockID());
	
	// get character data and write the amount of objects
	header.charOffset=ftell(f);
	std::map<QString, Character> characters=pcase.getCharacters();
	int charCount=characters.size();
	fwrite(&charCount, sizeof(int), 1, f);
	
	// iterate over character data
	for (CharacterMap::iterator it=characters.begin(); it!=characters.end(); ++it) {
		// write internal name
		writeString(f, (*it).second.getInternalName());
		
		// write displayed name
		writeString(f, (*it).second.getName());
		
		// write gender
		int gender=(*it).second.getGender();
		fwrite(&gender, sizeof(int), 1, f);
		
		// write caption
		writeString(f, (*it).second.getCaption());
		
		// write description
		writeString(f, (*it).second.getDescription());
		
		// write sprite name
		writeString(f, (*it).second.getSpriteName());
		
		// write text box tag existance
		bool hasTag=(*it).second.hasTextBoxTag();
		fwrite(&hasTag, sizeof(bool), 1, f);
		
		// write text box tag
		if (hasTag)
			writeImage(f, (*it).second.getTextBoxTag());
		
		// write headshot existance
		bool hasHeadshot=(*it).second.hasHeadshot();
		fwrite(&hasHeadshot, sizeof(bool), 1, f);
		
		// see if there is a headshot
		if (hasHeadshot) {
			// write the actual 70x70 headshot
			writeImage(f, (*it).second.getHeadshot());
			
			// create a scaled headshot and write it
			// TODO
			//Glib::RefPtr<Gdk::Pixbuf> scaled=(*it).second.get_headshot()->scale_simple(40, 40, Gdk::INTERP_HYPER);
			//writeImage(f, scaled);
		}
	}
	
	// get background map and write the amount of objects
	header.bgOffset=ftell(f);
	BackgroundMap backgrounds=pcase.getBackgrounds();
	int bgCount=backgrounds.size();
	fwrite(&bgCount, sizeof(int), 1, f);
	
	// iterate over backgrounds
	for (BackgroundMap::iterator it=backgrounds.begin(); it!=backgrounds.end(); ++it) {
		// write id
		writeString(f, (*it).second.id);
		
		// write type
		fwrite(&(*it).second.type, sizeof(int), 1, f);
		
		// write bitmap
		writeImage(f, (*it).second.pixbuf);
	}
	
	// get evidence map and write the amount of objects
	header.evidenceOffset=ftell(f);
	EvidenceMap evidence=pcase.getEvidence();
	int evidenceCount=evidence.size();
	fwrite(&evidenceCount, sizeof(int), 1, f);
	
	// iterate over evidence
	for (EvidenceMap::iterator it=evidence.begin(); it!=evidence.end(); ++it) {
		// write id
		writeString(f, (*it).second.id);
		
		// write name
		writeString(f, (*it).second.name);
		
		// write caption
		writeString(f, (*it).second.caption);
		
		// write description
		writeString(f, (*it).second.description);
		
		// write check id
		writeString(f, (*it).second.checkID);
		
		// write bitmap
		writeImage(f, (*it).second.pixbuf);
		
		// create a scaled thumbnail and write it as well
		// TODO
		//Glib::RefPtr<Gdk::Pixbuf> thumb=(*it).second.pixbuf->scale_simple(40, 40, Gdk::INTERP_HYPER);
		//writeImage(f, thumb);
	}
	
	// get image map and write amount of objects
	header.imgOffset=ftell(f);
	ImageMap images=pcase.getImages();
	int imageCount=images.size();
	fwrite(&imageCount, sizeof(int), 1, f);
	
	// iterate over images
	for (ImageMap::iterator it=images.begin(); it!=images.end(); ++it) {
		// write id
		writeString(f, (*it).second.id);
		
		// write image data
		writeImage(f, (*it).second.pixbuf);
	}
	
	// get location map and write amount of objects
	header.locationOffset=ftell(f);
	LocationMap locations=pcase.getLocations();
	int locationCount=locations.size();
	fwrite(&locationCount, sizeof(int), 1, f);
	
	// iterate over locations
	for (LocationMap::iterator it=locations.begin(); it!=locations.end(); ++it) {
		// write id
		writeString(f, (*it).second.id);
		
		// write name
		writeString(f, (*it).second.name);
		
		// write amount of hotspots
		int hcount=(*it).second.hotspots.size();
		fwrite(&hcount, sizeof(int), 1, f);
		
		// iterate over hotspots
		for (int i=0; i<hcount; i++) {
			Case::Hotspot hspot=(*it).second.hotspots[i];
			
			// write area and dimensions
			fwrite(&hspot.rect.x, sizeof(int), 1, f);
			fwrite(&hspot.rect.y, sizeof(int), 1, f);
			fwrite(&hspot.rect.w, sizeof(int), 1, f);
			fwrite(&hspot.rect.h, sizeof(int), 1, f);
			
			// write target block
			writeString(f, hspot.block);
		}
		
		// write amount of states
		int scount=(*it).second.states.size();
		fwrite(&scount, sizeof(int), 1, f);
		
		// iterate over states
		for (std::map<QString, QString>::iterator t=(*it).second.states.begin(); 
			t!=(*it).second.states.end(); ++t) {
			// write the id and bg id
			writeString(f, (*t).first);
			writeString(f, (*t).second);
		}
	}
	
	// get audio map and write count of samples
	header.audioOffset=ftell(f);
	AudioMap amap=pcase.getAudio();
	int audioCount=amap.size();
	fwrite(&audioCount, sizeof(int), 1, f);
	
	// iterate over audio
	for (AudioMap::iterator it=amap.begin(); it!=amap.end(); ++it) {
		// write id
		writeString(f, (*it).second.id);
		
		// write file name
		writeString(f, (*it).second.name);
	}
	
	// write count of testimonies
	header.testimonyOffset=ftell(f);
	TestimonyMap tmap=pcase.getTestimonies();
	int testimonyCount=tmap.size();
	fwrite(&testimonyCount, sizeof(int), 1, f);
	
	// iterate over testimonies
	for (TestimonyMap::iterator it=tmap.begin(); it!=tmap.end(); ++it) {
		// write testimony id
		writeString(f, (*it).first);
		
		// write title
		writeString(f, (*it).second.title);
		
		// write speaker
		writeString(f, (*it).second.speaker);
		
		// write next block
		writeString(f, (*it).second.nextBlock);
		
		// write follow location
		writeString(f, (*it).second.followLoc);
		
		// write cross examine end block
		writeString(f, (*it).second.xExamineEndBlock);
		
		// write amount of pieces
		int tpieceCount=(*it).second.pieces.size();
		fwrite(&tpieceCount, sizeof(int), 1, f);
		
		// iterate over pieces
		for (int i=0; i<tpieceCount; i++) {
			Case::TestimonyPiece piece=(*it).second.pieces[i];
			
			// write contents
			writeString(f, piece.text);
			
			// write present evidence id
			writeString(f, piece.presentId);
			
			// write present target
			writeString(f, piece.presentBlock);
			
			// write press target
			writeString(f, piece.pressBlock);
			
			// write hidden value
			fwrite(&piece.hidden, sizeof(bool), 1, f);
		}
	}
	
	// write count of blocks
	header.blockOffset=ftell(f);
	int bufferCount=buffers.size();
	fwrite(&bufferCount, sizeof(int), 1, f);
	
	// iterate over text blocks
	int i=0;
	for (BufferMap::const_iterator it=buffers.begin(); it!=buffers.end(); ++it) {
		// find the real id
		QString id=(*it).first;
		QString realId=id.mid(0, id.lastIndexOf("_"));
		
		// write buffer id
		writeString(f, realId);
		
		// get text for this buffer
		QString bufText=(*it).second;
		
		// write the text
		writeString(f, bufText);
	}
	
	// go back and fix the header
	rewind(f);
	header.ident=FILE_MAGIC_NUM;
	header.version=FILE_VERSION;
	fwrite(&header, sizeof(PWTHeader), 1, f);
	
	// wrap up
	fclose(f);
	return IO::CODE_OK;
}

// load a case from file
IO::Code IO::loadCaseFromFile(const QString &path, Case::Case &pcase, BufferMap &buffers,
			     std::map<QString, QString> &bufferDescriptions) {
	// open requested file
	FILE *f=fopen(path.toStdString().c_str(), "rb");
	if (!f)
		return IO::CODE_OPEN_FAILED;
	
	// read magic number and verify it
	if (fgetc(f)!='C' || fgetc(f)!='P' ||
	    fgetc(f)!='R' || fgetc(f)!='J' ||
	    fgetc(f)!='T')
		return IO::CODE_WRONG_MAGIC_NUM;
	
	// read file version and verify it
	int version;
	fread(&version, sizeof(int), 1, f);
	if (version!=FILE_VERSION)
		return IO::CODE_WRONG_VERSION;
	
	// create a new overview struct
	Case::Overview overview;
	
	// read in data
	overview.name=readString(f);
	overview.author=readString(f);
	fread(&overview.days, sizeof(int), 1, f);
	
	// read in core blocks
	for (int i=0; i<Case::Case::CORE_BLOCK_COUNT; i++)
		pcase.setCoreBlock(i, readString(f));
	
	// create new overrides object
	Case::Overrides ov;
	
	// read override details
	fread(&ov.textboxAlpha, sizeof(int), 1, f);
	ov.titleScreen=readString(f);
	
	// set the overrides
	pcase.setOverrides(ov);
	
	// set the overview
	pcase.setOverview(overview);
	
	// read initial block id
	QString initialBlock=readString(f);
	pcase.setInitialBlockID(initialBlock);
	
	// read amount of characters
	int charCount;
	fread(&charCount, sizeof(int), 1, f);
	
	// read each character
	for (int i=0; i<charCount; i++) {
		Character character;
		QString str;
		
		// read internal name
		str=readString(f);
		character.setInternalName(str);
		
		// read displayed name
		str=readString(f);
		character.setName(str);
		
		// read gender
		int gender=0;
		fread(&gender, sizeof(int), 1, f);
		character.setGender((gender==0 ? Character::GENDER_MALE : Character::GENDER_FEMALE));
		
		// read caption
		str=readString(f);
		character.setCaption(str);
		
		// read description
		str=readString(f);
		character.setDescription(str);
		
		// read sprite name
		str=readString(f);
		character.setSpriteName(str);
		
		// read text box tag existance
		bool hasTag;
		fread(&hasTag, sizeof(bool), 1, f);
		character.setHasTextBoxTag(hasTag);
		
		// read text box tag, if any
		if (hasTag)
			character.setTextBoxTag(readImage(f));
		
		// read headshot existance
		bool hasHeadshot;
		fread(&hasHeadshot, sizeof(bool), 1, f);
		character.setHasHeadshot(hasHeadshot);
		
		// read headshot, if any
		if (hasHeadshot)
			character.setHeadshot(readImage(f));
		
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
		bg.pixbuf=readImage(f);
		
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
		
		// read check image id
		evidence.checkID=readString(f);
		
		// read pixbuf data
		evidence.pixbuf=readImage(f);
		
		// add this evidence
		pcase.addEvidence(evidence);
	}
	
	// read amount of images
	int imageCount;
	fread(&imageCount, sizeof(int), 1, f);
	
	// iterate over images
	for (int i=0; i<imageCount; i++) {
		Case::Image img;
		
		// read id
		img.id=readString(f);
		
		// read image data
		img.pixbuf=readImage(f);
		
		// add this image
		pcase.addImage(img);
	}
	
	// read amount of locations
	int locationCount;
	fread(&locationCount, sizeof(int), 1, f);
	
	// iterate over locations
	for (int i=0; i<locationCount; i++) {
		Case::Location location;
		
		// read id
		location.id=readString(f);
		
		// read name
		location.name=readString(f);
		
		// read amount of hotspots
		int hcount;
		fread(&hcount, sizeof(int), 1, f);
		
		// iterate over hotspots
		for (int i=0; i<hcount; i++) {
			Case::Hotspot hspot;
			
			// read area and dimensions
			fread(&hspot.rect.x, sizeof(int), 1, f);
			fread(&hspot.rect.y, sizeof(int), 1, f);
			fread(&hspot.rect.w, sizeof(int), 1, f);
			fread(&hspot.rect.h, sizeof(int), 1, f);
			
			// read target block
			hspot.block=readString(f);
			
			// add this hotspot
			location.hotspots.push_back(hspot);
		}
		
		// read amount of states
		int scount;
		fread(&scount, sizeof(int), 1, f);
		
		// iterate over states
		for (int i=0; i<scount; i++) {
			QString sId=readString(f);
			QString bg=readString(f);
			
			location.states[sId]=bg;
		}
		
		// add this location
		pcase.addLocation(location);
	}
	
	// read amount of audio
	int audioCount;
	fread(&audioCount, sizeof(int), 1, f);
	
	// iterate over audio samples
	for (int i=0; i<audioCount; i++) {
		Case::Audio audio;
		
		// read id
		audio.id=readString(f);
		
		// read name
		audio.name=readString(f);
		
		// add this audio
		pcase.addAudio(audio);
	}
	
	// read count of testimonies
	int testimonyCount;
	fread(&testimonyCount, sizeof(int), 1, f);
	
	// iterate over testimonies
	for (int i=0; i<testimonyCount; i++) {
		Case::Testimony testimony;
		
		// read testimony id
		testimony.id=readString(f);
		
		// read title
		testimony.title=readString(f);
		
		// read speaker
		testimony.speaker=readString(f);
		
		// read next block
		testimony.nextBlock=readString(f);
		
		// read follow location
		testimony.followLoc=readString(f);
		
		// read cross examine end block
		testimony.xExamineEndBlock=readString(f);
		
		// read amount of pieces
		int tpieceCount;
		fread(&tpieceCount, sizeof(int), 1, f);
		
		// iterate over pieces
		for (int j=0; j<tpieceCount; j++) {
			Case::TestimonyPiece piece;
			
			// read contents
			piece.text=readString(f);
			
			// read present evidence id
			piece.presentId=readString(f);
			
			// read present target
			piece.presentBlock=readString(f);
			
			// read press target
			piece.pressBlock=readString(f);
			
			// read hidden value
			fread(&piece.hidden, sizeof(bool), 1, f);
			
			// add this piece
			testimony.pieces.push_back(piece);
		}
		
		// add this testimony
		pcase.addTestimony(testimony);
	}
	
	// read amount of text blocks
	int bufferCount;
	fread(&bufferCount, sizeof(int), 1, f);
	
	// iterate over text blocks
	for (int i=0; i<bufferCount; i++) {
		// read id
		QString bufferId=readString(f);
		
		// find real id
		QString realId=bufferId.mid(0, bufferId.lastIndexOf("_"));
		
		// read description
		QString bufferDescription=readString(f);
		bufferDescriptions[realId]=bufferDescription;
		
		// read text contents
		QString contents=readString(f);
		
		// allocate new text buffer and set the text contents
		
		// append this text buffer to the map
		buffers[bufferId]=contents;
	}
	
	// wrap up
	fclose(f);
	return IO::CODE_OK;
}

// save a sprite to file
IO::Code IO::saveSpriteToFile(const QString &path, const Sprite &spr) {
	// open the requested file
	FILE *f=fopen(path.toStdString().c_str(), "wb");
	if (!f)
		return IO::CODE_OPEN_FAILED;
	
	// write file header
	writeString(f, SPR_MAGIC_NUM);
	fwrite(&SPR_VERSION, sizeof(int), 1, f);
	
	// get sprite animations
	AnimationMap animations=spr.getAnimations();
	
	// write count of animations
	int count=animations.size();
	fwrite(&count, sizeof(int), 1, f);
	
	// show progress dialog
	// TODO
	
	// iterate over animations
	int c=0;
	for (AnimationMap::iterator it=animations.begin(); it!=animations.end(); ++it) {
		Animation anim=(*it).second;
		
		// write id
		writeString(f, anim.id);
		
		// write looping value
		fwrite(&anim.loop, sizeof(bool), 1, f);
		
		// write amount of frames
		int fcount=anim.frames.size();
		fwrite(&fcount, sizeof(int), 1, f);
		
		// iterate over frames
		for (int i=0; i<fcount; i++) {
			// write time delay
			fwrite(&anim.frames[i].time, sizeof(int), 1, f);
			
			// write sound effect
			writeString(f, anim.frames[i].sfx);
			
			// write pixbuf
			writeImage(f, anim.frames[i].pixbuf);
		}
		
		c++;
		
		// update progress
		// TODO
	}
	
	// wrap up
	fclose(f);
	return IO::CODE_OK;
}

// load a sprite from file
IO::Code IO::loadSpriteFromFile(const QString &path, Sprite &spr) {
	// open requested file
	FILE *f=fopen(path.toStdString().c_str(), "rb");
	if (!f)
		return IO::CODE_OPEN_FAILED;
	
	// read magic number and verify it
	QString mn=readString(f);
	if (mn!=SPR_MAGIC_NUM)
		return IO::CODE_WRONG_MAGIC_NUM;
	
	// read version and verify it
	int version;
	fread(&version, sizeof(int), 1, f);
	if (version!=SPR_VERSION)
		return IO::CODE_WRONG_VERSION;
	
	// show progress dialog
	// TODO
	
	// read amount of animations
	int count;
	fread(&count, sizeof(int), 1, f);
	
	// iterate over animations
	for (int i=0; i<count; i++) {
		Animation anim;
		
		// read id
		anim.id=readString(f);
		
		// read looping value
		fread(&anim.loop, sizeof(bool), 1, f);
		
		// read amount of frames
		int fcount;
		fread(&fcount, sizeof(int), 1, f);
		
		// read in frames
		for (int j=0; j<fcount; j++) {
			Frame fr;
			
			// read time
			fread(&fr.time, sizeof(int), 1, f);
			
			// read sound effect
			fr.sfx=readString(f);
			
			// read pixbuf
			fr.pixbuf=readImage(f);
			
			anim.frames.push_back(fr);
		}
		
		// add animation to sprite
		spr.addAnimation(anim);
		
		// progress the dialog
		// TODO
	}
	
	// wrap up
	fclose(f);
	return IO::CODE_OK;
}
// write a string to file
void IO::writeString(FILE *f, const QString &str) {
	// get the length of the string and write it
	int len=str.size();
	fwrite(&len, sizeof(int), 1, f);
	
	// now write the string
	for (int i=0; i<str.size(); i++) {
		QChar ch=(QChar) str[i];
		fwrite(&ch, sizeof(QChar), 1, f);
	}
}

// read a string from file
QString IO::readString(FILE *f) {
	// read string length
	int len;
	fread(&len, sizeof(int), 1, f);
	
	// read the string
	QString str="";
	for (int i=0; i<len; i++) {
		QChar ch;
		fread(&ch, sizeof(QChar), 1, f);
		str+=(QChar) ch;
	}
	
	return str;
}

// write a pixbuf to compressed, internal format
void IO::writeImage(FILE *f, const QPixmap &pixbuf) {
	// save the pixmap data to array of bytes
	QByteArray data;
	QBuffer buffer(&data);
	buffer.open(QIODevice::WriteOnly);
	pixbuf.save(&buffer, "PNG");
	
	// write buffer size
	int size=data.length();
	fwrite(&size, sizeof(int), 1, f);
	
	// write the actual contents now
	char *pixels=data.data();
	fwrite(pixels, sizeof(char), size, f);
	
	// we can close the buffer now
	buffer.close();
}

// read a pixbuf from file
QPixmap IO::readImage(FILE *f) {
	// read the size
	int size;
	fread(&size, sizeof(int), 1, f);
	
	// now read the actual data
	char pixels[size];
	fread(pixels, sizeof(char), size, f);
	
	// create the pixmap and return it
	QPixmap pixmap;
	QByteArray data(pixels);
	pixmap.loadFromData(data, "PNG");
		
	return pixmap;
}
