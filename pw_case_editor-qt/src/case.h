/***************************************************************************
 *   Copyright (C) 2008 by Mike Polan                                      *
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

#include <QPixmap>
#include <map>
#include <vector>

#include "character.h"
#include "common.h"

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

/// The type of background
enum BackgroundType { BG_SINGLE_SCREEN=0, BG_DOUBLE_SCREEN };

/// General case data and technical info
struct _Overview {
	/// The name of the case (Turnabout Whatever, etc)
	QString name;
	
	/// Who wrote this case and its contents
	QString author;
	
	/// The duration of the trial
	int days;
};
typedef struct _Overview Overview;

/// Struct to store user-defined overrides and customizations
struct _Overrides {
	/// Alpha value for the textbox
	int textboxAlpha;
	
	/// ID of custom image to use for the title screen
	QString titleScreen;
};
typedef struct _Overrides Overrides;

/// Information about a single background
struct _Background {
	/// ID referenced from within the script
	QString id;
	
	/// The amount of screens that this background spans
	BackgroundType type;
	
	/// Pixbuf containing image data
	QPixmap pixbuf;
};
typedef struct _Background Background;

/// Information about a piece of evidence
struct _Evidence {
	/// ID referenced from within the script
	QString id;
	
	/// The display name of this piece of evidence
	QString name;
	
	/// Text shown in the green box in the Court Record
	QString caption;
	
	/// Text displayed below the image and caption in the Court Record
	QString description;
	
	/// ID of image to display when the user clicks the Check button
	QString checkID;
	
	/// Pixbuf containing image data
	QPixmap pixbuf;
};
typedef struct _Evidence Evidence;

/// Information about an examinable hotspot in a location
struct _Hotspot {
	/// The bounding rectangle for this hotspot
	Rect rect;
	
	/// ID of block to set when this hotspot is examined
	QString block;
};
typedef struct _Hotspot Hotspot;

/// Information about a location
struct _Location {
	/// ID referenced from within the script
	QString id;
	
	/// The display name of this location
	QString name;
	
	/// Vector of examinable hotspots
	std::vector<Hotspot> hotspots;
	
	/** Vector of states.
	  * Each location can have many states, each with a different
	  * background. However, a location will always have at least
	  * one state with the normal background, known as "default"
	*/
	std::map<QString, QString> states;
};
typedef struct _Location Location;

/// Information about a sample of audio
struct _Audio {
	/// ID referenced from within the script
	QString id;
	
	/// Name of file relative to the case file
	QString name;
};
typedef struct _Audio Audio;

/// Information about an image
struct _Image {
	/// ID referenced from within the script
	QString id;
	
	/// Pixbuf containing image data
	QPixmap pixbuf;
};
typedef struct _Image Image;

/// A single piece of testimony
struct _TestimonyPiece {
	/// The dialogue spoken in this piece of testimony
	QString text;
	
	/// ID of evidence that can be presented at this point
	QString presentId;
	
	/// ID of block to set if the user presents evidence
	QString presentBlock;
	
	/// ID of block to set if the user presses the witness
	QString pressBlock;
	
	/// Whether or not this piece is initially hidden
	bool hidden;
};
typedef struct _TestimonyPiece TestimonyPiece;

/// A single testimony from a character
struct _Testimony {
	/// ID of this testimony referenced from within the script
	QString id;
	
	/// Display title of testimony
	QString title;
	
	/// The witness who presents this testimony
	QString speaker;
	
	/// Block to execute following testimony completion
	QString nextBlock;
	
	/// ID of court location to go to after testimony is completed
	QString followLoc;
	
	/// Block to execute if the cross examination is completed
	QString xExamineEndBlock;
	
	/// Vector of pieces making up this testimony
	std::vector<TestimonyPiece> pieces;
};
typedef struct _Testimony Testimony;

}; // namespace Case

// typedefs for cleaner code
typedef std::map<QString, Character> CharacterMap;
typedef std::map<QString, Case::Background> BackgroundMap;
typedef std::map<QString, Case::Evidence> EvidenceMap;
typedef std::map<QString, Case::Location> LocationMap;
typedef std::map<QString, Case::Audio> AudioMap;
typedef std::map<QString, Case::Image> ImageMap;
typedef std::map<QString, Case::Testimony> TestimonyMap;
typedef std::map<QString, QString > BufferMap;

namespace Case {

/// Default text blocks that come preloaded with each case
extern QString g_DefaultBlocks[16];

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
		
		/** Set the ID of the initial text block
		  * \param id ID of the text block
		*/
		void setInitialBlockID(const QString &id) { m_InitialBlockId=id; }
		
		/** Get the initial text block ID
		  * \return ID of the initial text block
		*/
		QString getInitialBlockID() const { return m_InitialBlockId; }
		
