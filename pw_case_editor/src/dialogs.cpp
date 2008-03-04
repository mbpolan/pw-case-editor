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
// dialogs.cpp: implementation of Dialog classes

#include <iomanip>
#include <glibmm/fileutils.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/frame.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/separator.h>
#include <gtkmm/table.h>
#include <sstream>

#include "dialogs.h"
#include "intl.h"
#include "mainwindow.h"
#include "testimonyeditor.h"

// constructor
LangDialog::LangDialog() {
	set_title(_("Set Language"));
	construct();
}

// get the chosen language
Glib::ustring LangDialog::get_selected() {
	return m_Langs[m_List->get_text(m_List->get_selected()[0], 0)];
}

// build the ui
void LangDialog::construct() {
	Gtk::VBox *vb=get_vbox();
	
	// allocate scrolled window
	m_SWindow=manage(new Gtk::ScrolledWindow);
	m_SWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_SWindow->set_size_request(50, 100);
	
	// allocate the list
	m_List=manage(new Gtk::ListViewText(1));
	m_List->set_column_title(0, _("Language"));
	m_SWindow->add(*m_List);
	
	// add default languages
	m_List->append_text(_("English"));
	m_List->append_text(_("Polish"));
	
	m_Langs[_("English")]="en";
	m_Langs[_("Polish")]="pl";
	
	// allocate labels
	m_LangLabel=manage(new Gtk::Label(_("Available Languages")));
	
	// pack the widgets
	vb->set_spacing(5);
	vb->set_border_width(10);
	vb->pack_start(*m_LangLabel, Gtk::PACK_SHRINK);
	vb->pack_start(*m_SWindow);
	
	add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

/***************************************************************************/

// constructor
ChangeSpeedDialog::ChangeSpeedDialog() {
	set_title(_("Change Text Speed"));
	construct();
}

// get the text speed
Glib::ustring ChangeSpeedDialog::get_text_speed() {
	Glib::ustring str;
	if (m_Scale->get_value()<0)
		str="-";
	else
		str="+";
	
	std::stringstream ss;
	ss << (int) abs(m_Scale->get_value());
	str+=ss.str();
	
	return str;
}

// build the ui
void ChangeSpeedDialog::construct() {
	// allocate table
	Gtk::Table *table=manage(new Gtk::Table);
	
	// allocate labels
	m_SlowerLabel=manage(new Gtk::Label(_("Slower")));
	m_FasterLabel=manage(new Gtk::Label(_("Faster")));
	
	// allocate slider
	Gtk::Adjustment *adj=manage(new Gtk::Adjustment(0, -9, 9, 1, 1, 1));
	m_Scale=manage(new Gtk::HScale(*adj));
	
	// place widgets
	table->attach(*m_SlowerLabel, 0, 1, 0, 1);
	table->attach(*m_FasterLabel, 1, 2, 0, 1);
	table->attach(*m_Scale, 0, 2, 1, 2);
	
	get_vbox()->pack_start(*table, Gtk::PACK_SHRINK);
	
	m_OKButton=add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	m_OKButton->set_sensitive(false);
	
	// connect signals
	m_Scale->signal_value_changed().connect(sigc::mem_fun(*this, &ChangeSpeedDialog::on_value_changed));
	
	show_all_children();
}

// handler for value changes in slider
void ChangeSpeedDialog::on_value_changed() {
	m_OKButton->set_sensitive(m_Scale->get_value()!=0);
}

/***************************************************************************/

// constructor
ChangeColorDialog::ChangeColorDialog() {
	set_title(_("Change Text Color"));
	construct();
}

// get the selected color
Glib::ustring ChangeColorDialog::get_color() {
	if (m_BlueButton->get_active())
		return "b";
	else if (m_GreenButton->get_active())
		return "g";
	else if (m_OrangeButton->get_active())
		return "o";
	else if (m_WhiteButton->get_active())
		return "w";
}

// build the ui
void ChangeColorDialog::construct() {
	// allocate layout vbox
	Gtk::VBox *vb=manage(new Gtk::VBox);
	vb->set_spacing(5);
	
	// allocate buttons
	m_BlueButton=manage(new Gtk::RadioButton(m_Group));
	m_GreenButton=manage(new Gtk::RadioButton(m_Group));
	m_OrangeButton=manage(new Gtk::RadioButton(m_Group));
	m_WhiteButton=manage(new Gtk::RadioButton(m_Group));
	
	// place the color widgets
	m_BlueButton->add(*manage(new ColorWidget(107, 198, 247)));
	m_GreenButton->add(*manage(new ColorWidget(0, 247, 0)));
	m_OrangeButton->add(*manage(new ColorWidget("orange")));
	m_WhiteButton->add(*manage(new ColorWidget(255, 255, 255)));
	
	// set white by default
	m_WhiteButton->set_active(true);
	
	// place the buttons
	vb->pack_start(*m_BlueButton, Gtk::PACK_SHRINK);
	vb->pack_start(*m_GreenButton, Gtk::PACK_SHRINK);
	vb->pack_start(*m_OrangeButton, Gtk::PACK_SHRINK);
	vb->pack_start(*m_WhiteButton, Gtk::PACK_SHRINK);
	
	get_vbox()->pack_start(*vb, Gtk::PACK_SHRINK);
	
	add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

/***************************************************************************/

// constructor
ProgressDialog::ProgressDialog(const Glib::ustring &label) {
	set_title(_("Progress"));
	construct();
	
	m_Label->set_text(label);
}

// set the amount of progress done
void ProgressDialog::set_progress(double amount) {
	// format percentage text
	std::stringstream ss;
	ss << std::setiosflags(std::ios::fixed) << std::setprecision(0) << (amount*100);
	
	// set fraction and text
	m_ProgBar->set_fraction(amount);
	m_ProgBar->set_text(ss.str()+"%");
}

// build the ui
void ProgressDialog::construct() {
	Gtk::VBox *vb=get_vbox();
	vb->set_spacing(5);
	vb->set_border_width(10);
	
	// allocate label
	m_Label=Gtk::manage(new Gtk::Label);
	
	// allocate progress bar
	m_ProgBar=Gtk::manage(new Gtk::ProgressBar);
	
	// pack widgets
	vb->pack_start(*m_Label, Gtk::PACK_SHRINK);
	vb->pack_start(*m_ProgBar, Gtk::PACK_SHRINK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
TestimonyManager::TestimonyManager(const TestimonyMap &tmap, const StringVector &testimonyIds) {
	set_title(_("Manage Testimonies"));
	construct();
	
	// store record of testimonies
	m_Testimonies=tmap;
	m_Ids=testimonyIds;
	
	// fill in list view
	for (TestimonyMap::const_iterator it=tmap.begin(); it!=tmap.end(); ++it) {
		int row=m_ListView->append_text((*it).first);
		m_ListView->set_text(row, 1, (*it).second.title);
	}
}

// build the ui
void TestimonyManager::construct() {
	// get vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate layout table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_TitleLabel=Gtk::manage(new Gtk::Label(_("Testimonies in this Case")));
	m_PreviewLabel=Gtk::manage(new Gtk::Label);
	m_PreviewLabel->set_markup("<b>"+_("Preview")+"</b>");
	
	// allocate buttons
	m_AddButton=Gtk::manage(new Gtk::Button(_("Add")));
	m_EditButton=Gtk::manage(new Gtk::Button(_("[verb]Edit")));
	m_DeleteButton=Gtk::manage(new Gtk::Button(_("Delete")));
	
	// connect signals
	m_AddButton->signal_clicked().connect(sigc::mem_fun(*this, &TestimonyManager::on_add_button_clicked));
	m_EditButton->signal_clicked().connect(sigc::mem_fun(*this, &TestimonyManager::on_edit_button_clicked));
	m_DeleteButton->signal_clicked().connect(sigc::mem_fun(*this, &TestimonyManager::on_delete_button_clicked));
	
	// allocate scrolled window
	m_SWindow=Gtk::manage(new Gtk::ScrolledWindow);
	m_SWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
	m_SWindow->set_size_request(200, 200);
	
	// allocate listview
	m_ListView=Gtk::manage(new Gtk::ListViewText(2));
	m_SWindow->add(*m_ListView);
	
	// connect signals
	m_ListView->get_selection()->signal_changed().connect(sigc::mem_fun(*this, &TestimonyManager::on_selection_changed));
	
	// set titles for columns
	m_ListView->set_column_title(0, _("ID"));
	m_ListView->set_column_title(1, _("Title"));
	
	// allocate button box
	Gtk::VButtonBox *buttons=Gtk::manage(new Gtk::VButtonBox(Gtk::BUTTONBOX_SPREAD));
	
	// pack widgets
	buttons->pack_start(*m_AddButton);
	buttons->pack_start(*m_EditButton);
	buttons->pack_start(*m_DeleteButton);
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_TitleLabel, 0, 2, 0, 1, xops, yops);
	table->attach(*m_SWindow, 0, 1, 1, 2, xops);
	table->attach(*buttons, 1, 2, 1, 2, xops, yops);
	table->attach(*m_PreviewLabel, 0, 2, 2, 3, xops, yops);
	
	vb->pack_start(*table);
	
	// add buttons
	add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

// add button click handler
void TestimonyManager::on_add_button_clicked() {
	// run testimony editor
	TestimonyEditor te(g_MainWnd->get_case()->get_characters(), g_MainWnd->get_case()->get_locations(),
			   g_MainWnd->get_case_buffers(), m_Ids);
	if (te.run()==Gtk::RESPONSE_OK) {
		// get the testimony
		Case::Testimony testimony=te.get_testimony_data();
		
		// add it to the map
		m_Testimonies[testimony.id]=testimony;
		
		// also, append a row for this testimony
		int row=m_ListView->append_text(testimony.id);
		m_ListView->set_text(row, 1, testimony.title);
	}
}

// edit button click handler
void TestimonyManager::on_edit_button_clicked() {
	if (m_ListView->get_selected().empty()) {
		Gtk::MessageDialog md("You must select a testimony to edit first.", false, Gtk::MESSAGE_INFO);
		md.run();
		
		return;
	}
	
	// get the selected testimony id and speaker
	int row=m_ListView->get_selected()[0];
	Glib::ustring id=m_ListView->get_text(row, 0);
	Glib::ustring speaker=m_ListView->get_text(row, 1);
	
	// prepare testimony editor, and feed in this testimony
	TestimonyEditor te(g_MainWnd->get_case()->get_characters(), g_MainWnd->get_case()->get_locations(),
			   g_MainWnd->get_case_buffers(), m_Ids);
	te.set_testimony(m_Testimonies[id]);
	
	// run this dialog
	if (te.run()==Gtk::RESPONSE_OK) {
		// simply update this testimony
		m_Testimonies[id]=te.get_testimony_data();
		
		// update title of testimony in list
		m_ListView->set_text(row, 1, m_Testimonies[id].title);
	}
}

// delete button click handler
void TestimonyManager::on_delete_button_clicked() {
	// confirm deletion
	Gtk::MessageDialog md("Are you sure you want to delete this testimony?", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
	if (md.run()==Gtk::RESPONSE_YES && !m_ListView->get_selected().empty()) {
		// find the testimony in question
		Glib::ustring id=m_ListView->get_text(m_ListView->get_selected()[0], 0);
		
		// new vector to store testimonies in
		std::vector<Case::Testimony> newVec;
		
		// iterate over testimonies
		for (TestimonyMap::iterator it=m_Testimonies.begin(); it!=m_Testimonies.end(); ++it) {
			if ((*it).first!=id)
				newVec.push_back((*it).second);
		}
		m_Testimonies.erase(id);
		
		// with this new vector, clear the list view and repopulate it
		m_ListView->clear_items();
		for (int i=0; i<newVec.size(); i++) {
			int row=m_ListView->append_text(newVec[i].id);
			m_ListView->set_text(row, 1, newVec[i].title);
		}
	}
}

// selection change handler
void TestimonyManager::on_selection_changed() {
	if (m_ListView->get_selected().empty())
		return;
	
	// get the selected testimony id
	Glib::ustring id=m_ListView->get_text(m_ListView->get_selected()[0], 0);
	
	// iterate over testimonies
	for (TestimonyMap::iterator it=m_Testimonies.begin(); it!=m_Testimonies.end(); ++it) {
		if (id==(*it).first) {
			// update the preview label
			Glib::ustring str="<b>"+_("Preview")+"</b>\n";
			str+=(*it).second.pieces[0].text;
			
			m_PreviewLabel->set_markup(str);
			break;
		}
	}
}

/***************************************************************************/

// constructor
ImageDialog::ImageDialog(const ImageMap &imap, const StringVector &imgIds) {
	set_title(_("Image Assets"));
	construct();
	
	// copy ids and map
	m_ImageIds=imgIds;
	m_Images=imap;
	
	// append rows
	for (int i=0; i<m_ImageIds.size(); i++)
		m_ImageList->append_text(m_ImageIds[i]);
}

// build the ui
void ImageDialog::construct() {
	// get vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate layout table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_ImageLabel=Gtk::manage(new Gtk::Label(_("Image Assets")));
	m_PreviewLabel=Gtk::manage(new Gtk::Label(_("Preview")));
	
	// allocate buttons
	m_AddButton=Gtk::manage(new Gtk::Button(_("Add")));
	m_DeleteButton=Gtk::manage(new Gtk::Button(_("Delete")));
	
	// connect signals
	m_AddButton->signal_clicked().connect(sigc::mem_fun(*this, &ImageDialog::on_add_clicked));
	m_DeleteButton->signal_clicked().connect(sigc::mem_fun(*this, &ImageDialog::on_delete_clicked));
	
	// allocate list and its container
	m_SWindow=Gtk::manage(new Gtk::ScrolledWindow);
	m_SWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_SWindow->set_size_request(100, 150);
	
	m_ImageList=Gtk::manage(new Gtk::ListViewText(1));
	m_ImageList->set_column_title(0, _("Internal ID"));
	m_SWindow->add(*m_ImageList);
	
	// connect signals
	m_ImageList->get_selection()->signal_changed().connect(sigc::mem_fun(*this, &ImageDialog::on_selection_changed));
	
	// allocate image
	m_Image=Gtk::manage(new Gtk::Image);
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_ImageLabel, 0, 2, 0, 1, xops, yops);
	table->attach(*m_AddButton, 0, 1, 1, 2, xops, yops);
	table->attach(*m_DeleteButton, 1, 2, 1, 2, xops, yops);
	table->attach(*m_SWindow, 0, 2, 2, 3);
	table->attach(*m_PreviewLabel, 2, 3, 0, 1, xops, yops);
	table->attach(*m_Image, 2, 3, 1, 3, xops, yops);
	
	vb->pack_start(*table);
	
	// add buttons
	add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

// add an image
void ImageDialog::on_add_clicked() {
	// prepare file chooser dialog
	Gtk::FileChooserDialog fcd(*this, _("Open Image"), Gtk::FILE_CHOOSER_ACTION_OPEN);
	fcd.add_button(_("Open"), Gtk::RESPONSE_OK);
	fcd.add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	// run it
	if (fcd.run()==Gtk::RESPONSE_OK) {
		// get the path
		Glib::ustring path=fcd.get_filename();
		
		// create a pixbuf
		Glib::RefPtr<Gdk::Pixbuf> pixbuf=Gdk::Pixbuf::create_from_file(path);
		if (pixbuf) {
			// ask for id
			TextInputDialog td(_("Internal ID"));
			if (td.run()==Gtk::RESPONSE_OK) {
				Glib::ustring id=td.get_text();
				
				// append a new row
				m_ImageList->append_text(id);
				
				// and add it to the map
				Case::Image img;
				img.id=id;
				img.pixbuf=pixbuf;
				m_Images[id]=img;
			}
		}
	}
}

// remove an image
void ImageDialog::on_delete_clicked() {
	int selected=m_ImageList->get_selected()[0];
	StringVector vec;
	
	// iterate over rows, and append text to temp vector for unselected rows
	for (int i=0; i<m_ImageList->size(); i++) {
		if (i!=selected)
			vec.push_back(m_ImageList->get_text(i, 0));
	}
	
	// now clear all rows
	m_ImageList->clear_items();
	
	// append the rows that were previously unselected
	for (int i=0; i<vec.size(); i++)
		m_ImageList->append_text(vec[i]);
}

// row changes handler
void ImageDialog::on_selection_changed() {
	// get our selected row
	int selected=m_ImageList->get_selected()[0];
	
	// iterate over rows
	for (int i=0; i<m_ImageList->size(); i++) {
		if (i==selected) {
			// get the id
			Glib::ustring text=m_ImageList->get_text(i, 0);
			
			// grab the pixbuf from map and display it
			m_Image->set(m_Images[text].pixbuf);
		}
	}
}

/***************************************************************************/

// constructor
NewHotspotDialog::NewHotspotDialog() {
	set_title(_("Location Hotspot"));
	construct();
}

// return the hotspot
Case::Hotspot NewHotspotDialog::get_hotspot() {
	// get the hotspot from the hotspot widget
	Case::Hotspot hspot=m_HSWidget->get_hotspot();
	
	// fill in target id
	hspot.block=m_BlockEntry->get_text();
	
	return hspot;
}

// build the ui
void NewHotspotDialog::construct() {
	// get default vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate layout table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_XLabel=Gtk::manage(new Gtk::Label(_("X")));
	m_YLabel=Gtk::manage(new Gtk::Label(_("Y")));
	m_WLabel=Gtk::manage(new Gtk::Label(_("Width")));
	m_HLabel=Gtk::manage(new Gtk::Label(_("Height")));
	m_BlockLabel=Gtk::manage(new Gtk::Label(_("Target Block ID")));
	
	// allocate entries
	m_XEntry=Gtk::manage(new Gtk::Entry);
	m_YEntry=Gtk::manage(new Gtk::Entry);
	m_WEntry=Gtk::manage(new Gtk::Entry);
	m_HEntry=Gtk::manage(new Gtk::Entry);
	m_BlockEntry=Gtk::manage(new Gtk::Entry);
	
	// connect entry signals
	m_XEntry->signal_changed().connect(sigc::mem_fun(*this, &NewHotspotDialog::on_coord_entry_changed));
	m_YEntry->signal_changed().connect(sigc::mem_fun(*this, &NewHotspotDialog::on_coord_entry_changed));
	m_WEntry->signal_changed().connect(sigc::mem_fun(*this, &NewHotspotDialog::on_dimension_entry_changed));
	m_HEntry->signal_changed().connect(sigc::mem_fun(*this, &NewHotspotDialog::on_dimension_entry_changed));
	
	// allocate hotspot widget
	m_HSWidget=Gtk::manage(new HotspotWidget);
	
	// connect signals
	m_HSWidget->signal_point_changed().connect(sigc::mem_fun(*this, &NewHotspotDialog::on_point_changed));
	m_HSWidget->signal_dimensions_changed().connect(sigc::mem_fun(*this, &NewHotspotDialog::on_dimensions_changed));
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_HSWidget, 0, 2, 0, 1, xops, yops);
	table->attach(*m_XLabel, 0, 1, 1, 2, xops, yops);
	table->attach(*m_XEntry, 1, 2, 1, 2, xops, yops);
	table->attach(*m_YLabel, 0, 1, 2, 3, xops, yops);
	table->attach(*m_YEntry, 1, 2, 2, 3, xops, yops);
	table->attach(*m_WLabel, 0, 1, 3, 4, xops, yops);
	table->attach(*m_WEntry, 1, 2, 3, 4, xops, yops);
	table->attach(*m_HLabel, 0, 1, 4, 5, xops, yops);
	table->attach(*m_HEntry, 1, 2, 4, 5, xops, yops);
	table->attach(*m_BlockLabel, 0, 1, 5, 6, xops, yops);
	table->attach(*m_BlockEntry, 1, 2, 5, 6, xops, yops);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	// add buttons
	add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

// signal handler for point changes
void NewHotspotDialog::on_point_changed(int x, int y) {
	// update entries
	std::stringstream ss;
	ss << x;
	
	m_XEntry->set_text(ss.str());
	ss.str("");
	
	ss << y;
	m_YEntry->set_text(ss.str());
	ss.str("");
}

// signal handler for dimension changes
void NewHotspotDialog::on_dimensions_changed(int w, int h) {
	// update entries
	std::stringstream ss;
	ss << w;
	
	m_WEntry->set_text(ss.str());
	ss.str("");
	
	ss << h;
	m_HEntry->set_text(ss.str());
	ss.str("");
}

// handler for coordinate entry changes
void NewHotspotDialog::on_coord_entry_changed() {
	// convert text to ints
	int x=atoi(m_XEntry->get_text().c_str());
	int y=atoi(m_YEntry->get_text().c_str());
	
	m_HSWidget->update_coords(x, y);
}

// handler for dimension entry changes
void NewHotspotDialog::on_dimension_entry_changed() {
	// convert text to ints
	int w=atoi(m_WEntry->get_text().c_str());
	int h=atoi(m_HEntry->get_text().c_str());
	
	m_HSWidget->update_dimensions(w, h);
}

/***************************************************************************/

// constructor
LocationsDialog::LocationsDialog(const LocationMap &locations, const BackgroundMap &bgs, const StringVector &usedIds) {
	set_title(_("Locations"));
	set_size_request(500, 400);
	
	// allocate background combo box
	m_BGCB=manage(new BGComboBox(bgs));
	
	construct();
	
	// copy data
	m_Locations=locations;
	m_Backgrounds=bgs;
	m_UsedIds=usedIds;
	
	// append ids to tree view
	for (int i=0; i<m_UsedIds.size(); i++) {
		Gtk::TreeModel::Row row=*(m_Model->append());
		row[m_ColumnRec.m_Column]=m_UsedIds[i];
	}
}

// build the ui
void LocationsDialog::construct() {
	// get default vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate notebook
	m_NB=manage(new Gtk::Notebook);
	
	// allocate layout tables
	Gtk::Table *mainTable=manage(new Gtk::Table);
	mainTable->set_spacings(5);
	
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	Gtk::Table *hsTable=manage(new Gtk::Table);
	hsTable->set_spacings(5);
	
	Gtk::Table *stTable=manage(new Gtk::Table);
	stTable->set_spacings(5);
	
	// allocate labels
	m_LocationsLabel=Gtk::manage(new Gtk::Label(_("Locations")));
	m_DetailsLabel=Gtk::manage(new Gtk::Label(_("Details")));
	m_IdLabel=Gtk::manage(new Gtk::Label(_("Internal ID")));
	m_NameLabel=Gtk::manage(new Gtk::Label(_("Name")));
	m_BGLabel=Gtk::manage(new Gtk::Label(_("Background ID")));
	m_HotspotsLabel=Gtk::manage(new Gtk::Label(_("Hotspots")));
	
	// allocate entries
	m_IdEntry=Gtk::manage(new Gtk::Entry);
	m_NameEntry=Gtk::manage(new Gtk::Entry);
	m_IdEntry->set_sensitive(false);
	
	// allocate buttons
	m_AddButton=Gtk::manage(new Gtk::Button(_("Add")));
	m_DeleteButton=Gtk::manage(new Gtk::Button(_("Delete")));
	m_AddHSButton=Gtk::manage(new Gtk::Button(_("Add")));
	m_DeleteHSButton=Gtk::manage(new Gtk::Button(_("Delete")));
	m_AmendButton=Gtk::manage(new Gtk::Button(_("Amend")));
	m_AddStateButton=Gtk::manage(new Gtk::Button(_("Add")));
	m_DeleteStateButton=Gtk::manage(new Gtk::Button(_("Delete")));
	
	// connect signals
	m_AddButton->signal_clicked().connect(sigc::mem_fun(*this, &LocationsDialog::on_add));
	m_DeleteButton->signal_clicked().connect(sigc::mem_fun(*this, &LocationsDialog::on_delete));
	m_AddHSButton->signal_clicked().connect(sigc::mem_fun(*this, &LocationsDialog::on_add_hotspot));
	m_DeleteHSButton->signal_clicked().connect(sigc::mem_fun(*this, &LocationsDialog::on_delete_hotspot));
	m_AmendButton->signal_clicked().connect(sigc::mem_fun(*this, &LocationsDialog::on_amend_button_clicked));
	m_AddStateButton->signal_clicked().connect(sigc::mem_fun(*this, &LocationsDialog::on_add_state));
	m_DeleteStateButton->signal_clicked().connect(sigc::mem_fun(*this, &LocationsDialog::on_delete_state));
	
	// allocate list view
	m_HotspotList=Gtk::manage(new Gtk::ListViewText(2));
	m_HotspotList->set_column_title(0, _("Area"));
	m_HotspotList->set_column_title(1, _("Target Block"));
	
	// allocate tree view for location ids
	m_Model=Gtk::ListStore::create(m_ColumnRec);
	m_TreeView=Gtk::manage(new Gtk::TreeView(m_Model));
	
	// append a default column
	m_TreeView->append_column(_("Location ID"), m_ColumnRec.m_Column);
	
	// allocate list view for states
	m_StateList=manage(new Gtk::ListViewText(2));
	m_StateList->set_column_title(0, _("State"));
	m_StateList->set_column_title(1, _("Background ID"));
	
	// connect selection change signals
	Glib::RefPtr<Gtk::TreeView::Selection> selection=m_TreeView->get_selection();
	selection->signal_changed().connect(sigc::mem_fun(*this, &LocationsDialog::on_selection_changed));
	
	// allocate containing scrolled window
	m_SWindow=Gtk::manage(new Gtk::ScrolledWindow);
	m_SWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_SWindow->set_size_request(50, 80);
	
	m_HotspotSWindow=Gtk::manage(new Gtk::ScrolledWindow);
	m_HotspotSWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_HotspotSWindow->set_size_request(50, 80);
	
	// add widgets to scrolled windows
	m_SWindow->add(*m_TreeView);
	m_SWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_HotspotSWindow->add(*m_HotspotList);
	
	// allocate containing scroll window for states
	m_StateSWindow=manage(new Gtk::ScrolledWindow);
	m_StateSWindow->set_size_request(50, 80);
	m_StateSWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_StateSWindow->add(*m_StateList);
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// place widgets
	mainTable->attach(*m_LocationsLabel, 0, 2, 0, 1, xops, yops);
	mainTable->attach(*m_AddButton, 0, 1, 1, 2, xops, yops);
	mainTable->attach(*m_DeleteButton, 1, 2, 1, 2, xops, yops);
	mainTable->attach(*m_SWindow, 0, 2, 2, 8);
	mainTable->attach(*m_DetailsLabel, 2, 4, 0, 1, xops, yops);
	mainTable->attach(*m_NB, 2, 4, 1, 7);
	mainTable->attach(*m_AmendButton, 3, 4, 7, 8, xops, yops);
	
	table->attach(*m_IdLabel, 0, 1, 0, 1, xops, yops);
	table->attach(*m_IdEntry, 1, 2, 0, 1, xops, yops);
	table->attach(*m_NameLabel, 0, 1, 1, 2, xops, yops);
	table->attach(*m_NameEntry, 1, 2, 1, 2, xops, yops);
	table->attach(*m_BGLabel, 0, 1, 2, 3, xops, yops);
	table->attach(*m_BGCB, 1, 2, 2, 3, xops, yops);
	
	hsTable->attach(*m_AddHSButton, 0, 1, 0, 1, yops, yops);
	hsTable->attach(*m_DeleteHSButton, 1, 2, 0, 1, yops, yops);
	hsTable->attach(*m_HotspotSWindow, 0, 2, 1, 2);
	
	stTable->attach(*m_AddStateButton, 0, 1, 0, 1, yops, yops);
	stTable->attach(*m_DeleteStateButton, 1, 2, 0, 1, yops, yops);
	stTable->attach(*m_StateSWindow, 0, 2, 1, 2);
	
	m_NB->append_page(*table, _("General"));
	m_NB->append_page(*hsTable, _("Hotspots"));
	m_NB->append_page(*stTable, _("States"));
	
	vb->pack_start(*mainTable);
	
	// add buttons
	add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

// add a location
void LocationsDialog::on_add() {
	// ask for an id
	TextInputDialog td(_("Location ID"));
	if (td.run()==Gtk::RESPONSE_OK) {
		// check the id
		Glib::ustring id=td.get_text();
		
		// see if it's already been used
		for (int i=0; i<m_UsedIds.size(); i++) {
			if (id==m_UsedIds[i]) {
				Gtk::MessageDialog md(*this, _("You must choose a unique ID."), false, Gtk::MESSAGE_WARNING);
				md.run();
				return;
			}
		}
		
		// create a new location struct
		Case::Location location;
		location.id=id;
		
		// append a new row
		Gtk::TreeModel::Row row=*(m_Model->append());
		row[m_ColumnRec.m_Column]=id;
		
		// also add it to the map
		m_Locations[id]=location;
	}
}

// remove a location
void LocationsDialog::on_delete() {
	// get the selection
	Glib::RefPtr<Gtk::TreeView::Selection> selection=m_TreeView->get_selection();
	if (selection) {
		Gtk::TreeModel::iterator it=selection->get_selected();
		if (it) {
			m_Locations.erase((*it)[m_ColumnRec.m_Column]);
			m_Model->erase(it);
		}
	}
}

// add a hotspot
void LocationsDialog::on_add_hotspot() {
	// see if this location exists
	if (m_Locations.find(m_IdEntry->get_text())==m_Locations.end())
		return;
	
	// set current location's pixbuf
	Case::Location location=m_Locations[m_IdEntry->get_text()];
	if (m_Backgrounds.find(location.states["default"])==m_Backgrounds.end()) {
		Gtk::MessageDialog md(*this, _("The background was not found."), false, Gtk::MESSAGE_ERROR);
		md.run();
		return;
	}
	
	// prepare the appropriate dialog
	NewHotspotDialog nhd;
	
	// set the background
	nhd.set_pixbuf(m_Backgrounds[location.states["default"]].pixbuf);
	
	// run it
	if (nhd.run()==Gtk::RESPONSE_OK) {
		// get the hotspot
		Case::Hotspot hspot=nhd.get_hotspot();
		
		// create strings for list
		std::stringstream ss;
		ss << "(" << hspot.rect.x << "," << hspot.rect.y << "); " << hspot.rect.w << "x" << hspot.rect.h;
		
		// append a new row
		int row=m_HotspotList->append_text(ss.str());
		m_HotspotList->set_text(row, 1, hspot.block);
	}
}

// remove a hotspot
void LocationsDialog::on_delete_hotspot() {
	// get the selection
	int selected=m_HotspotList->get_selected()[0];
	
	// clear this row out
	m_HotspotList->set_text(selected, 0, "null");
	m_HotspotList->set_text(selected, 1, "null");
}

// add a location state
void LocationsDialog::on_add_state() {
	// run the dialog
	LocationStateDialog diag(m_Backgrounds);
	if (diag.run()==Gtk::RESPONSE_OK) {
		int row=m_StateList->append_text(diag.get_data().first);
		m_StateList->set_text(row, 1, diag.get_data().second);
	}
}

// remove a location state
void LocationsDialog::on_delete_state() {
	// get the selection
	int selected=m_StateList->get_selected()[0];
	
	// clear this row out
	m_StateList->set_text(selected, 0, "null");
	m_StateList->set_text(selected, 1, "null");
}

// amend button click handler
void LocationsDialog::on_amend_button_clicked() {
	// get data of the current location
	Glib::ustring id=m_IdEntry->get_text();
	Glib::ustring name=m_NameEntry->get_text();
	Glib::ustring bg=m_BGCB->get_selected_internal();
	
	// clear hotspots and add new ones in
	m_Locations[id].hotspots.clear();
	for (int i=0; i<m_HotspotList->size(); i++) {
		// get the string for this cell
		Glib::ustring area=m_HotspotList->get_text(i, 0);
		
		// verify this is a valid string
		if (area=="null")
			continue;
		
		Case::Hotspot hspot;
		
		// get the area from the string
		sscanf(area.c_str(), "(%d,%d); %dx%d", &hspot.rect.x, &hspot.rect.y, &hspot.rect.w, &hspot.rect.h);
		
		// get the target block
		hspot.block=m_HotspotList->get_text(i, 1);
		
		// add this hotspot
		m_Locations[id].hotspots.push_back(hspot);
	}
	
	// clear states and add new ones
	m_Locations[id].states.clear();
	for (int i=0; i<m_StateList->size(); i++) {
		Glib::ustring sID=m_StateList->get_text(i, 0);
		
		// ignore deleted rows
		if (sID=="null")
			continue;
		
		// add this state
		m_Locations[id].states[sID]=m_StateList->get_text(i, 1);
	}
	
	// update the location
	m_Locations[id].name=name;
	m_Locations[id].states["default"]=bg;
}

// row changes handler
void LocationsDialog::on_selection_changed() {
	// get the selection
	Glib::RefPtr<Gtk::TreeView::Selection> selection=m_TreeView->get_selection();
	if (selection) {
		Gtk::TreeModel::iterator it=selection->get_selected();
		if (it) {
			// get row's id
			Glib::ustring id=(*it)[m_ColumnRec.m_Column];
			
			// get name and bg id
			Glib::ustring name=m_Locations[id].name;
			Glib::ustring bg=m_Locations[id].states["default"];
			
			// fill in the entries
			m_IdEntry->set_text(id);
			m_NameEntry->set_text(name);
			m_BGCB->set_active_internal(bg);
			
			// clear list of hotspots
			m_HotspotList->clear_items();
			
			for (int i=0; i<m_Locations[id].hotspots.size(); i++) {
				Case::Hotspot hspot=m_Locations[id].hotspots[i];
				
				// create strings for list
				std::stringstream ss;
				ss << "(" << hspot.rect.x << "," << hspot.rect.y << "); " << hspot.rect.w << "x" << hspot.rect.h;
				
				// append a new row
				int row=m_HotspotList->append_text(ss.str());
				m_HotspotList->set_text(row, 1, hspot.block);
			}
			
			// clear list of states
			m_StateList->clear_items();
			
			for (std::map<Glib::ustring, Glib::ustring>::iterator it=m_Locations[id].states.begin(); 
						  it!=m_Locations[id].states.end(); ++it) {
				// ignore the default state
				if ((*it).first=="default")
					continue;
				
				// append a new row
				int row=m_StateList->append_text((*it).first);
				m_StateList->set_text(row, 1, (*it).second);
			}
		}
	}
}

/***************************************************************************/

// constructor
SpriteChooserDialog::SpriteChooserDialog() {
	set_title(_("Sprite Editor"));
	construct();
}

// get the selected sprite option
SpriteChooserDialog::SpriteMode SpriteChooserDialog::get_sprite_mode() const {
	if (m_NewSpriteRB->get_active())
		return SPRITE_NEW;
	
	else if (m_OpenSpriteRB->get_active())
		return SPRITE_EXISTING;
	
	else
		return SPRITE_FROM_GIFS;
}

// build the ui
void SpriteChooserDialog::construct() {
	// get default vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate layout table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate radio buttons
	m_NewSpriteRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Blank Sprite")));
	m_OpenSpriteRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Open Existing")));
	m_CreateFromGifsRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Create from GIFs")));
	
	// connect signals
	m_NewSpriteRB->signal_toggled().connect(sigc::mem_fun(*this, &SpriteChooserDialog::on_new_sprite_toggled));
	m_OpenSpriteRB->signal_toggled().connect(sigc::mem_fun(*this, &SpriteChooserDialog::on_open_sprite_toggled));
	m_CreateFromGifsRB->signal_toggled().connect(sigc::mem_fun(*this, &SpriteChooserDialog::on_from_gifs_toggled));
	
	// check new sprite by default
	m_NewSpriteRB->set_active(true);
	
	// allocate labels
	m_SpriteLabel=Gtk::manage(new Gtk::Label(_("Sprite")));
	m_PathLabel=Gtk::manage(new Gtk::Label(_("Path")));
	
	// allocate buttons
	m_BrowseButton=Gtk::manage(new Gtk::Button("..."));
	
	// connect signals
	m_BrowseButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteChooserDialog::on_browse_button_clicked));
	
	// allocate entries
	m_PathEntry=Gtk::manage(new Gtk::Entry);
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_SpriteLabel, 0, 1, 0, 1, xops, yops);
	table->attach(*m_NewSpriteRB, 1, 2, 0, 1, xops, yops);
	table->attach(*m_OpenSpriteRB, 2, 3, 0, 1, xops, yops);
	table->attach(*m_CreateFromGifsRB, 3, 4, 0, 1, xops, yops);
	table->attach(*m_PathLabel, 0, 2, 1, 2, xops, yops);
	table->attach(*m_PathEntry, 2, 3, 1, 2, xops, yops);
	table->attach(*m_BrowseButton, 3, 4, 1, 2, yops, yops);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	// add buttons
	add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	// set defaults
	m_PathEntry->set_sensitive(false);
	m_BrowseButton->set_sensitive(false);
	
	show_all_children();
}

// browse button click handler
void SpriteChooserDialog::on_browse_button_clicked() {
	// open an existing sprite
	if (get_sprite_mode()==SPRITE_EXISTING) {
		// prepare file chooser
		Gtk::FileChooserDialog fcd(*this, _("Open Sprite"), Gtk::FILE_CHOOSER_ACTION_OPEN);
		fcd.add_button(_("Open"), Gtk::RESPONSE_OK);
		fcd.add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
		
		// add a filter
		Gtk::FileFilter filter;
		filter.add_pattern("*.spr");
		filter.set_name(_("Sprites")+" (*.spr)");
		fcd.add_filter(filter);
		
		// run the dialog
		if (fcd.run()==Gtk::RESPONSE_OK) {
			// get the selected sprite
			Glib::ustring path=fcd.get_filename();
			
			// set this text
			m_PathEntry->set_text(path);
		}
	}
	
	// otherwise, we're opening a folder
	else {
		// prepare file chooser
		Gtk::FileChooserDialog fcd(*this, _("Open Folder"), Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
		fcd.add_button(_("Open"), Gtk::RESPONSE_OK);
		fcd.add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
		
		// run the dialog
		if (fcd.run()==Gtk::RESPONSE_OK) {
			Glib::ustring path=fcd.get_current_folder();
			
			// set this text
			m_PathEntry->set_text(path);
		}
	}
}

// new sprite radio button toggle handler
void SpriteChooserDialog::on_new_sprite_toggled() {
	// disable file stuff
	m_PathEntry->set_sensitive(false);
	m_BrowseButton->set_sensitive(false);
}

// open sprite radio button toggle handler
void SpriteChooserDialog::on_open_sprite_toggled() {
	// enable file stuff
	m_PathEntry->set_sensitive(true);
	m_BrowseButton->set_sensitive(true);
}

// from gifs button toggled
void SpriteChooserDialog::on_from_gifs_toggled() {
	// enable file stuff
	m_PathEntry->set_sensitive(true);
	m_BrowseButton->set_sensitive(true);
}

/***************************************************************************/

// constructor
LocationStateDialog::LocationStateDialog(const BackgroundMap &map) {
	set_title(_("New Location State"));
	
	m_BGCB=manage(new BGComboBox(map));
	
	construct();
}

// get inputted data
StringPair LocationStateDialog::get_data() const {
	StringPair p;
	p.first=m_IDEntry->get_text();
	p.second=m_BGCB->get_selected_internal();
	
	return p;
}

// build the ui
void LocationStateDialog::construct() {
	// allocate layout table
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_IDLabel=manage(new Gtk::Label(_("State ID")));
	m_BGLabel=manage(new Gtk::Label(_("Background")));
	
	// allocate entries
	m_IDEntry=manage(new Gtk::Entry);
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_IDLabel, 0, 1, 0, 1, xops, yops);
	table->attach(*m_IDEntry, 1, 2, 0, 1, xops, yops);
	table->attach(*m_BGLabel, 0, 1, 1, 2, xops, yops);
	table->attach(*m_BGCB, 1, 2, 1, 2, xops, yops);
	
	get_vbox()->pack_start(*table, Gtk::PACK_SHRINK);
	
	add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	show_all_children();
	
}

/***************************************************************************/

// constructor
TextInputDialog::TextInputDialog(const Glib::ustring &label, const Glib::ustring &defaultValue) {
	set_title(_("Input Text"));
	construct(label, defaultValue);
}

// build the ui
void TextInputDialog::construct(const Glib::ustring &label, const Glib::ustring &defaultValue="") {
	// get the default vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate hbox
	Gtk::HBox *hb=Gtk::manage(new Gtk::HBox);
	hb->set_spacing(5);
	
	// allocate label
	m_Label=Gtk::manage(new Gtk::Label(label));
	
	// allocate entry
	m_Entry=Gtk::manage(new Gtk::Entry);
	m_Entry->set_text(defaultValue);
	
	// pack widgets
	hb->pack_start(*m_Label, Gtk::PACK_SHRINK);
	hb->pack_start(*m_Entry, Gtk::PACK_SHRINK);
	vb->pack_start(*hb);
	
	// add buttons
	add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

/***************************************************************************/

// constructor
AudioDialog::AudioDialog() {
	set_title(_("Audio Assets"));
	set_size_request(250, 300);
	
	construct();
}

// set audio to use
void AudioDialog::set_audio(const AudioMap &audio) {
	// clear out previously used list
	m_Model->clear();
	
	// iterate over audio
	for (AudioMap::const_iterator it=audio.begin(); it!=audio.end(); ++it) {
		// append a new row
		Gtk::TreeModel::Row row=*(m_Model->append());
		
		// set text
		row[m_ColRec.m_IdCol]=(*it).second.id;
		row[m_ColRec.m_NameCol]=(*it).second.name;
	}
}

// return the list of audio
AudioMap AudioDialog::get_audio_data() {
	AudioMap audioMap;
	
	// iterate over rows
	for (Gtk::TreeModel::Children::iterator it=m_Model->children().begin(); it!=m_Model->children().end(); ++it) {
		Case::Audio audio;
		
		// fill in data
		audio.id=(*it)[m_ColRec.m_IdCol];
		audio.name=(*it)[m_ColRec.m_NameCol];
		
		// add this sample
		audioMap[audio.id]=audio;
	}
	
	return audioMap;
}

// build the ui
void AudioDialog::construct() {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate layout table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate scrolled window
	m_SWindow=Gtk::manage(new Gtk::ScrolledWindow);
	m_SWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_SWindow->set_size_request(80, 100);
	
	// allocate labels
	m_AudioLabel=Gtk::manage(new Gtk::Label(_("Audio Assets")));
	
	// allocate buttons
	m_AddButton=Gtk::manage(new Gtk::Button(_("Add")));
	m_DeleteButton=Gtk::manage(new Gtk::Button(_("Remove")));
	
	// connect signals
	m_AddButton->signal_clicked().connect(sigc::mem_fun(*this, &AudioDialog::on_add_audio));
	m_DeleteButton->signal_clicked().connect(sigc::mem_fun(*this, &AudioDialog::on_remove_audio));
	
	// allocate list view
	m_Model=Gtk::ListStore::create(m_ColRec);
	m_AudioList=Gtk::manage(new Gtk::TreeView(m_Model));
	m_SWindow->add(*m_AudioList);
	
	// append columns
	m_AudioList->append_column_editable(_("File Name"), m_ColRec.m_NameCol);
	m_AudioList->append_column(_("ID"), m_ColRec.m_IdCol);
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_AudioLabel, 0, 2, 0, 1, xops, yops);
	table->attach(*m_AddButton, 0, 1, 1, 2, xops, yops);
	table->attach(*m_DeleteButton, 1, 2, 1, 2, xops, yops);
	table->attach(*m_SWindow, 0, 2, 2, 3);
	
	vb->pack_start(*table);
	
	// add buttons
	add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

// add button handler
void AudioDialog::on_add_audio() {
	// ask for internal id
	TextInputDialog td(_("Internal ID"));
	if (td.run()==Gtk::RESPONSE_OK) {
		std::string id=td.get_text();
		
		// add a new row
		Gtk::TreeModel::Row row=*(m_Model->append());
		row[m_ColRec.m_IdCol]=id;
	}
}

// delete button handler
void AudioDialog::on_remove_audio() {
	// get selection
	Glib::RefPtr<Gtk::TreeView::Selection> selection=m_AudioList->get_selection();
	if (selection) {
		Gtk::TreeModel::iterator it=selection->get_selected();
		if (it) {
			// remove this row
			m_Model->erase(it);
		}
	}
}

/***************************************************************************/

// constructor
NewEvidenceDialog::NewEvidenceDialog(const StringVector &ids) {
	set_title(_("Add Evidence"));
	construct();
	
	m_UsedIds=ids;
}

// get inputted evidence data
Case::Evidence NewEvidenceDialog::get_evidence_data() {
	Case::Evidence evidence;
	
	// fill in all pertinent data
	evidence.id=m_IdEntry->get_text();
	
	return evidence;
}

// build the ui
void NewEvidenceDialog::construct() {
	// get default vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate layout table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_PathLabel=Gtk::manage(new Gtk::Label(_("Path to Image")));
	m_IdLabel=Gtk::manage(new Gtk::Label(_("Internal ID")));
	
	// allocate entries
	m_PathEntry=Gtk::manage(new Gtk::Entry);
	m_IdEntry=Gtk::manage(new Gtk::Entry);
	
	// connect signals
	m_IdEntry->signal_changed().connect(sigc::mem_fun(*this, &NewEvidenceDialog::on_id_changed));
	
	// allocate buttons
	m_BrowseButton=Gtk::manage(new Gtk::Button("..."));
	
	// connect signals
	m_BrowseButton->signal_clicked().connect(sigc::mem_fun(*this, &NewEvidenceDialog::on_browse_button_clicked));
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_PathLabel, 0, 1, 0, 1, xops, yops);
	table->attach(*m_PathEntry, 1, 2, 0, 1, xops, yops);
	table->attach(*m_BrowseButton, 2, 3, 0, 1, yops, yops);
	table->attach(*m_IdLabel, 0, 1, 1, 2, xops, yops);
	table->attach(*m_IdEntry, 1, 3, 1, 2, xops, yops);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	// add buttons
	m_OKButton=add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	m_OKButton->set_sensitive(false);
	
	show_all_children();
}

// id text changes handler
void NewEvidenceDialog::on_id_changed() {
	// get the current id
	Glib::ustring current=m_IdEntry->get_text();
	
	// iterate over used ids and check them against this one
	for (int i=0; i<m_UsedIds.size(); i++) {
		if (m_UsedIds[i]==current) {
			m_OKButton->set_sensitive(false);
			return;
		}
	}
	
	m_OKButton->set_sensitive(true);
}

// browse button clicks
void NewEvidenceDialog::on_browse_button_clicked() {
	// prepare file chooser
	Gtk::FileChooserDialog fcd(*this, _("Open Image"), Gtk::FILE_CHOOSER_ACTION_OPEN);
	fcd.add_button(_("Open"), Gtk::RESPONSE_OK);
	fcd.add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	// run the dialog
	if (fcd.run()==Gtk::RESPONSE_OK) {
		// get the selected image
		Glib::ustring path=fcd.get_filename();
		
		// set this text
		m_PathEntry->set_text(path);
	}
}

/***************************************************************************/

// constructor
EvidenceDialog::EvidenceDialog(const EvidenceMap &evidence,
			       const ImageMap &images,
			       const StringVector &evidenceIds) {
	set_title(_("Evidence Assets"));
	construct(images);
	
	// copy data
	m_Evidence=evidence;
	m_EvidenceIds=evidenceIds;
	
	// iterate over evidence ids and add them into the list
	for (int i=0; i<m_EvidenceIds.size(); i++) {
		Gtk::TreeModel::Row row=*(m_Model->append());
		row[m_ColumnRec.m_Column]=m_EvidenceIds[i];
	}
}

// build the ui
void EvidenceDialog::construct(const ImageMap &images) {
	// get default vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate labels
	m_EvidenceLabel=Gtk::manage(new Gtk::Label(_("Evidence Assets")));
	m_PreviewLabel=Gtk::manage(new Gtk::Label(_("Preview")));
	m_InternalLabel=Gtk::manage(new Gtk::Label(_("Internal ID")));
	m_NameLabel=Gtk::manage(new Gtk::Label(_("Name")));
	m_CaptionLabel=Gtk::manage(new Gtk::Label(_("Caption")));
	m_DescLabel=Gtk::manage(new Gtk::Label(_("Description")));
	
	// allocate image
	m_Image=Gtk::manage(new Gtk::Image);
	m_Image->set_size_request(70, 70);
	
	// allocate check buttons
	m_HasImgCB=Gtk::manage(new Gtk::CheckButton(_("Check Image")));
	m_HasImgCB->signal_toggled().connect(sigc::mem_fun(*this, &EvidenceDialog::on_check_img_toggled));
	
	// allocate combo boxes
	m_ImgCB=Gtk::manage(new ImgComboBox(images));
	
	// allocate tree view and its model
	m_Model=Gtk::ListStore::create(m_ColumnRec);
	m_TreeView=Gtk::manage(new Gtk::TreeView(m_Model));
	
	// append column
	m_TreeView->append_column(_("Evidence ID"), m_ColumnRec.m_Column);
	
	// connect signals
	Glib::RefPtr<Gtk::TreeView::Selection> selection=m_TreeView->get_selection();
	selection->signal_changed().connect(sigc::mem_fun(*this, &EvidenceDialog::on_selection_changed));
	
	// allocate containing scrolled window
	m_SWindow=Gtk::manage(new Gtk::ScrolledWindow);
	m_SWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_SWindow->add(*m_TreeView);
	m_SWindow->set_size_request(150, 200);
	
	// allocate entries
	m_InternalEntry=Gtk::manage(new Gtk::Entry);
	m_InternalEntry->set_sensitive(false);
	m_NameEntry=Gtk::manage(new Gtk::Entry);
	m_CaptionEntry=Gtk::manage(new Gtk::Entry);
	m_DescEntry=Gtk::manage(new Gtk::Entry);
	
	// allocate buttons
	m_AddButton=Gtk::manage(new Gtk::Button(_("Add")));
	m_DeleteButton=Gtk::manage(new Gtk::Button(_("Delete")));
	m_AmendButton=Gtk::manage(new Gtk::Button(_("Amend")));
	
	// connect signals
	m_AddButton->signal_clicked().connect(sigc::mem_fun(*this, &EvidenceDialog::on_add));
	m_DeleteButton->signal_clicked().connect(sigc::mem_fun(*this, &EvidenceDialog::on_delete));
	m_AmendButton->signal_clicked().connect(sigc::mem_fun(*this, &EvidenceDialog::on_amend_button_clicked));
	
	// allocate layout table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_EvidenceLabel, 0, 2, 0, 1, xops, yops);
	table->attach(*m_AddButton, 0, 1, 1, 2, yops, yops);
	table->attach(*m_DeleteButton, 1, 2, 1, 2, yops, yops);
	table->attach(*m_SWindow, 0, 2, 2, 8);
	table->attach(*Gtk::manage(new Gtk::VSeparator), 2, 3, 0, 8);
	table->attach(*m_PreviewLabel, 3, 5, 0, 1, xops, yops);
	table->attach(*m_Image, 3, 5, 1, 2);
	table->attach(*m_InternalLabel, 3, 4, 2, 3, xops, yops);
	table->attach(*m_InternalEntry, 4, 5, 2, 3, xops, yops);
	table->attach(*m_NameLabel, 3, 4, 3, 4, xops, yops);
	table->attach(*m_NameEntry, 4, 5, 3, 4, xops, yops);
	table->attach(*m_CaptionLabel, 3, 4, 4, 5, xops, yops);
	table->attach(*m_CaptionEntry, 4, 5, 4, 5, xops, yops);
	table->attach(*m_DescLabel, 3, 4, 5, 6, xops, yops);
	table->attach(*m_DescEntry, 4, 5, 5, 6, xops, yops);
	table->attach(*m_HasImgCB, 3, 4, 6, 7, xops, yops);
	table->attach(*m_ImgCB, 4, 5, 6, 7, xops, yops);
	table->attach(*m_AmendButton, 4, 5, 7, 8, xops, yops);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	// set defaults
	on_check_img_toggled();
	
	// add buttons
	add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

// has check image button toggled
void EvidenceDialog::on_check_img_toggled() {
	if (!m_ImgCB->empty())
		m_ImgCB->set_sensitive(m_HasImgCB->get_active());
}

// add evidence
void EvidenceDialog::on_add() {
	// bring up dialog
	NewEvidenceDialog nd(m_EvidenceIds);
	
	// run the dialog
	if (nd.run()==Gtk::RESPONSE_OK) {
		// get evidence data struct
		Case::Evidence evidence=nd.get_evidence_data();
		
		// now get the filename of the image
		Glib::ustring path=nd.get_image_path();
		
		// try to open this image
		Glib::RefPtr<Gdk::Pixbuf> pixbuf=Gdk::Pixbuf::create_from_file(path);
		if (pixbuf) {
			// check image dimensions
			if (pixbuf->get_width()>70 || pixbuf->get_height()>70) {
				Gtk::MessageDialog md(*this, "Evidence images must be exactly 70 pixels wide and 70 pixels high.", false,
						      Gtk::MESSAGE_ERROR);
				md.run();
				return;
			}
			
			// we're good to go
			evidence.checkID="null";
			evidence.pixbuf=pixbuf;
			
			// append a new row
			Gtk::TreeModel::Row row=*(m_Model->append());
			row[m_ColumnRec.m_Column]=evidence.id;
			
			// add this evidence to the map
			m_Evidence[evidence.id]=evidence;
		}
		
		else {
			Gtk::MessageDialog md(*this, _("Unable to load selected image."), false, Gtk::MESSAGE_ERROR);
			md.run();
		}
	}
}

// remove evidence
void EvidenceDialog::on_delete() {
	// get the selection
	Glib::RefPtr<Gtk::TreeView::Selection> selection=m_TreeView->get_selection();
	if (selection) {
		Gtk::TreeModel::iterator it=selection->get_selected();
		if (it) {
			m_Evidence.erase((*it)[m_ColumnRec.m_Column]);
			m_Model->erase(it);
		}
	}
}

// amend button click handler
void EvidenceDialog::on_amend_button_clicked() {
	// get current data about this item
	Glib::ustring id=m_InternalEntry->get_text();
	Glib::ustring name=m_NameEntry->get_text();
	Glib::ustring cap=m_CaptionEntry->get_text();
	Glib::ustring desc=m_DescEntry->get_text();
	Glib::ustring checkID=(m_HasImgCB->get_active() ? m_ImgCB->get_selected_internal() : "null");
	
	// update this data
	Case::Evidence &evidence=m_Evidence[id];
	evidence.name=name;
	evidence.caption=cap;
	evidence.description=desc;
	evidence.checkID=checkID;
}

// row changes handler
void EvidenceDialog::on_selection_changed() {
	Glib::RefPtr<Gtk::TreeView::Selection> selection=m_TreeView->get_selection();
	if (selection) {
		Gtk::TreeModel::iterator it=selection->get_selected();
		if (it) {
			// get row's id
			Glib::ustring id=(*it)[m_ColumnRec.m_Column];
			
			// update form with row's data
			Case::Evidence evidence=m_Evidence[id];
			
			// set the data from stored struct
			m_InternalEntry->set_text(evidence.id);
			m_NameEntry->set_text(evidence.name);
			m_CaptionEntry->set_text(evidence.caption);
			m_DescEntry->set_text(evidence.description);
			m_Image->set(evidence.pixbuf);
			
			if (evidence.checkID!="null") {
				m_HasImgCB->set_active(true);
				m_ImgCB->set_active_text(evidence.checkID);
			}
			else
				m_HasImgCB->set_active(false);
			
			on_check_img_toggled();
		}
	}
	
}

/***************************************************************************/

// constructor
BackgroundsDialog::BackgroundsDialog(const BackgroundMap &bgs,
				     const StringVector &bgIds) {
	set_title(_("Background Assets"));
	construct();
	
	// copy data
	m_Backgrounds=bgs;
	m_BGIds=bgIds;
	
	// iterate over backgrouds and add them into the list
	for (int i=0; i<m_BGIds.size(); i++) {
		Gtk::TreeModel::Row row=*(m_ListModel->append());
		row[m_ColumnRec.m_Column]=m_BGIds[i];
	}
}

// build the ui
void BackgroundsDialog::construct() {
	// get default vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate labels
	m_BGLabel=Gtk::manage(new Gtk::Label(_("Background Assets")));
	m_PreviewLabel=Gtk::manage(new Gtk::Label(_("Preview")));
	
	// allocate buttons
	m_AddButton=Gtk::manage(new Gtk::Button(_("Add")));
	m_DeleteButton=Gtk::manage(new Gtk::Button(_("Delete")));
	
	// connect signals
	m_AddButton->signal_clicked().connect(sigc::mem_fun(*this, &BackgroundsDialog::on_add));
	m_DeleteButton->signal_clicked().connect(sigc::mem_fun(*this, &BackgroundsDialog::on_delete));
	
	// allocate scrolled window
	m_SWindow=Gtk::manage(new Gtk::ScrolledWindow);
	m_SWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	
	// allocate list view
	m_ListModel=Gtk::ListStore::create(m_ColumnRec);
	m_ListView=Gtk::manage(new Gtk::TreeView(m_ListModel));
	
	// configure list view
	m_ListView->append_column(_("Background ID"), m_ColumnRec.m_Column);
	
	// connect selection signals
	Glib::RefPtr<Gtk::TreeView::Selection> selection=m_ListView->get_selection();
	selection->signal_changed().connect(sigc::mem_fun(*this, &BackgroundsDialog::on_selection_changed));
	
	// place list view in its container
	m_SWindow->add(*m_ListView);
	
	// allocate image
	m_Image=Gtk::manage(new Gtk::Image);
	m_Image->set_size_request(256, 192);
	
	// allocate layout table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(10);
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_BGLabel, 0, 2, 0, 1, xops, yops);
	table->attach(*m_AddButton, 0, 1, 1, 2, xops, yops);
	table->attach(*m_DeleteButton, 1, 2, 1, 2, xops, yops);
	table->attach(*m_SWindow, 0, 2, 2, 3);
	table->attach(*m_PreviewLabel, 2, 3, 0, 1, xops, yops);
	table->attach(*m_Image, 2, 3, 1, 3);
	
	vb->pack_start(*table);
	
	// add buttons
	add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

// add a background
void BackgroundsDialog::on_add() {
	// create new background dialog
	NewBackgroundDialog nbd(m_BGIds);
	
	// run the dialog
	if (nbd.run()==Gtk::RESPONSE_OK) {
		// get the background
		Case::Background bg=nbd.get_background_data();
		
		// get the path
		Glib::ustring path=nbd.get_path();
		
		// load the image into a pixbuf
		Glib::RefPtr<Gdk::Pixbuf> pixbuf=Gdk::Pixbuf::create_from_file(path);
		if (pixbuf) {
			// copy the pointer
			bg.pixbuf=pixbuf;
			
			// add an entry to the list
			Gtk::TreeModel::Row row=*(m_ListModel->append());
			row[m_ColumnRec.m_Column]=bg.id;
			
			// add this background to the map
			m_Backgrounds[bg.id]=bg;
		}
	}
}

// remove a background
void BackgroundsDialog::on_delete() {
	// get the selection
	Glib::RefPtr<Gtk::TreeView::Selection> selection=m_ListView->get_selection();
	if (selection) {
		Gtk::TreeModel::iterator it=selection->get_selected();
		if (it) {
			m_Backgrounds.erase((*it)[m_ColumnRec.m_Column]);
			m_ListModel->erase(it);
		}
	}
}

// row changes handler
void BackgroundsDialog::on_selection_changed() {
	// get the selection
	Glib::RefPtr<Gtk::TreeView::Selection> selection=m_ListView->get_selection();
	if (selection) {
		Gtk::TreeModel::iterator it=selection->get_selected();
		if (it) {
			// get row name
			Glib::ustring id=(*it)[m_ColumnRec.m_Column];
			
			// get pixbuf and display it
			m_Image->set(m_Backgrounds[id].pixbuf);
		}
	}
}

/***************************************************************************/

// constructor
NewBackgroundDialog::NewBackgroundDialog(const StringVector &bgIds) {
	set_title(_("Add Background"));
	construct();
	m_UsedBGs=bgIds;
}

// get the data in the dialog compiled in Background struct
Case::Background NewBackgroundDialog::get_background_data() {
	Case::Background bg;
	
	// fill in available data
	bg.id=m_IdEntry->get_text();
	bg.type=(m_SingleScreenRB->get_active() ? Case::BG_SINGLE_SCREEN : Case::BG_DOUBLE_SCREEN);
	
	return bg;
}

// build the ui
void NewBackgroundDialog::construct() {
	// get default vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate labels
	m_PathLabel=Gtk::manage(new Gtk::Label(_("Path to Image")));
	m_IdLabel=Gtk::manage(new Gtk::Label(_("Internal ID")));
	m_TypeLabel=Gtk::manage(new Gtk::Label(_("Type")));
	
	// allocate buttons
	m_BrowseButton=Gtk::manage(new Gtk::Button("..."));
	
	// connect button signals
	m_BrowseButton->signal_clicked().connect(sigc::mem_fun(*this, &NewBackgroundDialog::on_browse_button_clicked));
	
	// allocate entries
	m_PathEntry=Gtk::manage(new Gtk::Entry);
	m_IdEntry=Gtk::manage(new Gtk::Entry);
	
	// connect entry signals
	m_IdEntry->signal_changed().connect(sigc::mem_fun(*this, &NewBackgroundDialog::on_id_changed));
	
	// allocate radio buttons
	m_SingleScreenRB=Gtk::manage(new Gtk::RadioButton(m_RBGroup, _("Single Screen")));
	m_DoubleScreenRB=Gtk::manage(new Gtk::RadioButton(m_RBGroup, _("Double Screen")));
	
	// allocate layout table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_PathLabel, 0, 1, 0, 1, xops, yops);
	table->attach(*m_PathEntry, 1, 2, 0, 1, xops, yops);
	table->attach(*m_BrowseButton, 2, 3, 0, 1, yops, yops);
	table->attach(*m_IdLabel, 0, 1, 1, 2, xops, yops);
	table->attach(*m_IdEntry, 1, 2, 1, 2), xops, yops;
	table->attach(*m_TypeLabel, 0, 1, 2, 3, xops, yops);
	table->attach(*m_SingleScreenRB, 1, 2, 2, 3, xops, yops);
	table->attach(*m_DoubleScreenRB, 2, 3, 2, 3, xops, yops);
	
	// add buttons
	m_OKButton=add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	// disable ok button initially
	m_OKButton->set_sensitive(false);
	
	// wrap up
	vb->pack_start(*table);
	show_all_children();
	
}

// browse button click handler
void NewBackgroundDialog::on_browse_button_clicked() {
	// create file chooser dialog
	Gtk::FileChooserDialog fcd(*this, _("Open Background Image"), Gtk::FILE_CHOOSER_ACTION_OPEN);
	fcd.add_button(_("Open"), Gtk::RESPONSE_OK);
	fcd.add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	// run the dialog
	if (fcd.run()==Gtk::RESPONSE_OK) {
		// get pathname
		Glib::ustring path=fcd.get_filename();
		
		// set this text
		m_PathEntry->set_text(path);
	}
}

// id entry text change handler
void NewBackgroundDialog::on_id_changed() {
	// make sure this id isn't used yet
	Glib::ustring text=m_IdEntry->get_text();
	
	// iterate over used ids
	for (int i=0; i<m_UsedBGs.size(); i++) {
		// found a match
		if (text==m_UsedBGs[i]) {
			m_OKButton->set_sensitive(false);
			return;
		}
	}
	
	// enable ok button
	m_OKButton->set_sensitive(true);
}

/***************************************************************************/

// constructor
NewDialog::NewDialog() {
	set_title(_("New Case"));
	construct();
}

// set case overview
void NewDialog::set_overview(const Case::Overview &overview) {
	m_CaseNameEntry->set_text(overview.name);
	m_CaseAuthorEntry->set_text(overview.author);
	
	Glib::ustring lawTxt;
	switch(overview.lawSys) {
		case Case::TWO_DAY: lawTxt="Two Day Trial (JFA/T&T)";
		case Case::THREE_DAY: lawTxt="Three Day Trial (AA)";
		case Case::SINGLE_TRIAL: lawTxt="Single Trial";
	}
	
	m_LawSysCB->set_active_text(lawTxt);
}

// get case information
Case::Overview NewDialog::get_overview() {
	// make a new struct for this information
	Case::Overview overview;
	
	// fill it in
	overview.name=m_CaseNameEntry->get_text();
	overview.author=m_CaseAuthorEntry->get_text();
	
	Case::LawSystem sys;
	
	if (m_LawSysCB->get_active_text()=="Two Day Trial (JFA/T&T)")
		overview.lawSys=Case::TWO_DAY;
	else if (m_LawSysCB->get_active_text()=="Three Day Trial (AA)")
		overview.lawSys=Case::THREE_DAY;
	else
		overview.lawSys=Case::SINGLE_TRIAL;
	
	return overview;
}

// build the ui
void NewDialog::construct() {
	// get the default vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_spacing(5);
	vb->set_border_width(10);
	
	// allocate labels
	m_CaseNameLabel=Gtk::manage(new Gtk::Label(_("Case Name")));
	m_CaseAuthorLabel=Gtk::manage(new Gtk::Label(_("Author")));
	m_LawSysLabel=Gtk::manage(new Gtk::Label(_("Law System")));
	
	// allocate entries
	m_CaseNameEntry=Gtk::manage(new Gtk::Entry);
	m_CaseAuthorEntry=Gtk::manage(new Gtk::Entry);
	
	// allocate combo boxes
	m_LawSysCB=Gtk::manage(new Gtk::ComboBoxText);
	m_LawSysCB->append_text("Single Trial");
	m_LawSysCB->append_text("Two Day Trial (JFA/T&T)");
	m_LawSysCB->append_text("Three Day Trial (AA)");
	m_LawSysCB->set_active(0);
	
	// allocate container table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// pack widgets
	table->attach(*m_CaseNameLabel, 0, 1, 0, 1);
	table->attach(*m_CaseNameEntry, 1, 2, 0, 1);
	table->attach(*m_CaseAuthorLabel, 0, 1, 1, 2);
	table->attach(*m_CaseAuthorEntry, 1, 2, 1, 2);
	table->attach(*m_LawSysLabel, 0, 1, 2, 3);
	table->attach(*m_LawSysCB, 1, 2, 2, 3);
	
	vb->pack_start(*table);
	
	// add buttons
	add_button(_("OK"), Gtk::RESPONSE_OK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
CharBrowser::CharBrowser(const CharacterMap &cmap) {
	set_title(_("Browse Characters"));
	construct();
	
	// set values
	m_CharacterMap=cmap;
	m_CharChanged=false;
	
	// append characters
	for (CharacterMap::iterator it=m_CharacterMap.begin(); it!=m_CharacterMap.end(); ++it)
		m_CharCB->append_text((*it).first);
	m_CharCB->set_active(0);
	
	on_combo_box_changed();
}

// build the ui
void CharBrowser::construct() {
	// get default vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate layout table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_CharacterLabel=Gtk::manage(new Gtk::Label(_("Character")));
	m_InternalLabel=Gtk::manage(new Gtk::Label(_("Internal ID")));
	m_NameLabel=Gtk::manage(new Gtk::Label(_("Name")));
	m_DescLabel=Gtk::manage(new Gtk::Label(_("Description")));
	
	// allocate entries
	m_InternalEntry=Gtk::manage(new Gtk::Entry); m_InternalEntry->set_sensitive(false);
	m_NameEntry=Gtk::manage(new Gtk::Entry); m_NameEntry->set_sensitive(false);
	m_DescEntry=Gtk::manage(new Gtk::Entry); m_DescEntry->set_sensitive(false);
	
	// allocate buttons
	m_EditButton=Gtk::manage(new Gtk::Button(_("[verb]Edit")));
	
	// connect signals
	m_EditButton->signal_clicked().connect(sigc::mem_fun(*this, &CharBrowser::on_edit_button_clicked));
	
	// allocate combo box
	m_CharCB=Gtk::manage(new Gtk::ComboBoxText);
	
	// connect signals
	m_CharCB->signal_changed().connect(sigc::mem_fun(*this, &CharBrowser::on_combo_box_changed));
	
	// attach flags
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_CharacterLabel, 0, 1, 0, 1, xops, yops);
	table->attach(*m_CharCB, 1, 2, 0, 1, xops, yops);
	table->attach(*m_InternalLabel, 0, 1, 1, 2, xops, yops);
	table->attach(*m_InternalEntry, 1, 2, 1, 2, xops, yops);
	table->attach(*m_NameLabel, 0, 1, 2, 3, xops, yops);
	table->attach(*m_NameEntry, 1, 2, 2, 3, xops, yops);
	table->attach(*m_DescLabel, 0, 1, 3, 4, xops, yops);
	table->attach(*m_DescEntry, 1, 2, 3, 4, xops, yops);
	table->attach(*m_EditButton, 0, 1, 4, 5, xops, yops);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	// add buttons
	add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

// edit button handler
void CharBrowser::on_edit_button_clicked() {
	// form a vector of names
	StringVector vec;
	for (CharacterMap::iterator it=m_CharacterMap.begin(); it!=m_CharacterMap.end(); ++it)
		vec.push_back((*it).second.get_internal_name());
	
	// get our current character
	Character ch=m_CharacterMap[m_NameEntry->get_text()];
	
	// create a new character dialog (reuse it ;P)
	NewCharDialog ncd(vec);
	ncd.set_character_data(ch);
	if (ncd.run()==Gtk::RESPONSE_OK) {
		// flag that a change was made
		m_CharChanged=true;
		
		// get the character data
		Character changed=ncd.get_character_data();
		
		// set this new data
		m_CharacterMap[changed.get_name()]=changed;
	}
}

// combo box selectio changes
void CharBrowser::on_combo_box_changed() {
	// get selected text
	Glib::ustring name=m_CharCB->get_active_text();
	
	// get character
	Character character=m_CharacterMap[name];
	
	// set entries
	m_InternalEntry->set_text(character.get_internal_name());
	m_NameEntry->set_text(character.get_name());
	m_DescEntry->set_text(character.get_description());
}

/***************************************************************************/

// constructor
NewCharDialog::NewCharDialog(const StringVector &names) {
	set_title(_("Character Manager"));
	m_UsedNames=names;
	m_CheckInternals=true;
	construct();
}

// set character data
void NewCharDialog::set_character_data(const Character &ch) {
	// we don't care about internal names when editing
	m_CheckInternals=false;
	
	// set data
	m_CodeNameEntry->set_text(ch.get_internal_name());
	m_NameEntry->set_text(ch.get_name());
	m_CapEntry->set_text(ch.get_caption());
	m_DescEntry->set_text(ch.get_description());
	m_SpriteEntry->set_text(ch.get_sprite_name());
	
	if (ch.get_gender()==Character::GENDER_MALE)
		m_MaleRB->set_active(true);
	else if (ch.get_gender()==Character::GENDER_FEMALE)
		m_FemaleRB->set_active(true);
	else
		m_UnknownRB->set_active(true);
	
	m_HasTagCB->set_active(ch.has_text_box_tag());
	if (m_HasTagCB->get_active())
		m_TextBoxImage->set(ch.get_text_box_tag());
	
	m_HasHeadshotCB->set_active(ch.has_headshot());
	if (m_HasHeadshotCB->get_active())
		m_HeadshotImage->set(ch.get_headshot());
	
	// make entries uneditable
	m_CodeNameEntry->set_sensitive(false);
	m_NameEntry->set_sensitive(false);
	m_OKButton->set_sensitive(true);
}

// return the data in the dialog
Character NewCharDialog::get_character_data() {
	Character character;
	
	// fill in data
	character.set_internal_name(m_CodeNameEntry->get_text());
	character.set_name(m_NameEntry->get_text());
	character.set_caption(m_CapEntry->get_text());
	character.set_description(m_DescEntry->get_text());
	character.set_sprite_name(m_SpriteEntry->get_text());
	
	if (m_MaleRB->get_active())
		character.set_gender(Character::GENDER_MALE);
	else if (m_FemaleRB->get_active())
		character.set_gender(Character::GENDER_FEMALE);
	else
		character.set_gender(Character::GENDER_UNKNOWN);
	
	character.set_has_text_box_tag(m_HasTagCB->get_active());
	if (m_HasTagCB->get_active())
		character.set_text_box_tag(m_TextBoxImage->get_pixbuf());
	
	character.set_has_headshot(m_HasHeadshotCB->get_active());
	if (m_HasHeadshotCB->get_active())
		character.set_headshot(m_HeadshotImage->get_pixbuf());
	
	return character;
}

// build the ui
void NewCharDialog::construct() {
	// get default vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_spacing(5);
	
	// allocate notebook
	m_NB=Gtk::manage(new Gtk::Notebook);
	
	// attach pages
	m_NB->append_page(*build_general_page(), _("General"));
	m_NB->append_page(*build_graphics_page(), _("Graphics"));
	
	// pack widgets
	vb->pack_start(*m_NB);
	
	// add buttons
	m_OKButton=add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	// initially set the ok button to be insensitive
	m_OKButton->set_sensitive(false);
	
	show_all_children();
}

// signal handler for internal name text changes
void NewCharDialog::on_internal_name_changed() {
	if (!m_CheckInternals)
		return;
	
	// make sure this name isn't used yet
	Glib::ustring text=m_CodeNameEntry->get_text();
	
	// iterate over used names
	for (int i=0; i<m_UsedNames.size(); i++) {
		// found a match
		if (text==m_UsedNames[i]) {
			m_OKButton->set_sensitive(false);
			return;
		}
	}
	
	// enable ok button
	m_OKButton->set_sensitive(true);
}

// signal handler for check button clicks
void NewCharDialog::on_check_button_clicked(const Glib::ustring &buttonId) {
	// handler for tag check button clicks
	if (buttonId=="m_HasTagCB") {
		bool clicked=m_HasTagCB->get_active();
		
		// modify widget sensitivities based on check button state
		m_TagBrowseButton->set_sensitive(clicked);
		m_TextBoxImage->set_sensitive(clicked);
		m_TextBoxLabel->set_sensitive(clicked);
	}
	
	// handler for headshot clicks
	else if (buttonId=="m_HasHeadshotCB") {
		bool clicked=m_HasHeadshotCB->get_active();
		
		// modify widget states
		m_HeadshotBrowseButton->set_sensitive(clicked);
		m_HeadshotLabel->set_sensitive(clicked);
		m_HeadshotImage->set_sensitive(clicked);
	}
}

// signal handler for browse button clicks
void NewCharDialog::on_browse_button_clicked(const Glib::ustring &buttonId) {
	// create file chooser dialog
	Gtk::FileChooserDialog fcd(*this, _("Open Image"), Gtk::FILE_CHOOSER_ACTION_OPEN);
	fcd.add_button(_("Open"), Gtk::RESPONSE_OK);
	fcd.add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	// this is where we will store our resulting pixbuf
	Glib::RefPtr<Gdk::Pixbuf> pixbuf;
	
	// run the dialog
	if (fcd.run()==Gtk::RESPONSE_OK) {
		// get pathname
		Glib::ustring path=fcd.get_filename();
		
		// try to open the image
		pixbuf=Gdk::Pixbuf::create_from_file(path);
	}
	
	// button handler for text box tag
	if (buttonId=="m_TagBrowseButton") {
		if (pixbuf)
			m_TextBoxImage->set(pixbuf);
	}
	
	// button handler for headshot
	else if (buttonId=="m_HeadshotBrowseButton") {
		if (pixbuf)
			m_HeadshotImage->set(pixbuf);
	}
}

// build the general page
Gtk::Container* NewCharDialog::build_general_page() {
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	table->set_border_width(10);
	
	// attach flags
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// allocate labels
	m_CodeNameLabel=Gtk::manage(new Gtk::Label(_("Internal ID")));
	m_NameLabel=Gtk::manage(new Gtk::Label(_("Name")));
	m_GenderLabel=Gtk::manage(new Gtk::Label(_("Gender")));
	m_CapLabel=Gtk::manage(new Gtk::Label(_("Caption")));
	m_DescLabel=Gtk::manage(new Gtk::Label(_("Description")));
	m_SpriteLabel=Gtk::manage(new Gtk::Label(_("Sprite Name")));
	
	// allocate radio buttons
	m_MaleRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Male")));
	m_FemaleRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Female")));
	m_UnknownRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Unknown")));
	m_MaleRB->set_active(true);
	
	// allocate entries
	m_CodeNameEntry=Gtk::manage(new Gtk::Entry);
	m_NameEntry=Gtk::manage(new Gtk::Entry);
	m_CapEntry=Gtk::manage(new Gtk::Entry);
	m_DescEntry=Gtk::manage(new Gtk::Entry);
	m_SpriteEntry=Gtk::manage(new Gtk::Entry);
	
	// attach widgets
	table->attach(*m_CodeNameLabel, 0, 1, 0, 1, xops, yops);
	table->attach(*m_CodeNameEntry, 1, 4, 0, 1, xops, yops);
	table->attach(*m_NameLabel, 0, 1, 1, 2, xops, yops);
	table->attach(*m_NameEntry, 1, 4, 1, 2, xops, yops);
	table->attach(*m_GenderLabel, 0, 1, 2, 3, xops, yops);
	table->attach(*m_MaleRB, 1, 2, 2, 3, xops, yops);
	table->attach(*m_FemaleRB, 2, 3, 2, 3, xops, yops);
	table->attach(*m_UnknownRB, 3, 4, 2, 3, xops, yops);
	table->attach(*m_CapLabel, 0, 1, 3, 4, xops, yops);
	table->attach(*m_CapEntry, 1, 4, 3, 4, xops, yops);
	table->attach(*m_DescLabel, 0, 1, 4, 5, xops, yops);
	table->attach(*m_DescEntry, 1, 4, 4, 5, xops, yops);
	table->attach(*m_SpriteLabel, 0, 1, 5, 6, xops, yops);
	table->attach(*m_SpriteEntry, 1, 4, 5, 6, xops, yops);
	
	// connect signals
	m_CodeNameEntry->signal_changed().connect(sigc::mem_fun(*this, &NewCharDialog::on_internal_name_changed));
	
	return table;
}

// build the graphics page
Gtk::Container* NewCharDialog::build_graphics_page() {
	// main vbox
	Gtk::VBox *vb=Gtk::manage(new Gtk::VBox);
	vb->set_spacing(5);
	vb->set_border_width(10);
	
	// attach flags
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// allocate tag frame layout table
	Gtk::Table *tagTable=Gtk::manage(new Gtk::Table);
	tagTable->set_spacings(5);
	tagTable->set_border_width(5);
	
	// allocate frames
	Gtk::Frame *tagFrame=Gtk::manage(new Gtk::Frame(_("Text Box")));
	
	// allocate check buttons
	m_HasTagCB=Gtk::manage(new Gtk::CheckButton(_("Has Text Box Tag")));
	m_HasTagCB->set_active(false);
	
	// connect signals
	m_HasTagCB->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, 
					     &NewCharDialog::on_check_button_clicked), "m_HasTagCB"));
	
	// allocate labels
	m_TextBoxLabel=Gtk::manage(new Gtk::Label(_("Text Box Tag")));
	
	// allocate buttons
	m_TagBrowseButton=Gtk::manage(new Gtk::Button("..."));
	
	// connect signals
	m_TagBrowseButton->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this,
						    &NewCharDialog::on_browse_button_clicked), "m_TagBrowseButton"));
	
	// allocate images
	m_TextBoxImage=Gtk::manage(new Gtk::Image);
	
	// place widgets
	tagTable->attach(*m_HasTagCB, 0, 2, 0, 1, xops, yops);
	tagTable->attach(*m_TextBoxLabel, 0, 1, 1, 2, xops, yops);
	tagTable->attach(*m_TagBrowseButton, 1, 2, 1, 2, yops, yops);
	tagTable->attach(*m_TextBoxImage, 0, 2, 2, 3, yops, yops);
	
	////////////////////////////////////////////////////////////////////////////
	
	// allocate headshot layout table
	Gtk::Table *hshotTable=Gtk::manage(new Gtk::Table);
	hshotTable->set_spacings(5);
	hshotTable->set_border_width(5);
	
	// allocate frames
	Gtk::Frame *hshotFrame=Gtk::manage(new Gtk::Frame(_("Headshot")));
	
	// allocate check buttons
	m_HasHeadshotCB=Gtk::manage(new Gtk::CheckButton(_("Has Headshot")));
	m_HasHeadshotCB->set_active(false);
	
	// connect signals
	m_HasHeadshotCB->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, 
						  &NewCharDialog::on_check_button_clicked), "m_HasHeadshotCB"));
	
	// allocate labels
	m_HeadshotLabel=Gtk::manage(new Gtk::Label(_("Headshot (Profile) Image")));
	
	// allocate buttons
	m_HeadshotBrowseButton=Gtk::manage(new Gtk::Button("..."));
	
	// connect signals
	m_HeadshotBrowseButton->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this,
							 &NewCharDialog::on_browse_button_clicked), "m_HeadshotBrowseButton"));
	
	// allocate images
	m_HeadshotImage=Gtk::manage(new Gtk::Image);
	m_HeadshotImage->set_size_request(70, 70);
	
	// place widgets
	hshotTable->attach(*m_HasHeadshotCB, 0, 2, 0, 1, xops, yops);
	hshotTable->attach(*m_HeadshotLabel, 0, 1, 1, 2, xops, yops);
	hshotTable->attach(*m_HeadshotBrowseButton, 1, 2, 1, 2, yops, yops);
	hshotTable->attach(*m_HeadshotImage, 0, 2, 2, 3, yops, yops);
	
	// place tables in frames
	tagFrame->add(*tagTable);
	hshotFrame->add(*hshotTable);
	
	// pack widgets
	vb->pack_start(*tagFrame);
	vb->pack_start(*hshotFrame);
	
	// modify states
	on_check_button_clicked("m_HasTagCB");
	on_check_button_clicked("m_HasHeadshotCB");
	
	return vb;
}

