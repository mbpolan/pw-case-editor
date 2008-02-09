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
	
	// add some default blocks
	addBuffer("INTERNAL_testimony", STR_NULL);
}

// destructor
Case::Case::~Case() {
	m_Characters.clear();
	m_Evidence.clear();
	m_Images.clear();
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

// add an image
void Case::Case::addImage(const Image &img) {
	m_Images[img.id]=img;
}

// add a location
void Case::Case::addLocation(const Location &loc) {
	m_Locations[loc.id]=loc;
}

// add a testimony
void Case::Case::addTestimony(const Testimony &testimony) {
	m_Testimonies[testimony.id]=testimony;
}

// add a text buffer
void Case::Case::addBuffer(const ustring &id, const ustring &contents) {
	m_Buffers[id]=contents;
}

// get a character
Character* Case::Case::getCharacter(const ustring &id) {
	if (m_Characters.find(id)!=m_Characters.end())
		return &m_Characters[id];
	else
		return NULL;
}

// get a background
Case::Background* Case::Case::getBackground(const ustring &id) {
	if (m_Backgrounds.find(id)!=m_Backgrounds.end())
		return &m_Backgrounds[id];
	else
		return NULL;
}

// get a piece of evidence
Case::Evidence* Case::Case::getEvidence(const ustring &id) {
	if (m_Evidence.find(id)!=m_Evidence.end())
		return &m_Evidence[id];
	else
		return NULL;
}

// get an image
Case::Image* Case::Case::getImage(const ustring &id) {
	if (m_Images.find(id)!=m_Images.end())
		return &m_Images[id];
	else
		return NULL;
}

// get a location
Case::Location* Case::Case::getLocation(const ustring &id) {
	if (m_Locations.find(id)!=m_Locations.end())
		return &m_Locations[id];
	else
		return NULL;
}

// get a testimony
Case::Testimony* Case::Case::getTestimony(const ustring &id) {
	if (m_Testimonies.find(id)!=m_Testimonies.end())
		return &m_Testimonies[id];
	else
		return NULL;
}

// return a vector of evidence based on vector of string ids
std::vector<Case::Evidence*> Case::Case::getEvidenceFromIds(const StringVector &vec) {
	std::vector<Evidence*> ret;
	for (int i=0; i<vec.size(); i++)
		ret.push_back(getEvidence(vec[i]));
	
	return ret;
}

// return a vector of characters based on vector of string ids
std::vector<Character*> Case::Case::getCharactersFromIds(const StringVector &vec) {
	std::vector<Character*> ret;
	for (int i=0; i<vec.size(); i++)
		ret.push_back(getCharacter(vec[i]));
	
	return ret;
}

// clear the case information
void Case::Case::clear() {
	// clear out overrides
	m_Overrides.textboxAlpha=-1;
	m_Overrides.titleScreen=STR_NULL;
	
	// clear out overview
	m_Overview.name="";
	m_Overview.author="";
	m_Overview.lawSys=TWO_DAY;
	
	m_InitialBlockId=STR_NULL;
	
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