		/** Get a core block from the internal map
		  * \param block Which block to get
		  * \return The requested block
		*/
		QString getCoreBlock(int block) const { return m_CoreBlocks[block]; }
		
		/** Replace a core block with a new one
		  * \param block Which block to set
		  * \param contents Contents of new block
		*/
		void setCoreBlock(int block, const QString &content) { m_CoreBlocks[block]=content; }
		
		/** Add a character to the internal map
		  * \param character The character
		*/
		void addCharacter(const Character &character);
		
		/** Remove a character based on name
		  * \param name The name of the character to remove
		*/
		void removeCharacter(const QString &name);
		
		/** Add an image to the internal map
		  * \param image The image
		*/
		void addImage(const Image &image);
		
		/** Remove an image from the case
		  * \param id The ID of the image to remove
		*/
		void removeImage(const QString &id);
		
		/** Add a testimony to the internal map
		  * \param testimony The testimony
		*/
		void addTestimony(const Testimony &testimony);
		
		/** Remove a testimony from the case
		  * \param id ID of testimony to remove
		*/
		void removeTestimony(const QString &id);
		
		/** Add a background to the internal map
		  * \param bg The background
		*/
		void addBackground(const Background &bg);
		
		/** Remove a background from the case
		  * \param id ID of background to remove
		*/
		void removeBackground(const QString &id);
		
		/** Add a piece of evidence to the internal map
		  * \param evidence The evidence
		*/
		void addEvidence(const Evidence &evidence);
		
		/** Remove a piece of evidence from the case
		  * \param id ID of evidence to remove
		*/
		void removeEvidence(const QString &id);
		
		/** Add a location to the internal map
		  * \param loc The location
		*/
		void addLocation(const Location &loc);
		
		/** Remove a location from the case
		  * \param id ID of location
		*/
		void removeLocation(const QString &id);
		
		/** Add an audio sample to the internal map
		  * \param audio The audio sample
		*/
		void addAudio(const Audio &audio);
		
		/** Get all character internal names
		  * \return A vector with internal names
		*/
		StringVector getCharacterNames();
		
		/** Get all image internal IDs
		  * \return A vector with image IDs
		*/
		StringVector getImageIDs();
		
		/** Get all background internal IDs
		 * \return A vector with background IDs
		*/
		StringVector getBackgroundIDs();
		
		/** Get all evidence internal IDs
		 * \return A vector with evidence IDs
		*/
		StringVector getEvidenceIDs();
		
		/** Get all location internal IDs
		 * \return A vector with location IDs
		*/
		StringVector getLocationIDs();
		
		/** Get all audio internal IDs
		 * \return A vector with audio IDs
		*/
		StringVector getAudioIDs();
		
		/** Get all testimony internal IDs
		 * \return A vector with testimony IDs
		*/
		StringVector getTestimonyIDs();
		
		/// Clear the entire case information
		void clear();
		
		/// Clear all backgrounds
		void clearBackgrounds() { m_Backgrounds.clear(); }
		
		/// Clear all images
		void clearImages() { m_Images.clear(); }
		
		/// Clear all characters
		void clearCharacters() { m_Characters.clear(); }
		
		/// Clear all of the evidence
		void clearEvidence() { m_Evidence.clear(); }
		
		/// Clear all locations
		void clearLocations() { m_Locations.clear(); }
		
		/// Clear all audio samples
		void clearAudio() { m_Audio.clear(); }
		
		/// Clear all testimonies
		void clearTestimonies() { m_Testimonies.clear(); }
		
		/** Get a full map of characters
		  * \return Map of every character
		*/
		CharacterMap getCharacters() const { return m_Characters; }
		
		/** Get a full map of images
		  * \return Map of every image
		*/
		ImageMap getImages() const { return m_Images; }
		
		/** Get a full map of backgrounds
		  * \return Map of every background
		*/
		BackgroundMap getBackgrounds() const { return m_Backgrounds; }
		
		/** Get a full map of evidence
		  * \return Map of every piece of evidence
		*/
		EvidenceMap getEvidence() const { return m_Evidence; }
		
		/** Get a full map of locations
		  * \return Map of every location
		*/
		LocationMap getLocations() const { return m_Locations; }
		
		/** Get a full map of audio
		  * \return Map of every audio sample
		*/
		AudioMap getAudio() const { return m_Audio; }
		
		/** Get a full map of testimonies
		  * \return Map of every testimony
		*/
		TestimonyMap getTestimonies() const { return m_Testimonies; }
	
	private:
		/// User-defined overrides
		Overrides m_Overrides;
		
		/// General case data
		Overview m_Overview;
		
		/// Initial text block that will be displayed when the case starts
		QString m_InitialBlockId;
		
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
		
		/// Vector of core blocks
		StringVector m_CoreBlocks;
		
};

}; // namespace Case

#endif
