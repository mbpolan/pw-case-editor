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

// combo box for characters
class CharComboBox: public Gtk::ComboBoxText {
	public:
		// constructor
		CharComboBox(const CharacterMap &characters);
		
		// set the active character by internal name
		void set_active_internal(const Glib::ustring &name);
		
		// get the selected character's name
		Glib::ustring get_selected_name() const;
		
		// get the selected character's internal name
		Glib::ustring get_selected_internal() const;
		
		// get the selected character
		Character* get_selected_character();
		
	protected:
		CharacterMap m_Characters;
};

/***************************************************************************/

// combo box for locations
class LocationComboBox: public Gtk::ComboBoxText {
	public:
		// constructor
		LocationComboBox(const LocationMap &locations);
		
		// set the active location via internal name
		void set_active_internal(const Glib::ustring &id);
		
		// get the selected location's name
		Glib::ustring get_selected_name() const;
		
		// get the selected locations's internal name
		Glib::ustring get_selected_internal();
		
		// get the selected location
		Case::Location* get_selected_location();
		
	protected:
		// check if a location is a court location (uses display name as argument)
		Glib::ustring is_court_location(const Glib::ustring &str);
		
		// check if a location is a court location (uses internal id as argument)
		Glib::ustring internal_is_court_location(const Glib::ustring &str);
		
		LocationMap m_Locations;
};

/***************************************************************************/

// combo box for evidence
class EvidenceComboBox: public Gtk::ComboBoxText {
	public:
		// constructor
		EvidenceComboBox(const EvidenceMap &ev);
		
		// get the selected location's name
		Glib::ustring get_selected_name() const;
		
		// get the selected locations's internal name
		Glib::ustring get_selected_internal() const;
		
		// get the selected evidence
		Case::Evidence* get_selected_evidence();
		
	protected:
		EvidenceMap m_Evidence;
};

/***************************************************************************/

// combo box for text blocks
class BlockComboBox: public Gtk::ComboBoxText {
	public:
		// constructor
		BlockComboBox(const BufferMap &blocks);
		
		// get the selected block internal name
		Glib::ustring get_selected_internal() const;
		
		// get the selected block
		Glib::RefPtr<Gtk::TextBuffer> get_selected_block();
		
	protected:
		BufferMap m_Buffers;
};

/***************************************************************************/

// combo box for audio
class AudioComboBox: public Gtk::ComboBoxText {
	public:
		enum Filter { FILTER_NONE, FILTER_SFX, FILTER_MUSIC };
		
		// constructor
		AudioComboBox(const AudioMap &map, const Filter &filter=FILTER_NONE);
		
		// get the selected audio's internal name
		Glib::ustring get_selected_internal() const;
		
		// get the selected evidence
		Case::Audio* get_selected_audio();
		
	protected:
		AudioMap m_Audio;
};

/***************************************************************************/

// combo box for images
class ImgComboBox: public Gtk::ComboBoxText {
	public:
		// constructor
		ImgComboBox(const ImageMap &map);
		
		// get the selected image's internal name
		Glib::ustring get_selected_internal() const;
		
		// get the selected image
		Case::Image* get_selected_image();
		
	protected:
		ImageMap m_Images;
};

#endif
