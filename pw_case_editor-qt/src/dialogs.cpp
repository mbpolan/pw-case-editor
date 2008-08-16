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
// dialogs.cpp: implementations of dialogs

#include "dialogs.h"

// constructor
NewCaseDialog::NewCaseDialog(QWidget *parent): QDialog(parent) {
	setupUi(this);
}

/************************************************************************************/

// constructor
CharacterManager::CharacterManager(const CharacterMap &chars, QWidget *parent): QDialog(parent) {
	setupUi(this);
	
	// connect signals
	connect(m_AddButton, SIGNAL(clicked()), this, SLOT(onAddClicked()));
	connect(m_EditButton, SIGNAL(clicked()), this, SLOT(onEditClicked()));
	connect(m_DeleteButton, SIGNAL(clicked()), this, SLOT(onDeleteClicked()));
	
	m_Characters=chars;
	
	// add each character as a new line
	for (CharacterMap::iterator it=m_Characters.begin(); it!=m_Characters.end(); ++it)
		m_CharList->addItem((*it).first);
}

// handler for add button clicks
void CharacterManager::onAddClicked() {
	// launch the editor
	CharacterEditor ce;
	if (ce.exec()==QDialog::Accepted) {
		Character nchar=ce.getCharacter();
		
		// add a new line
		m_CharList->addItem(nchar.getName());
		
		// and add the character to the map
		m_Characters[nchar.getName()]=nchar;
	}
}

// handler for edit button clicks
void CharacterManager::onEditClicked() {
	// launch the editor
	CharacterEditor ce;
	QString old=m_CharList->currentItem()->text();
	
	// set our data
	ce.setCharacter(m_Characters[old]);
	
	// run the dialog
	if (ce.exec()==QDialog::Accepted) {
		// update old data
		Character nchar=ce.getCharacter();
		m_Characters[nchar.getInternalName()]=nchar;
		
		// see if we need to update the list
		if (old!=nchar.getName()) {
			QListWidgetItem* item=*(m_CharList->findItems(old, Qt::MatchFixedString).begin());
			item->setText(nchar.getName());
			m_CharList->editItem(item);
			
			// remove old reference and add new one to map
			m_Characters.erase(old);
			m_Characters[nchar.getName()]=nchar;
		}
	}
}

// handler for delete button clicks
void CharacterManager::onDeleteClicked() {
	int index=m_CharList->currentRow();
	if (index>=0) {
		QListWidgetItem *item=m_CharList->takeItem(index);
	
		// remove reference from map
		m_Characters.erase(item->text());
		delete item;
	}
}

/************************************************************************************/

// constructor
CharacterEditor::CharacterEditor(QWidget *parent): QDialog(parent) {
	setupUi(this);
}

// set character data to use in the dialog
void CharacterEditor::setCharacter(const Character &character) {
	if (character.getGender()==Character::GENDER_MALE)
		m_GenderCB->setCurrentIndex(0);
	else if (character.getGender()==Character::GENDER_FEMALE)
		m_GenderCB->setCurrentIndex(1);
	else
		m_GenderCB->setCurrentIndex(2);
	
	m_IDEdit->setText(character.getInternalName());
	m_NameEdit->setText(character.getName());
	m_CaptionEdit->setText(character.getCaption());
	m_DescEdit->setText(character.getDescription());
	m_SpriteEdit->setText(character.getSpriteName());
}

// get serialized character data from dialog
Character CharacterEditor::getCharacter() {
	Character nchar;
	
	// fill in values
	int index=m_GenderCB->currentIndex();
	if (index==0)
		nchar.setGender(Character::GENDER_MALE);
	else if (index==1)
		nchar.setGender(Character::GENDER_FEMALE);
	else
		nchar.setGender(Character::GENDER_UNKNOWN);
	
	nchar.setInternalName(m_IDEdit->text());
	nchar.setName(m_NameEdit->text());
	nchar.setCaption(m_CaptionEdit->text());
	nchar.setDescription(m_DescEdit->text());
	nchar.setSpriteName(m_SpriteEdit->text());
	
	return nchar;
}

/************************************************************************************/

// constructor
BackgroundManager::BackgroundManager(const BackgroundMap &bgs, QWidget *parent): QDialog(parent) {
	setupUi(this);
	
	// connect signals
	connect(m_AddButton, SIGNAL(clicked()), this, SLOT(onAddClicked()));
	connect(m_DeleteButton, SIGNAL(clicked()), this, SLOT(onDeleteClicked()));
	
	m_BGs=bgs;
	
	// add each character as a new line
	for (BackgroundMap::iterator it=m_BGs.begin(); it!=m_BGs.end(); ++it)
		m_BGList->addItem((*it).first);
}

// handler for adding backgrounds
void BackgroundManager::onAddClicked() {
}

// handler for deleting backgrounds
void BackgroundManager::onDeleteClicked() {
}

/************************************************************************************/

// constructor
EvidenceEditor::EvidenceEditor(const EvidenceMap &emap, QWidget *parent): QDialog(parent) {
	setupUi(this);
	
	m_Evidence=emap;
}

/************************************************************************************/

// constructor
LocationEditor::LocationEditor(const LocationMap &lmap, QWidget *parent): QDialog(parent) {
	setupUi(this);
	
	m_Locations=lmap;
	
	// iterate and add each location
	for (LocationMap::iterator it=m_Locations.begin(); it!=m_Locations.end(); ++it) {
		QListWidgetItem *item=new QListWidgetItem((*it).second.name, m_LocationList);
		item->setData(Qt::UserRole, QVariant((*it).second.id));
		
		m_LocationList->addItem(item);
	}
}
