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
// customizedialog.h: the CustomizeDialog class

#ifndef CUSTOMIZEDIALOG_H
#define CUSTOMIZEDIALOG_H

#include <gtkmm/checkbutton.h>
#include <gtkmm/dialog.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/notebook.h>
#include <gtkmm/spinbutton.h>

#include "casecombobox.h"

// dialog used to customize internal graphics
class CustomizeDialog: public Gtk::Dialog {
	public:
		// constructor
		CustomizeDialog(const ImageMap &images);
		
	protected:
		// build the dialog
		void construct(const ImageMap &images);
		
		// text box customize check button clicks
		void on_text_box_customize_toggled();
		
		// text box image combo box changes
		void on_text_box_image_changed();
		
		// text box alpha value changed
		void on_text_box_alpha_changed();
		
		// build the text box page
		Gtk::Widget* build_text_box_page();
		
		// main notebook
		Gtk::Notebook *m_NB;
		
		// labels
		Gtk::Label *m_TextBoxImgLabel;
		Gtk::Label *m_TextBoxAlphaLabel;
		
		// images
		Gtk::Image *m_TextBoxPreview;
		
		// combo box boxes
		ImgComboBox *m_TextBoxImgCB;
		
		// spin buttons
		Gtk::SpinButton *m_TextBoxAlphaSB;
		
		// check buttons
		Gtk::CheckButton *m_TextBoxCB;
};

#endif
