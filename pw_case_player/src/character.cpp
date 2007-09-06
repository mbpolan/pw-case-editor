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
// character.cpp: implementation of Character class

#include "character.h"

// constructor
Character::Character(const std::string &internal, const std::string &name, const std::string &caption, const std::string &description): 
		m_InternalName(internal), m_Name(name), m_Caption(caption), m_Description(description) {
	m_HasTextBoxTag=false;
	m_HasHeadshot=false;
	m_SpriteName="null";
	m_Gender=GENDER_MALE;
}

// add a talk option
void Character::addTalkOption(const std::string &viewStr, const std::string &block) {
	m_TalkOptions.push_back(std::make_pair<std::string, std::string>(viewStr, block));
}

// remove a talk option
void Character::removeTalkOption(const std::string &id) {
	// iterate over talk options
	for (std::vector<StringPair>::iterator it=m_TalkOptions.begin(); it!=m_TalkOptions.end(); ++it) {
		if ((*it).first==id) {
			m_TalkOptions.erase(it);
			return;
		}
	}
}
