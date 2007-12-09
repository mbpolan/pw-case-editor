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
// casecombobox.cpp: implementation of combo boxes

#include "casecombobox.h"

// constructor
CharComboBox::CharComboBox(const CharacterMap &characters): m_Characters(characters) {
	// iterate over characters
	for (CharacterMap::iterator it=m_Characters.begin(); it!=m_Characters.end(); ++it)
		append_text((*it).second.get_name());
	
	set_active(0);
}

// get the selected character's name
Glib::ustring CharComboBox::get_selected_name() const {
	return get_active_text();
}

// get the selected character's internal name
Glib::ustring CharComboBox::get_selected_internal() const {
	Glib::ustring name=get_active_text();
	for (CharacterMap::const_iterator it=m_Characters.begin(); it!=m_Characters.end(); ++it) {
		if ((*it).second.get_name()==name)
			return (*it).second.get_internal_name();
	}
}

// get the selected character
Character* CharComboBox::get_selected_character() {
	return &(m_Characters[get_selected_internal()]);
}

/***************************************************************************/

// constructor
LocationComboBox::LocationComboBox(const LocationMap &locations):
		m_Locations(locations) {
	
	// iterate over locations
	for (LocationMap::const_iterator it=locations.begin(); it!=locations.end(); ++it)
		append_text((*it).second.name);
	
	set_active(0);
}

// get the selected location's name
Glib::ustring LocationComboBox::get_selected_name() const {
	return get_active_text();
}

// get the selected locations's internal name
Glib::ustring LocationComboBox::get_selected_internal() const {
	Glib::ustring name=get_active_text();
	for (LocationMap::const_iterator it=m_Locations.begin(); it!=m_Locations.end(); ++it) {
		if ((*it).second.name==name)
			return (*it).first;
	}
}

// get the selected location
Case::Location* LocationComboBox::get_selected_location() {
	return &(m_Locations[get_selected_internal()]);
}

/***************************************************************************/

// constructor
EvidenceComboBox::EvidenceComboBox(const EvidenceMap &ev): m_Evidence(ev) {
	// iterate over evidence
	for (EvidenceMap::const_iterator it=ev.begin(); it!=ev.end(); ++it)
		append_text((*it).second.name);
	
	set_active(0);
}

// get the selected evidence's name
Glib::ustring EvidenceComboBox::get_selected_name() const {
	return get_active_text();
}

// get the selected evidence's internal name
Glib::ustring EvidenceComboBox::get_selected_internal() const {
	Glib::ustring name=get_active_text();
	for (EvidenceMap::const_iterator it=m_Evidence.begin(); it!=m_Evidence.end(); ++it) {
		if ((*it).second.name==name)
			return (*it).first;
	}
}

// get the selected evidence
Case::Evidence* EvidenceComboBox::get_selected_evidence() {
	return &(m_Evidence[get_selected_internal()]);
}

/***************************************************************************/

// constructor
BlockComboBox::BlockComboBox(const BufferMap &blocks): m_Buffers(blocks) {
	// iterate over buffers
	for (BufferMap::const_iterator it=blocks.begin(); it!=blocks.end(); ++it) {
		Glib::ustring id=(*it).first;
		id.erase(id.rfind("_"), id.size());
		append_text(id);
	}
	
	set_active(0);
}

// get the selected block's internal name
Glib::ustring BlockComboBox::get_selected_internal() const {
	return get_active_text();
}

// get the selected block
Glib::RefPtr<Gtk::TextBuffer> BlockComboBox::get_selected_block() {
	return m_Buffers[get_selected_internal()];
}
