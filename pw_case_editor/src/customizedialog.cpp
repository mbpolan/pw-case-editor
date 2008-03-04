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
// customizedialog.cpp: implementation of CustomizeDialog

#include <gtkmm/messagedialog.h>
#include <gtkmm/table.h>

#include "customizedialog.h"
#include "intl.h"

// constructor
CustomizeDialog::CustomizeDialog(const Case::Overrides &overrides, const ImageMap &images) {
	set_title(_("Customize"));
	construct(images);
	
	// set the data based on overrides data
	if (overrides.textboxAlpha>=0) {
		m_TextBoxCB->set_active(true);
		m_TextBoxAlphaSB->set_value(overrides.textboxAlpha);
		
		on_text_box_customize_toggled();
		on_text_box_alpha_changed();
	}
	
	if (overrides.titleScreen!="null") {
		m_TitleScreenCB->set_active(true);
		m_TitleScreenImgCB->set_active_internal(overrides.titleScreen);
		
		on_title_screen_customize_toggled();
		on_title_screen_image_changed();
	}
}

// get the modified data
Case::Overrides CustomizeDialog::get_data() {
	Case::Overrides ov;
	
	if (m_TextBoxCB->get_active())
		ov.textboxAlpha=(int) m_TextBoxAlphaSB->get_value();
	else
		ov.textboxAlpha=-1;
	
	if (m_TitleScreenCB->get_active())
		ov.titleScreen=m_TitleScreenImgCB->get_selected_internal();
	else
		ov.titleScreen="null";
	
	return ov;
}

// build the dialog
void CustomizeDialog::construct(const ImageMap &images) {
	// allocate notebook
	m_NB=Gtk::manage(new Gtk::Notebook);
	
	// allocate any map dependent combo boxes now
	m_TextBoxImgCB=Gtk::manage(new ImgComboBox(images));
	m_TitleScreenImgCB=Gtk::manage(new ImgComboBox(images));
	
	// create and append pages
	m_NB->append_page(*build_text_box_page(), _("Text Box"));
	m_NB->append_page(*build_title_screen_page(), _("Title Screen"));
	
	// connect signals for combo boxes
	m_TextBoxImgCB->signal_changed().connect(sigc::mem_fun(*this, &CustomizeDialog::on_text_box_image_changed));
	m_TitleScreenImgCB->signal_changed().connect(sigc::mem_fun(*this, &CustomizeDialog::on_title_screen_image_changed));
	
	get_vbox()->set_border_width(10);
	get_vbox()->pack_start(*m_NB);
	
	// set defaults
	on_text_box_custom_image_toggled();
	on_text_box_customize_toggled();
	on_title_screen_customize_toggled();
	on_title_screen_image_changed();
	
	// add buttons
	add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

// build the text box page
Gtk::Widget* CustomizeDialog::build_text_box_page() {
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate check button
	m_TextBoxCB=Gtk::manage(new Gtk::CheckButton(_("Customize")));
	m_TextBoxCB->set_active(false);
	m_TextBoxCB->signal_toggled().connect(sigc::mem_fun(*this, &CustomizeDialog::on_text_box_customize_toggled));
	
	m_CustomImageCB=Gtk::manage(new Gtk::CheckButton(_("Text Box Image")));
	m_CustomImageCB->set_sensitive(false); // FIXME: fix this once this feature works
	m_CustomImageCB->set_active(false);
	m_CustomImageCB->signal_toggled().connect(sigc::mem_fun(*this, &CustomizeDialog::on_text_box_custom_image_toggled));
	
	// allocate image
	m_TextBoxPreview=Gtk::manage(new AlphaImage("textbox.png"));
	m_TextBoxPreview->set_size_request(256, 64);
	
	// allocate spin button
	Gtk::Adjustment *adj=Gtk::manage(new Gtk::Adjustment(255, 0, 255));
	m_TextBoxAlphaSB=Gtk::manage(new Gtk::SpinButton(*adj, 1));
	m_TextBoxAlphaSB->signal_value_changed().connect(sigc::mem_fun(*this, &CustomizeDialog::on_text_box_alpha_changed));
	
	// allocate labels
	m_TextBoxAlphaLabel=Gtk::manage(new Gtk::Label(_("Transparency")));
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_TextBoxCB, 0, 2, 0, 1, xops, yops);
	table->attach(*m_TextBoxPreview, 0, 2, 1, 2, xops, yops);
	table->attach(*m_CustomImageCB, 0, 1, 2, 3, xops, yops);
	table->attach(*m_TextBoxImgCB, 1, 2, 2, 3, xops, yops);
	table->attach(*m_TextBoxAlphaLabel, 0, 1, 3, 4, xops, yops);
	table->attach(*m_TextBoxAlphaSB, 1, 2, 3, 4, xops, yops);
	
	return table;
}

