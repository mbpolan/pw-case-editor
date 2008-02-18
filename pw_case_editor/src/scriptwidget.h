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

/** Widget that implements the text editor and other relevant widgets.
  * The body of a MainWindow class consists primarily of this widget. It implements 
  * the text editor where the text blocks are displayed, along with the series of 
  * lists where the blocks themselves are organized.
*/
class ScriptWidget: public Gtk::VBox {
	public:
		/** Constructor
		  * \param system The type of LawSystem in this case
		*/
		ScriptWidget(Case::LawSystem system=Case::TWO_DAY);
		
		/// Destructor
		~ScriptWidget();
		
		/** Completely reset the entire widget
		  * \param system The new LawSystem to use
		*/
		void clear(Case::LawSystem system);
		
		/** Switch between the Investigation and Trial notebook tabs
		  * \param trial <b>true</b> for Trial, <b>false</b> for Investigation
		*/
		void activate_trial_notebook_tab(bool trial);
		
		/** Add a character to a text block list
		  * Passing -1 for both day and stage will add this character to all lists
		  * \param day The day (based on LawSystem), 0 based index
		  * \param stage 0 for Investigation, 1 for Trial
		  * \param name The display name for the character
		  * \param internal The internal name for the character
		*/
		void add_character(int day, int stage, const Glib::ustring &name, const Glib::ustring &internal);
		
		/** Remove a character from the lists
		  * \param name Either internal or display name of character to remove
		*/
		void remove_character(const Glib::ustring &name);
		
		/** Add a text block under an exiting top-level node
		  * \param day The day of the case (based on LawSystem), 0 based index
		  * \param stage 0 for Investigation, 1 for Trial
		  * \param parent The name of the parent node
		  * \param blockName The name of the block
		  * \param desc Brief description of the block
		  * \param buffer The text of the block
		*/
		void add_text_block(int day, int stage, const Glib::ustring &parent,
				    const Glib::ustring &blockName, const Glib::ustring &desc,
				    const Glib::RefPtr<Gtk::TextBuffer> &buffer);
		
		/** Locate a block within the tree views and returns the top-level tree view
		  * \param id ID of the block
		  * \param index 0 based index of the lists
		  * \return Pointer to a list where the block was found, null if not found
		*/
		CListView* find_block(const Glib::ustring &id, int &index);
		
		/** Set a list to display in a notebook tab
		  * \param index 0 based index of the list
		*/
		void set_trial_notebook_list(int index);
		
		/** Insert text at the cursor in current block
		  * \param str The text
		*/
		void insert_text_at_cursor(const Glib::ustring &str);
		
		/** Get all of the buffers in this widget
		  * \return A map of all buffers
		*/
		BufferMap get_buffers() const;
		
		/** Get the buffer descriptions
		  * \return A map of block IDs and descriptions
		*/
		std::map<Glib::ustring, Glib::ustring> get_buffer_descriptions();
		
		/** Get the currently displayed block
		  * \return The text of the current block
		*/
		Glib::RefPtr<Gtk::TextBuffer> get_current_buffer() { return m_TextView->get_buffer(); }
	
	private:
		/// Build the widget's UI
		void construct();
		
		/** Get the currently displayed list
		  * \return Pointer to current list
		*/
		CListView* get_current_list();
		
		/** Get a unique ID for a text block
		  * \param rootString The text to preprend to the ID
		  * \return An ID that is unique for this block
		*/
		Glib::ustring unique_id(const Glib::ustring &rootString);
		
		/** Handler to add a new text block
		  * \param root The root node
		  * \param isCharacter Whether or not this is a character's text block
		  * \param list The list to add the block to
		*/
		void on_list_add_text_block(const Glib::ustring &root, bool isCharacter, CListView *list);
		
		/// Reset the combo box with the case days
		void reset_combo_box();
		
		/// Handler for day combo box selection changes
		void on_combo_box_changed();
		
		/** Handler to display a buffer in the editor
		  * \param id The ID of the block
		  * \param buffer The text of the buffer
		*/
		void on_display_buffer(Glib::ustring id, Glib::RefPtr<Gtk::TextBuffer> buffer);
		
		/** Handler to select a row of the current list
		  * \param it Iterator pointing to the node
		*/
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
		
		/// Vector of lists
		std::vector<CListView*> m_TreeViews;
		
		/// Internal record of LawSystem
		Case::LawSystem m_LawSystem;
};

#endif
