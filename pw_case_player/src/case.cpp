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

// destructor
Case::Case::~Case() {
	m_Characters.clear();
	m_Evidence.clear();
	m_Backgrounds.clear();
	m_Locations.clear();
}

// add a character
void Case::Case::addCharacter(const Character &character) {
	m_Characters[character.getInternalName()]=character;
}

// add a piece of evidence
void Case::Case::addEvidence(const Evidence &evidence) {
	m_Evidence[evidence.id]=evidence;
}

// add a background
void Case::Case::addBackground(const Background &bg) {
	m_Backgrounds[bg.id]=bg;
}

// add a location
void Case::Case::addLocation(const Location &loc) {
	m_Locations[loc.id]=loc;
}

// add a text buffer
void Case::Case::addBuffer(const std::string &id, const std::string &contents) {
	m_Buffers[id]=contents;
}

// get a character
Character* Case::Case::getCharacter(const std::string &id) {
	if (m_Characters.find(id)!=m_Characters.end())
		return &m_Characters[id];
	else
		return NULL;
}

// get a background
Case::Background* Case::Case::getBackground(const std::string &id) {
	if (m_Backgrounds.find(id)!=m_Backgrounds.end())
		return &m_Backgrounds[id];
	else
		return NULL;
}

// get a piece of evidence
Case::Evidence* Case::Case::getEvidence(const std::string &id) {
	if (m_Evidence.find(id)!=m_Evidence.end())
		return &m_Evidence[id];
	else
		return NULL;
}

// get a location
Case::Location* Case::Case::getLocation(const std::string &id) {
	if (m_Locations.find(id)!=m_Locations.end())
		return &m_Locations[id];
	else
		return NULL;
}

// clear the case information
void Case::Case::clear() {
	// clear out overview
	m_Overview.name="";
	m_Overview.author="";
	m_Overview.lawSys=TWO_DAY;
	
	m_InitialBlockId="null";
	
	// clear out the rest of the pertinent stuff
	m_Evidence.clear();
	m_Backgrounds.clear();
	m_Characters.clear();
	m_Locations.clear();
	m_Buffers.clear();
}

// set the case overview
void Case::Case::setOverview(const Overview &overview) {
	m_Overview=overview;
}
