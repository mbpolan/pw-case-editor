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
#include <iomanip>
#include <iostream>
#include <libxml/parser.h>
#include <sstream>
#include "SDL_image.h"
#include "SDL_rotozoom.h"

#include "audio.h"
#include "font.h"
#include "iohandler.h"
#include "utilities.h"

// unpack the resource file
bool IO::unpackResourceFile(const ustring &path) {
	FILE *f=fopen(path.c_str(), "rb");
	if (!f) {
		Utils::alert("Unable to open resource file: "+path);
		return false;
	}
	
	// create a new archive handle
	struct archive *ar=archive_read_new();
	
	// add compression and tar support
	archive_read_support_compression_gzip(ar);
	archive_read_support_format_tar(ar);
	
	// open the archive
	if (archive_read_open_FILE(ar, f)!=ARCHIVE_OK) {
		fclose(f);
		return false;
	}
	
	// create a new file entry handle
	struct archive_entry *entry=archive_entry_new();
	
	// make our temporary directory
	Utils::FS::makeDir(".temp");
	
	// read each file, and extract it
	while(archive_read_next_header(ar, &entry)==ARCHIVE_OK) {
		archive_read_extract(ar, entry, 0);
		archive_read_data_skip(ar);
	}
	
	// windows needs to have an absolute path
	ustring to=".temp";
#ifdef __WIN32__
	to+="\\data";
#endif
	
	// move our data directory, to the hidden temporary directory
	Utils::FS::move("data", to);
	
	// wrap up
	archive_read_finish(ar);
	fclose(f);
	
	return true;
}

