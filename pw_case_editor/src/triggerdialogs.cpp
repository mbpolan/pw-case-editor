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
// triggerdialogs.cpp: implementation of triggerdialogs.h classes

#include <gtkmm/buttonbox.h>
#include <gtkmm/separator.h>
#include <gtkmm/table.h>

#include "triggerdialogs.h"

// abstract dialog constructor
InsertAbstractDialog::InsertAbstractDialog(const Glib::ustring &trigger) {
	Gtk::VBox *vb=get_vbox();
	vb->set_spacing(5);
	vb->set_border_width(10);
	
	// allocate label
	m_TriggerLabel=manage(new Gtk::Label);
	m_TriggerLabel->set_markup("Trigger: <i>"+trigger+"</i>");
	
	// pack the default label and separator
	vb->pack_start(*m_TriggerLabel);
	vb->pack_start(*manage(new Gtk::HSeparator));
	
	// add buttons
	add_button("OK", Gtk::RESPONSE_OK);
	add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

/***************************************************************************/

// constructor
InsertGotoDialog::InsertGotoDialog(BufferMap blocks): InsertAbstractDialog("goto") {
	construct();
	
	// toggle some widgets
	m_TimeLabel->set_sensitive(false);
	m_TimeEntry->set_sensitive(false);
	
	// fill in combo box
	for (BufferMap::iterator it=blocks.begin(); it!=blocks.end(); ++it) {
		// remove list id
		Glib::ustring id=(*it).first;
		id.erase(id.rfind('_'), id.size());
		
		// add the block
		m_BlockCB->append_text(id);
	}
	
	m_BlockCB->set_active(0);
}

// get the goto trigger data
InsertGotoDialog::Data InsertGotoDialog::get_data() {
	InsertGotoDialog::Data data;
	
	// fill in values
	data.target=m_BlockCB->get_active_text();
	data.time=atoi(m_TimeEntry->get_text().c_str());
	
	if (m_NormalRB->get_active())
		data.type=GOTO_NORMAL;
	else if (m_DirectRB->get_active())
		data.type=GOTO_DIRECT;
	else
		data.type=GOTO_TIMED;
	
	return data;
}

// build the dialog
void InsertGotoDialog::construct() {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate button box
	Gtk::HButtonBox *hbb=manage(new Gtk::HButtonBox);
	
	// allocate radio buttons
	m_NormalRB=manage(new Gtk::RadioButton(m_RadioGroup, "Standard"));
	m_DirectRB=manage(new Gtk::RadioButton(m_RadioGroup, "Direct"));
	m_TimedRB=manage(new Gtk::RadioButton(m_RadioGroup, "Timed"));
	
	// allocate entries
	m_TimeEntry=manage(new Gtk::Entry);
	
	// connect signals
	m_NormalRB->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &InsertGotoDialog::on_radio_button_toggled), "standard"));
	m_DirectRB->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &InsertGotoDialog::on_radio_button_toggled), "direct"));
	m_TimedRB->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &InsertGotoDialog::on_radio_button_toggled), "timed"));
	
	// allocate labels
	m_TypeLabel=manage(new Gtk::Label("Goto Type"));
	m_TargetLabel=manage(new Gtk::Label("Target Block"));
	m_TimeLabel=manage(new Gtk::Label("Delay Time (ms)"));
	
	// allocate combo boxes
	m_BlockCB=manage(new Gtk::ComboBoxText);
	
	// pack buttons
	hbb->pack_start(*m_NormalRB, Gtk::PACK_SHRINK);
	hbb->pack_start(*m_DirectRB, Gtk::PACK_SHRINK);
	hbb->pack_start(*m_TimedRB, Gtk::PACK_SHRINK);
	
	// place widgets
	table->attach(*m_TypeLabel, 0, 1, 0, 1);
	table->attach(*hbb, 1, 2, 0, 1);
	table->attach(*m_TargetLabel, 0, 1, 1, 2);
	table->attach(*m_BlockCB, 1, 2, 1, 2);
	table->attach(*m_TimeLabel, 0, 1, 2, 3);
	table->attach(*m_TimeEntry, 1, 2, 2, 3);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	show_all_children();
}

// radio button toggle handler
void InsertGotoDialog::on_radio_button_toggled(const Glib::ustring &button) {
	bool b;
	
	// toggle widgets
	if (button=="standard" || button=="direct")
		b=false;
	
	else
		b=true;
	
	// time stuff is only relavent for timed goto
	m_TimeLabel->set_sensitive(b);
	m_TimeEntry->set_sensitive(b);
}
