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

// set the active character by internal name
void CharComboBox::set_active_internal(const Glib::ustring &name) {
	for (CharacterMap::iterator it=m_Characters.begin(); it!=m_Characters.end(); ++it) {
		if ((*it).second.get_internal_name()==name) {
			set_active_text((*it).second.get_name());
			return;
		}
	}
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
	
	// add court locations
	append_text("Prosecutor Stand");
	append_text("Defense Stand");
	append_text("Co-Counsel Stand");
	append_text("Witness Stand");
	append_text("Judge's Stand");
	append_text("Courtroom Overview");
	
	// iterate over locations
	for (LocationMap::const_iterator it=locations.begin(); it!=locations.end(); ++it)
		append_text((*it).second.name);
	
	set_active(0);
}

// set the active location via internal name
void LocationComboBox::set_active_internal(const Glib::ustring &id) {
	Glib::ustring court=internal_is_court_location(id);
	if (court!="null") {
		set_active_text(court);
		return;
	}
	
	for (LocationMap::iterator it=m_Locations.begin(); it!=m_Locations.end(); ++it) {
		if ((*it).second.id==id) {
			set_active_text((*it).second.name);
			return;
		}
	}
}

// get the selected location's name
Glib::ustring LocationComboBox::get_selected_name() const {
	return get_active_text();
}

// get the selected locations's internal name
Glib::ustring LocationComboBox::get_selected_internal() {
	Glib::ustring name=get_active_text();
	
	Glib::ustring court=is_court_location(name);
	if (court!="null")
		return court;
	
	// not a court location
	for (LocationMap::const_iterator it=m_Locations.begin(); it!=m_Locations.end(); ++it) {
		if ((*it).second.name==name)
			return (*it).first;
	}
}

// get the selected location
Case::Location* LocationComboBox::get_selected_location() {
	return &(m_Locations[get_selected_internal()]);
}

// check if a location is a court location (uses display name as argument)
Glib::ustring LocationComboBox::is_court_location(const Glib::ustring &name) {
	if (name=="Prosecutor Stand")
		return "prosecutor_stand";
	
	else if (name=="Defense Stand")
		return "defense_stand";
	
	else if (name=="Co-Counsel Stand")
		return "defense_helper_stand";
	
	else if (name=="Witness Stand")
		return "witness_stand";
	
	else if (name=="Judge's Stand")
		return "judge_stand";
	
	else if (name=="Courtroom Overview")
		return "courtroom";
	
	else
		return "null";
}

// check if a location is a court location (uses internal id as argument)
Glib::ustring LocationComboBox::internal_is_court_location(const Glib::ustring &str) {
	if (str=="prosecutor_stand")
		return "Prosecutor Stand";
	
	else if (str=="defense_stand")
		return "Defense Stand";
	
	else if (str=="defense_helper_stand")
		return "Co-Counsel Stand";
	
	else if (str=="witness_stand")
		return "Witness Stand";
	
	else if (str=="judge_stand")
		return "Judge's Stand";
	
	else if (str=="courtroom")
		return "Courtroom Overview";
	
	else
		return "null";
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

/***************************************************************************/

// constructor
AudioComboBox::AudioComboBox(const AudioMap &map, const Filter &filter): m_Audio(map) {
	// iterate over evidence
	for (AudioMap::const_iterator it=map.begin(); it!=map.end(); ++it) {
		bool add=true;
		
		if (filter==AudioComboBox::FILTER_MUSIC) {
			if ((*it).second.name.rfind(".mp3")==-1 && (*it).second.name.rfind(".ogg")==-1)
				add=false;
		}
		
		else if (filter==AudioComboBox::FILTER_SFX) {
			if ((*it).second.name.rfind(".mp3")!=-1 && (*it).second.name.rfind(".ogg")!=-1)
				add=false;
		}
		
		// only add the text is it is not filtered
		if (add)
			append_text((*it).second.id);
	}
	
	set_active(0);
}

// get the selected audio's internal name
Glib::ustring AudioComboBox::get_selected_internal() const {
	return get_active_text();
}

// get the selected audio
Case::Audio* AudioComboBox::get_selected_audio() {
	return &(m_Audio[get_selected_internal()]);
}

/***************************************************************************/

// constructor
ImgComboBox::ImgComboBox(const ImageMap &map): m_Images(map) {
	// iterate over evidence
	for (ImageMap::const_iterator it=map.begin(); it!=map.end(); ++it)
		append_text((*it).second.id);
	
	set_active(0);
}

// get the selected image's internal name
Glib::ustring ImgComboBox::get_selected_internal() const {
	return get_active_text();
}

// get the selected image
Case::Image* ImgComboBox::get_selected_image() {
	return &(m_Images[get_selected_internal()]);
}
