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
// testimonyeditor.h: the TestimonyEditor class

#ifndef TESTIMONYEDITOR_H
#define TESTIMONYEDITOR_H

#include <gtkmm/checkbutton.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>

#include "case.h"

// class that handles editing of testimonies
class TestimonyEditor: public Gtk::Dialog {
	public:
		// constructor
		TestimonyEditor(const StringVector &testimonyIds);
		
		// set a testimony to use
		void set_testimony(const Case::Testimony &testimony);
		
		// return the current testimony data
		Case::Testimony get_testimony_data();
		
	private:
		// build the ui
		void construct();
		
		// create a default testimony piece
		Case::TestimonyPiece create_testimony_piece();
		
		// update the dialog state
		void update();
		
		// previous piece button click handler
		void on_prev_button_clicked();
		
		// next piece button click handler
		void on_next_button_clicked();
		
		// append piece button click handler
		void on_append_button_clicked();
		
		// insert piece button click handler
		void on_insert_button_clicked();
		
		// delete piece button click handler
		void on_delete_button_clicked();
		
		// amend button click handler
		void on_amend_button_clicked();
		
		// id entry change handler
		void on_id_entry_changed();
		
		// handler for populate signal from text view
		void on_text_view_populate_menu(Gtk::Menu *menu);
		
		// handler for right clicks on list
		void on_list_button_pressed();
		
		// buttons
		Gtk::Button *m_PrevButton;
		Gtk::Button *m_NextButton;
		Gtk::Button *m_AppendButton;
		Gtk::Button *m_InsertButton;
		Gtk::Button *m_DeleteButton;
		Gtk::Button *m_AmendButton;
		Gtk::Button *m_OKButton;
		
		// labels
		Gtk::Label *m_IdLabel;
		Gtk::Label *m_SpeakerLabel;
		Gtk::Label *m_PieceLabel;
		Gtk::Label *m_PresentLabel;
		Gtk::Label *m_PressLabel;
		
		// text entries
		Gtk::Entry *m_IdEntry;
		Gtk::Entry *m_SpeakerEntry;
		Gtk::Entry *m_PresentIdEntry;
		Gtk::Entry *m_PresentTargetEntry;
		Gtk::Entry *m_PressEntry;
		
		// check buttons
		Gtk::CheckButton *m_HiddenCB;
		
		// text view
		Gtk::TextView *m_TextView;
		
		// containers
		Gtk::ScrolledWindow *m_SWindow;
		Gtk::Frame *m_PieceFrame;
		
		// current testimony
		Case::Testimony m_Testimony;
		
		// current testimony piece
		int m_CurPiece;
		
		// record of testimony ids
		StringVector m_Ids;
};

#endif
