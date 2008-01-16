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

#include <archive.h>
#include <archive_entry.h>
#include <cstdio>
#include <dirent.h>

#include "clistview.h"
#include "dialogs.h"
#include "iohandler.h"
#include "utilities.h"

// save a case and its associated data to file
IO::Code IO::save_case_to_file(const Glib::ustring &path, const Case::Case &pcase,
			   const BufferMap &buffers,
			   std::map<Glib::ustring, Glib::ustring> &bufferDescriptions) {
	// open the requested file
	FILE *f=fopen(path.c_str(), "wb");
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
	Case::Overview overview=pcase.get_overview();
	
	// write overview details
	write_string(f, overview.name);
	write_string(f, overview.author);
	fwrite(&overview.lawSys, sizeof(int), 1, f);
	
	// get overrides
	Case::Overrides ov=pcase.get_overrides();
	
	// write override details
	fwrite(&ov.textboxAlpha, sizeof(int), 1, f);
	write_string(f, ov.titleScreen);
	
	// write initial block id
	write_string(f, pcase.get_initial_block_id());
	
	// get character data and write the amount of objects
	std::map<Glib::ustring, Character> characters=pcase.get_characters();
	int charCount=characters.size();
	fwrite(&charCount, sizeof(int), 1, f);
	
	// iterate over character data
	for (CharacterMap::iterator it=characters.begin(); it!=characters.end(); ++it) {
		// write internal name
		write_string(f, (*it).second.get_internal_name());
		
		// write displayed name
		write_string(f, (*it).second.get_name());
		
		// write gender
		int gender=(*it).second.get_gender();
		fwrite(&gender, sizeof(int), 1, f);
		
		// write caption
		write_string(f, (*it).second.get_caption());
		
		// write description
		write_string(f, (*it).second.get_description());
		
		// write sprite name
		write_string(f, (*it).second.get_sprite_name());
		
		// write text box tag existance
		bool hasTag=(*it).second.has_text_box_tag();
		fwrite(&hasTag, sizeof(bool), 1, f);
		
		// write text box tag
		if (hasTag)
			write_pixbuf(f, (*it).second.get_text_box_tag());
		
		// write headshot existance
		bool hasHeadshot=(*it).second.has_headshot();
		fwrite(&hasHeadshot, sizeof(bool), 1, f);
		
		// write headshot
		if (hasHeadshot)
			write_pixbuf(f, (*it).second.get_headshot());
	}
	
	// get background map and write the amount of objects
	BackgroundMap backgrounds=pcase.get_backgrounds();
	int bgCount=backgrounds.size();
	fwrite(&bgCount, sizeof(int), 1, f);
	
	// iterate over backgrounds
	for (BackgroundMap::iterator it=backgrounds.begin(); it!=backgrounds.end(); ++it) {
		// write id
		write_string(f, (*it).second.id);
		
		// write type
		fwrite(&(*it).second.type, sizeof(int), 1, f);
		
		// write pixbuf data
		write_pixbuf(f, (*it).second.pixbuf);
	}
	
	// get evidence map and write the amount of objects
	EvidenceMap evidence=pcase.get_evidence();
	int evidenceCount=evidence.size();
	fwrite(&evidenceCount, sizeof(int), 1, f);
	
	// iterate over evidence
	for (EvidenceMap::iterator it=evidence.begin(); it!=evidence.end(); ++it) {
		// write id
		write_string(f, (*it).second.id);
		
		// write name
		write_string(f, (*it).second.name);
		
		// write caption
		write_string(f, (*it).second.caption);
		
		// write description
		write_string(f, (*it).second.description);
		
		// write check id
		write_string(f, (*it).second.checkID);
		
		// write pixbuf
		write_pixbuf(f, (*it).second.pixbuf);
	}
	
	// get image map and write amount of objects
	ImageMap images=pcase.get_images();
	int imageCount=images.size();
	fwrite(&imageCount, sizeof(int), 1, f);
	
	// iterate over images
	for (ImageMap::iterator it=images.begin(); it!=images.end(); ++it) {
		// write id
		write_string(f, (*it).second.id);
		
		// write image data
		write_pixbuf(f, (*it).second.pixbuf);
	}
	
	// get location map and write amount of objects
	LocationMap locations=pcase.get_locations();
	int locationCount=locations.size();
	fwrite(&locationCount, sizeof(int), 1, f);
	
	// iterate over locations
	for (LocationMap::iterator it=locations.begin(); it!=locations.end(); ++it) {
		// write id
		write_string(f, (*it).second.id);
		
		// write name
		write_string(f, (*it).second.name);
		
		// write bg id
		write_string(f, (*it).second.bg);
		
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
			write_string(f, hspot.block);
		}
	}
	
	// get audio map and write count of samples
	AudioMap amap=pcase.get_audio();
	int audioCount=amap.size();
	fwrite(&audioCount, sizeof(int), 1, f);
	
	// iterate over audio
	for (AudioMap::iterator it=amap.begin(); it!=amap.end(); ++it) {
		Case::Audio audio=(*it).second;
		
		// write id
		write_string(f, audio.id);
		
		// write file name
		write_string(f, audio.name);
	}
	
	// write count of testimonies
	TestimonyMap tmap=pcase.get_testimonies();
	int testimonyCount=tmap.size();
	fwrite(&testimonyCount, sizeof(int), 1, f);
	
	// iterate over testimonies
	for (TestimonyMap::iterator it=tmap.begin(); it!=tmap.end(); ++it) {
		// write testimony id
		write_string(f, (*it).first);
		
		// write title
		write_string(f, (*it).second.title);
		
		// write speaker
		write_string(f, (*it).second.speaker);
		
		// write next block
		write_string(f, (*it).second.nextBlock);
		
		// write follow location
		write_string(f, (*it).second.followLoc);
		
		// write cross examine end block
		write_string(f, (*it).second.xExamineEndBlock);
		
		// write amount of pieces
		int tpieceCount=(*it).second.pieces.size();
		fwrite(&tpieceCount, sizeof(int), 1, f);
		
		// iterate over pieces
		for (int i=0; i<tpieceCount; i++) {
			Case::TestimonyPiece piece=(*it).second.pieces[i];
			
			// write contents
			write_string(f, piece.text);
			
			// write present evidence id
			write_string(f, piece.presentId);
			
			// write present target
			write_string(f, piece.presentBlock);
			
			// write press target
			write_string(f, piece.pressBlock);
			
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
		write_string(f, (*it).first);
		
		// find the real id
		Glib::ustring realId=(*it).first.substr(0, (*it).first.rfind("_"));
		
		// write mapped buffer description
		Glib::ustring bd=bufferDescriptions[realId];
		write_string(f, bd);
		
		// get text and write it to file
		Glib::ustring bufText=(*it).second->get_text(true);
		write_string(f, bufText);
	}
	
	// wrap up
	fclose(f);
	return IO::CODE_OK;
}

