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
// case.cpp: implementations of Case structs and classes

#include "case.h"

namespace Case {

QString g_DefaultBlocks[16];

}; // namespace Case

// constructor
Case::Case::Case() {
	// clear this case out
	clear();
}

// add a character
void Case::Case::addCharacter(const Character &character) {
	m_Characters[character.getName()]=character;
}

// remove a character based on name
void Case::Case::removeCharacter(const QString &name) {
	// iterate over characters and find the requested one
	for (CharacterMap::iterator it=m_Characters.begin(); it!=m_Characters.end(); ++it) {
		if ((*it).first==name) {
			m_Characters.erase(it);
			return;
		}
	}
}

// add an image
void Case::Case::addImage(const Image &image) {
	m_Images[image.id]=image;
}

// remove an image
void Case::Case::removeImage(const QString &id) {
	// iterate over images and find the requested one
	for (ImageMap::iterator it=m_Images.begin(); it!=m_Images.end(); ++it) {
		if ((*it).first==id) {
			m_Images.erase(it);
			return;
		}
	}
}

// add a piece of evidence
void Case::Case::addEvidence(const Evidence &evidence) {
	m_Evidence[evidence.id]=evidence;
}

// remove a piece of evidence based on id
void Case::Case::removeEvidence(const QString &id) {
	// iterate over evidence and find the requested one
	for (std::map<QString, Evidence>::iterator it=m_Evidence.begin(); it!=m_Evidence.end(); ++it) {
		if ((*it).first==id) {
			m_Evidence.erase(it);
			return;
		}
	}
}

// add testimony
void Case::Case::addTestimony(const Testimony &testimony) {
	m_Testimonies[testimony.id]=testimony;
}

// remove testimony
void Case::Case::removeTestimony(const QString &id) {
	// iterate over testimony and find the requested one
	for (std::map<QString, Testimony>::iterator it=m_Testimonies.begin(); it!=m_Testimonies.end(); ++it) {
		if ((*it).first==id) {
			m_Testimonies.erase(it);
			return;
		}
	}
}

// add a background
void Case::Case::addBackground(const Background &bg) {
	m_Backgrounds[bg.id]=bg;
}

// remove a background based on id
void Case::Case::removeBackground(const QString &id) {
	// iterate over entries
	for (std::map<QString, Background>::iterator it=m_Backgrounds.begin(); it!=m_Backgrounds.end(); ++it) {
		if ((*it).first==id) {
			m_Backgrounds.erase(it);
			return;
		}
	}
}

// add a location
void Case::Case::addLocation(const Location &loc) {
	m_Locations[loc.id]=loc;
}

// add an audio sample
void Case::Case::addAudio(const Audio &audio) {
	m_Audio[audio.id]=audio;
}

// remove a location based on id
void Case::Case::removeLocation(const QString &id) {
	m_Locations.erase(id);
}

// return a vector of character internal names
StringVector Case::Case::getCharacterNames() {
	StringVector vec;
	
	// simply add names to the vector
	for (CharacterMap::iterator it=m_Characters.begin(); it!=m_Characters.end(); ++it)
		vec.push_back((*it).second.getInternalName());
	
	return vec;
}

// return a vector of image internal ids
StringVector Case::Case::getImageIDs() {
	StringVector vec;
	
	// simply add ids to the vector
	for (ImageMap::iterator it=m_Images.begin(); it!=m_Images.end(); ++it)
		vec.push_back((*it).first);
	
	return vec;
}

// return a vector of background internal ids
StringVector Case::Case::getBackgroundIDs() {
	StringVector vec;
	
	// add background ids to vector
	for (BackgroundMap::iterator it=m_Backgrounds.begin(); it!=m_Backgrounds.end(); ++it)
		vec.push_back((*it).second.id);
	
	return vec;
}

// return a vector of evidence internal ids
StringVector Case::Case::getEvidenceIDs() {
	StringVector vec;
	
	// add evidence ids to vector
	for (EvidenceMap::iterator it=m_Evidence.begin(); it!=m_Evidence.end(); ++it)
		vec.push_back((*it).second.id);
	
	return vec;
}

// return a vector of location internal ids
StringVector Case::Case::getLocationIDs() {
	StringVector vec;
	
	// add location ids to vector
	for (LocationMap::iterator it=m_Locations.begin(); it!=m_Locations.end(); ++it)
		vec.push_back((*it).second.id);
	
	return vec;
}

// return a vector of audio internal ids
StringVector Case::Case::getAudioIDs() {
	StringVector vec;
	
	// add location ids to vector
	for (AudioMap::iterator it=m_Audio.begin(); it!=m_Audio.end(); ++it)
		vec.push_back((*it).second.id);
	
	return vec;
}

// return a vector of testimony internal ids
StringVector Case::Case::getTestimonyIDs() {
	StringVector vec;
	
	// add location ids to vector
	for (TestimonyMap::iterator it=m_Testimonies.begin(); it!=m_Testimonies.end(); ++it)
		vec.push_back((*it).second.id);
	
	return vec;
}

// clear the case information
void Case::Case::clear() {
	// clear out overview
	m_Overview.name="";
	m_Overview.author="";
	m_Overview.days=3;
	
	// clear out overrides
	m_Overrides.textboxAlpha=-1;
	m_Overrides.titleScreen="null";
	
	m_InitialBlockId="null";
	
	// clear core blocks
	m_CoreBlocks.clear();
	m_CoreBlocks.push_back("");
	m_CoreBlocks.push_back("");
	
	// clear out the rest of the pertinent stuff
	clearBackgrounds();
	clearCharacters();
	clearImages();
	clearEvidence();
	clearLocations();
	clearAudio();
}

// set the case overview
void Case::Case::setOverview(const Overview &overview) {
	m_Overview=overview;
}
