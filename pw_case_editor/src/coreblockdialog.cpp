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
// coreblockdialog.cpp: implementation of CoreBlockDialog

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/separator.h>
#include <gtkmm/table.h>

#include "clistview.h"
#include "coreblockdialog.h"
#include "intl.h"

// constructor
CoreBlockDialog::CoreBlockDialog(const Data &data) {
	construct();
	
	// update buffers
	m_BadEvidenceView->get_buffer()->set_text(data.xExamineBadEv);
	m_XExamineFailView->get_buffer()->set_text(data.xExamineFail);
}

// build the ui
void CoreBlockDialog::construct() {
	// allocate notebook
	m_NB=manage(new Gtk::Notebook);
	
	// allocate page for 1st page
	Gtk::Table *tables[2];
	tables[0]=manage(new Gtk::Table);
	tables[1]=manage(new Gtk::Table);
	
	tables[0]->set_border_width(10);
	tables[0]->set_spacings(5);
	
	tables[1]->set_border_width(10);
	tables[1]->set_spacings(5);
	
	// allocate labels
	m_BadEvidenceLabel=manage(new Gtk::Label(_("This block is executed when the user presents\n"
						   "evidence that is not applicable during the\n"
						   "cross examination of a witness.")));
	
	m_XExamineFailLabel=manage(new Gtk::Label(_("When the user gets too many penalties during\n"
						    "cross examination, this block is executed to\n"
						    "carry out the game over sequence.")));
	
	// allocate text views
	m_BadEvidenceView=manage(new Gtk::TextView(CListView::create_buffer()));
	m_XExamineFailView=manage(new Gtk::TextView(CListView::create_buffer()));
	
	// allocate containing scrolled windows
	Gtk::ScrolledWindow *windows[2];
	windows[0]=manage(new Gtk::ScrolledWindow);
	windows[1]=manage(new Gtk::ScrolledWindow);
	
	windows[0]->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
	windows[1]->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
	
	windows[0]->add(*m_BadEvidenceView);
	windows[1]->add(*m_XExamineFailView);
	
	Gtk::AttachOptions xops=Gtk::EXPAND | Gtk::FILL;
	Gtk::AttachOptions yops=Gtk::SHRINK;
	
	// place widgets
	tables[0]->attach(*m_BadEvidenceLabel, 0, 1, 0, 1, xops, yops);
	tables[0]->attach(*manage(new Gtk::HSeparator), 0, 1, 1, 2, xops, yops);
	tables[0]->attach(*windows[0], 0, 1, 2, 3);
	
	tables[1]->attach(*m_XExamineFailLabel, 0, 1, 0, 1, xops, yops);
	tables[1]->attach(*manage(new Gtk::HSeparator), 0, 1, 1, 2, xops, yops);
	tables[1]->attach(*windows[1], 0, 1, 2, 3);
	
	// append notebook pages
	m_NB->append_page(*tables[0], _("Cross Examine"));
	m_NB->append_page(*tables[1], _("Game Over"));
	
	get_vbox()->pack_start(*m_NB);
	
	// wreszcie, dodaj przyciski
	add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	show_all_children();
	
}

// get update data
CoreBlockDialog::Data CoreBlockDialog::get_updated_data() {
	Data data;
	data.xExamineBadEv=m_BadEvidenceView->get_buffer()->get_text();
	data.xExamineFail=m_XExamineFailView->get_buffer()->get_text();
	
	return data;
}