// export a case to file
IO::Code IO::export_case_to_file(const Glib::ustring &path, const Case::Case &pcase, const BufferMap &buffers) {
	// open the requested file
	FILE *f=fopen(path.c_str(), "wb");
	if (!f)
		return IO::CODE_OPEN_FAILED;
	
	// write the header
	PWTHeader header;
	fwrite(&header, sizeof(PWTHeader), 1, f);
	
	// get case overview
	Case::Overview overview=pcase.get_overview();
	
	// write overview details
	header.overviewOffset=ftell(f);
	write_string(f, overview.name);
	write_string(f, overview.author);
	fwrite(&overview.lawSys, sizeof(int), 1, f);
	
	// get overrides
	Case::Overrides ov=pcase.get_overrides();
	
	// write override details
	header.overridesOffset=ftell(f);
	fwrite(&ov.textboxAlpha, sizeof(int), 1, f);
	write_string(f, ov.titleScreen);
	
	// write initial block id
	write_string(f, pcase.get_initial_block_id());
	
	// get character data and write the amount of objects
	header.charOffset=ftell(f);
	std::map<Glib::ustring, Character> characters=pcase.get_characters();
	int charCount=characters.size();
	fwrite(&charCount, sizeof(int), 1, f);
	
	// iterate over character data
	for (CharacterMap::iterator it=characters.begin(); it!=characters.end(); ++it) {
		// write internal name
		write_string(f, (*it).second.get_internal_name());
		
		// write displayed name
		write_string(f, (*it).second.get_name());
		
		// write gender
		int gender=(*it).second.get_gender();
		fwrite(&gender, sizeof(int), 1, f);
		
		// write caption
		write_string(f, (*it).second.get_caption());
		
		// write description
		write_string(f, (*it).second.get_description());
		
		// write sprite name
		write_string(f, (*it).second.get_sprite_name());
		
		// write text box tag existance
		bool hasTag=(*it).second.has_text_box_tag();
		fwrite(&hasTag, sizeof(bool), 1, f);
		
		// write text box tag
		if (hasTag)
			write_export_image(f, (*it).second.get_text_box_tag());
		
		// write headshot existance
		bool hasHeadshot=(*it).second.has_headshot();
		fwrite(&hasHeadshot, sizeof(bool), 1, f);
		
		// see if there is a headshot
		if (hasHeadshot) {
			// write the actual 70x70 headshot
			write_export_image(f, (*it).second.get_headshot());
			
			// create a scaled headshot and write it
			Glib::RefPtr<Gdk::Pixbuf> scaled=(*it).second.get_headshot()->scale_simple(40, 40, Gdk::INTERP_HYPER);
			write_export_image(f, scaled);
		}
	}
	
	// get background map and write the amount of objects
	header.bgOffset=ftell(f);
	BackgroundMap backgrounds=pcase.get_backgrounds();
	int bgCount=backgrounds.size();
	fwrite(&bgCount, sizeof(int), 1, f);
	
	// iterate over backgrounds
	for (BackgroundMap::iterator it=backgrounds.begin(); it!=backgrounds.end(); ++it) {
		// write id
		write_string(f, (*it).second.id);
		
		// write type
		fwrite(&(*it).second.type, sizeof(int), 1, f);
		
		// write bitmap
		write_export_image(f, (*it).second.pixbuf);
	}
	
	// get evidence map and write the amount of objects
	header.evidenceOffset=ftell(f);
	EvidenceMap evidence=pcase.get_evidence();
	int evidenceCount=evidence.size();
	fwrite(&evidenceCount, sizeof(int), 1, f);
	
	// iterate over evidence
	for (EvidenceMap::iterator it=evidence.begin(); it!=evidence.end(); ++it) {
		// write id
		write_string(f, (*it).second.id);
		
		// write name
		write_string(f, (*it).second.name);
		
		// write caption
		write_string(f, (*it).second.caption);
		
		// write description
		write_string(f, (*it).second.description);
		
		// write check id
		write_string(f, (*it).second.checkID);
		
		// write bitmap
		write_export_image(f, (*it).second.pixbuf);
		
		// create a scaled thumbnail and write it as well
		Glib::RefPtr<Gdk::Pixbuf> thumb=(*it).second.pixbuf->scale_simple(40, 40, Gdk::INTERP_HYPER);
		write_export_image(f, thumb);
	}
	
	// get image map and write amount of objects
	header.imgOffset=ftell(f);
	ImageMap images=pcase.get_images();
	int imageCount=images.size();
	fwrite(&imageCount, sizeof(int), 1, f);
	
	// iterate over images
	for (ImageMap::iterator it=images.begin(); it!=images.end(); ++it) {
		// write id
		write_string(f, (*it).second.id);
		
		// write image data
		write_export_image(f, (*it).second.pixbuf);
	}
	
	// get location map and write amount of objects
	header.locationOffset=ftell(f);
	LocationMap locations=pcase.get_locations();
	int locationCount=locations.size();
	fwrite(&locationCount, sizeof(int), 1, f);
	
	// iterate over locations
	for (LocationMap::iterator it=locations.begin(); it!=locations.end(); ++it) {
		// write id
		write_string(f, (*it).second.id);
		
		// write name
		write_string(f, (*it).second.name);
		
		// write bg id
		write_string(f, (*it).second.bg);
		
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
			write_string(f, hspot.block);
		}
	}
	
	// get audio map and write count of samples
	header.audioOffset=ftell(f);
	AudioMap amap=pcase.get_audio();
	int audioCount=amap.size();
	fwrite(&audioCount, sizeof(int), 1, f);
	
	// iterate over audio
	for (AudioMap::iterator it=amap.begin(); it!=amap.end(); ++it) {
		// write id
		write_string(f, (*it).second.id);
		
		// write file name
		write_string(f, (*it).second.name);
	}
	
	// write count of testimonies
	header.testimonyOffset=ftell(f);
	TestimonyMap tmap=pcase.get_testimonies();
	int testimonyCount=tmap.size();
	fwrite(&testimonyCount, sizeof(int), 1, f);
	
	// iterate over testimonies
	for (TestimonyMap::iterator it=tmap.begin(); it!=tmap.end(); ++it) {
		// write testimony id
		write_string(f, (*it).first);
		
		// write title
		write_string(f, (*it).second.title);
		
		// write speaker
		write_string(f, (*it).second.speaker);
		
		// write next block
		write_string(f, (*it).second.nextBlock);
		
		// write follow location
		write_string(f, (*it).second.followLoc);
		
		// write cross examine end block
		write_string(f, (*it).second.xExamineEndBlock);
		
		// write amount of pieces
		int tpieceCount=(*it).second.pieces.size();
		fwrite(&tpieceCount, sizeof(int), 1, f);
		
		// iterate over pieces
		for (int i=0; i<tpieceCount; i++) {
			Case::TestimonyPiece piece=(*it).second.pieces[i];
			
			// write contents
			write_string(f, piece.text);
			
			// write present evidence id
			write_string(f, piece.presentId);
			
			// write present target
			write_string(f, piece.presentBlock);
			
			// write press target
			write_string(f, piece.pressBlock);
			
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
		Glib::ustring id=(*it).first;
		Glib::ustring realId=id.substr(0, id.rfind("_"));
		
		// write buffer id
		write_string(f, realId);
		
		// get text for this buffer
		Glib::ustring bufText=(*it).second->get_text(true);
		
		// write the text
		write_string(f, bufText);
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
IO::Code IO::load_case_from_file(const Glib::ustring &path, Case::Case &pcase,
			     BufferMap &buffers,
			     std::map<Glib::ustring, Glib::ustring> &bufferDescriptions) {
	// open requested file
	FILE *f=fopen(path.c_str(), "rb");
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
	overview.name=read_string(f);
	overview.author=read_string(f);
	fread(&overview.lawSys, sizeof(int), 1, f);
	
	// create new overrides object
	Case::Overrides ov;
	
	// read override details
	fread(&ov.textboxAlpha, sizeof(int), 1, f);
	ov.titleScreen=read_string(f);
	
	// set the overrides
	pcase.set_overrides(ov);
	
	// set the overview
	pcase.set_overview(overview);
	
	// read initial block id
	Glib::ustring initialBlock=read_string(f);
	pcase.set_initial_block_id(initialBlock);
	
	// read amount of characters
	int charCount;
	fread(&charCount, sizeof(int), 1, f);
	
	// read each character
	for (int i=0; i<charCount; i++) {
		Character character;
		Glib::ustring str;
		
		// read internal name
		str=read_string(f);
		character.set_internal_name(str);
		
		// read displayed name
		str=read_string(f);
		character.set_name(str);
		
		// read gender
		int gender=0;
		fread(&gender, sizeof(int), 1, f);
		character.set_gender((gender==0 ? Character::GENDER_MALE : Character::GENDER_FEMALE));
		
		// read caption
		str=read_string(f);
		character.set_caption(str);
		
		// read description
		str=read_string(f);
		character.set_description(str);
		
		// read sprite name
		str=read_string(f);
		character.set_sprite_name(str);
		
		// read text box tag existance
		bool hasTag;
		fread(&hasTag, sizeof(bool), 1, f);
		character.set_has_text_box_tag(hasTag);
		
		// read text box tag, if any
		if (hasTag)
			character.set_text_box_tag(read_pixbuf(f));
		
		// read headshot existance
		bool hasHeadshot;
		fread(&hasHeadshot, sizeof(bool), 1, f);
		character.set_has_headshot(hasHeadshot);
		
		// read headshot, if any
		if (hasHeadshot)
			character.set_headshot(read_pixbuf(f));
		
		// include this character
		pcase.add_character(character);
	}
	
	// read amount of backgrounds
	int bgCount;
	fread(&bgCount, sizeof(int), 1, f);
	
	// iterate over backgrounds
	for (int i=0; i<bgCount; i++) {
		Case::Background bg;
		
		// read id
		bg.id=read_string(f);
		
		// read type
		int bgType;
		fread(&bgType, sizeof(int), 1, f);
		bg.type=(bgType==0 ? Case::BG_SINGLE_SCREEN : Case::BG_DOUBLE_SCREEN);
		
		// read pixbuf data
		bg.pixbuf=read_pixbuf(f);
		
		// add this background
		pcase.add_background(bg);
	}
	
	// read amount of evidence
	int evidenceCount;
	fread(&evidenceCount, sizeof(int), 1, f);
	
	// iterate over evidence
	for (int i=0; i<evidenceCount; i++) {
		Case::Evidence evidence;
		
		// read id
		evidence.id=read_string(f);
		
		// read name
		evidence.name=read_string(f);
		
		// read caption
		evidence.caption=read_string(f);
		
		// read description
		evidence.description=read_string(f);
		
		// read check image id
		evidence.checkID=read_string(f);
		
		// read pixbuf data
		evidence.pixbuf=read_pixbuf(f);
		
		// add this evidence
		pcase.add_evidence(evidence);
	}
	
	// read amount of images
	int imageCount;
	fread(&imageCount, sizeof(int), 1, f);
	
	// iterate over images
	for (int i=0; i<imageCount; i++) {
		Case::Image img;
		
		// read id
		img.id=read_string(f);
		
		// read image data
		img.pixbuf=read_pixbuf(f);
		
		// add this image
		pcase.add_image(img);
	}
	
	// read amount of locations
	int locationCount;
	fread(&locationCount, sizeof(int), 1, f);
	
	// iterate over locations
	for (int i=0; i<locationCount; i++) {
		Case::Location location;
		
		// read id
		location.id=read_string(f);
		
		// read name
		location.name=read_string(f);
		
		// read bg id
		location.bg=read_string(f);
		
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
			hspot.block=read_string(f);
			
			// add this hotspot
			location.hotspots.push_back(hspot);
		}
		
		// add this location
		pcase.add_location(location);
	}
	
	// read amount of audio
	int audioCount;
	fread(&audioCount, sizeof(int), 1, f);
	
	// iterate over audio samples
	for (int i=0; i<audioCount; i++) {
		Case::Audio audio;
		
		// read id
		audio.id=read_string(f);
		
		// read name
		audio.name=read_string(f);
		
		// add this audio
		pcase.add_audio(audio);
	}
	
	// read count of testimonies
	int testimonyCount;
	fread(&testimonyCount, sizeof(int), 1, f);
	
	// iterate over testimonies
	for (int i=0; i<testimonyCount; i++) {
		Case::Testimony testimony;
		
		// read testimony id
		testimony.id=read_string(f);
		
		// read title
		testimony.title=read_string(f);
		
		// read speaker
		testimony.speaker=read_string(f);
		
		// read next block
		testimony.nextBlock=read_string(f);
		
		// read follow location
		testimony.followLoc=read_string(f);
		
		// read cross examine end block
		testimony.xExamineEndBlock=read_string(f);
		
		// read amount of pieces
		int tpieceCount;
		fread(&tpieceCount, sizeof(int), 1, f);
		
		// iterate over pieces
		for (int j=0; j<tpieceCount; j++) {
			Case::TestimonyPiece piece;
			
			// read contents
			piece.text=read_string(f);
			
			// read present evidence id
			piece.presentId=read_string(f);
			
			// read present target
			piece.presentBlock=read_string(f);
			
			// read press target
			piece.pressBlock=read_string(f);
			
			// read hidden value
			fread(&piece.hidden, sizeof(bool), 1, f);
			
			// add this piece
			testimony.pieces.push_back(piece);
		}
		
		// add this testimony
		pcase.add_testimony(testimony);
	}
	
	// read amount of text blocks
	int bufferCount;
	fread(&bufferCount, sizeof(int), 1, f);
	
	// iterate over text blocks
	for (int i=0; i<bufferCount; i++) {
		// read id
		Glib::ustring bufferId=read_string(f);
		
		// find real id
		Glib::ustring realId=bufferId.substr(0, bufferId.rfind("_"));
		
		// read description
		Glib::ustring bufferDescription=read_string(f);
		bufferDescriptions[realId]=bufferDescription;
		
		// read text contents
		Glib::ustring contents=read_string(f);
		
		// allocate new text buffer and set the text contents
		Glib::RefPtr<Gtk::TextBuffer> tbuffer=CListView::create_buffer();
		tbuffer->set_text(contents);
		
		// append this text buffer to the map
		buffers[bufferId]=tbuffer;
	}
	
	// wrap up
	fclose(f);
	return IO::CODE_OK;
}

// save a sprite to file
IO::Code IO::save_sprite_to_file(const Glib::ustring &path, const Sprite &spr) {
	// open the requested file
	FILE *f=fopen(path.c_str(), "wb");
	if (!f)
		return IO::CODE_OPEN_FAILED;
	
	// write file header
	write_string(f, SPR_MAGIC_NUM);
	fwrite(&SPR_VERSION, sizeof(int), 1, f);
	
	// get sprite animations
	AnimationMap animations=spr.get_animations();
	
	// write count of animations
	int count=animations.size();
	fwrite(&count, sizeof(int), 1, f);
	
	// show progress dialog
	ProgressDialog pd("Saving sprite...");
	pd.show();
	Utils::flush_events();
	
	// iterate over animations
	int c=0;
	for (AnimationMap::iterator it=animations.begin(); it!=animations.end(); ++it) {
		Animation anim=(*it).second;
		
		// write id
		write_string(f, anim.id);
		
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
			write_string(f, anim.frames[i].sfx);
			
			// write pixbuf
			write_pixbuf(f, anim.frames[i].pixbuf);
		}
		
		c++;
		
		// update progress
		double prog=(double) c/(double) count;
		pd.set_progress(prog);
	}
	
	pd.hide();
	
	// wrap up
	fclose(f);
	return IO::CODE_OK;
}

