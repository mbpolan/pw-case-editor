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
// case.h: various Case related classes

#ifndef CASE_H
#define CASE_H

#include <map>
#include <vector>
#include "SDL.h"
#include "SDL_mixer.h"

#include "character.h"
#include "common.h"

namespace Case {

// law system for cases to follow
enum LawSystem { TWO_DAY=0, THREE_DAY };

// background types
enum BackgroundType { BG_SINGLE_SCREEN=0, BG_DOUBLE_SCREEN };

// general case data and technical info
struct _Overview {
	ustring name; // the name of the case (Turnabout Whatever, etc)
	ustring author; // who wrote this case and its contents
	LawSystem lawSys; // the law system (how many days for a trial)
};
typedef struct _Overview Overview;

// user-defined overrides
struct _Overrides {
	int textboxAlpha; // alpha value for text box
	ustring titleScreen; // custom title screen image
};
typedef struct _Overrides Overrides;

// background data
struct _Background {
	ustring id; // id referenced from within the script
	BackgroundType type; // type of background (spans 2 screens or 1)
	SDL_Surface *texture; // texture id
};
typedef struct _Background Background;

// evidence image/item data
struct _Evidence {
	ustring id; // id referenced from within the script
	ustring name; // name of this piece of evidence
	ustring caption; // caption displaying info about evidence
	ustring description; // string describing the image
	ustring checkID; // image id for check button
	SDL_Surface *texture; // texture id
	SDL_Surface *thumb; // scaled thumbnail for evidence window
};
typedef struct _Evidence Evidence;

// an examinable area in a location
struct _Hotspot {
	Rect rect;
	ustring block; // the block to execute if examined
};
typedef struct _Hotspot Hotspot;

// location data
struct _Location {
	ustring id; // id referenced from within the script
	ustring name; // name of this location
	ustring triggerBlock; // block to execute upon next arrival at location
	
	ustring character; // character located here
	ustring music; // music being played at this location
	
	ustring bg; // background id referenced from within script
	SDL_Surface *bgScaled; // scaled background image
	
	std::vector<Hotspot> hotspots; // vector of examinable hotspots
	std::vector<ustring> moveLocations; // ids of locations that player can move to from here
};
typedef struct _Location Location;

// arbitrary image
struct _Image {
	ustring id; // id referenced from within the script
	SDL_Surface *texture; // texture image itself
};
typedef struct _Image Image;

// a single piece of testimony
struct _TestimonyPiece {
	ustring text;
	ustring presentId;
	ustring presentBlock;
	ustring pressBlock;
	bool hidden;
};
typedef struct _TestimonyPiece TestimonyPiece;

// a single testimony from a character
struct _Testimony {
	// id of this testimony
	ustring id;
	
	// testimony title
	ustring title;
	
	// speaker
	ustring speaker;
	
	// next block to follow upon completion
	ustring nextBlock;
	ustring followLocation;
	
	// next block to follow upon cross examination end
	ustring xExamineEndBlock;
	
	// vector of testimony pieces
	std::vector<TestimonyPiece> pieces;
};
typedef struct _Testimony Testimony;

}; // namespace Case

// typedefs for cleaner code
typedef std::map<ustring, Character> CharacterMap;
typedef std::map<ustring, Case::Background> BackgroundMap;
typedef std::map<ustring, Case::Evidence> EvidenceMap;
typedef std::map<ustring, Case::Image> ImageMap;
typedef std::map<ustring, Case::Location> LocationMap;
typedef std::map<ustring, Case::Testimony> TestimonyMap;
typedef std::map<ustring, ustring> BufferMap;

typedef std::vector<ustring> StringVector;

namespace Case {

// core class that holds all pertinent data about a case
// this class should also be reused as-is in the engine
class Case {
	public:
		// constructor
		Case();
		
		// destructor
		~Case();
		
		// set the case overrides
		void setOverrides(const Overrides &ov) { m_Overrides=ov; }
		
		// get the case overrides
		Overrides getOverrides() const { return m_Overrides; }
		
		// set the initial text block id
		void setInitialBlockId(const ustring &id) { m_InitialBlockId=id; }
		
		// get the initial text block id
		ustring getInitialBlockId() const { return m_InitialBlockId; }
		
		// add a character
		void addCharacter(const Character &character);
		
		// add a background
		void addBackground(const Background &bg);
		
		// add a piece of evidence
		void addEvidence(const Evidence &evidence);
		
		// add an image
		void addImage(const Image &image);
		
		// add a location
		void addLocation(const Location &loc);
		
		// add a testimony
		void addTestimony(const Testimony &testimony);
		
		// add a text buffer
		void addBuffer(const ustring &id, const ustring &contents);
		
		// get a character
		Character* getCharacter(const ustring &id);
		
		// get a background
		Background* getBackground(const ustring &id);
		
		// get a piece of evidence
		Evidence* getEvidence(const ustring &id);
		
		// get an image
		Image* getImage(const ustring &id);
		
		// get a location
		Location* getLocation(const ustring &id);
		
		// get a testimony
		Testimony* getTestimony(const ustring &id);
		
		// return a vector of evidence based on vector of string ids
		std::vector<Evidence*> getEvidenceFromIds(const StringVector &vec);
		
		// return a vector of characters based on vector of string ids
		std::vector<Character*> getCharactersFromIds(const StringVector &vec);
		
		// clear the entire case information
		void clear();
		
		// set overview
		void setOverview(const Overview &overview);
		
		// return the current case overview
		Overview getOverview() const { return m_Overview; }
		
		// return full map of characters
		CharacterMap getCharacters() const { return m_Characters; }
		
		// return full map of backgrounds
		BackgroundMap getBackgrounds() const { return m_Backgrounds; }
		
		// return full map of evidence
		EvidenceMap getEvidence() const { return m_Evidence; }
		
		// return full map of images
		ImageMap getImages() const { return m_Images; }
		
		// return a full map of locations
		LocationMap getLocations() const { return m_Locations; }
		
		// return a full map of testimonies
		TestimonyMap getTestimonies() const { return m_Testimonies; }
		
		// return map of buffers
		BufferMap getBuffers() const { return m_Buffers; }
	
	private:
		// overrides
		Overrides m_Overrides;
		
		// general case data
		Overview m_Overview;
		
		// initial text block that will be displayed when the case starts
		ustring m_InitialBlockId;
		
		// map of characters
		CharacterMap m_Characters;
		
		// map of backgrounds
		BackgroundMap m_Backgrounds;
		
		// map of evidence
		EvidenceMap m_Evidence;
		
		// map of images
		ImageMap m_Images;
		
		// map of locations
		LocationMap m_Locations;
		
		// map of testimonies
		TestimonyMap m_Testimonies;
		
		// map of buffers
		BufferMap m_Buffers;
};

}; // namespace Case

#endif