// build the title screen page
Gtk::Widget* CustomizeDialog::build_title_screen_page() {
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate check buttons
	m_TitleScreenCB=Gtk::manage(new Gtk::CheckButton(_("Customize")));
	m_TitleScreenCB->set_active(false);
	m_TitleScreenCB->signal_toggled().connect(sigc::mem_fun(*this, &CustomizeDialog::on_title_screen_customize_toggled));
	
	// allocate labels
	m_TitleScreenLabel=Gtk::manage(new Gtk::Label(_("Custom Image")));
	
	// allocate images
	m_TitleScreenPreview=Gtk::manage(new Gtk::Image);
	m_TitleScreenPreview->set_size_request(256, 192);
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_TitleScreenCB, 0, 2, 0, 1, xops, yops);
	table->attach(*m_TitleScreenPreview, 0, 2, 1, 2, xops, yops);
	table->attach(*m_TitleScreenLabel, 0, 1, 2, 3, xops, yops);
	table->attach(*m_TitleScreenImgCB, 1, 2, 2, 3, xops, yops);
	
	return table;
}

// text box custom image check button clicks
void CustomizeDialog::on_text_box_custom_image_toggled() {
	m_TextBoxImgCB->set_sensitive(m_CustomImageCB->get_active());
}

// text box customize check button clicks
void CustomizeDialog::on_text_box_customize_toggled() {
	bool b=m_TextBoxCB->get_active();
	
	// toggle widgets
	m_TextBoxPreview->set_sensitive(b);
	/*m_CustomImageCB->set_sensitive(b);
	m_TextBoxImgCB->set_sensitive(b);*/
	m_TextBoxAlphaLabel->set_sensitive(b);
	m_TextBoxAlphaSB->set_sensitive(b);
	
}

// text box image combo box changes
void CustomizeDialog::on_text_box_image_changed() {
	// update the image
	//m_TextBoxPreview->set(m_TextBoxImgCB->get_selected_image()->pixbuf);
}

// text box alpha value changed
void CustomizeDialog::on_text_box_alpha_changed() {
	// make sure to clamp the value to [0,1]
	m_TextBoxPreview->set_alpha((m_TextBoxAlphaSB->get_value()/2.55)/100.0);
}

// title screen customize toggled
void CustomizeDialog::on_title_screen_customize_toggled() {
	bool b=m_TitleScreenCB->get_active();
	if (b) {
		// check to make sure we have any images
		if (m_TitleScreenImgCB->empty()) {
			Gtk::MessageDialog md(*this, _("You need at least 1 image to enable this customization."));
			md.run();
			
			m_TitleScreenCB->set_active(false);
			
			return;
		}
	}
	
	m_TitleScreenPreview->set_sensitive(b);
	m_TitleScreenLabel->set_sensitive(b);
	m_TitleScreenImgCB->set_sensitive(b);
}

// title screen image changed
void CustomizeDialog::on_title_screen_image_changed() {
	if (m_TitleScreenImgCB->empty())
		return;
	
	// get our selected image
	m_TitleScreenPreview->set(m_TitleScreenImgCB->get_selected_image()->pixbuf);
}
