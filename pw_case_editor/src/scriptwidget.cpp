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
// scriptwidget.cpp: implementation of ScriptWidget class

#include "scriptwidget.h"

// constructor
ScriptWidget::ScriptWidget() {
	construct();
}

// clear all data
void ScriptWidget::clear() {
	// clear out text block list
	m_TreeView->clear();
}

// add a character to the list
void ScriptWidget::add_character(const Glib::ustring &name, const Glib::ustring &internal) {
	// form the string
	Glib::ustring str=name;
	str+=" (";
	str+=internal;
	str+=")";
	
	// append this text
	m_TreeView->append_toplevel_text(str);
}

// remove a character from the list
void ScriptWidget::remove_character(const Glib::ustring &name) {
	m_TreeView->delete_toplevel_text(name);
}

// add a text block under an exiting category
void ScriptWidget::add_text_block(const Glib::ustring &parent, const Glib::ustring &blockName, const Glib::ustring &desc,
				  const Glib::RefPtr<Gtk::TextBuffer> &buffer) {
	// form the string
	Glib::ustring str=blockName;
	str+=" (";
	str+=desc;
	str+=")";
	
	// append this text
	m_TreeView->append_child_text(parent, blockName, str, buffer);
}

// build the ui
void ScriptWidget::construct() {
	
	// allocate label
	m_CurBlockLabel=manage(new Gtk::Label("Current Text Block: "));
	
	// allocate the text view widget
	m_TextView=manage(new Gtk::TextView);
	
	// disable editing
	m_TextView->set_editable(false);
	
	// allocate vbox
	Gtk::VBox *vb=manage(new Gtk::VBox);
	
	// allocate its containing window
	m_TextSWindow=manage(new Gtk::ScrolledWindow);
	m_TextSWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
	
	// pack widgets
	m_TextSWindow->add(*m_TextView);
	vb->pack_start(*m_CurBlockLabel, Gtk::PACK_SHRINK);
	vb->pack_start(*m_TextSWindow);
	
	// allocate tree view
	m_TreeView=manage(new CListView);
	
	// connect treeview signals
	m_TreeView->signal_display_buffer().connect(sigc::mem_fun(*this, &ScriptWidget::on_display_buffer));
	m_TreeView->signal_select().connect(sigc::mem_fun(*this, &ScriptWidget::on_select_row));
	
	// allocate its containing window
	m_TreeSWindow=manage(new Gtk::ScrolledWindow);
	m_TreeSWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
	
	// pack widgets
	m_TreeSWindow->add(*m_TreeView);
	
	// allocate parent pane
	m_HPane=manage(new Gtk::HPaned);
	m_HPane->pack1(*m_TreeSWindow);
	m_HPane->pack2(*vb);
	
	// set an initial position
	m_HPane->set_position(10);
	
	pack_start(*m_HPane);
	
	show_all_children();
}

// display buffer handler
void ScriptWidget::on_display_buffer(Glib::ustring id, Glib::RefPtr<Gtk::TextBuffer> buffer) {
	// make this the current buffer to edit
	m_TextView->set_buffer(buffer);
	
	// update label
	Glib::ustring str="Current Text Block: ";
	str+=id;
	m_CurBlockLabel->set_text(str);
	
	// enable editing
	m_TextView->set_editable(true);
}

// select a row
void ScriptWidget::on_select_row(Gtk::TreeModel::iterator it) {
	m_TreeView->get_selection()->select(it);
}
