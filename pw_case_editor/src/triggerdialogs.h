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
// triggerdialogs.h: various dialogs for inserting triggers into script

#ifndef TRIGGERDIALOGS_H
#define TRIGGERDIALOGS_H

#include <gtkmm/comboboxtext.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/radiobutton.h>

#include "case.h"

// abstract dialog for inserting triggers
class InsertAbstractDialog: public Gtk::Dialog {
	public:
		// constructor
		InsertAbstractDialog(const Glib::ustring &trigger);
		
	protected:
		// labels
		Gtk::Label *m_TriggerLabel;
};

/***************************************************************************/

// goto (and its relatives) trigger
class InsertGotoDialog: public InsertAbstractDialog {
	public:
		// type
		enum Type { GOTO_NORMAL, GOTO_DIRECT, GOTO_TIMED };
		
		// data related to a goto trigger
		struct _Data {
			Glib::ustring target;
			int time;
			Type type;
		};
		typedef struct _Data Data;
		
		
		// constructor
		InsertGotoDialog(BufferMap blocks);
		
		// get the goto data
		Data get_data();
		
	protected:
		// build the dialog
		void construct();
		
		// radio button toggle handler
		void on_radio_button_toggled(const Glib::ustring &button);
		
		// radio buttons
		Gtk::RadioButtonGroup m_RadioGroup;
		Gtk::RadioButton *m_NormalRB;
		Gtk::RadioButton *m_DirectRB;
		Gtk::RadioButton *m_TimedRB;
		
		// labels
		Gtk::Label *m_TypeLabel;
		Gtk::Label *m_TargetLabel;
		Gtk::Label *m_TimeLabel;
		
		// entries
		Gtk::Entry *m_TimeEntry;
		
		// combo boxes
		Gtk::ComboBoxText *m_BlockCB;
};

#endif
