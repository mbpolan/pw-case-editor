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
// scriptwidget.h: the ScriptWidget class

#ifndef SCRIPTWIDGET_H
#define SCRIPTWIDGET_H

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/paned.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>

#include "clistview.h"

// widget that implements the text editor and other relevant widgets
class ScriptWidget: public Gtk::VBox {
	public:
		// constructor
		ScriptWidget();
		
		// clear all data
		void clear();
		
		// add a character to the list
		void add_character(const Glib::ustring &name, const Glib::ustring &internal);
		
		// remove a character from the list based on name (either displayed or internal)
		void remove_character(const Glib::ustring &name);
		
		// add a text block under an exiting category
		void add_text_block(const Glib::ustring &parent, const Glib::ustring &blockName, const Glib::ustring &desc,
				    const Glib::RefPtr<Gtk::TextBuffer> &buffer);
		
		// return buffers used in internal list
		BufferMap get_buffers() const { return m_TreeView->get_buffers(); }
		
		// return buffer descriptions
		std::map<Glib::ustring, Glib::ustring> get_buffer_descriptions() { return m_TreeView->get_buffer_descriptions(); }
		
		// return the currently displayed buffer
		Glib::RefPtr<Gtk::TextBuffer> get_current_buffer() { return m_TextView->get_buffer(); }
	
	private:
		// build the ui
		void construct();
		
		// display buffer handler
		void on_display_buffer(Glib::ustring id, Glib::RefPtr<Gtk::TextBuffer> buffer);
		
		// select a row
		void on_select_row(Gtk::TreeModel::iterator it);
		
		// labels
		Gtk::Label *m_CurBlockLabel;
		
		// container widgets
		Gtk::ScrolledWindow *m_TextSWindow;
		Gtk::ScrolledWindow *m_TreeSWindow;
		Gtk::HPaned *m_HPane;
		
		// the text view for editing the script
		Gtk::TextView *m_TextView;
		
		// list view
		CListView *m_TreeView;
};

#endif
