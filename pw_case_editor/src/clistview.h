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

/** Implements a customized TreeView interface.
  * Normally, GTK's list widget provides the usual functions and API needed
  * to take care of most editor requirements. This class extends that API 
  * with custom functions that deal specifically with the editor's unique 
  * needs for handling text block nodes, descriptions, and everything else.
*/
class CListView: public Gtk::TreeView {
	public:
		/// Default constructor
		CListView();
		
		/** Create a buffer ready for use in this list.
		  * All text blocks should be created using this function, as it 
		  * also configures the buffer to use styling tags and whatnot.
		  * \return The text block
		*/
		static Glib::RefPtr<Gtk::TextBuffer> create_buffer();
		
		/// Clear all data in this list
		void clear();
		
		/** Append top-level text to this list
		  * \param text The text to append
		*/
		void append_toplevel_text(const Glib::ustring &text);
		
		/** Append text as child of a parent node
		  * \param parent The ID of the parent node
		  * \param id The ID of the new node
		  * \param text The description of the node
		  * \param buffer The text block to associate with this node
		*/
		void append_child_text(const Glib::ustring &parent, const Glib::ustring &id, 
				       const Glib::ustring &text, const Glib::RefPtr<Gtk::TextBuffer> &buffer);
		
		/** Remove a top-level text element
		  * \param text The text node to remove
		*/
		void delete_toplevel_text(const Glib::ustring &text);
		
		/** Activate a text block and emit the display_buffer signal
		  * \param block Either the ID or full text of block
		  * \param row GTK row pointing to the block
		*/
		void select_block(const Glib::ustring &block, Gtk::TreeRow *row);
		
		/** Get the buffers present in the list
		  * \return The map of buffers
		*/
		BufferMap get_buffers() const { return m_Buffers; }
		
		/** Get buffer descriptions for this list
		  * Map parameter 1 is the buffer ID; parameter 2 is the actual description
		  * \return The map of buffer descriptions
		*/
		std::map<Glib::ustring, Glib::ustring> get_buffer_descriptions();
		
		/// Signal to request buffer to be displayed
		sigc::signal<void, Glib::ustring, Glib::RefPtr<Gtk::TextBuffer> > 
				signal_display_buffer() const { return m_DisplayBufferSignal; }
		
		/// Signal to request adding a new text block
		sigc::signal<void, Glib::ustring, bool, CListView*> signal_add_text_block() const { return m_AddBlockSignal; }
		
		/// Signal to request a selection change
		sigc::signal<void, Gtk::TreeModel::iterator> signal_select() const { return m_SelectSignal; }
		
	private:
		/// Signal object to request buffer to be displayed
		sigc::signal<void, Glib::ustring, Glib::RefPtr<Gtk::TextBuffer> > m_DisplayBufferSignal;
		
		/// Signal object to request a row be selected
		sigc::signal<void, Gtk::TreeModel::iterator> m_SelectSignal;
		
		/// Signal object to request adding a new text block
		sigc::signal<void, Glib::ustring, bool, CListView*> m_AddBlockSignal;
		
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
		
		/// Map of buffers
		BufferMap m_Buffers;
};

#endif