// export a sprite to file
IO::Code IO::export_sprite_to_file(const Glib::ustring &path, const Sprite &spr) {
	// open the requested file
	FILE *f=fopen(path.c_str(), "wb");
	if (!f)
		return IO::CODE_OPEN_FAILED;
	
	// write file header
	fputc('P', f);
	fputc('W', f);
	fputc('S', f);
	fwrite(&SPR_VERSION, sizeof(int), 1, f);
	
	// get sprite animations
	AnimationMap animations=spr.get_animations();
	
	// write count of animations
	int count=animations.size();
	fwrite(&count, sizeof(int), 1, f);
	
	// show progress dialog
	ProgressDialog pd("Exporting sprite...");
	pd.show();
	Utils::flush_events();
	
	// iterate over animations
	int c=0;
	for (AnimationMap::iterator it=animations.begin(); it!=animations.end(); ++it) {
		Animation anim=(*it).second;
		
		// write id
		write_string(f, anim.id);
		
		// write looping value
		fwrite(&anim.loop, sizeof(bool), 1, f);
		
		// write amount of frames
		int fcount=anim.frames.size();
		fwrite(&fcount, sizeof(int), 1, f);
		
		// iterate over frames
		for (int i=0; i<fcount; i++) {
			// write frame time
			fwrite(&anim.frames[i].time, sizeof(int), 1, f);
			
			// write sound effect
			write_string(f, anim.frames[i].sfx);
			
			// write image
			write_export_image(f, anim.frames[i].pixbuf);
		}
		
		c++;
		
		// update progress
		double prog=(double) c/(double) count;
		pd.set_progress(prog);
	}
	
	pd.hide();
	
	// wrap up
	fclose(f);
	return IO::CODE_OK;
}