/***************************************************************************/

// constructor
InitialBlockDialog::InitialBlockDialog(const Glib::ustring &id, BufferMap blocks) {
	set_title(_("Initial Text Block"));
	
	// iterate over blocks and remove the list ids
	for (BufferMap::iterator it=blocks.begin(); it!=blocks.end(); ++it) {
		// remove list id
		Glib::ustring id=(*it).first;
		id.erase(id.rfind('_'), id.size());
		
		// add the block
		m_Blocks[id]=(*it).second;
	}
	
	construct();
	
	// set the active block id
	if (id!="null")
		m_BlocksCB->set_active_text(id);
}

// build the ui
void InitialBlockDialog::construct() {
	// get default vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// table for layout
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_BlockLabel=Gtk::manage(new Gtk::Label(_("Initial block to execute")));
	m_PreviewLabel=Gtk::manage(new Gtk::Label(_("Text Block Preview")));
	
	// allocate combo box
	m_BlocksCB=Gtk::manage(new Gtk::ComboBoxText);
	
	// connect signals
	m_BlocksCB->signal_changed().connect(sigc::mem_fun(*this, &InitialBlockDialog::on_block_changed));
	
	// append the names of the text blocks
	for (BufferMap::iterator it=m_Blocks.begin();
	     it!=m_Blocks.end(); ++it) {
		// trim the end of the string
		Glib::ustring str=(*it).first;
		m_BlocksCB->append_text(str);
	}
	
	// allocate text view
	m_TextPreview=Gtk::manage(new Gtk::TextView);
	m_TextPreview->set_editable(false);
	
	// allocate its containing window
	m_SWindow=Gtk::manage(new Gtk::ScrolledWindow);
	m_SWindow->add(*m_TextPreview);
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_BlockLabel, 0, 1, 0, 1, xops, yops);
	table->attach(*m_BlocksCB, 0, 1, 1, 2, xops, yops);
	table->attach(*m_PreviewLabel, 0, 1, 2, 3, xops, yops);
	table->attach(*m_SWindow, 0, 1, 3, 4, xops);
	
	// add buttons
	add_button(_("OK"), Gtk::RESPONSE_OK);
	add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	// wrap up
	vb->pack_start(*table);
	show_all_children();
}

// block cb changes
void InitialBlockDialog::on_block_changed() {
	// get the active text
	Glib::ustring text=m_BlocksCB->get_active_text();
	
	// display the buffer
	m_TextPreview->set_buffer(m_Blocks[text]);
}
