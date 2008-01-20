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

#include <gdkmm/pixbuf.h>
#include <glibmm/ustring.h>
#include <gtkmm/textbuffer.h>
#include <map>
#include <vector>

#include "character.h"

// a rectangle
struct _Rect {
	int x, y;
	int w, h;
};
typedef struct _Rect Rect;


// case namespace
namespace Case {

// law system for cases to follow
enum LawSystem { SINGLE_TRIAL=1, TWO_DAY, THREE_DAY };

// background types
enum BackgroundType { BG_SINGLE_SCREEN=0, BG_DOUBLE_SCREEN };

// general case data and technical info
struct _Overview {
	Glib::ustring name; // the name of the case (Turnabout Whatever, etc)
	Glib::ustring author; // who wrote this case and its contents
	LawSystem lawSys; // the law system (how many days for a trial)
};
typedef struct _Overview Overview;

// user-defined overrides
struct _Overrides {
	int textboxAlpha; // alpha value for text box
	Glib::ustring titleScreen; // custom title screen image
};
typedef struct _Overrides Overrides;

// background data
struct _Background {
	Glib::ustring id; // id referenced from within the script
	BackgroundType type; // type of background (spans 2 screens or 1)
	Glib::RefPtr<Gdk::Pixbuf> pixbuf; // pixbuf containing image data
};
typedef struct _Background Background;

// evidence image/item data
struct _Evidence {
	Glib::ustring id; // id referenced from within the script
	Glib::ustring name; // name of this piece of evidence
	Glib::ustring caption; // who this evidence was received from, etc
	Glib::ustring description; // string describing the image
	Glib::ustring checkID; // check image id
	Glib::RefPtr<Gdk::Pixbuf> pixbuf; // pixbuf containing image data
};
typedef struct _Evidence Evidence;

// location hotspot
struct _Hotspot {
	Rect rect; // the rectangle in which this hotspot resides
	Glib::ustring block; // block to parse when examined
};
typedef struct _Hotspot Hotspot;

// location data
struct _Location {
	Glib::ustring id; // id referenced from within the script
	Glib::ustring name; // name of this location
	Glib::ustring bg; // background id to show in locations list
	std::vector<Hotspot> hotspots; // examinable hotspots
};
typedef struct _Location Location;

// audio data
struct _Audio {
	Glib::ustring id; // id referenced from within the script
	Glib::ustring name; // name of file
};
typedef struct _Audio Audio;

// misc image
struct _Image {
	Glib::ustring id; // id referenced from within the script
	Glib::RefPtr<Gdk::Pixbuf> pixbuf; // image data
};
typedef struct _Image Image;

// a single piece of testimony
struct _TestimonyPiece {
	Glib::ustring text;
	Glib::ustring presentId;
	Glib::ustring presentBlock;
	Glib::ustring pressBlock;
	bool hidden;
};
typedef struct _TestimonyPiece TestimonyPiece;

// a single testimony from a character
struct _Testimony {
	// id of this testimony
	Glib::ustring id;
	
	// testimony title
	Glib::ustring title;
	
	// speaker
	Glib::ustring speaker;
	
	// block to execute following testimony completion
	Glib::ustring nextBlock;
	Glib::ustring followLoc;
	
	// block to execute if the cross examination is scrolled through
	Glib::ustring xExamineEndBlock;
	
	// vector of testimony pieces
	std::vector<TestimonyPiece> pieces;
};
typedef struct _Testimony Testimony;

}; // namespace Case

// typedefs for cleaner code
typedef std::map<Glib::ustring, Character> CharacterMap;
typedef std::map<Glib::ustring, Case::Background> BackgroundMap;
typedef std::map<Glib::ustring, Case::Evidence> EvidenceMap;
typedef std::map<Glib::ustring, Case::Location> LocationMap;
typedef std::map<Glib::ustring, Case::Audio> AudioMap;
typedef std::map<Glib::ustring, Case::Image> ImageMap;
typedef std::map<Glib::ustring, Case::Testimony> TestimonyMap;
typedef std::map<Glib::ustring, Glib::RefPtr<Gtk::TextBuffer> > BufferMap;
typedef std::vector<Glib::ustring> StringVector;
typedef std::pair<Glib::ustring, Glib::ustring> StringPair;

