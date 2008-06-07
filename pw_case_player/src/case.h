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

#include <GL/gl.h>
#include <map>
#include <vector>
#include "SDL.h"
#include "SDL_mixer.h"

#include "character.h"
#include "common.h"

/// Namespace for all Case related objects and definitions
namespace Case {

/// The amount of days that each trial lasts
enum LawSystem { TWO_DAY=0, THREE_DAY };

/// The type of background
enum BackgroundType { BG_SINGLE_SCREEN=0, BG_DOUBLE_SCREEN };

/// General case data and technical info
struct _Overview {
	/// The name of the case (Turnabout Whatever, etc)
	ustring name;
	
	/// Who wrote this case and its contents
	ustring author;
	
	/// The law system (how many days for a trial)
	LawSystem lawSys;
};
typedef struct _Overview Overview;

/// Struct to store user-defined overrides and customizations
struct _Overrides {
	/// Alpha value for the textbox
	int textboxAlpha;
	
	/// ID of custom image to use for the title screen
	ustring titleScreen;
};
typedef struct _Overrides Overrides;

/// Information about a single background
struct _Background {
	/// ID referenced from within the script
	ustring id;
	
	/// The amount of screens that this background spans
	BackgroundType type;
	
	/// The actual background image
	GLuint texture;
};
typedef struct _Background Background;

/// Information about a piece of evidence
struct _Evidence {
	/// ID referenced from within the script
	ustring id;
	
	/// The display name of this piece of evidence
	ustring name;
	
	/// Text shown in the green box in the Court Record
	ustring caption;
	
	/// Text displayed below the image and caption in the Court Record
	ustring description;
	
	/// ID of image to display when the user clicks the Check button
	ustring checkID;
	
	/// The actual image of the evidence
	GLuint texture;
	
	/// Scaled image of the evidence
	GLuint thumb;
};
typedef struct _Evidence Evidence;

/// Information about an examinable hotspot in a location
struct _Hotspot {
	/// The bounding rectangle for this hotspot
	Rect rect;
	
	/// ID of block to set when this hotspot is examined
	ustring block;
};
typedef struct _Hotspot Hotspot;

/// Information about a location
struct _Location {
	/// ID referenced from within the script
	ustring id;
	
	/// The display name of this location
	ustring name;
	
	/// Block to execute upon arriving at location
	ustring triggerBlock;
	
	/// Character placed at this location
	ustring character;
	
	/// Music at this location
	ustring music;
	
	/// The current state of the location
	ustring state;
	
	/// Scaled background image
	GLuint bgScaled;
	
	/// Vector of examinable hotspots
	std::vector<Hotspot> hotspots;
	
	/// IDs of locations that player can move to from here
	std::vector<ustring> moveLocations;
	
	/** Vector of states.
	  * Each location can have many states, each with a different
	  * background. However, a location will always have at least
	  * one state with the normal background, known as "default"
	*/
	std::map<ustring, ustring> states;
};
typedef struct _Location Location;

/// Information about an image
struct _Image {
	/// ID referenced from within the script
	ustring id;
	
	/// The image data
	GLuint texture;
};
typedef struct _Image Image;

/// A single piece of testimony
struct _TestimonyPiece {
	/// The dialogue spoken in this piece of testimony
	ustring text;
	
	/// ID of evidence that can be presented at this point
	ustring presentId;
	
	/// ID of block to set if the user presents evidence
	ustring presentBlock;
	
	/// ID of block to set if the user presses the witness
	ustring pressBlock;
	
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
	Glib::ustring followLocation;
	
	/// Block to execute if the cross examination is completed
	Glib::ustring xExamineEndBlock;
	
	/// Vector of pieces making up this testimony
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

/** Core class that holds all pertinent data about a case.
  * This class stores all of the individual bits and pieces of a case, 
  * such as testimonies, text blocks, images, characters, etc.
*/
class Case {
	public:
		/// Enumeration for core case blocks
		enum CoreBlock { CORE_XEXAMINE_BAD_EV=0,	///< Incorrect evidence presented at trial
				 CORE_XEXAMINE_FAIL,		///< Player ran out of penalties at trial
				 CORE_BLOCK_COUNT		///< Amount of core blocks
				};

		
		/// Default constructor
		Case();
		
		/// Destructor
		~Case();
		
		/** Set the case overview
		  * \param overview An Overview struct
		*/
		void setOverview(const Overview &overview);
		
		/** Get the current case overview
		  * \return The Overview struct for this case
		*/
		Overview getOverview() const { return m_Overview; }
		
		/** Set the overrides for this case
		  * \param ov A filled Overrides struct
		*/
		void setOverrides(const Overrides &ov) { m_Overrides=ov; }
		
		/** Get the case overrides
		  * \return The set Overrides struct
		*/
		Overrides getOverrides() const { return m_Overrides; }
		
