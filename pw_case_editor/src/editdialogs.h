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
// editdialogs.h: various Edit menu based dialogs

#ifndef EDITDIALOGS_H
#define EDITDIALOGS_H

#include <gtkmm/button.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/listviewtext.h>
#include <gtkmm/scrolledwindow.h>

#include "case.h"

/** Dialog used to search through text blocks.
  * Queries can be carried out using this dialog, which performs a literal
  * search of all text blocks for user provided substrings.
*/
class FindDialog: public Gtk::Dialog {
	public:
		/** Constructor
		  * \param buffers Map of all case buffers
		*/
		FindDialog(const BufferMap &buffers);
		
		/** Get the selected block result
		  * \return The selected result, which is the ID of a text block
		*/
		Glib::ustring get_selected() { return m_ResultsList->get_text(m_ResultsList->get_selected()[0], 0); }
		
	private:
		/// Build the dialog's UI
		void construct();
		
		// search button click handler
		void on_search_clicked();
		
		// query text changed handler
		void on_entry_text_changed();
		
		// list selection change handler
		void on_selection_changed();
		
		// selected block
		Glib::ustring m_CurBlock;
		
		// labels
		Gtk::Label *m_QueryLabel;
		Gtk::Label *m_ResultsLabel;
		
		// buttons
		Gtk::Button *m_SearchButton;
		Gtk::Button *m_GoButton;
		
		// enties
		Gtk::Entry *m_QueryEntry;
		
		// containers
		Gtk::ScrolledWindow *m_SWindow;
		
		// listview for results
		Gtk::ListViewText *m_ResultsList;
		
		/// Record of case text blocks
		BufferMap m_Buffers;
};

#endif