// load a sprite from file
IO::Code IO::load_sprite_from_file(const Glib::ustring &path, Sprite &spr) {
	// open requested file
	FILE *f=fopen(path.c_str(), "rb");
	if (!f)
		return IO::CODE_OPEN_FAILED;
	
	// read magic number and verify it
	Glib::ustring mn=read_string(f);
	if (mn!=SPR_MAGIC_NUM)
		return IO::CODE_WRONG_MAGIC_NUM;
	
	// read version and verify it
	int version;
	fread(&version, sizeof(int), 1, f);
	if (version!=SPR_VERSION)
		return IO::CODE_WRONG_VERSION;
	
	// show progress dialog
	ProgressDialog pd("Loading sprite...");
	pd.show();
	Utils::flush_events();
	
	// read amount of animations
	int count;
	fread(&count, sizeof(int), 1, f);
	
	// iterate over animations
	for (int i=0; i<count; i++) {
		Animation anim;
		
		// read id
		anim.id=read_string(f);
		
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
			fr.sfx=read_string(f);
			
			// read pixbuf
			fr.pixbuf=read_pixbuf(f);
			
			anim.frames.push_back(fr);
		}
		
		// add animation to sprite
		spr.add_animation(anim);
		
		// progress the dialog
		double prog=(double) i/(double) count;
		pd.set_progress(prog);
		
		Utils::flush_events();
	}
	
	pd.hide();
	
	// wrap up
	fclose(f);
	return IO::CODE_OK;
}