		/** Get a core block from the internal map
		 * \param block Which block to get
		 * \return The requested block
		 */
		ustring getCoreBlock(int block) const { return m_CoreBlocks[block]; }
		
		/** Replace a core block with a new one
		 * \param block Which block to set
		 * \param contents Contents of new block
		 */
		void setCoreBlock(int block, const ustring &content) { m_CoreBlocks[block]=content; }

		
		/** Set the ID of the initial text block
		  * \param id ID of the text block
		*/
		void setInitialBlockId(const ustring &id) { m_InitialBlockId=id; }
		
		/** Get the initial text block ID
		  * \return ID of the initial text block
		*/
		ustring getInitialBlockId() const { return m_InitialBlockId; }
		
		/** Add a character to the internal map
		  * \param character The character
		*/
		void addCharacter(const Character &character);
		
		/** Add a background to the internal map
		  * \param bg The background
		*/
		void addBackground(const Background &bg);
		
		/** Add a piece of evidence to the internal map
		  * \param evidence The evidence
		*/
		void addEvidence(const Evidence &evidence);
		
		/** Add an image to the internal map
		  * \param image The image
		*/
		void addImage(const Image &image);
		
		/** Add a location to the internal map
		  * \param loc The location
		*/
		void addLocation(const Location &loc);
		
		/** Add a testimony to the internal map
		  * \param testimony The testimony
		*/
		void addTestimony(const Testimony &testimony);
		
		/** Add a text buffer to the internal map
		  * \param id The ID of the block
		  * \param contents The block contents
		*/
		void addBuffer(const ustring &id, const ustring &contents);
		
		/** Get a character
		  * \param id The ID of the character
		  * \return Pointer to a Character object
		*/
		Character* getCharacter(const ustring &id);
		
		/** Get a background
		  * \param id The ID of the background
		  * \return Pointer to a Case::Background object
		*/
		Background* getBackground(const ustring &id);
		
		/** Get a piece of evidence
		  * \param id The ID of the evidence
		  * \return Pointer to a Case::Evidence object
		*/
		Evidence* getEvidence(const ustring &id);
		
		/** Get an image
		  * \param id The ID of the image
		  * \return Pointer to a Case::Image object
		*/
		Image* getImage(const ustring &id);
		
		/** Get a location
		  * \param id The ID of the location
		  * \return Pointer to a Case::Location object
		*/
		Location* getLocation(const ustring &id);
		
		/** Get a testimony
		  * \param id The ID of the testimony
		  * \return Pointer to a Case::Testimony object
		*/
		Testimony* getTestimony(const ustring &id);
		
		/** Get a vector of evidence based on vector of string IDs
		  * \param vec The vector of IDs
		  * \return A vector of Case::Evidence pointers matching the IDs
		*/
		std::vector<Evidence*> getEvidenceFromIds(const StringVector &vec);
		
		/** Get a vector of characters based on vector of string IDs
		  * \param vec Vector of IDs
		  * \return A vector of Character pointers matching the IDs
		*/
		std::vector<Character*> getCharactersFromIds(const StringVector &vec);
		
		/// Clear the entire case information
		void clear();
		
		/** Get a full map of characters
		  * \return Map of all characters
		*/
		CharacterMap getCharacters() const { return m_Characters; }
		
		/** Get a full map of backgrounds
		  * \return Map of all backgrounds
		*/
		BackgroundMap getBackgrounds() const { return m_Backgrounds; }
		
		/** Get a full map of evidence
		  * \return Map of all evidence
		*/
		EvidenceMap getEvidence() const { return m_Evidence; }
		
		/** Get a full map of images
		  * \return Map of all images
		*/
		ImageMap getImages() const { return m_Images; }
		
		/** Get a full map of locations
		  * \return Map of all locations
		*/
		LocationMap getLocations() const { return m_Locations; }
		
		/** Get a full map of testimonies
		  * \return Map of all testimonies
		*/
		TestimonyMap getTestimonies() const { return m_Testimonies; }
		
		/** Get a map of buffers
		  * \return Map of all text buffers
		*/
		BufferMap getBuffers() const { return m_Buffers; }
	
	private:
		/// Case overrides
		Overrides m_Overrides;
		
		/// General case data
		Overview m_Overview;
		
		/// Initial text block that will be displayed when the case starts
		ustring m_InitialBlockId;
		
		/// Map of characters
		CharacterMap m_Characters;
		
		/// Map of backgrounds
		BackgroundMap m_Backgrounds;
		
		/// Map of evidence
		EvidenceMap m_Evidence;
		
		/// Map of images
		ImageMap m_Images;
		
		/// Map of locations
		LocationMap m_Locations;
		
		/// Map of testimonies
		TestimonyMap m_Testimonies;
		
		/// Map of buffers
		BufferMap m_Buffers;
		
		/// Vector of core blocks
		StringVector m_CoreBlocks;

};

}; // namespace Case

#endif
