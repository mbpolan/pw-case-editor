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

/// A basic struct representing a rectangle.
struct _Rect {
	/// The x-coordinate
	int x;
	
	/// The y-coordinate
	int y;
	
	/// The width of the rectangle
	int w;
	
	/// The height of the rectangle
	int h;
};
typedef struct _Rect Rect;


/// Namespace for all Case related objects and definitions
namespace Case {

/// The amount of days that each trial lasts
enum LawSystem { SINGLE_TRIAL=1, TWO_DAY, THREE_DAY };

/// The type of background
enum BackgroundType { BG_SINGLE_SCREEN=0, BG_DOUBLE_SCREEN };

/// General case data and technical info
struct _Overview {
	/// The name of the case (Turnabout Whatever, etc)
	Glib::ustring name;
	
	/// Who wrote this case and its contents
	Glib::ustring author;
	
	 /// The law system (how many days for a trial)
	LawSystem lawSys;
};
typedef struct _Overview Overview;

/// Struct to store user-defined overrides and customizations
struct _Overrides {
	/// Alpha value for the textbox
	int textboxAlpha;
	
	/// ID of custom image to use for the title screen
	Glib::ustring titleScreen;
};
typedef struct _Overrides Overrides;

/// Information about a single background
struct _Background {
	/// ID referenced from within the script
	Glib::ustring id;
	
	/// The amount of screens that this background spans
	BackgroundType type;
	
	/// Pixbuf containing image data
	Glib::RefPtr<Gdk::Pixbuf> pixbuf;
};
typedef struct _Background Background;

/// Information about a piece of evidence
struct _Evidence {
	/// ID referenced from within the script
	Glib::ustring id;
	
	/// The display name of this piece of evidence
	Glib::ustring name;
	
	/// Text shown in the green box in the Court Record
	Glib::ustring caption;
	
	/// Text displayed below the image and caption in the Court Record
	Glib::ustring description;
	
	/// ID of image to display when the user clicks the Check button
	Glib::ustring checkID;
	
	/// Pixbuf containing image data
	Glib::RefPtr<Gdk::Pixbuf> pixbuf;
};
typedef struct _Evidence Evidence;

/// Information about an examinable hotspot in a location
struct _Hotspot {
	/// The bounding rectangle for this hotspot
	Rect rect;
	
	/// ID of block to set when this hotspot is examined
	Glib::ustring block;
};
typedef struct _Hotspot Hotspot;

/// Information about a location
struct _Location {
	/// ID referenced from within the script
	Glib::ustring id;
	
	/// The display name of this location
	Glib::ustring name;
	
	/// Vector of examinable hotspots
	std::vector<Hotspot> hotspots;
	
	/** Vector of states.
	  * Each location can have many states, each with a different
	  * background. However, a location will always have at least
	  * one state with the normal background, known as "default"
	*/
	std::map<Glib::ustring, Glib::ustring> states;
};
typedef struct _Location Location;

/// Information about a sample of audio
struct _Audio {
	/// ID referenced from within the script
	Glib::ustring id;
	
	/// Name of file relative to the case file
	Glib::ustring name;
};
typedef struct _Audio Audio;

/// Information about an image
struct _Image {
	/// ID referenced from within the script
	Glib::ustring id;
	
	/// Pixbuf containing image data
	Glib::RefPtr<Gdk::Pixbuf> pixbuf;
};
typedef struct _Image Image;

/// A single piece of testimony
struct _TestimonyPiece {
	/// The dialogue spoken in this piece of testimony
	Glib::ustring text;
	
	/// ID of evidence that can be presented at this point
	Glib::ustring presentId;
	
	/// ID of block to set if the user presents evidence
	Glib::ustring presentBlock;
	
	/// ID of block to set if the user presses the witness
	Glib::ustring pressBlock;
	
	/// Whether or not this piece is initially hidden
	bool hidden;
};
typedef struct _TestimonyPiece TestimonyPiece;

/// A single testimony from a character
struct _Testimony {
	/// ID of this testimony referenced from within the script
	Glib::ustring id;
	
	/// Display title of testimony
	Glib::ustring title;
	
	/// The witness who presents this testimony
	Glib::ustring speaker;
	
	/// Block to execute following testimony completion
	Glib::ustring nextBlock;
	
	/// ID of court location to go to after testimony is completed
	Glib::ustring followLoc;
	
	/// Block to execute if the cross examination is completed
	Glib::ustring xExamineEndBlock;
	
	/// Vector of pieces making up this testimony
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

/** Core class that holds all pertinent data about a case.
  * This class stores all of the individual bits and pieces of a case, 
  * such as testimonies, text blocks, images, characters, etc.
*/ 
class Case {
	public:
		/// Default constructor
		Case();
		
		/** Set the case overview
		 * \param overview An Overview struct
		 */
		void set_overview(const Overview &overview);
		
		/** Get the current case overview
		 * \return The Overview struct for this case
		 */
		Overview get_overview() const { return m_Overview; }
		
		/** Set the overrides for this case
		  * \param ov A filled Overrides struct
		*/
		void set_overrides(const Overrides &ov) { m_Overrides=ov; }
		
		/** Get the case overrides
		  * \return The set Overrides struct
		*/
		Overrides get_overrides() const { return m_Overrides; }
		
