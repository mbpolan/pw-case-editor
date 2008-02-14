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
// textboxdialog.cpp: implementation of TextBoxDialog widget

#include <gtkmm/table.h>

#include "textboxdialog.h"

// constructor
TextBoxDialog::TextBoxDialog() {
	set_title("Format Dialogue");
	set_resizable(false);
	
	construct();
}

// get the formatted text
Glib::ustring TextBoxDialog::get_text() const {
	Glib::ustring final="";
	
	// mark whether we should append a \b to end of string
	bool insertBreak=m_BreakCB->get_active();
	
	// combine text from lines
	final+=m_Line1Entry->get_text();
	if (m_Line2Entry->get_text().empty()) {
		if (insertBreak)
			final+="\\b";
	}
	
	else {
		// line 2
		final+="\\n\n";
		final+=m_Line2Entry->get_text();
		
		if (m_Line3Entry->get_text().empty()) {
			if (insertBreak)
				final+="\\b";
		}
		
		else {
			// line 3
			final+="\\n\n";
			final+=m_Line3Entry->get_text();
			
			if (insertBreak)
				final+="\\b";
		}
	}
	
	return final;
}

// build the ui
void TextBoxDialog::construct() {
	// get default vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate check button
	m_BreakCB=Gtk::manage(new Gtk::CheckButton("Insert break (\\b) after this dialogue"));
	
	// allocate labels
	m_IntroLabel=Gtk::manage(new Gtk::Label("Format character dialogue by entering\ntext into the following entries."));
	m_Line1Label=Gtk::manage(new Gtk::Label("Line 1"));
	m_Line2Label=Gtk::manage(new Gtk::Label("Line 2"));
	m_Line3Label=Gtk::manage(new Gtk::Label("Line 3"));
	
	// allocate entries
	m_Line1Entry=Gtk::manage(new Gtk::Entry);
	m_Line2Entry=Gtk::manage(new Gtk::Entry);
	m_Line3Entry=Gtk::manage(new Gtk::Entry);
	
	// allocate text box preview
	m_TextBox=Gtk::manage(new TextBoxEditor);
	
	// connect signals
	m_Line1Entry->signal_changed().connect(sigc::bind(sigc::mem_fun(*this, &TextBoxDialog::on_entry_changed), 1));
	m_Line2Entry->signal_changed().connect(sigc::bind(sigc::mem_fun(*this, &TextBoxDialog::on_entry_changed), 2));
	m_Line3Entry->signal_changed().connect(sigc::bind(sigc::mem_fun(*this, &TextBoxDialog::on_entry_changed), 3));
	
	// allocate layout table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// place widgets
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	table->attach(*m_IntroLabel, 0, 2, 0, 1, xops, yops);
	table->attach(*m_Line1Label, 0, 1, 1, 2, xops, yops);
	table->attach(*m_Line1Entry, 1, 2, 1, 2, xops, yops);
	table->attach(*m_Line2Label, 0, 1, 2, 3, xops, yops);
	table->attach(*m_Line2Entry, 1, 2, 2, 3, xops, yops);
	table->attach(*m_Line3Label, 0, 1, 3, 4, xops, yops);
	table->attach(*m_Line3Entry, 1, 2, 3, 4, xops, yops);
	table->attach(*m_BreakCB, 0, 2, 4, 5, xops, yops);
	table->attach(*m_TextBox, 0, 2, 5, 6, yops, yops);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	// add buttons
	add_button("OK", Gtk::RESPONSE_OK);
	add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

// entry change handler
void TextBoxDialog::on_entry_changed(int line) {
	// get the appropriate text
	Glib::ustring str="";
	switch(line) {
		case 1: str=m_Line1Entry->get_text(); break;
		case 2: str=m_Line2Entry->get_text(); break;
		case 3: str=m_Line3Entry->get_text(); break;
	}
	
	// and set it
	m_TextBox->set_text(line, str);
	
	queue_draw();
}
