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

#include "iohandler.h"

// save a case and its associated data to file
bool IO::save_case_to_file(const Glib::ustring &path, const Case::Case &pcase,
			   const BufferMap &buffers,
			   std::map<Glib::ustring, Glib::ustring> &bufferDescriptions) {
	// open the requested file
	FILE *f=fopen(path.c_str(), "wb");
	if (!f)
		return false;
	
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
	
	// write initial block id
	write_string(f, pcase.get_initial_block_id());
	
	// get character data and write the amount of objects
	std::map<Glib::ustring, Character> characters=pcase.get_characters();
	int charCount=characters.size();
	fwrite(&charCount, sizeof(int), 1, f);
	
	// iterate over character data
	for (CharacterMapIter it=characters.begin(); it!=characters.end(); ++it) {
		// write internal name
		write_string(f, (*it).second.get_internal_name());
		
		// write displayed name
		write_string(f, (*it).second.get_name());
		
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
	for (BackgroundMapIter it=backgrounds.begin(); it!=backgrounds.end(); ++it) {
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
	for (EvidenceMapIter it=evidence.begin(); it!=evidence.end(); ++it) {
		// write id
		write_string(f, (*it).second.id);
		
		// write name
		write_string(f, (*it).second.name);
		
		// write caption
		write_string(f, (*it).second.caption);
		
		// write description
		write_string(f, (*it).second.description);
		
		// write pixbuf
		write_pixbuf(f, (*it).second.pixbuf);
	}
	
	// get location map and write amount of objects
	LocationMap locations=pcase.get_locations();
	int locationCount=locations.size();
	fwrite(&locationCount, sizeof(int), 1, f);
	
	// iterate over locations
	for (LocationMapIter it=locations.begin(); it!=locations.end(); ++it) {
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
	
	// write count of blocks
	int bufferCount=buffers.size();
	fwrite(&bufferCount, sizeof(int), 1, f);
	
	// iterate over text blocks
	int i=0;
	for (BufferMap::const_iterator it=buffers.begin(); it!=buffers.end(); ++it) {
		// write buffer id
		write_string(f, (*it).first);
		
		// write mapped buffer description
		Glib::ustring bd=bufferDescriptions[(*it).first];
		write_string(f, bd);
		
		// get text and write it to file
		Glib::ustring bufText=(*it).second->get_text(true);
		write_string(f, bufText);
	}
	
	// wrap up
	fclose(f);
	return true;
}

// export a case to file
bool IO::export_case_to_file(const Glib::ustring &path, const Case::Case &pcase, const BufferMap &buffers) {
	// open the requested file
	FILE *f=fopen(path.c_str(), "wb");
	if (!f)
		return false;
	
	// first and foremost, we write the file information
	write_string(f, FILE_MAGIC_NUM);
	fwrite(&FILE_VERSION, sizeof(int), 1, f);
	
	// get case overview
	Case::Overview overview=pcase.get_overview();
	
	// write overview details
	write_string(f, overview.name);
	write_string(f, overview.author);
	fwrite(&overview.lawSys, sizeof(int), 1, f);
	
	// write initial block id
	write_string(f, pcase.get_initial_block_id());
	
	// get character data and write the amount of objects
	std::map<Glib::ustring, Character> characters=pcase.get_characters();
	int charCount=characters.size();
	fwrite(&charCount, sizeof(int), 1, f);
	
	// iterate over character data
	for (CharacterMapIter it=characters.begin(); it!=characters.end(); ++it) {
		// write internal name
		write_string(f, (*it).second.get_internal_name());
		
		// write displayed name
		write_string(f, (*it).second.get_name());
		
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
			write_bmp(f, (*it).second.get_text_box_tag());
		
		// write headshot existance
		bool hasHeadshot=(*it).second.has_headshot();
		fwrite(&hasHeadshot, sizeof(bool), 1, f);
		
		// see if there is a headshot
		if (hasHeadshot) {
			// write the actual 70x70 headshot
			write_bmp(f, (*it).second.get_headshot());
			
			// create a scaled headshot and write it
			Glib::RefPtr<Gdk::Pixbuf> scaled=(*it).second.get_headshot()->scale_simple(40, 40, Gdk::INTERP_HYPER);
			write_bmp(f, scaled);
		}
	}
	
	// get background map and write the amount of objects
	BackgroundMap backgrounds=pcase.get_backgrounds();
	int bgCount=backgrounds.size();
	fwrite(&bgCount, sizeof(int), 1, f);
	
	// iterate over backgrounds
	for (BackgroundMapIter it=backgrounds.begin(); it!=backgrounds.end(); ++it) {
		// write id
		write_string(f, (*it).second.id);
		
		// write type
		fwrite(&(*it).second.type, sizeof(int), 1, f);
		
		// write bitmap
		write_bmp(f, (*it).second.pixbuf);
	}
	
	// get evidence map and write the amount of objects
	EvidenceMap evidence=pcase.get_evidence();
	int evidenceCount=evidence.size();
	fwrite(&evidenceCount, sizeof(int), 1, f);
	
	// iterate over evidence
	for (EvidenceMapIter it=evidence.begin(); it!=evidence.end(); ++it) {
		// write id
		write_string(f, (*it).second.id);
		
		// write name
		write_string(f, (*it).second.name);
		
		// write caption
		write_string(f, (*it).second.caption);
		
		// write description
		write_string(f, (*it).second.description);
		
		// write bitmap
		write_bmp(f, (*it).second.pixbuf);
		
		// create a scaled thumbnail and write it as well
		Glib::RefPtr<Gdk::Pixbuf> thumb=(*it).second.pixbuf->scale_simple(40, 40, Gdk::INTERP_HYPER);
		write_bmp(f, thumb);
	}
	
	// get location map and write amount of objects
	LocationMap locations=pcase.get_locations();
	int locationCount=locations.size();
	fwrite(&locationCount, sizeof(int), 1, f);
	
	// iterate over locations
	for (LocationMapIter it=locations.begin(); it!=locations.end(); ++it) {
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
	
	// write count of blocks
	int bufferCount=buffers.size();
	fwrite(&bufferCount, sizeof(int), 1, f);
	
	// iterate over text blocks
	int i=0;
	for (BufferMap::const_iterator it=buffers.begin(); it!=buffers.end(); ++it) {
		// write buffer id
		write_string(f, (*it).first);
		
		// get text for this buffer
		Glib::ustring bufText=(*it).second->get_text(true);
		
		// write the text
		write_string(f, bufText);
	}
	
	// wrap up
	fclose(f);
	return true;
}

// load a case from file
bool IO::load_case_from_file(const Glib::ustring &path, Case::Case &pcase,
			     BufferMap &buffers,
			     std::map<Glib::ustring, Glib::ustring> &bufferDescriptions) {
	// open requested file
	FILE *f=fopen(path.c_str(), "rb");
	if (!f)
		return false;
	
	// read magic number and verify it
	if (fgetc(f)!='C' || fgetc(f)!='P' ||
	    fgetc(f)!='R' || fgetc(f)!='J' ||
	    fgetc(f)!='T') {
		g_message("Incorrect magic number.");
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
	overview.name=read_string(f);
	overview.author=read_string(f);
	fread(&overview.lawSys, sizeof(int), 1, f);
	
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
		
		// read pixbuf data
		evidence.pixbuf=read_pixbuf(f);
		
		// add this evidence
		pcase.add_evidence(evidence);
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
	
	// read amount of text blocks
	int bufferCount;
	fread(&bufferCount, sizeof(int), 1, f);
	
	// iterate over text blocks
	for (int i=0; i<bufferCount; i++) {
		// read id
		Glib::ustring bufferId=read_string(f);
		
		// read description
		Glib::ustring bufferDescription=read_string(f);
		bufferDescriptions[bufferId]=bufferDescription;
		
		// read text contents
		Glib::ustring contents=read_string(f);
		
		// allocate new text buffer and set the text contents
		Glib::RefPtr<Gtk::TextBuffer> tbuffer=Gtk::TextBuffer::create();
		tbuffer->set_text(contents);
		
		// append this text buffer to the map
		buffers[bufferId]=tbuffer;
	}
	
	// wrap up
	fclose(f);
	return true;
}

// save a sprite to file
bool IO::save_sprite_to_file(const Glib::ustring &path, const Sprite &spr) {
	// open the requested file
	FILE *f=fopen(path.c_str(), "wb");
	if (!f)
		return false;
	
	// write file header
	write_string(f, SPR_MAGIC_NUM);
	fwrite(&SPR_VERSION, sizeof(int), 1, f);
	
	// get sprite animations
	AnimationMap animations=spr.get_animations();
	
	// write count of animations
	int count=animations.size();
	fwrite(&count, sizeof(int), 1, f);
	
	// iterate over animations
	for (AnimationMapIter it=animations.begin(); it!=animations.end(); ++it) {
		Animation anim=(*it).second;
		
		// write id
		write_string(f, anim.id);
		
		// write amount of frames
		int fcount=anim.frames.size();
		fwrite(&fcount, sizeof(int), 1, f);
		
		// iterate over frames
		for (int i=0; i<fcount; i++) {
			fwrite(&anim.frames[i].time, sizeof(int), 1, f);
			write_pixbuf(f, anim.frames[i].pixbuf);
		}
	}
	
	// wrap up
	fclose(f);
	return true;
}

// export a sprite to file
bool IO::export_sprite_to_file(const Glib::ustring &path, const Sprite &spr) {
	// open the requested file
	FILE *f=fopen(path.c_str(), "wb");
	if (!f)
		return false;
	
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
	
	// iterate over animations
	for (AnimationMapIter it=animations.begin(); it!=animations.end(); ++it) {
		Animation anim=(*it).second;
		
		// write id
		write_string(f, anim.id);
		
		// write amount of frames
		int fcount=anim.frames.size();
		fwrite(&fcount, sizeof(int), 1, f);
		
		// iterate over frames
		for (int i=0; i<fcount; i++) {
			// write frame time
			fwrite(&anim.frames[i].time, sizeof(int), 1, f);
			
			// write image
			write_bmp(f, anim.frames[i].pixbuf);
		}
	}
	
	// wrap up
	fclose(f);
	return true;
}

// load a sprite from file
bool IO::load_sprite_from_file(const Glib::ustring &path, Sprite &spr) {
	// open requested file
	FILE *f=fopen(path.c_str(), "rb");
	if (!f)
		return false;
	
	// read magic number and verify it
	Glib::ustring mn=read_string(f);
	if (mn!=SPR_MAGIC_NUM)
		return false;
	
	// read version and verify it
	int version;
	fread(&version, sizeof(int), 1, f);
	if (version!=SPR_VERSION)
		return false;
	
	// read amount of animations
	int count;
	fread(&count, sizeof(int), 1, f);
	
	// iterate over animations
	for (int i=0; i<count; i++) {
		Animation anim;
		
		// read id
		anim.id=read_string(f);
		
		// read amount of frames
		int fcount;
		fread(&fcount, sizeof(int), 1, f);
		
		// read in frames
		for (int fr=0; fr<fcount; fr++) {
			Frame fr;
			
			// read time
			fread(&fr.time, sizeof(int), 1, f);
			
			// read pixbuf
			fr.pixbuf=read_pixbuf(f);
			
			anim.frames.push_back(fr);
		}
		
		// add animation to sprite
		spr.add_animation(anim);
	}
	
	// wrap up
	fclose(f);
	return true;
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

// write a pixbuf in bmp format to file
void IO::write_bmp(FILE *f, const Glib::RefPtr<Gdk::Pixbuf> &pixbuf) {
	// flip the buffer vertically
	Glib::RefPtr<Gdk::Pixbuf> flipped=pixbuf->flip(false);
	char *buffer;
	gsize bsize;
	flipped->save_to_buffer(buffer, bsize, "bmp");
	
	// write buffer size
	int bytes=fwrite(&bsize, sizeof(gsize), 1, f);
	
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
	fwrite((guint8*) pixels, w*h*bps, 1, f);
}

// read a pixbuf from file
Glib::RefPtr<Gdk::Pixbuf> IO::read_pixbuf(FILE *f) {
	// buffer attributes
	int w, h, bps, rs, alpha;
	guint8 *pixels;
	
	// read attributes
	fread((int*) &w, sizeof(int), 1, f);
	fread((int*) &h, sizeof(int), 1, f);
	fread((int*) &bps, sizeof(int), 1, f);
	fread((int*) &rs, sizeof(int), 1, f);
	fread((int*) &alpha, sizeof(int), 1, f);
	
	// calculate buffer length
	long buflen=w*h*bps;
	pixels=(guint8*) malloc(buflen);
	
	// read in the buffer
	fread((guint8*) pixels, buflen, 1, f);
	
	// create pixbuf from given data
	Glib::RefPtr<Gdk::Pixbuf> pixbuf=Gdk::Pixbuf::create_from_data(pixels, Gdk::COLORSPACE_RGB, alpha, bps, w, h, rs);
	
	return pixbuf;
}
