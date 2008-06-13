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
// auxdialogs.h: Auxillary dialogs

#ifndef AUXDIALOGS_H
#define AUXDIALOGS_H

#include <gtkmm/checkbutton.h>
#include <gtkmm/messagedialog.h>

/** Dialog used to display a warning before exporting a case, if
  * the user has no characters or evidence added.
*/
class ExWarningDialog: public Gtk::MessageDialog {
	public:
		/// Data returned upon dialog closure
		struct _Data {
			int returnCode;
			bool keepWarning;
		};
		typedef struct _Data Data;
		
		/// Constructor
		ExWarningDialog();
		
		/** Run the dialog
		  * \return Data struct with responses
		*/
		Data run();
		
	private:
		/// Build the interface
		void construct();
		
		// check buttons
		Gtk::CheckButton *m_KeepCB;
};

#endif
