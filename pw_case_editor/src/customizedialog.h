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

#include "alphaimage.h"
#include "casecombobox.h"

/** Dialog used to customize internal graphics.
  * Users can choose to customize certain parts of the player that are
  * otherwise hardcoded. This dialog provides an easy way for the user to
  * change the defaults
*/
class CustomizeDialog: public Gtk::Dialog {
	public:
		/** Constructor
		  * \param overrides Case::Overrides struct to set default values in dialog
		  * \param images Map of images
		*/
		CustomizeDialog(const Case::Overrides &overrides, const ImageMap &images);
		
		/** Get the modified overrides data
		  * \return Updated Case::Overrides struct
		*/
		Case::Overrides get_data();
		
	protected:
		/** Build the dialog's UI
		  * \param images Map of images
		*/
		void construct(const ImageMap &images);
		
		// text box custom image check button clicks
		void on_text_box_custom_image_toggled();
		
		// text box customize check button clicks
		void on_text_box_customize_toggled();
		
		// text box image combo box changes
		void on_text_box_image_changed();
		
		// text box alpha value changed
		void on_text_box_alpha_changed();
		
		// title screen customize toggled
		void on_title_screen_customize_toggled();
		
		// title screen image changed
		void on_title_screen_image_changed();
		
		// build the text box page
		Gtk::Widget* build_text_box_page();
		
		// build the title screen page
		Gtk::Widget* build_title_screen_page();
		
		// main notebook
		Gtk::Notebook *m_NB;
		
		// labels
		Gtk::Label *m_TextBoxAlphaLabel;
		Gtk::Label *m_TitleScreenLabel;
		
		// images
		Gtk::Image *m_TitleScreenPreview;
		AlphaImage *m_TextBoxPreview;
		
		// combo box boxes
		ImgComboBox *m_TextBoxImgCB;
		ImgComboBox *m_TitleScreenImgCB;
		
		// spin buttons
		Gtk::SpinButton *m_TextBoxAlphaSB;
		
		// check buttons
		Gtk::CheckButton *m_TextBoxCB;
		Gtk::CheckButton *m_TitleScreenCB;
		Gtk::CheckButton *m_CustomImageCB;
};

#endif