// load a case from file
bool IO::loadCaseFromFile(const ustring &path, Case::Case &pcase) {
	// open requested file
	FILE *f=fopen(path.c_str(), "rb");
	if (!f)
		return false;
	
	// get the root path
	int npos;
#ifndef __WIN32__
	npos=path.rfind('/');
#else
	npos=path.rfind('\\');
#endif
	ustring root=path.substr(0, npos+1);
	
	// read the header
	PWTHeader header;
	fread(&header, sizeof(PWTHeader), 1, f);
	
	// compare magic number
	if (header.ident!=FILE_MAGIC_NUM) {
		fclose(f);
		return false;
	}
	
	// compare version
	if (header.version!=FILE_VERSION) {
		fclose(f);
		return false;
	}
	
	// create a new overview struct
	Case::Overview overview;
	
	// skip to overview
	fseek(f, header.overviewOffset, SEEK_SET);
	
	// read in data
	overview.name=readString(f);
	overview.author=readString(f);
	fread(&overview.lawSys, sizeof(int), 1, f);
	
	// read in core blocks
	for (int i=0; i<Case::Case::CORE_BLOCK_COUNT; i++)
		pcase.setCoreBlock(i, readString(f));
	
	// skip to overrides
	fseek(f, header.overridesOffset, SEEK_SET);
	
	// create a new overrides object
	Case::Overrides ov;
	
	// read data
	fread(&ov.textboxAlpha, sizeof(int), 1, f);
	ov.titleScreen=readString(f);
	
	// set overrides
	pcase.setOverrides(ov);
	
	// set the overview
	pcase.setOverview(overview);
	
	// read initial block id
	ustring initialBlock=readString(f);
	pcase.setInitialBlockId(initialBlock);
	
	// skip to characters
	fseek(f, header.charOffset, SEEK_SET);
	
	// read amount of characters
	int ucharCount;
	fread(&ucharCount, sizeof(int), 1, f);
	
	// read each character
	for (int i=0; i<ucharCount; i++) {
		Character character;
		ustring str;
		
		// read internal name
		str=readString(f);
		character.setInternalName(str);
		
		// read displayed name
		str=readString(f);
		character.setName(str);
		
		// read gender
		int gender;
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
		
		// if the sprite name is not invalid, try loading said sprite
		if (str!=STR_NULL && str!="") {
			// the sprite should be in the /spr directory, extension .pws
			ustring sprPath=root+"spr/"+str+".pws";
			
			// try to load it
			Sprite sprite;
			if (!IO::loadSpriteFromFile(sprPath, sprite))
				Utils::alert("Unable to load sprite for '"+character.getInternalName()+"': '"+sprPath+"'", Utils::MESSAGE_WARNING);
			
			else
				character.setSprite(sprite);
		}
		
		// see if this character has a text box tag
		bool tag;
		fread(&tag, sizeof(bool), 1, f);
		character.setHasTextBoxTag(tag);
		
		// if the tag exists, read the image
		if (tag) {
			GLuint texTag=Textures::createTexture(STR_NULL, readImage(f), 225);
			character.setTextBoxTag(texTag);
		}
		
		// see if this character has a headshot image
		bool headshot;
		fread(&headshot, sizeof(bool), 1, f);
		character.setHasHeadshot(headshot);
		
		// if the headshot exists, read the image
		if (headshot) {
			// read full image
			GLuint headshot=Textures::createTexture(STR_NULL, readImage(f));
			
			// read scaled thumbnail
			GLuint thumb=Textures::createTexture(STR_NULL, readImage(f));
			
			character.setHeadshot(headshot, thumb);
		}
		
		// include this character
		pcase.addCharacter(character);
	}
	
	// skip to background
	fseek(f, header.bgOffset, SEEK_SET);
	
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
	
	// skip to evidence
	fseek(f, header.evidenceOffset, SEEK_SET);
	
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
		evidence.texture=Textures::createTexture(STR_NULL, readImage(f));
		
		// read thumbnail data
		evidence.thumb=Textures::createTexture(STR_NULL, readImage(f));
		
		// add this evidence
		pcase.addEvidence(evidence);
	}
	
	// skip to images
	fseek(f, header.imgOffset, SEEK_SET);
	
	// read amount of images
	int imageCount;
	fread(&imageCount, sizeof(int), 1, f);
	
	// iterate over images
	for (int i=0; i<imageCount; i++) {
		Case::Image img;
		
		// read id
		img.id=readString(f);
		
		// read image
		img.texture=Textures::createTexture(STR_NULL, readImage(f));
		
		// add this image
		pcase.addImage(img);
	}
	
	// skip to locations
	fseek(f, header.locationOffset, SEEK_SET);
	
	// read amount of locations
	int locationCount;
	fread(&locationCount, sizeof(int), 1, f);
	
	// iterate over locations
	for (int i=0; i<locationCount; i++) {
		Case::Location location;
		location.triggerBlock=STR_NULL;
		location.character=STR_NULL;
		location.music=STR_NULL;
		location.state="default";
		
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
			
			// read area
			int x, y, w, h;
			fread(&x, sizeof(int), 1, f);
			fread(&y, sizeof(int), 1, f);
			fread(&w, sizeof(int), 1, f);
			fread(&h, sizeof(int), 1, f);
			hspot.rect=Rect(Point(x, y), w, h+197);
			
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
			// read the state id
			ustring state=readString(f);
			
			// and then the background id
			ustring id=readString(f);
			
			location.states[state]=id;
		}
		
		// get the background image in question
		if (pcase.getBackground(location.states["default"])) {
			Case::Background *bg=pcase.getBackground(location.states["default"]);
			
			// FIXME
			// scale it
			//location.bgScaled=Textures::createTexture(zoomSurface(bg->texture, 0.3125, 0.3125, SMOOTHING_ON));
		}
		
		// add this location
		pcase.addLocation(location);
	}
	
	// skip to audio
	fseek(f, header.audioOffset, SEEK_SET);
	
	// read amount of audio samples
	int audioCount;
	fread(&audioCount, sizeof(int), 1, f);
	
	// iterate over audio
	for (int i=0; i<audioCount; i++) {
		Audio::Sample audio;
		
		// read id
		audio.id=readString(f);
		
		// read filename
		ustring afile=readString(f);
		
		// form full string and load the audio sample
		if (Audio::loadSample(root+"audio/"+afile, audio))
			Audio::pushAudio(audio.id, audio);
	}
	
	// skip to testimonies
	fseek(f, header.testimonyOffset, SEEK_SET);
	
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
		
		// read follow up location
		testimony.followLocation=readString(f);
		
		// read cross examination follow block
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
	
	// skip to blocks
	fseek(f, header.blockOffset, SEEK_SET);
	
	// read amount of text blocks
	int bufferCount;
	fread(&bufferCount, sizeof(int), 1, f);
	
	// iterate over text blocks
	for (int i=0; i<bufferCount; i++) {
		// read id
		ustring bufferId=readString(f);
		
		// read text contents
		ustring contents=readString(f);
		
		// append this text buffer to the map
		pcase.addBuffer(bufferId, contents);
	}
	
	// wrap up
	fclose(f);
	std::cout << "Done loading case.\n";
	return true;
}

