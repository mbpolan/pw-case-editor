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
// casecombobox.h: convenience comboboxes

#ifndef CASECOMBOBOX_H
#define CASECOMBOBOX_H

#include <gtkmm/comboboxtext.h>

#include "case.h"

// combo box for characters
class CharComboBox: public Gtk::ComboBoxText {
	public:
		// constructor
		CharComboBox(const CharacterMap &characters);
		
		// get the selected character's name
		Glib::ustring get_selected_name() const;
		
		// get the selected character's internal name
		Glib::ustring get_selected_internal() const;
		
	protected:
		CharacterMap m_Characters;
};

/***************************************************************************/

// combo box for locations
class LocationComboBox: public Gtk::ComboBoxText {
	public:
		// constructor
		LocationComboBox(const LocationMap &locations);
		
		// get the selected location's name
		Glib::ustring get_selected_name() const;
		
		// get the selected locations's internal name
		Glib::ustring get_selected_internal() const;
		
	protected:
		LocationMap m_Locations;
};

#endif
