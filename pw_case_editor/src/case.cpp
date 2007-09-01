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
// case.cpp: implementations of Case structs and classes

#include "case.h"

// constructor
Case::Case::Case() {
	// clear this case out
	clear();
}

// add a character
void Case::Case::add_character(const Character &character) {
	m_Characters[character.get_name()]=character;
}

// remove a character based on name
void Case::Case::remove_character(const Glib::ustring &name) {
	// iterate over characters and find the requested one
	for (CharacterMapIter it=m_Characters.begin(); it!=m_Characters.end(); ++it) {
		if ((*it).first==name) {
			m_Characters.erase(it);
			return;
		}
	}
}

// add a piece of evidence
void Case::Case::add_evidence(const Evidence &evidence) {
	m_Evidence[evidence.id]=evidence;
}

// remove a piece of evidence based on id
void Case::Case::remove_evidence(const Glib::ustring &id) {
	// iterate over evidence and find the requested one
	for (std::map<Glib::ustring, Evidence>::iterator it=m_Evidence.begin(); it!=m_Evidence.end(); ++it) {
		if ((*it).first==id) {
			m_Evidence.erase(it);
			return;
		}
	}
}

// add a background
void Case::Case::add_background(const Background &bg) {
	m_Backgrounds[bg.id]=bg;
}

// remove a background based on id
void Case::Case::remove_background(const Glib::ustring &id) {
	// iterate over entries
	for (std::map<Glib::ustring, Background>::iterator it=m_Backgrounds.begin(); it!=m_Backgrounds.end(); ++it) {
		if ((*it).first==id) {
			m_Backgrounds.erase(it);
			return;
		}
	}
}

// add a location
void Case::Case::add_location(const Location &loc) {
	m_Locations[loc.id]=loc;
}

// remove a location based on id
void Case::Case::remove_location(const Glib::ustring &id) {
	m_Locations.erase(id);
}

// return a vector of character internal names
StringVector Case::Case::get_character_names() {
	StringVector vec;
	
	// simply add names to the vector
	for (CharacterMapIter it=m_Characters.begin(); it!=m_Characters.end(); ++it)
		vec.push_back((*it).second.get_internal_name());
	
	return vec;
}

// return a vector of background internal ids
StringVector Case::Case::get_background_ids() {
	StringVector vec;
	
	// add background ids to vector
	for (BackgroundMapIter it=m_Backgrounds.begin(); it!=m_Backgrounds.end(); ++it)
		vec.push_back((*it).second.id);
	
	return vec;
}

// return a vector of evidence internal ids
StringVector Case::Case::get_evidence_ids() {
	StringVector vec;
	
	// add evidence ids to vector
	for (EvidenceMapIter it=m_Evidence.begin(); it!=m_Evidence.end(); ++it)
		vec.push_back((*it).second.id);
	
	return vec;
}

// return a vector of location internal ids
StringVector Case::Case::get_location_ids() {
	StringVector vec;
	
	// add location ids to vector
	for (LocationMapIter it=m_Locations.begin(); it!=m_Locations.end(); ++it)
		vec.push_back((*it).second.id);
	
	return vec;
}

// clear the case information
void Case::Case::clear() {
	// clear out overview
	m_Overview.name="";
	m_Overview.author="";
	m_Overview.lawSys=TWO_DAY;
	
	m_InitialBlockId="null";
	
	// clear out the rest of the pertinent stuff
	clear_backgrounds();
	clear_characters();
	clear_evidence();
	clear_locations();
}

// set the case overview
void Case::Case::set_overview(const Overview &overview) {
	m_Overview=overview;
}