// write a string to file
void IO::write_string(FILE *f, const Glib::ustring &str) {
	// get the length of the string and write it
	int len=str.size();
	fwrite(&len, sizeof(int), 1, f);
	
	// now write the string
	for (int i=0; i<str.size(); i++)
		fputc(str[i], f);
}

// read a string from file
Glib::ustring IO::read_string(FILE *f) {
	// read string length
	int len;
	fread(&len, sizeof(int), 1, f);
	
	// read the string
	Glib::ustring str="";
	for (int i=0; i<len; i++)
		str+=(char) fgetc(f);
	
	return str;
}

// write a pixbuf to compressed, internal format
void IO::write_export_image(FILE *f, const Glib::RefPtr<Gdk::Pixbuf> &pixbuf) {
	char *buffer;
	gsize bsize;
	
	// options/keys for png buffer
	std::vector<Glib::ustring> ops; ops.push_back("compression");
	std::vector<Glib::ustring> keys; keys.push_back("9");
	
	// serialize the pixbuf to usable buffer
	pixbuf->save_to_buffer(buffer, bsize, "png", ops, keys);
	
	// write size
	int bytes=fwrite(&bsize, sizeof(unsigned int), 1, f);
	
	// write buffer
	bytes=fwrite(buffer, sizeof(char), bsize, f);
	if (bytes!=bsize) {
		g_message("Wrote/Size: %d/%d", bytes, bsize);
		perror("Write error");
	}
}

