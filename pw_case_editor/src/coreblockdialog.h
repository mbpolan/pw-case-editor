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
// coreblockdialog.h: the CoreBlockDialog class

#ifndef COREBLOCKDIALOG_H
#define COREBLOCKDIALOG_H

#include <gtkmm/dialog.h>
#include <gtkmm/label.h>
#include <gtkmm/notebook.h>
#include <gtkmm/textview.h>

/** Dialog used to editor the core text blocks
*/
class CoreBlockDialog: public Gtk::Dialog {
	public:
		/// Data specific to this dialog
		struct _Data {
			Glib::ustring xExamineBadEv;
			Glib::ustring xExamineFail;
		};
		typedef struct _Data Data;
		
		/** Constructor
		  * \param data Block data to use
		*/
		CoreBlockDialog(const Data &data);
		
		/** Get the updated dialog data
		  * \return Updated core block data
		*/
		Data get_updated_data();
		
	private:
		/// Build the UI
		void construct();
		
		// labels
		Gtk::Label *m_BadEvidenceLabel;
		Gtk::Label *m_XExamineFailLabel;
		
		// text views
		Gtk::TextView *m_BadEvidenceView;
		Gtk::TextView *m_XExamineFailView;
		
		// containing notebook
		Gtk::Notebook *m_NB;
};

#endif
