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

#include <gtkmm/table.h>

#include "customizedialog.h"

// constructor
CustomizeDialog::CustomizeDialog(const ImageMap &images) {
	construct(images);
}

// build the dialog
void CustomizeDialog::construct(const ImageMap &images) {
	// allocate notebook
	m_NB=manage(new Gtk::Notebook);
	
	// allocate any map dependent combo boxes now
	m_TextBoxImgCB=manage(new ImgComboBox(images));
	
	// create and append pages
	m_NB->append_page(*build_text_box_page(), "Text Box");
	
	// connect signals for combo boxes
	m_TextBoxImgCB->signal_changed().connect(sigc::mem_fun(*this, &CustomizeDialog::on_text_box_image_changed));
	
	get_vbox()->set_border_width(10);
	get_vbox()->pack_start(*m_NB);
	
	// set defaults
	on_text_box_customize_toggled();
	
	// add buttons
	add_button("OK", Gtk::RESPONSE_OK);
	add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

// build the text box page
Gtk::Widget* CustomizeDialog::build_text_box_page() {
	// allocate table
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate check button
	m_TextBoxCB=manage(new Gtk::CheckButton("Customize"));
	m_TextBoxCB->set_active(false);
	m_TextBoxCB->signal_toggled().connect(sigc::mem_fun(*this, &CustomizeDialog::on_text_box_customize_toggled));
	
	// allocate image
	m_TextBoxPreview=manage(new Gtk::Image);
	m_TextBoxPreview->set_size_request(256, 64);
	
	// allocate spin button
	Gtk::Adjustment *adj=manage(new Gtk::Adjustment(50, 0, 255));
	m_TextBoxAlphaSB=manage(new Gtk::SpinButton(*adj, 1));
	m_TextBoxAlphaSB->signal_value_changed().connect(sigc::mem_fun(*this, &CustomizeDialog::on_text_box_alpha_changed));
	
	// allocate labels
	m_TextBoxImgLabel=manage(new Gtk::Label("Text Box Image"));
	m_TextBoxAlphaLabel=manage(new Gtk::Label("Transparency"));
	
	// place widgets
	table->attach(*m_TextBoxCB, 0, 2, 0, 1);
	table->attach(*m_TextBoxPreview, 0, 2, 1, 2);
	table->attach(*m_TextBoxImgLabel, 0, 1, 2, 3);
	table->attach(*m_TextBoxImgCB, 1, 2, 2, 3);
	table->attach(*m_TextBoxAlphaLabel, 0, 1, 3, 4);
	table->attach(*m_TextBoxAlphaSB, 1, 2, 3, 4);
	
	return table;
}

// text box customize check button clicks
void CustomizeDialog::on_text_box_customize_toggled() {
	bool b=m_TextBoxCB->get_active();
	
	// toggle widgets
	m_TextBoxPreview->set_sensitive(b);
	m_TextBoxImgLabel->set_sensitive(b);
	m_TextBoxImgCB->set_sensitive(b);
	m_TextBoxAlphaLabel->set_sensitive(b);
	m_TextBoxAlphaSB->set_sensitive(b);
	
}

// text box image combo box changes
void CustomizeDialog::on_text_box_image_changed() {
	// update the image
	m_TextBoxPreview->set(m_TextBoxImgCB->get_selected_image()->pixbuf);
}

// text box alpha value changed
void CustomizeDialog::on_text_box_alpha_changed() {
	// TODO
}
