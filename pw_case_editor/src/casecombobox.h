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
// casecombobox.h: convenience comboboxes

#ifndef CASECOMBOBOX_H
#define CASECOMBOBOX_H

#include <gtkmm/comboboxtext.h>

#include "case.h"

/** Combo box displaying characters
*/
class CharComboBox: public Gtk::ComboBoxText {
	public:
		/** Constructor
		  * \param characters Map of characters to include
		*/
		CharComboBox(const CharacterMap &characters);
		
		/** See if the combo box is empty
		  * \return <b>true</b> if empty, <b>false</b> otherwise
		*/
		bool empty() const { return m_Characters.empty(); }
		
		/** Set the active character by internal name
		  * \param name The internal name
		*/
		void set_active_internal(const Glib::ustring &name);
		
		/** Get the selected character's name
		  * \return The character's display name
		*/
		Glib::ustring get_selected_name() const;
		
		/** Get the selected character's internal name
		  * \return The character's internal name
		*/
		Glib::ustring get_selected_internal() const;
		
		/** Get the selected character
		  * \return Pointer to selected Character object
		*/
		Character* get_selected_character();
		
	protected:
		/// Internal map of characters
		CharacterMap m_Characters;
};

/***************************************************************************/

/** Combo box to display locations
*/
class LocationComboBox: public Gtk::ComboBoxText {
	public:
		/** Constructor
		  * \param locations Map of locations to display
		*/
		LocationComboBox(const LocationMap &locations);
		
		/** See if the combo box is empty
		  * \return <b>true</b> if empty, <b>false</b> otherwise
		*/
		bool empty() const { return m_Locations.empty(); }
		
		/** Set the active location by internal name
		  * \param id The internal ID
		*/
		void set_active_internal(const Glib::ustring &id);
		
		/** Get the selected location's name
		  * \return The selected location's display name
		*/
		Glib::ustring get_selected_name() const;
		
		/** Get the selected locations's internal name
		  * \return The internal name
		*/
		Glib::ustring get_selected_internal();
		
		/** Get the selected location
		  * \return Pointer to selected Case::Location object
		*/
		Case::Location* get_selected_location();
		
	protected:
		/** Check if a location is a court location
		  * \param str The display name of the location to test
		  * \return "null" if not a court location, internal ID of court location otherwise
		*/
		Glib::ustring is_court_location(const Glib::ustring &str);
		
		/** Check if a location is a court location by internal ID
		  * \param str The internal ID of the location to test
		  * \return "null" if not a court location, display name of court location otherwise
		*/
		Glib::ustring internal_is_court_location(const Glib::ustring &str);
		
		/// Internal map of locations
		LocationMap m_Locations;
};

/***************************************************************************/

/** Combo box to display evidence
*/
class EvidenceComboBox: public Gtk::ComboBoxText {
	public:
		/** Constructor
		  * \param ev Map of evidence to display
		*/
		EvidenceComboBox(const EvidenceMap &ev);
		
		/** See if the combo box is empty
		  * \return <b>true</b> if empty, <b>false</b> otherwise
		*/
		bool empty() const { return m_Evidence.empty(); }
		
		/** Set the active evidence by internal ID
		  * \param id The internal ID
		*/
		void set_active_internal(const Glib::ustring &id);
		
		/** Get the selected evidence's name
		  * \return The display name of the selected evidence
		*/
		Glib::ustring get_selected_name() const;
		
		/** Get the selected evidence's internal ID
		  * \return The internal ID of the selected evidence
		*/
		Glib::ustring get_selected_internal() const;
		
		/** Get the selected evidence
		  * \return Pointer to selected Case::Evidence object
		*/
		Case::Evidence* get_selected_evidence();
		
	protected:
		/// Internal record of evidence
		EvidenceMap m_Evidence;
};

/***************************************************************************/

/** Combo box to display text blocks
*/
class BlockComboBox: public Gtk::ComboBoxText {
	public:
		/** Constructor
		  * \param blocks Map of buffers to display
		*/
		BlockComboBox(const BufferMap &blocks);
		
		/** See if the combo box is empty
		  * \return <b>true</b> if empty, <b>false</b> otherwise
		*/
		bool empty() const { return m_Buffers.empty(); }
		
		/** Get the selected block's internal ID
		  * \return Internal ID of selected block
		*/
		Glib::ustring get_selected_internal() const;
		
		/** Get the selected block's contents
		  * \return The selected block's text
		*/
		Glib::RefPtr<Gtk::TextBuffer> get_selected_block();
		
	protected:
		/// Internal map of blocks
		BufferMap m_Buffers;
};

/***************************************************************************/

/** Combo box to display audio samples
*/
class AudioComboBox: public Gtk::ComboBoxText {
	public:
		/// Specifies what samples should not be included in the combo box
		enum Filter { FILTER_NONE, FILTER_SFX, FILTER_MUSIC };
		
		/** Constructor
		  * \param map Map of audio samples to display
		  * \param filter What types of samples to cull
		*/
		AudioComboBox(const AudioMap &map, const Filter &filter=FILTER_NONE);
		
		/** See if the combo box is empty
		  * \return <b>true</b> if empty, <b>false</b> otherwise
		*/
		bool empty() const { return m_Audio.empty(); }
		
		/** Get the selected audio sample's internal ID
		  * \return Internal ID of selected sample
		*/
		Glib::ustring get_selected_internal() const;
		
		/** Get the selected audio sample
		  * \return Pointer to selected Case::Audio object
		*/
		Case::Audio* get_selected_audio();
		
	protected:
		/// Internal map of audio samples
		AudioMap m_Audio;
};

/***************************************************************************/

/** Combo box to display images
*/
class ImgComboBox: public Gtk::ComboBoxText {
	public:
		/** Constructor
		  * \param map Map of images to display
		*/
		ImgComboBox(const ImageMap &map);
		
		/** See if the combo box is empty
		  * \return <b>true</b> if empty, <b>false</b> otherwise
		*/
		bool empty() const { return m_Images.empty(); }
		
		/** Set the active image by internal ID
		  * \param id Internal ID of image
		*/
		void set_active_internal(const Glib::ustring &id);
		
		/** Get the selected image's internal ID
		  * \return The selected image's internal ID
		*/
		Glib::ustring get_selected_internal() const;
		
		/** Get the selected image
		  * \return Pointer to selected Case::Image object
		*/
		Case::Image* get_selected_image();
		
	protected:
		/// Internal record of images
		ImageMap m_Images;
};

#endif