namespace Case {

// core class that holds all pertinent data about a case
// this class should also be reused as-is in the engine
class Case {
	public:
		// constructor
		Case();
		
		// set the case overrides
		void set_overrides(const Overrides &ov) { m_Overrides=ov; }
		
		// get the case overrides
		Overrides get_overrides() const { return m_Overrides; }
		
		// set the initial text block id
		void set_initial_block_id(const Glib::ustring &id) { m_InitialBlockId=id; }
		
		// get the initial text block id
		Glib::ustring get_initial_block_id() const { return m_InitialBlockId; }
		
		// add a character
		void add_character(const Character &character);
		
		// remove a character based on name
		void remove_character(const Glib::ustring &name);
		
		// add an image
		void add_image(const Image &image);
		
		// remove an image
		void remove_image(const Glib::ustring &id);
		
		// add testimony
		void add_testimony(const Testimony &testimony);
		
		// remove testimony
		void remove_testimony(const Glib::ustring &id);
		
		// add a background
		void add_background(const Background &bg);
		
		// remove a background based on id
		void remove_background(const Glib::ustring &id);
		
		// add a piece of evidence
		void add_evidence(const Evidence &evidence);
		
		// remove a piece of evidence based on id
		void remove_evidence(const Glib::ustring &id);
		
		// add a location
		void add_location(const Location &loc);
		
		// add an audio sample
		void add_audio(const Audio &audio);
		
		// remove a location based on id
		void remove_location(const Glib::ustring &id);
		
		// return a vector of character internal names
		StringVector get_character_names();
		
		// return a vector of image internal ids
		StringVector get_image_ids();
		
		// return a vector of background internal ids
		StringVector get_background_ids();
		
		// return a vector of evidence internal ids
		StringVector get_evidence_ids();
		
		// return a vector of location internal ids
		StringVector get_location_ids();
		
		// return a vector of audio internal ids
		StringVector get_audio_ids();
		
		// return a vector of testimony internal ids
		StringVector get_testimony_ids();
		
		// clear the entire case information
		void clear();
		
		// clear backgrounds
		void clear_backgrounds() { m_Backgrounds.clear(); }
		
		// clear images
		void clear_images() { m_Images.clear(); }
		
		// clear characters
		void clear_characters() { m_Characters.clear(); }
		
		// clear the evidence (a potential crime for tampering :D)
		void clear_evidence() { m_Evidence.clear(); }
		
		// clear location map
		void clear_locations() { m_Locations.clear(); }
		
		// clear audio map
		void clear_audio() { m_Audio.clear(); }
		
		// clear testimony map
		void clear_testimonies() { m_Testimonies.clear(); }
		
		// set the case overview
		void set_overview(const Overview &overview);
		
		// return the current case overview
		Overview get_overview() const { return m_Overview; }
		
		// return full map of characters
		CharacterMap get_characters() const { return m_Characters; }
		
		// return full map of images
		ImageMap get_images() const { return m_Images; }
		
		// return full map of backgrounds
		BackgroundMap get_backgrounds() const { return m_Backgrounds; }
		
		// return full map of evidence
		EvidenceMap get_evidence() const { return m_Evidence; }
		
		// return full map of locations
		LocationMap get_locations() const { return m_Locations; }
		
		// return a full map of audio
		AudioMap get_audio() const { return m_Audio; }
		
		// return a full map of testimonies
		TestimonyMap get_testimonies() const { return m_Testimonies; }
	
	private:
		// user-defined overrides
		Overrides m_Overrides;
		
		// general case data
		Overview m_Overview;
		
		// initial text block that will be displayed when the case starts
		Glib::ustring m_InitialBlockId;
		
		// map of characters
		CharacterMap m_Characters;
		
		// map of images
		ImageMap m_Images;
		
		// map of backgrounds
		BackgroundMap m_Backgrounds;
		
		// map of evidence
		EvidenceMap m_Evidence;
		
		// map of locations
		LocationMap m_Locations;
		
		// map of audio
		AudioMap m_Audio;
		
		// map of testimonies
		TestimonyMap m_Testimonies;
		
};

}; // namespace Case

#endif