// load a sprite from file
bool IO::loadSpriteFromFile(const ustring &path, Sprite &sprite) {
	// open the requested file
	FILE *f=fopen(path.c_str(), "rb");
	if (!f)
		return false;
	
	// read file header 
	if (fgetc(f)!=(char) 'P' || fgetc(f)!=(char) 'W' || fgetc(f)!=(char) 'S') {
		fclose(f);
		Utils::alert("Error loading sprite file: '"+path+"'\nReason: Unrecognized file format.");
		return false;
	}
	
	int version;
	fread(&version, sizeof(int), 1, f);
	if (version!=10) {
		fclose(f);
		Utils::alert("Error loading sprite file: '"+path+"'\nReason: Unsupported file version: "+Utils::itoa(version)+".");
		return false;
	}
	
	// read count of animations
	int count;
	fread(&count, sizeof(int), 1, f);
	
	if (Utils::g_IDebugOn)
		Utils::message("Loading sprite: ");
	
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
		
		// load over frames
		for (int j=0; j<fcount; j++) {
			Frame fr;
			
			// read time
			fread(&fr.time, sizeof(int), 1, f);
			
			// read sound effect
			fr.sfx=readString(f);
			
			// read image
			fr.image=Textures::createTexture(STR_NULL, readImage(f));
			
			// add this frame
			anim.frames.push_back(fr);
		}
		
		// add this animation
		sprite.addAnimation(anim);
		
		if (Utils::g_IDebugOn)
			Utils::message(Utils::itoa(count-(i+1))+" ");
	}
	
	if (Utils::g_IDebugOn)
		Utils::message("[done]\n");
	
	// wrap up
	fclose(f);
	return true;
}

// load stock assets config file
bool IO::loadStockFile(const ustring &path, Case::Case *pcase) {
	// open the path
	FILE *f=fopen(path.c_str(), "rb");
	if (!f) {
		Utils::alert("Unable to open stock assets file: '"+path+"'.");
		return false;
	}
	
	// read the lines in the file
	char line[256];
	while(!feof(f)) {
		// get this line
		fgets(line, 256, f);
		
		// ignore useless lines
		if (line[0]=='#')
			continue;
		
		// extract id and path
		char id[256], file[256];
		sscanf(line, "%s %s", id, file);
		
		// form a string objects
		ustring sId(id);
		ustring sFile(file);
		
		// see if this is a sound effect
		if (sId.substr(0, 4)=="sfx:") {
			// erase the identifier
			sId.erase(0, 4);
			
			// try to load this sample
			Audio::Sample sample;
			if (!Audio::loadSample(ustring(".temp/")+sFile, sample))
				return false;
			
			// add this sample
			Audio::pushAudio(sId, sample);
		}
		
		// sprite
		else if (sId.substr(0, 4)=="spr:") {
			// erase the identifier
			sId.erase(0, 4);
			
			// try to load the sprite
			Sprite sprite;
			if (!IO::loadSpriteFromFile(ustring(".temp/")+sFile, sprite)) {
				Utils::alert("Unable to load stock sprite: '.temp/"+sFile+"'");
				return false;
			}
			
			// set defaults
			sprite.setAnimation("normal_idle");
			
			// extract data from string
			StringVector vec=Utils::explodeString(',', sId);
			
			// create character struct
			Character character(vec[0], vec[1], vec[2], vec[3]);
			character.setSprite(sprite);
			
			// see if a text box tag is present
			if (vec[4]!=STR_NULL) {
				// load the tag
				GLuint tag=Textures::createTexture(STR_NULL, ".temp/data/stock/"+vec[4], 165);
				
				// set this tag
				character.setHasTextBoxTag(true);
				character.setTextBoxTag(tag);
			}
			
			// add this sprite
			pcase->addCharacter(character);
		}
		
		// location
		else if (sId.substr(0, 4)=="loc:") {
			// erase identifier
			sId.erase(0, 4);
			
			// create location struct
			Case::Location location;
			location.id=sId;
			location.state="default";
			location.states["default"]=sFile;
			location.triggerBlock=STR_NULL;
			location.character=STR_NULL;
			location.music=STR_NULL;
			
			// add this location
			pcase->addLocation(location);
		}
		
		// must be a texture otherwise
		else {
			// see if we should also add this as a background
			bool bg=false;
			if (sId[0]=='*') {
				bg=true;
				
				// erase the asterisk
				sId.erase(0, 1);
			}
			
			// see if there is a specified alpha
			int npos;
			int alpha=255;
			ustring rFile=sFile;
			if ((npos=sFile.find(','))!=-1) {
				rFile=sFile.substr(0, npos);
				ustring a=sFile.substr(npos+1, sFile.size());
				alpha=atoi(a.c_str());
			}
			
			// create a surface
			GLuint tex=Textures::createTexture(sId, ustring(".temp/")+rFile, alpha);
			if (Textures::isNull(tex) && sId!="no_texture")
				Utils::alert("Unable to create stock texture: "+sId);
			
			// add a background if needed
			if (bg) {
				Case::Background background;
				
				// fill in data
				background.id=sId;
				background.type=Case::BG_SINGLE_SCREEN;
				background.texture=tex;
				
				// and add it
				pcase->addBackground(background);
			}
		}
	}
	
	// close the file
	fclose(f);
	return true;
}

