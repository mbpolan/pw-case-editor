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
// locationwidget.h: the LocationWidget class

#ifndef LOCATIONWIDGET_H
#define LOCATIONWIDGET_H

#include <gtkmm/box.h>
#include <gtkmm/notebook.h>
#include <gtkmm/radiobutton.h>

#include "case.h"
#include "casecombobox.h"

// widget that lists built-in locations and let's the user choose one
class LocationWidget: public Gtk::Notebook {
	public:
		// constructor
		LocationWidget(const LocationMap &locations);
		
		// get the selected location
		Glib::ustring get_selected_location() const;
		
	private:
		// labels
		Gtk::Label *m_LocLabel;
		
		// combo box
		LocationComboBox *m_LocCB;
		
		// main grouo
		Gtk::RadioButtonGroup m_Group;
		
		// radio buttons
		Gtk::RadioButton *m_ProsecutorRB;
		Gtk::RadioButton *m_DefenseRB;
		Gtk::RadioButton *m_DefenseHelperRB;
		Gtk::RadioButton *m_WitnessRB;
		Gtk::RadioButton *m_CourtRB;
		Gtk::RadioButton *m_JudgeRB;
};

#endif
