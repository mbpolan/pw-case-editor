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

namespace Case {

// law system for cases to follow
enum LawSystem { TWO_DAY=0, THREE_DAY };

// background types
enum BackgroundType { BG_SINGLE_SCREEN=0, BG_DOUBLE_SCREEN };

// general case data and technical info
struct _Overview {
	std::string name; // the name of the case (Turnabout Whatever, etc)
	std::string author; // who wrote this case and its contents
	LawSystem lawSys; // the law system (how many days for a trial)
};
typedef struct _Overview Overview;

// background data
struct _Background {
	std::string id; // id referenced from within the script
	BackgroundType type; // type of background (spans 2 screens or 1)
	SDL_Surface *texture; // texture id
};
typedef struct _Background Background;

// evidence image/item data
struct _Evidence {
	std::string id; // id referenced from within the script
	std::string name; // name of this piece of evidence
	std::string caption; // caption displaying info about evidence
	std::string description; // string describing the image
	SDL_Surface *texture; // texture id
	SDL_Surface *thumb; // scaled thumbnail for evidence window
};
typedef struct _Evidence Evidence;

// an examinable area in a location
struct _Hotspot {
	int x;
	int y;
	int w;
	int h;
	
	std::string block; // the block to execute if examined
};
typedef struct _Hotspot Hotspot;

// location data
struct _Location {
	std::string id; // id referenced from within the script
	std::string name; // name of this location
	std::string triggerBlock; // block to execute upon next arrival at location
	
	std::string character; // character located here
	std::string music; // music being played at this location
	
	std::string bg; // background id referenced from within script
	SDL_Surface *bgScaled; // scaled background image
	
	std::vector<Hotspot> hotspots; // vector of examinable hotspots
	std::vector<std::string> moveLocations; // ids of locations that player can move to from here
};
typedef struct _Location Location;

// arbitrary image
struct _Image {
	std::string id; // id referenced from within the script
	SDL_Surface *texture; // texture image itself
};
typedef struct _Image Image;

// a single piece of testimony
struct _TestimonyPiece {
	std::string text;
	std::string presentId;
	std::string presentBlock;
	std::string pressBlock;
	bool hidden;
};
typedef struct _TestimonyPiece TestimonyPiece;

// a single testimony from a character
struct _Testimony {
	// id of this testimony
	std::string id;
	
	// speaker
	std::string speaker;
	
	// vector of testimony pieces
	std::vector<TestimonyPiece> pieces;
};
typedef struct _Testimony Testimony;

}; // namespace Case

// typedefs for cleaner code
typedef std::map<std::string, Character> CharacterMap;
typedef std::map<std::string, Case::Background> BackgroundMap;
typedef std::map<std::string, Case::Evidence> EvidenceMap;
typedef std::map<std::string, Case::Image> ImageMap;
typedef std::map<std::string, Case::Location> LocationMap;
typedef std::map<std::string, Case::Testimony> TestimonyMap;
typedef std::map<std::string, std::string> BufferMap;

typedef std::vector<std::string> StringVector;

namespace Case {

// core class that holds all pertinent data about a case
// this class should also be reused as-is in the engine
class Case {
	public:
		// constructor
		Case();
		
		// destructor
		~Case();
		
		// set the initial text block id
		void setInitialBlockId(const std::string &id) { m_InitialBlockId=id; }
		
		// get the initial text block id
		std::string getInitialBlockId() const { return m_InitialBlockId; }
		
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
		void addBuffer(const std::string &id, const std::string &contents);
		
		// get a character
		Character* getCharacter(const std::string &id);
		
		// get a background
		Background* getBackground(const std::string &id);
		
		// get a piece of evidence
		Evidence* getEvidence(const std::string &id);
		
		// get an image
		Image* getImage(const std::string &id);
		
		// get a location
		Location* getLocation(const std::string &id);
		
		// get a testimony
		Testimony* getTestimony(const std::string &id);
		
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
		// general case data
		Overview m_Overview;
		
		// initial text block that will be displayed when the case starts
		std::string m_InitialBlockId;
		
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