		/** Set the ID of the initial text block
		  * \param id ID of the text block
		*/
		void set_initial_block_id(const Glib::ustring &id) { m_InitialBlockId=id; }
		
		/** Get the initial text block ID
		  * \return ID of the initial text block
		*/
		Glib::ustring get_initial_block_id() const { return m_InitialBlockId; }
		
		/** Add a character to the internal map
		  * \param character The character
		*/
		void add_character(const Character &character);
		
		/** Remove a character based on name
		  * \param name The name of the character to remove
		*/
		void remove_character(const Glib::ustring &name);
		
		/** Add an image to the internal map
		  * \param image The image
		*/
		void add_image(const Image &image);
		
		/** Remove an image from the case
		  * \param id The ID of the image to remove
		*/
		void remove_image(const Glib::ustring &id);
		
		/** Add a testimony to the internal map
		  * \param testimony The testimony
		*/
		void add_testimony(const Testimony &testimony);
		
		/** Remove a testimony from the case
		  * \param id ID of testimony to remove
		*/
		void remove_testimony(const Glib::ustring &id);
		
		/** Add a background to the internal map
		  * \param bg The background
		*/
		void add_background(const Background &bg);
		
		/** Remove a background from the case
		  * \param id ID of background to remove
		*/
		void remove_background(const Glib::ustring &id);
		
		/** Add a piece of evidence to the internal map
		  * \param evidence The evidence
		*/
		void add_evidence(const Evidence &evidence);
		
		/** Remove a piece of evidence from the case
		  * \param id ID of evidence to remove
		*/
		void remove_evidence(const Glib::ustring &id);
		
		/** Add a location to the internal map
		  * \param loc The location
		*/
		void add_location(const Location &loc);
		
		/** Remove a location from the case
		  * \param id ID of location
		*/
		void remove_location(const Glib::ustring &id);
		
		/** Add an audio sample to the internal map
		  * \param audio The audio sample
		*/
		void add_audio(const Audio &audio);
		
		/** Get all character internal names
		  * \return A vector with internal names
		*/
		StringVector get_character_names();
		
		/** Get all image internal IDs
		  * \return A vector with image IDs
		*/
		StringVector get_image_ids();
		
		/** Get all background internal IDs
		 * \return A vector with background IDs
		*/
		StringVector get_background_ids();
		
		/** Get all evidence internal IDs
		 * \return A vector with evidence IDs
		*/
		StringVector get_evidence_ids();
		
		/** Get all location internal IDs
		 * \return A vector with location IDs
		*/
		StringVector get_location_ids();
		
		/** Get all audio internal IDs
		 * \return A vector with audio IDs
		*/
		StringVector get_audio_ids();
		
		/** Get all testimony internal IDs
		 * \return A vector with testimony IDs
		*/
		StringVector get_testimony_ids();
		
		/// Clear the entire case information
		void clear();
		
		/// Clear all backgrounds
		void clear_backgrounds() { m_Backgrounds.clear(); }
		
		/// Clear all images
		void clear_images() { m_Images.clear(); }
		
		/// Clear all characters
		void clear_characters() { m_Characters.clear(); }
		
		/// Clear all of the evidence
		void clear_evidence() { m_Evidence.clear(); }
		
		/// Clear all locations
		void clear_locations() { m_Locations.clear(); }
		
		/// Clear all audio samples
		void clear_audio() { m_Audio.clear(); }
		
		/// Clear all testimonies
		void clear_testimonies() { m_Testimonies.clear(); }
		
		/** Get a full map of characters
		  * \return Map of every character
		*/
		CharacterMap get_characters() const { return m_Characters; }
		
		/** Get a full map of images
		  * \return Map of every image
		*/
		ImageMap get_images() const { return m_Images; }
		
		/** Get a full map of backgrounds
		  * \return Map of every background
		*/
		BackgroundMap get_backgrounds() const { return m_Backgrounds; }
		
		/** Get a full map of evidence
		  * \return Map of every piece of evidence
		*/
		EvidenceMap get_evidence() const { return m_Evidence; }
		
		/** Get a full map of locations
		  * \return Map of every location
		*/
		LocationMap get_locations() const { return m_Locations; }
		
		/** Get a full map of audio
		  * \return Map of every audio sample
		*/
		AudioMap get_audio() const { return m_Audio; }
		
		/** Get a full map of testimonies
		  * \return Map of every testimony
		*/
		TestimonyMap get_testimonies() const { return m_Testimonies; }
	
	private:
		/// User-defined overrides
		Overrides m_Overrides;
		
		/// General case data
		Overview m_Overview;
		
		/// Initial text block that will be displayed when the case starts
		Glib::ustring m_InitialBlockId;
		
		/// Map of characters
		CharacterMap m_Characters;
		
		/// Map of images
		ImageMap m_Images;
		
		/// Map of backgrounds
		BackgroundMap m_Backgrounds;
		
		/// Map of evidence
		EvidenceMap m_Evidence;
		
		/// Map of locations
		LocationMap m_Locations;
		
		/// Map of audio
		AudioMap m_Audio;
		
		/// Map of testimonies
		TestimonyMap m_Testimonies;
		
};

}; // namespace Case

#endif
