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
// auxdialogs.cpp: implementations of classes in auxdialogs.h

#include "auxdialogs.h"
#include "intl.h"

// constructor
ExWarningDialog::ExWarningDialog():
	Gtk::MessageDialog(_("Your case seems to have no characters or evidence.\n"
			     "Would you still like to export your case?"),
		           false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_YES_NO, true) {
	construct();
}

// run the dialog
ExWarningDialog::Data ExWarningDialog::run() {
	Data data;
	data.returnCode=Gtk::MessageDialog::run();
	data.keepWarning=!m_KeepCB->get_active();
	
	return data;
}

// build the interface
void ExWarningDialog::construct() {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate check button
	m_KeepCB=manage(new Gtk::CheckButton(_("Don't show me this message anymore.")));
	m_KeepCB->set_active(true);
	
	vb->pack_start(*m_KeepCB, Gtk::PACK_SHRINK);
	
	vb->show_all_children();
}