// write a pixbuf to file
void IO::write_pixbuf(FILE *f, const Glib::RefPtr<Gdk::Pixbuf> &pixbuf) {
	// get buffer attributes
	int w=pixbuf->get_width();
	int h=pixbuf->get_height();
	int bps=pixbuf->get_bits_per_sample();
	int rs=pixbuf->get_rowstride();
	int alpha=pixbuf->get_has_alpha();
	guint8 *pixels=pixbuf->get_pixels();
	
	// write attributes
	fwrite((int*) &w, sizeof(int), 1, f);
	fwrite((int*) &h, sizeof(int), 1, f);
	fwrite((int*) &bps, sizeof(int), 1, f);
	fwrite((int*) &rs, sizeof(int), 1, f);
	fwrite((int*) &alpha, sizeof(int), 1, f);
	
	fwrite((guint8*) pixels, rs*h, 1, f);
}

// read a pixbuf from file
Glib::RefPtr<Gdk::Pixbuf> IO::read_pixbuf(FILE *f) {
	// buffer attributes
	int w, h, bps, rs, alpha;
	size_t size;
	guint8 *pixels;
	
	// read attributes
	fread((int*) &w, sizeof(int), 1, f);
	fread((int*) &h, sizeof(int), 1, f);
	fread((int*) &bps, sizeof(int), 1, f);
	fread((int*) &rs, sizeof(int), 1, f);
	fread((int*) &alpha, sizeof(int), 1, f);
	
	// calculate buffer length
	long buflen=rs*h;
	pixels=(guint8*) malloc(buflen);
	
	// read in the buffer
	fread((guint8*) pixels, buflen, 1, f);
	
	// create pixbuf from given data
	Glib::RefPtr<Gdk::Pixbuf> pixbuf=Gdk::Pixbuf::create_from_data(pixels, Gdk::COLORSPACE_RGB, alpha, bps, w, h, rs);
	
	return pixbuf;
}

