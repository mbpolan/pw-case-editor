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
// locationwidget.cpp: implementation of LocationWidget class

#include <gtkmm/buttonbox.h>
#include <gtkmm/table.h>

#include "intl.h"
#include "locationwidget.h"

// constructor
LocationWidget::LocationWidget(const LocationMap &locations) {
	// allocate table for custom location page
	Gtk::Table *ctable=Gtk::manage(new Gtk::Table);
	ctable->set_spacings(5);
	
	// allocate labels
	m_LocLabel=Gtk::manage(new Gtk::Label(_("Location")));
	
	// allocate combo box
	m_LocCB=Gtk::manage(new LocationComboBox(locations));
	
	// allocate radio buttons
	m_ProsecutorRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Prosecutor Stand")));
	m_DefenseRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Defense Stand")));
	m_DefenseHelperRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Co-Counsel Stand")));
	m_WitnessRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Witness Stand")));
	m_JudgeRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Judge's Stand")));
	m_CourtRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Courtroom Overview")));
	
	// allocate button box
	Gtk::VButtonBox *vbb=Gtk::manage(new Gtk::VButtonBox);
	
	// place widgets
	ctable->attach(*m_LocLabel, 0, 1, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::SHRINK);
	ctable->attach(*m_LocCB, 1, 2, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::SHRINK);
	
	// pack buttons
	vbb->pack_start(*m_ProsecutorRB);
	vbb->pack_start(*m_DefenseRB);
	vbb->pack_start(*m_DefenseHelperRB);
	vbb->pack_start(*m_WitnessRB);
	vbb->pack_start(*m_JudgeRB);
	vbb->pack_start(*m_CourtRB);
	
	append_page(*ctable, _("Custom"));
	append_page(*vbb, _("Court"));
	
	show_all_children();
}

// get the selected location
Glib::ustring LocationWidget::get_selected_location() const {
	if (get_current_page()==0)
		return m_LocCB->get_selected_internal();
	
	else {
		if (m_ProsecutorRB->get_active())
			return "prosecutor_stand";
		
		else if (m_DefenseRB->get_active())
			return "defense_stand";
		
		else if (m_DefenseHelperRB->get_active())
			return "defense_helper_stand";
		
		else if (m_WitnessRB->get_active())
			return "witness_stand";
		
		else if (m_JudgeRB->get_active())
			return "judge_stand";
		
		else
			return "courtroom";
	}
}
