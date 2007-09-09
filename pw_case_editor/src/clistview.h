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
// clistview.h: customized TreeView class

#ifndef CLISTVIEW_H
#define CLISTVIEW_H

#include <gtkmm/menu.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeview.h>

#include "case.h"

// implements a customized TreeView interface
class CListView: public Gtk::TreeView {
	public:
		// constructor
		CListView();
		
		// create a buffer ready for use in this list
		static Glib::RefPtr<Gtk::TextBuffer> create_buffer();
		
		// clear all data
		void clear();
		
		// append toplevel text to list
		void append_toplevel_text(const Glib::ustring &text);
		
		// append text as child of a parent node
		void append_child_text(const Glib::ustring &parent, const Glib::ustring &id, 
				       const Glib::ustring &text, const Glib::RefPtr<Gtk::TextBuffer> &buffer);
		
		// remove a toplevel text element
		void delete_toplevel_text(const Glib::ustring &text);
		
		// return the buffers present in the list
		// template argument 1 is buffer id; argument 2 is the buffer
		BufferMap get_buffers() const { return m_Buffers; }
		
		// return buffer descriptions
		// template argument 1: buffer id; argument 2 is the actual description
		std::map<Glib::ustring, Glib::ustring> get_buffer_descriptions();
		
		// signal to request buffer to be displayed
		sigc::signal<void, Glib::ustring, Glib::RefPtr<Gtk::TextBuffer> > 
				signal_display_buffer() const { return m_DisplayBufferSignal; }
		
		// signal to request a selection
		sigc::signal<void, Gtk::TreeModel::iterator> signal_select() const { return m_SelectSignal; }
		
	private:
		// signal to request buffer to be displayed
		sigc::signal<void, Glib::ustring, Glib::RefPtr<Gtk::TextBuffer> > m_DisplayBufferSignal;
		
		// signal to request a row be selected
		sigc::signal<void, Gtk::TreeModel::iterator> m_SelectSignal;
		
		// handle selection changes
		void on_selection_changed();
		
		// add a text block handler
		void on_add_text_block();
		
		// delete a text block handler
		void on_delete_text_block();
		
		// edit a text block's description
		void on_edit_description();
		
		// follow a block's GOTO trigger to its target block
		void on_follow_goto();
		
		// button press handler
		virtual bool on_button_press_event(GdkEventButton *e);
		
		// context menu
		Gtk::Menu m_ContextMenu;
		
		// model
		Glib::RefPtr<Gtk::TreeStore> m_Model;
		
		// column record
		// column record for tree view
		class ColumnRec: public Gtk::TreeModel::ColumnRecord {
			public:
				// constructor
				ColumnRec() {
					add(m_Column);
				}
				
				// columns
				Gtk::TreeModelColumn<Glib::ustring> m_Column;
		};
		
		// column record instance
		ColumnRec m_ColumnRec;
		
		// map of buffers (first template argument is internal block id without description)
		BufferMap m_Buffers;
};

#endif
