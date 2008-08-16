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
// dialogs.h: various dialogs

#ifndef DIALOGS_H
#define DIALOGS_H

#include "ui/ui_bgmanager.h"
#include "ui/ui_charactereditor.h"
#include "ui/ui_charactermanager.h"
#include "ui/ui_evidenceeditor.h"
#include "ui/ui_locationeditor.h"
#include "ui/ui_newcasedialog.h"

#include "case.h"

/** Dialog used to create a new case
*/
class NewCaseDialog: public QDialog, private Ui::NewCaseDialog {
	Q_OBJECT
	
	public:
		/// Constructor
		NewCaseDialog(QWidget *parent=NULL);
};

/************************************************************************************/

/** Dialog used to manage available characters
*/
class CharacterManager: public QDialog, private Ui::CharacterManager {
	Q_OBJECT
	
	public:
		/** Constructor
		  * \param chars Map of characters
		*/
		CharacterManager(const CharacterMap &chars, QWidget *parent=NULL);
		
		/** Return a map of updated characters
		  * \return Updated map of characters
		*/
		CharacterMap getCharacters() const { return m_Characters; }
	
	private slots:
		/// Handler for adding characters
		void onAddClicked();
		
		/// Handler for editing characters
		void onEditClicked();
		
		/// Handler for deleting characters
		void onDeleteClicked();
	
	protected:
		/// Record of characters
		CharacterMap m_Characters;
};

/************************************************************************************/

/** Dialog used to edit character data
*/
class CharacterEditor: public QDialog, private Ui::CharacterEditor {
	Q_OBJECT
	
	public:
		/// Constructor
		CharacterEditor(QWidget *parent=NULL);
		
		/** Set character data for the dialog
		  * \param character Character whose data should be used
		*/
		void setCharacter(const Character &character);
		
		/** Return data from the dialog as a Character object
		  * \return Character object, with data filled in
		*/
		Character getCharacter();
};

/************************************************************************************/

/** Dialog used to manage available backgrounds
*/
class BackgroundManager: public QDialog, private Ui::BGManager {
	Q_OBJECT
	
	public:
		/** Constructor
		  * \param chars Map of backgrounds
		*/
		BackgroundManager(const BackgroundMap &bgs, QWidget *parent=NULL);
		
		/** Return a map of updated backgrounds
		  * \return Updated map of backgrounds
		*/
		BackgroundMap getBackgrounds() const { return m_BGs; }
	
	private slots:
		/// Handler for adding backgrounds
		void onAddClicked();
		
		/// Handler for deleting backgrounds
		void onDeleteClicked();
	
	protected:
		/// Record of backgrounds
		BackgroundMap m_BGs;
};

/************************************************************************************/

/** Dialog used to edit evidence attributes
*/
class EvidenceEditor: public QDialog, private Ui::EvidenceEditor {
	Q_OBJECT
	
	public:
		/** Constructor
		  * \param emap Map of evidence
		*/
		EvidenceEditor(const EvidenceMap &emap, QWidget *parent=NULL);
		
		/** Return updated map of evidence
		  * \return Map of updated evidence
		*/
		EvidenceMap getEvidence() const { return m_Evidence; }
	
	protected:
		/// Record of evidence
		EvidenceMap m_Evidence;
};

/************************************************************************************/

/** Dialog used to edit location attributes
*/
class LocationEditor: public QDialog, private Ui::LocationEditor {
	Q_OBJECT
	
	public:
		/** Constructor
		  * \param lmap Map of locations
		*/
		LocationEditor(const LocationMap &lmap, QWidget *parent=NULL);
	
		/** Return the update location map
		  * \return Map of update locations
		*/
		LocationMap getLocations() const { return m_Locations; }
	
	protected:
		/// Record of locations
		LocationMap m_Locations;
};

#endif
