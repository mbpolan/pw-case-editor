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
Character::Character(const ustring &internal, const ustring &name, const ustring &caption, const ustring &description): 
		m_InternalName(internal), m_Name(name), m_Caption(caption), m_Description(description) {
	m_HasTextBoxTag=false;
	m_HasHeadshot=false;
	m_SpriteName="null";
	m_RootAnim="normal";
	m_BadPresentableBlock="null";
	m_Gender=GENDER_MALE;
}

// add a talk option
void Character::addTalkOption(const ustring &viewStr, const ustring &block) {
	m_TalkOptions.push_back(std::make_pair<ustring, ustring>(viewStr, block));
}

// remove a talk option
void Character::removeTalkOption(const ustring &id) {
	// iterate over talk options
	for (std::vector<StringPair>::iterator it=m_TalkOptions.begin(); it!=m_TalkOptions.end(); ++it) {
		if ((*it).first==id) {
			m_TalkOptions.erase(it);
			return;
		}
	}
}

// add a presentable piece of evidence
void Character::addPresentable(const ustring &id, const ustring &target) {
	m_AcceptedItems.push_back(std::make_pair<ustring, ustring>(id, target));
}

// remove presentable evidence
void Character::removePresentable(const ustring &id) {
	// iterate over evidence
	for (std::vector<StringPair>::iterator it=m_AcceptedItems.begin(); it!=m_AcceptedItems.end(); ++it) {
		if ((*it).first==id) {
			m_AcceptedItems.erase(it);
			return;
		}
	}
}