// add a file to the recent files record
void IO::add_recent_file(const Glib::ustring &uri, const Glib::ustring &display) {
	// try to read the recent files record file
	std::vector<StringPair> vec;
	if (!IO::read_recent_files(vec)) {
		// create the file
		FILE *f=fopen("editor.rfs", "wb");
		if (!f) {
			g_warning("Couldn't create editor.rfs file!");
			return;
		}
		
		// write a single amount
		int amount=1;
		fwrite(&amount, sizeof(int), 1, f);
		
		// write info
		write_string(f, uri);
		write_string(f, display);
		
		// that's all
		fclose(f);
	}
	
	// we opened the record
	else {
		// add this as the most recent file
		vec.push_back(std::make_pair<Glib::ustring, Glib::ustring> (uri, display));
		
		// now, reopen the recent file store
		FILE *f=fopen("editor.rfs", "wb");
		if (!f) {
			g_warning("Unable to reopen recent file store for addition of new file!");
			return;
		}
		
		// write amount of files (cap to 5)
		int amount=(vec.size()>5 ? 5 : vec.size());
		fwrite(&amount, sizeof(int), 1, f);
		
		// iterate over our files
		for (int i=amount-1; i>=0; i--) {
			write_string(f, vec[i].first);
			write_string(f, vec[i].second);
		}
		
		fclose(f);
	}
}