// load a translation file
bool IO::loadTranslationFile(const ustring &path, std::map<ustring, ustring> &map) {
	// open the file
	FILE *f=fopen(path.c_str(), "r");
	if (!f)
		return false;
	
	// iterate that many times and read all translations
	while(1) {
		// read a line of text
		char str[512];
		if (fgets(str, 512, f)==NULL)
			break;
		
		ustring gstr=str;
		
		// make sure that there is a translation on this line
		if (gstr.find('~')==-1 || gstr[0]=='#')
			continue;
		
		// extract the english key
		gstr.erase(0, 1);
		ustring key=gstr.substr(0, gstr.find('~')-1);
		gstr.erase(0, gstr.find('~')+2);
		
		// extract the translation
		ustring trs=gstr.substr(0, gstr.find('"'));
		
		// add this key
		map[key]=trs;
	}
	
	fclose(f);
	return true;
}

// load theme from xml
bool IO::loadThemeXML(const ustring &path, Theme::ColorMap &map) {
	xmlDocPtr doc=xmlParseFile(path.c_str());
	if (!doc) {
		Utils::alert("Unable to load theme XML file: '"+path+"'");
		return false;
	}
	
	// get our root element
	xmlNodePtr root=xmlDocGetRootElement(doc);
	xmlNodePtr child=root->children;
	
	// count how many nodes we've loaded
	int count=0;
	
	// iterate over the child nodes
	while(child) {
		// element node
		if (xmlStrcmp(child->name, (const xmlChar*) "element")==0) {
			// get the id of the item
			ustring id=(const char*) xmlGetProp(child, (const xmlChar*) "id");
			
			// get our rgba color
			int r=atoi((const char*) xmlGetProp(child, (const xmlChar*) "r"));
			int g=atoi((const char*) xmlGetProp(child, (const xmlChar*) "g"));
			int b=atoi((const char*) xmlGetProp(child, (const xmlChar*) "b"));
			
			// sometimes, a node may not have an alpha value
			int a=255;
			if (xmlHasProp(child, (const xmlChar*) "a"))
				a=atoi((const char*) xmlGetProp(child, (const xmlChar*) "a"));
			
			// add this color
			Theme::g_Theme[id]=Color(r, g, b, a);
			
			count++;
		}
		
		child=child->next;
	}
	
	xmlFreeDoc(doc);
	
	std::cout << "Loaded " << count << " element(s) from theme file.\n";
	
	return true;
}

// read image data from file
SDL_Surface* IO::readImage(FILE *f) {
	// read buffer size
	int size;
	fread(&size, sizeof(int), 1, f);
	
	// read buffer
	char *buffer=new char[size];
	fread(buffer, sizeof(char), size, f);
	
	// read in the jpeg from memory
	SDL_RWops *rw=SDL_RWFromMem(buffer, size);
	if (!rw) {
		Utils::alert("Error reading internal image: '"+ustring(SDL_GetError())+"'");
		return NULL;
	}
	
	// load our image from raw data (no need to free RWops structure)
	SDL_Surface *srf=IMG_Load_RW(rw, 1);
	if (!srf) {
		Utils::alert("Error loading internal image: '"+ustring(SDL_GetError())+"'");
		return NULL;
	}
	
	// we're done with the buffer itself; now SDL_RWops will handle
	// our data so we can forget about our originally read buffer
	delete [] buffer;
	
	return srf;
}

// read a string from file
ustring IO::readString(FILE *f) {
	// read string length
	int len;
	fread(&len, sizeof(int), 1, f);
	
	// read the string
	ustring str="";
	for (int i=0; i<len; i++) {
		// character is stored as 2 bytes, but then needs to be
		// expanded to 4 bytes to work with ustring
		gunichar ch;
		fread(&ch, sizeof(gunichar), 1, f);
		str+=ch;
	}
	
	return str;
}
