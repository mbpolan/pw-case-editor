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
// textboxdialog.h: the TextBoxDialog class

#ifndef TEXTBOXDIALOG_H
#define TEXTBOXDIALOG_H

#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>

#include "textboxeditor.h"

// dialog used for formatting dialogue text
class TextBoxDialog: public Gtk::Dialog {
	public:
		// constructor
		TextBoxDialog();
		
		// get the formatted text
		Glib::ustring get_text() const;
		
	private:
		// build the ui
		void construct();
		
		// entry change handler
		void on_entry_changed(int line);
		
		// labels
		Gtk::Label *m_IntroLabel;
		Gtk::Label *m_Line1Label;
		Gtk::Label *m_Line2Label;
		Gtk::Label *m_Line3Label;
		
		// entries
		Gtk::Entry *m_Line1Entry;
		Gtk::Entry *m_Line2Entry;
		Gtk::Entry *m_Line3Entry;
		
		// textbox editor widget for preview
		TextBoxEditor *m_TextBox;
};

#endif
