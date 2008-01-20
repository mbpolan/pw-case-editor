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
#include <gtkmm/comboboxtext.h>
#include <gtkmm/label.h>
#include <gtkmm/notebook.h>
#include <gtkmm/paned.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>

#include "clistview.h"

// widget that implements the text editor and other relevant widgets
class ScriptWidget: public Gtk::VBox {
	public:
		// constructor
		ScriptWidget(Case::LawSystem system=Case::TWO_DAY);
		
		// destructor
		~ScriptWidget();
		
		// clear all data
		void clear(Case::LawSystem system);
		
		// activate a page of the internal notebook for trial parts
		void activate_trial_notebook_tab(bool trial);
		
		// add a character to the list
		// passing -1 for both day and stage will add this character to all lists
		void add_character(int day, int stage, const Glib::ustring &name, const Glib::ustring &internal);
		
		// remove a character from the list based on name (either displayed or internal)
		void remove_character(const Glib::ustring &name);
		
		// add a text block under an exiting category
		void add_text_block(int day, int stage, const Glib::ustring &parent,
				    const Glib::ustring &blockName, const Glib::ustring &desc,
				    const Glib::RefPtr<Gtk::TextBuffer> &buffer);
		
		// locates a block within the tree views and returns the toplevel tree view
		CListView* find_block(const Glib::ustring &id, int &index);
		
		// set a list in a notebook tab
		void set_trial_notebook_list(int index);
		
		// insert a trigger at the cursor
		void insert_trigger_at_cursor(const Glib::ustring &str);
		
		// return buffers used in internal list
		BufferMap get_buffers() const;
		
		// return buffer descriptions
		std::map<Glib::ustring, Glib::ustring> get_buffer_descriptions();
		
		// return the currently displayed buffer
		Glib::RefPtr<Gtk::TextBuffer> get_current_buffer() { return m_TextView->get_buffer(); }
	
	private:
		// build the ui
		void construct();
		
		// get the current tree view
		CListView* get_current_list();
		
		// get a unique id for a text block
		Glib::ustring unique_id(const Glib::ustring &rootString);
		
		// clistview handler to add a new text block
		void on_list_add_text_block(const Glib::ustring &root, bool isCharacter, CListView *list);
		
		// reset the combo box
		void reset_combo_box();
		
		// combo box selection handler
		void on_combo_box_changed();
		
		// display buffer handler
		void on_display_buffer(Glib::ustring id, Glib::RefPtr<Gtk::TextBuffer> buffer);
		
		// select a row
		void on_select_row(Gtk::TreeModel::iterator it);
		
		// combo box
		Gtk::ComboBoxText *m_DayCB;
		
		// labels
		Gtk::Label *m_CurBlockLabel;
		
		// container widgets
		Gtk::Notebook *m_StageNB;
		Gtk::ScrolledWindow *m_TextSWindow;
		Gtk::ScrolledWindow *m_TreeSWindow;
		Gtk::HPaned *m_HPane;
		
		// the text view for editing the script
		Gtk::TextView *m_TextView;
		
		// list views
		std::vector<CListView*> m_TreeViews;
		
		// keep track of law system
		Case::LawSystem m_LawSystem;
};

#endif
