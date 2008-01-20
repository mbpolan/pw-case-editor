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
// editdialogs.cpp: implementation of dialogs

#include <gtkmm/separator.h>
#include <gtkmm/table.h>
#include <gtkmm/treemodel.h>
#include <sstream>

#include "editdialogs.h"

// constructor
FindDialog::FindDialog(const BufferMap &buffers): m_Buffers(buffers) {
	construct();
	m_CurBlock="null";
}

// build the ui
void FindDialog::construct() {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate table
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_QueryLabel=manage(new Gtk::Label("Query"));
	m_ResultsLabel=manage(new Gtk::Label);
	
	// allocate buttons
	m_SearchButton=manage(new Gtk::Button("Search"));
	m_SearchButton->set_sensitive(false);
	m_SearchButton->signal_clicked().connect(sigc::mem_fun(*this, &FindDialog::on_search_clicked));
	
	// allocate entries
	m_QueryEntry=manage(new Gtk::Entry);
	m_QueryEntry->signal_changed().connect(sigc::mem_fun(*this, &FindDialog::on_entry_text_changed));
	
	// allocate list view
	m_ResultsList=manage(new Gtk::ListViewText(2));
	m_ResultsList->set_column_title(0, "Block ID");
	m_ResultsList->set_column_title(1, "Context");
	
	// connect selection change signal
	m_ResultsList->get_selection()->signal_changed().connect(
				     sigc::mem_fun(*this, &FindDialog::on_selection_changed));
	
	// allocate containers
	m_SWindow=manage(new Gtk::ScrolledWindow);
	m_SWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_SWindow->add(*m_ResultsList);
	m_SWindow->set_size_request(200, 250);
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_QueryLabel, 0, 1, 0, 1, xops, yops);
	table->attach(*m_QueryEntry, 1, 2, 0, 1, xops, yops);
	table->attach(*m_SearchButton, 2, 3, 0, 1, xops, yops);
	table->attach(*manage(new Gtk::HSeparator), 0, 3, 1, 2, xops, yops);
	table->attach(*m_ResultsLabel, 0, 3, 2, 3, xops, yops);
	table->attach(*m_SWindow, 0, 3, 3, 4, xops);
	
	vb->pack_start(*table);
	
	// disable go button by default
	m_GoButton=add_button("Go to Block", Gtk::RESPONSE_OK);
	m_GoButton->set_sensitive(false);
	
	add_button("Close", Gtk::RESPONSE_CLOSE);
	
	show_all_children();
}

// search button click handler
void FindDialog::on_search_clicked() {
	Glib::ustring txt=m_QueryEntry->get_text();
	
	// set the label text to inform that user
	m_ResultsLabel->set_markup("<i>Searching...</i>");
	
	// disable go button
	m_GoButton->set_sensitive(false);
	
	// clear the current list
	m_ResultsList->clear_items();
	
	// count our results
	int c=0;
	
	// go through each block, and see if the text is found
	for (BufferMap::iterator it=m_Buffers.begin(); it!=m_Buffers.end(); ++it) {
		std::string block=(*it).second->get_text();
		
		// calculate begin and end positions
		int pos;
		int begin=pos, end=pos;
		while ((pos=block.find(txt))!=-1) {
			c++;
			
			// get the offset for beginning of this line
			Gtk::TextBuffer::iterator n=(*it).second->get_iter_at_offset(pos);
			if (n.backward_line()) {
				begin=n.get_offset();
				n.forward_line();
			}
			
			// and for the end of the line
			if (n.forward_line()) {
				end=n.get_offset()-begin;
				n.backward_line();
			}
			
			if (begin==-1)
				begin=pos;
			if (end==-1)
				end=block.size();
			
			// extract a bit of this string
			Glib::ustring cx=block.substr(begin, end);
			cx="... "+cx+" ...";
			
			// remove the trailing identifier from block id
			Glib::ustring id=(*it).first;
			id.erase(id.rfind("_"), id.size());
			
			// add it to the list
			int row=m_ResultsList->append_text(id);
			m_ResultsList->set_text(row, 1, cx);
			
			block.erase(begin, end);
		}
	}
	
	// update label
	std::stringstream ss;
	ss << "<i>Found " << c << " instance" << (c!=1 ? "s" : "") << " of your query.</i>";
	m_ResultsLabel->set_markup(ss.str());
}

// query text changed handler
void FindDialog::on_entry_text_changed() {
	bool b=!m_QueryEntry->get_text().empty();
	m_SearchButton->set_sensitive(b);
}

// list selection change handler
void FindDialog::on_selection_changed() {
	m_GoButton->set_sensitive(!m_ResultsList->get_selected().empty() && m_ResultsList->size()>0);
}