// read the recent files record
IO::Code IO::read_recent_files(std::vector<StringPair> &vec) {
	Glib::ustring rpath=Utils::FS::cwd();
	rpath+="editor.rfs";
	FILE *f=fopen(rpath.c_str(), "rb");
	if (!f)
		return IO::CODE_OPEN_FAILED;
	
	// read amount of files
	int amount;
	fread(&amount, sizeof(int), 1, f);
	
	// iterate over amount of files
	for (int i=0; i<amount; i++) {
		// five files max
		if (i>5)
			break;
		
		// read info
		Glib::ustring uri=read_string(f);
		Glib::ustring display=read_string(f);
		
		// see if this file still exists
		FILE *f=fopen(uri.c_str(), "rb");
		if (!f)
			continue;
		fclose(f);
		
		// and add it
		vec.push_back(std::make_pair<Glib::ustring, Glib::ustring> (uri, display));
	}
	
	fclose(f);
	return IO::CODE_OK;
}

// unpack the resource file
IO::Code IO::unpack_resource_file(const Glib::ustring &file) {
	// before anything, validate this resource file
	FILE *f=fopen(file.c_str(), "rb");
	if (!f)
		return IO::CODE_OPEN_FAILED;
	
	// calculate the size of the file
	fseek(f, 0, SEEK_END);
	long size=ftell(f);
	rewind(f);

	fclose(f);
	
	// first, create a new archive and entry
	struct archive *ar=archive_read_new();
	if (!ar) {
		g_message("Failed to create handle");
		return IO::CODE_OPEN_FAILED;
	}
	
	// the file is in gzip'd tar format
	archive_read_support_compression_gzip(ar);
	archive_read_support_format_tar(ar);
	
	// try to open the archive from our file
	if (archive_read_open_file(ar, file.c_str(), 1024)!=ARCHIVE_OK)
		return IO::CODE_OPEN_FAILED;
		
	struct archive_entry *entry=archive_entry_new();
	
	// make our temporary directory
	system("mkdir .temp");
	
	// windows is a jerk and doesn't hide by prefixed dot, so we
	// need to set some attributes
#ifdef __WIN32__
	Glib::ustring wincmd=Glib::ustring("attrib +h +s ")+Utils::FS::cwd()+".temp";
	system(wincmd.c_str());
#endif
	
	// iterate over files in archive
	while(archive_read_next_header(ar, &entry)==ARCHIVE_OK) {
		// and extract this file temporarily
		archive_read_extract(ar, entry, 0);
		
		// move the file
		Glib::ustring to=Utils::FS::cwd()+Glib::ustring(".temp/")+archive_entry_pathname(entry);
		
		Utils::FS::move(Utils::FS::cwd()+archive_entry_pathname(entry), to);
		
		archive_read_data_skip(ar);
	}
	
	// we're done
	archive_read_finish(ar);
	fclose(f);
	
	return IO::CODE_OK;
}

// read icons from a theme file
IO::Code IO::read_icons_from_file(IconMap &icons) {
	// open the temporary file directory
	DIR *dir=opendir(Glib::ustring(Utils::FS::cwd()+".temp").c_str());
	if (!dir)
		return IO::CODE_OPEN_FAILED;
	
	// iterate over each entry
	struct dirent *entry;
	while((entry=readdir(dir))) {
		// get the original name of the file
		Glib::ustring fname=Glib::ustring(entry->d_name);
		
		// check to make sure this is an icon
		if (fname.size()<5 || fname.substr(fname.size()-5, 5)!=".pngG")
			continue;
		
		// create a pixbuf from this file
		Glib::ustring real=Utils::FS::cwd()+".temp/";
		real+=fname;
		Glib::RefPtr<Gdk::Pixbuf> pixbuf=Gdk::Pixbuf::create_from_file(real);
		
		// add this icon
		if (pixbuf)
			icons[fname]=pixbuf;
		else
			g_message("IO: unable to create pixbuf for icon '%s' from file!", fname.c_str());
	}
	
	// close this directory
	closedir(dir);
	
	return IO::CODE_OK;
}
