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

#include <gtkmm/filechooserdialog.h>
#include <gtkmm/frame.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/table.h>
#include <sstream>

#include "dialogs.h"

// constructor
NewHotspotDialog::NewHotspotDialog() {
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
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_XLabel=manage(new Gtk::Label("X"));
	m_YLabel=manage(new Gtk::Label("Y"));
	m_WLabel=manage(new Gtk::Label("Width"));
	m_HLabel=manage(new Gtk::Label("Height"));
	m_BlockLabel=manage(new Gtk::Label("Target Block Id"));
	
	// allocate entries
	m_XEntry=manage(new Gtk::Entry);
	m_YEntry=manage(new Gtk::Entry);
	m_WEntry=manage(new Gtk::Entry);
	m_HEntry=manage(new Gtk::Entry);
	m_BlockEntry=manage(new Gtk::Entry);
	
	// connect entry signals
	m_XEntry->signal_changed().connect(sigc::mem_fun(*this, &NewHotspotDialog::on_coord_entry_changed));
	m_YEntry->signal_changed().connect(sigc::mem_fun(*this, &NewHotspotDialog::on_coord_entry_changed));
	m_WEntry->signal_changed().connect(sigc::mem_fun(*this, &NewHotspotDialog::on_dimension_entry_changed));
	m_HEntry->signal_changed().connect(sigc::mem_fun(*this, &NewHotspotDialog::on_dimension_entry_changed));
	
	// allocate hotspot widget
	m_HSWidget=manage(new HotspotWidget);
	
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
	add_button("OK", Gtk::RESPONSE_OK);
	add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
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
	
	// allocate layout table
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_LocationsLabel=manage(new Gtk::Label("Locations"));
	m_DetailsLabel=manage(new Gtk::Label("Details"));
	m_IdLabel=manage(new Gtk::Label("Internal Id"));
	m_NameLabel=manage(new Gtk::Label("Name"));
	m_BGLabel=manage(new Gtk::Label("Background Id"));
	m_HotspotsLabel=manage(new Gtk::Label("Hotspots"));
	
	// allocate entries
	m_IdEntry=manage(new Gtk::Entry);
	m_NameEntry=manage(new Gtk::Entry);
	m_BGEntry=manage(new Gtk::Entry);
	m_IdEntry->set_sensitive(false);
	
	// allocate buttons
	m_AddButton=manage(new Gtk::Button("Add"));
	m_DeleteButton=manage(new Gtk::Button("Delete"));
	m_AddHSButton=manage(new Gtk::Button("Add"));
	m_DeleteHSButton=manage(new Gtk::Button("Delete"));
	m_AmendButton=manage(new Gtk::Button("Amend"));
	
	// connect signals
	m_AddButton->signal_clicked().connect(sigc::mem_fun(*this, &LocationsDialog::on_add));
	m_DeleteButton->signal_clicked().connect(sigc::mem_fun(*this, &LocationsDialog::on_delete));
	m_AddHSButton->signal_clicked().connect(sigc::mem_fun(*this, &LocationsDialog::on_add_hotspot));
	m_DeleteHSButton->signal_clicked().connect(sigc::mem_fun(*this, &LocationsDialog::on_delete_hotspot));
	m_AmendButton->signal_clicked().connect(sigc::mem_fun(*this, &LocationsDialog::on_amend_button_clicked));
	
	// allocate list view
	m_HotspotList=manage(new Gtk::ListViewText(2));
	m_HotspotList->set_column_title(0, "Area");
	m_HotspotList->set_column_title(1, "Target Block");
	
	// allocate tree view
	m_Model=Gtk::ListStore::create(m_ColumnRec);
	m_TreeView=manage(new Gtk::TreeView(m_Model));
	m_TreeView->set_size_request(50, 100);
	
	// append a default column
	m_TreeView->append_column("Location Id", m_ColumnRec.m_Column);
	
	// connect selection change signals
	Glib::RefPtr<Gtk::TreeView::Selection> selection=m_TreeView->get_selection();
	selection->signal_changed().connect(sigc::mem_fun(*this, &LocationsDialog::on_selection_changed));
	
	// allocate containing scrolled window
	m_SWindow=manage(new Gtk::ScrolledWindow);
	m_SWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_SWindow->set_size_request(50, 80);
	
	m_HotspotSWindow=manage(new Gtk::ScrolledWindow);
	m_HotspotSWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_HotspotSWindow->set_size_request(50, 80);
	
	// add widgets to scrolled windows
	m_SWindow->add(*m_TreeView);
	m_HotspotSWindow->add(*m_HotspotList);
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_LocationsLabel, 0, 2, 0, 1, xops, yops);
	table->attach(*m_AddButton, 0, 1, 1, 2, xops, yops);
	table->attach(*m_DeleteButton, 1, 2, 1, 2, xops, yops);
	table->attach(*m_SWindow, 0, 2, 2, 8);
	table->attach(*m_DetailsLabel, 2, 4, 0, 1, xops, yops);
	table->attach(*m_IdLabel, 2, 3, 1, 2, xops, yops);
	table->attach(*m_IdEntry, 3, 4, 1, 2, xops, yops);
	table->attach(*m_NameLabel, 2, 3, 2, 3, xops, yops);
	table->attach(*m_NameEntry, 3, 4, 2, 3, xops, yops);
	table->attach(*m_BGLabel, 2, 3, 3, 4, xops, yops);
	table->attach(*m_BGEntry, 3, 4, 3, 4, xops, yops);
	table->attach(*m_HotspotsLabel, 2, 4, 4, 5, xops, yops);
	table->attach(*m_AddHSButton, 2, 3, 5, 6, yops, yops);
	table->attach(*m_DeleteHSButton, 3, 4, 5, 6, yops, yops);
	table->attach(*m_HotspotSWindow, 2, 4, 6, 7);
	table->attach(*m_AmendButton, 3, 4, 7, 8, xops, yops);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	// add buttons
	add_button("OK", Gtk::RESPONSE_OK);
	add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

// add a location
void LocationsDialog::on_add() {
	// ask for an id
	TextInputDialog td("Location Id");
	if (td.run()==Gtk::RESPONSE_OK) {
		// check the id
		Glib::ustring id=td.get_text();
		
		// see if it's already been used
		for (int i=0; i<m_UsedIds.size(); i++) {
			if (id==m_UsedIds[i]) {
				Gtk::MessageDialog md(*this, "You must choose a unique id.", false, Gtk::MESSAGE_WARNING);
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
	if (m_Backgrounds.find(location.bg)==m_Backgrounds.end()) {
		Gtk::MessageDialog md(*this, "The background was not found.", false, Gtk::MESSAGE_ERROR);
		md.run();
		return;
	}
	
	// prepare the appropriate dialog
	NewHotspotDialog nhd;
	
	// set the background
	nhd.set_pixbuf(m_Backgrounds[location.bg].pixbuf);
	
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

// amend button click handler
void LocationsDialog::on_amend_button_clicked() {
	// get data of the current location
	Glib::ustring id=m_IdEntry->get_text();
	Glib::ustring name=m_NameEntry->get_text();
	Glib::ustring bg=m_BGEntry->get_text();
	
	// update the location
	m_Locations[id].name=name;
	m_Locations[id].bg=bg;
	
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
			Glib::ustring bg=m_Locations[id].bg;
			
			// fill in the entries
			m_IdEntry->set_text(id);
			m_NameEntry->set_text(name);
			m_BGEntry->set_text(bg);
			
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
		}
	}
}

/***************************************************************************/

// constructor
SpriteChooserDialog::SpriteChooserDialog() {
	construct();
}

// build the ui
void SpriteChooserDialog::construct() {
	// get default vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate layout table
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate radio buttons
	m_NewSpriteRB=manage(new Gtk::RadioButton(m_Group, "New"));
	m_OpenSpriteRB=manage(new Gtk::RadioButton(m_Group, "Existing"));
	
	// connect signals
	m_NewSpriteRB->signal_toggled().connect(sigc::mem_fun(*this, &SpriteChooserDialog::on_new_sprite_toggled));
	m_OpenSpriteRB->signal_toggled().connect(sigc::mem_fun(*this, &SpriteChooserDialog::on_open_sprite_toggled));
	
	// check new sprite by default
	m_NewSpriteRB->set_active(true);
	
	// allocate labels
	m_SpriteLabel=manage(new Gtk::Label("Sprite"));
	m_PathLabel=manage(new Gtk::Label("Path Entry"));
	
	// allocate buttons
	m_BrowseButton=manage(new Gtk::Button("..."));
	
	// connect signals
	m_BrowseButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteChooserDialog::on_browse_button_clicked));
	
	// allocate entries
	m_PathEntry=manage(new Gtk::Entry);
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_SpriteLabel, 0, 1, 0, 1, xops, yops);
	table->attach(*m_NewSpriteRB, 1, 2, 0, 1, xops, yops);
	table->attach(*m_OpenSpriteRB, 2, 3, 0, 1, xops, yops);
	table->attach(*m_PathLabel, 0, 1, 1, 2, xops, yops);
	table->attach(*m_PathEntry, 1, 2, 1, 2, xops, yops);
	table->attach(*m_BrowseButton, 2, 3, 1, 2, xops, yops);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	// add buttons
	add_button("OK", Gtk::RESPONSE_OK);
	add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	// set defaults
	m_PathEntry->set_sensitive(false);
	m_BrowseButton->set_sensitive(false);
	
	show_all_children();
}

// browse button click handler
void SpriteChooserDialog::on_browse_button_clicked() {
	// prepare file chooser
	Gtk::FileChooserDialog fcd(*this, "Open Sprite", Gtk::FILE_CHOOSER_ACTION_OPEN);
	fcd.add_button("Open", Gtk::RESPONSE_OK);
	fcd.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	// add a filter
	Gtk::FileFilter filter;
	filter.add_pattern("*.spr");
	filter.set_name("Sprites (*.spr)");
	fcd.add_filter(filter);
	
	// run the dialog
	if (fcd.run()==Gtk::RESPONSE_OK) {
		// get the selected sprite
		Glib::ustring path=fcd.get_filename();
		
		// set this text
		m_PathEntry->set_text(path);
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

/***************************************************************************/

// constructor
TextInputDialog::TextInputDialog(const Glib::ustring &label, const Glib::ustring &defaultValue) {
	construct(label, defaultValue);
}

// build the ui
void TextInputDialog::construct(const Glib::ustring &label, const Glib::ustring &defaultValue="") {
	// get the default vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate hbox
	Gtk::HBox *hb=manage(new Gtk::HBox);
	hb->set_spacing(5);
	
	// allocate label
	m_Label=manage(new Gtk::Label(label));
	
	// allocate entry
	m_Entry=manage(new Gtk::Entry);
	m_Entry->set_text(defaultValue);
	
	// pack widgets
	hb->pack_start(*m_Label, Gtk::PACK_SHRINK);
	hb->pack_start(*m_Entry, Gtk::PACK_SHRINK);
	vb->pack_start(*hb);
	
	// add buttons
	add_button("OK", Gtk::RESPONSE_OK);
	add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

/***************************************************************************/

// constructor
NewEvidenceDialog::NewEvidenceDialog(const StringVector &ids) {
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
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_PathLabel=manage(new Gtk::Label("Path to Image"));
	m_IdLabel=manage(new Gtk::Label("Internal Id"));
	
	// allocate entries
	m_PathEntry=manage(new Gtk::Entry);
	m_IdEntry=manage(new Gtk::Entry);
	
	// connect signals
	m_IdEntry->signal_changed().connect(sigc::mem_fun(*this, &NewEvidenceDialog::on_id_changed));
	
	// allocate buttons
	m_BrowseButton=manage(new Gtk::Button("..."));
	
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
	m_OKButton=add_button("OK", Gtk::RESPONSE_OK);
	add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
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
	Gtk::FileChooserDialog fcd(*this, "Open Image", Gtk::FILE_CHOOSER_ACTION_OPEN);
	fcd.add_button("Open", Gtk::RESPONSE_OK);
	fcd.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
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
			       const StringVector &evidenceIds) {
	construct();
	
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
void EvidenceDialog::construct() {
	// get default vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate labels
	m_EvidenceLabel=manage(new Gtk::Label("Evidence Assets"));
	m_PreviewLabel=manage(new Gtk::Label("Preview"));
	m_InternalLabel=manage(new Gtk::Label("Internal Id"));
	m_NameLabel=manage(new Gtk::Label("Name"));
	m_CaptionLabel=manage(new Gtk::Label("Caption"));
	m_DescLabel=manage(new Gtk::Label("Description"));
	
	// allocate image
	m_Image=manage(new Gtk::Image);
	
	// allocate tree view and its model
	m_Model=Gtk::ListStore::create(m_ColumnRec);
	m_TreeView=manage(new Gtk::TreeView(m_Model));
	
	// append column
	m_TreeView->append_column("Evidence Id", m_ColumnRec.m_Column);
	
	// connect signals
	Glib::RefPtr<Gtk::TreeView::Selection> selection=m_TreeView->get_selection();
	selection->signal_changed().connect(sigc::mem_fun(*this, &EvidenceDialog::on_selection_changed));
	
	// allocate containing scrolled window
	m_SWindow=manage(new Gtk::ScrolledWindow);
	m_SWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_SWindow->add(*m_TreeView);
	
	// allocate entries
	m_InternalEntry=manage(new Gtk::Entry);
	m_InternalEntry->set_sensitive(false);
	m_NameEntry=manage(new Gtk::Entry);
	m_CaptionEntry=manage(new Gtk::Entry);
	m_DescEntry=manage(new Gtk::Entry);
	
	// allocate buttons
	m_AddButton=manage(new Gtk::Button("Add"));
	m_DeleteButton=manage(new Gtk::Button("Delete"));
	m_AmendButton=manage(new Gtk::Button("Amend"));
	
	// connect signals
	m_AddButton->signal_clicked().connect(sigc::mem_fun(*this, &EvidenceDialog::on_add));
	m_DeleteButton->signal_clicked().connect(sigc::mem_fun(*this, &EvidenceDialog::on_delete));
	m_AmendButton->signal_clicked().connect(sigc::mem_fun(*this, &EvidenceDialog::on_amend_button_clicked));
	
	// allocate layout table
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_EvidenceLabel, 0, 2, 0, 1, xops, yops);
	table->attach(*m_AddButton, 0, 1, 1, 2, yops, yops);
	table->attach(*m_DeleteButton, 1, 2, 1, 2, yops, yops);
	table->attach(*m_SWindow, 0, 2, 2, 6);
	table->attach(*m_PreviewLabel, 2, 4, 0, 1, xops, yops);
	table->attach(*m_Image, 2, 4, 1, 2);
	table->attach(*m_InternalLabel, 2, 3, 2, 3, xops, yops);
	table->attach(*m_InternalEntry, 3, 4, 2, 3, xops, yops);
	table->attach(*m_NameLabel, 2, 3, 3, 4, xops, yops);
	table->attach(*m_NameEntry, 3, 4, 3, 4, xops, yops);
	table->attach(*m_CaptionLabel, 2, 3, 4, 5, xops, yops);
	table->attach(*m_CaptionEntry, 3, 4, 4, 5, xops, yops);
	table->attach(*m_DescLabel, 2, 3, 5, 6, xops, yops);
	table->attach(*m_DescEntry, 3, 4, 5, 6, xops, yops);
	table->attach(*m_AmendButton, 3, 4, 6, 7, xops, yops);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	// add buttons
	add_button("OK", Gtk::RESPONSE_OK);
	add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

// add a background
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
			evidence.pixbuf=pixbuf;
			
			// append a new row
			Gtk::TreeModel::Row row=*(m_Model->append());
			row[m_ColumnRec.m_Column]=evidence.id;
			
			// add this evidence to the map
			m_Evidence[evidence.id]=evidence;
		}
		
		else {
			Gtk::MessageDialog md(*this, "Unable to load selected image.", false, Gtk::MESSAGE_ERROR);
			md.run();
		}
	}
}

// remove a background
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
	
	// update this data
	Case::Evidence &evidence=m_Evidence[id];
	evidence.name=name;
	evidence.caption=cap;
	evidence.description=desc;
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
		}
	}
	
}

/***************************************************************************/

// constructor
BackgroundsDialog::BackgroundsDialog(const BackgroundMap &bgs,
				     const StringVector &bgIds) {
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
	m_BGLabel=manage(new Gtk::Label("Background Assets"));
	m_PreviewLabel=manage(new Gtk::Label("Preview"));
	
	// allocate buttons
	m_AddButton=manage(new Gtk::Button("Add"));
	m_DeleteButton=manage(new Gtk::Button("Delete"));
	
	// connect signals
	m_AddButton->signal_clicked().connect(sigc::mem_fun(*this, &BackgroundsDialog::on_add));
	m_DeleteButton->signal_clicked().connect(sigc::mem_fun(*this, &BackgroundsDialog::on_delete));
	
	// allocate scrolled window
	m_SWindow=manage(new Gtk::ScrolledWindow);
	m_SWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	
	// allocate list view
	m_ListModel=Gtk::ListStore::create(m_ColumnRec);
	m_ListView=manage(new Gtk::TreeView(m_ListModel));
	
	// configure list view
	m_ListView->append_column("Background IDs", m_ColumnRec.m_Column);
	
	// connect selection signals
	Glib::RefPtr<Gtk::TreeView::Selection> selection=m_ListView->get_selection();
	selection->signal_changed().connect(sigc::mem_fun(*this, &BackgroundsDialog::on_selection_changed));
	
	// place list view in its container
	m_SWindow->add(*m_ListView);
	
	// allocate image
	m_Image=manage(new Gtk::Image);
	m_Image->set_size_request(256, 192);
	
	// allocate layout table
	Gtk::Table *table=manage(new Gtk::Table);
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
	add_button("OK", Gtk::RESPONSE_OK);
	add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
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
	m_PathLabel=manage(new Gtk::Label("Path to Image"));
	m_IdLabel=manage(new Gtk::Label("Internal ID"));
	m_TypeLabel=manage(new Gtk::Label("Type"));
	
	// allocate buttons
	m_BrowseButton=manage(new Gtk::Button("..."));
	
	// connect button signals
	m_BrowseButton->signal_clicked().connect(sigc::mem_fun(*this, &NewBackgroundDialog::on_browse_button_clicked));
	
	// allocate entries
	m_PathEntry=manage(new Gtk::Entry);
	m_IdEntry=manage(new Gtk::Entry);
	
	// connect entry signals
	m_IdEntry->signal_changed().connect(sigc::mem_fun(*this, &NewBackgroundDialog::on_id_changed));
	
	// allocate radio buttons
	m_SingleScreenRB=manage(new Gtk::RadioButton(m_RBGroup, "Single Screen"));
	m_DoubleScreenRB=manage(new Gtk::RadioButton(m_RBGroup, "Double Screen"));
	
	// allocate layout table
	Gtk::Table *table=manage(new Gtk::Table);
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
	m_OKButton=add_button("OK", Gtk::RESPONSE_OK);
	add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	// disable ok button initially
	m_OKButton->set_sensitive(false);
	
	// wrap up
	vb->pack_start(*table);
	show_all_children();
	
}

// browse button click handler
void NewBackgroundDialog::on_browse_button_clicked() {
	// create file chooser dialog
	Gtk::FileChooserDialog fcd(*this, "Open Background Image", Gtk::FILE_CHOOSER_ACTION_OPEN);
	fcd.add_button("Open", Gtk::RESPONSE_OK);
	fcd.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
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
	construct();
}

// set case overview
void NewDialog::set_overview(const Case::Overview &overview) {
	m_CaseNameEntry->set_text(overview.name);
	m_CaseAuthorEntry->set_text(overview.author);
	m_LawSysCB->set_active_text((overview.lawSys==Case::TWO_DAY ? "Two Day Trial (JFA)" : "Three Day Trial (AA)"));
}

// get case information
Case::Overview NewDialog::get_overview() {
	// make a new struct for this information
	Case::Overview overview;
	
	// fill it in
	overview.name=m_CaseNameEntry->get_text();
	overview.author=m_CaseAuthorEntry->get_text();
	overview.lawSys=(m_LawSysCB->get_active_text()=="Two Day Trial (JFA)" ? Case::TWO_DAY : Case::THREE_DAY);
	
	return overview;
}

// build the ui
void NewDialog::construct() {
	// get the default vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_spacing(5);
	vb->set_border_width(10);
	
	// allocate labels
	m_CaseNameLabel=manage(new Gtk::Label("Case Name"));
	m_CaseAuthorLabel=manage(new Gtk::Label("Author"));
	m_LawSysLabel=manage(new Gtk::Label("Law System"));
	
	// allocate entries
	m_CaseNameEntry=manage(new Gtk::Entry);
	m_CaseAuthorEntry=manage(new Gtk::Entry);
	
	// allocate combo boxes
	m_LawSysCB=manage(new Gtk::ComboBoxText);
	m_LawSysCB->append_text("Two Day Trial (JFA)");
	m_LawSysCB->append_text("Three Day Trial (AA)");
	m_LawSysCB->set_active(0);
	
	// allocate container table
	Gtk::Table *table=manage(new Gtk::Table);
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
	add_button("OK", Gtk::RESPONSE_OK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
CharBrowser::CharBrowser(const CharacterMap &cmap) {
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
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_CharacterLabel=manage(new Gtk::Label("Character"));
	m_InternalLabel=manage(new Gtk::Label("Internal Id"));
	m_NameLabel=manage(new Gtk::Label("Name"));
	m_DescLabel=manage(new Gtk::Label("Description"));
	
	// allocate entries
	m_InternalEntry=manage(new Gtk::Entry); m_InternalEntry->set_sensitive(false);
	m_NameEntry=manage(new Gtk::Entry); m_NameEntry->set_sensitive(false);
	m_DescEntry=manage(new Gtk::Entry); m_DescEntry->set_sensitive(false);
	
	// allocate buttons
	m_EditButton=manage(new Gtk::Button("Edit"));
	
	// connect signals
	m_EditButton->signal_clicked().connect(sigc::mem_fun(*this, &CharBrowser::on_edit_button_clicked));
	
	// allocate combo box
	m_CharCB=manage(new Gtk::ComboBoxText);
	
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
	add_button("OK", Gtk::RESPONSE_OK);
	add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

// edit button handler
void CharBrowser::on_edit_button_clicked() {
	// form a vector of names
	StringVector vec;
	for (CharacterMapIter it=m_CharacterMap.begin(); it!=m_CharacterMap.end(); ++it)
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
	m_DescEntry->set_text(ch.get_description());
	m_SpriteEntry->set_text(ch.get_sprite_name());
	
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
	character.set_description(m_DescEntry->get_text());
	character.set_sprite_name(m_SpriteEntry->get_text());
	
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
	m_NB=manage(new Gtk::Notebook);
	
	// attach pages
	m_NB->append_page(*build_general_page(), "General");
	m_NB->append_page(*build_graphics_page(), "Graphics");
	
	// pack widgets
	vb->pack_start(*m_NB);
	
	// add buttons
	m_OKButton=add_button("OK", Gtk::RESPONSE_OK);
	add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
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
	Gtk::FileChooserDialog fcd(*this, "Open Image", Gtk::FILE_CHOOSER_ACTION_OPEN);
	fcd.add_button("Open", Gtk::RESPONSE_OK);
	fcd.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
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
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	table->set_border_width(10);
	
	// attach flags
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// allocate labels
	m_CodeNameLabel=manage(new Gtk::Label("Internal Code Name"));
	m_NameLabel=manage(new Gtk::Label("Name"));
	m_DescLabel=manage(new Gtk::Label("Description"));
	m_SpriteLabel=manage(new Gtk::Label("Sprite Name"));
	
	// allocate entries
	m_CodeNameEntry=manage(new Gtk::Entry);
	m_NameEntry=manage(new Gtk::Entry);
	m_DescEntry=manage(new Gtk::Entry);
	m_SpriteEntry=manage(new Gtk::Entry);
	
	// attach widgets
	table->attach(*m_CodeNameLabel, 0, 1, 0, 1, xops, yops);
	table->attach(*m_CodeNameEntry, 1, 2, 0, 1, xops, yops);
	table->attach(*m_NameLabel, 0, 1, 1, 2, xops, yops);
	table->attach(*m_NameEntry, 1, 2, 1, 2, xops, yops);
	table->attach(*m_DescLabel, 0, 1, 2, 3, xops, yops);
	table->attach(*m_DescEntry, 1, 2, 2, 3, xops, yops);
	table->attach(*m_SpriteLabel, 0, 1, 3, 4, xops, yops);
	table->attach(*m_SpriteEntry, 1, 2, 3, 4, xops, yops);
	
	// connect signals
	m_CodeNameEntry->signal_changed().connect(sigc::mem_fun(*this, &NewCharDialog::on_internal_name_changed));
	
	return table;
}

// build the graphics page
Gtk::Container* NewCharDialog::build_graphics_page() {
	// main vbox
	Gtk::VBox *vb=manage(new Gtk::VBox);
	vb->set_spacing(5);
	vb->set_border_width(10);
	
	// attach flags
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// allocate tag frame layout table
	Gtk::Table *tagTable=manage(new Gtk::Table);
	tagTable->set_spacings(5);
	tagTable->set_border_width(5);
	
	// allocate frames
	Gtk::Frame *tagFrame=manage(new Gtk::Frame("Text Box"));
	
	// allocate check buttons
	m_HasTagCB=manage(new Gtk::CheckButton("Has Text Box Tag"));
	m_HasTagCB->set_active(false);
	
	// connect signals
	m_HasTagCB->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, 
					     &NewCharDialog::on_check_button_clicked), "m_HasTagCB"));
	
	// allocate labels
	m_TextBoxLabel=manage(new Gtk::Label("Text Box Tag"));
	
	// allocate buttons
	m_TagBrowseButton=manage(new Gtk::Button("..."));
	
	// connect signals
	m_TagBrowseButton->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this,
						    &NewCharDialog::on_browse_button_clicked), "m_TagBrowseButton"));
	
	// allocate images
	m_TextBoxImage=manage(new Gtk::Image);
	
	// place widgets
	tagTable->attach(*m_HasTagCB, 0, 2, 0, 1, xops, yops);
	tagTable->attach(*m_TextBoxLabel, 0, 1, 1, 2, xops, yops);
	tagTable->attach(*m_TagBrowseButton, 1, 2, 1, 2, yops, yops);
	tagTable->attach(*m_TextBoxImage, 0, 2, 2, 3, yops, yops);
	
	////////////////////////////////////////////////////////////////////////////
	
	// allocate headshot layout table
	Gtk::Table *hshotTable=manage(new Gtk::Table);
	hshotTable->set_spacings(5);
	hshotTable->set_border_width(5);
	
	// allocate frames
	Gtk::Frame *hshotFrame=manage(new Gtk::Frame("Headshot"));
	
	// allocate check buttons
	m_HasHeadshotCB=manage(new Gtk::CheckButton("Has Headshot"));
	m_HasHeadshotCB->set_active(false);
	
	// connect signals
	m_HasHeadshotCB->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, 
						  &NewCharDialog::on_check_button_clicked), "m_HasHeadshotCB"));
	
	// allocate labels
	m_HeadshotLabel=manage(new Gtk::Label("Headshot (Profile) Image"));
	
	// allocate buttons
	m_HeadshotBrowseButton=manage(new Gtk::Button("..."));
	
	// connect signals
	m_HeadshotBrowseButton->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this,
							 &NewCharDialog::on_browse_button_clicked), "m_HeadshotBrowseButton"));
	
	// allocate images
	m_HeadshotImage=manage(new Gtk::Image);
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
	m_Blocks=blocks;
	
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
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_BlockLabel=manage(new Gtk::Label("Initial block to execute:"));
	m_PreviewLabel=manage(new Gtk::Label("Text Block Preview"));
	
	// allocate combo box
	m_BlocksCB=manage(new Gtk::ComboBoxText);
	
	// connect signals
	m_BlocksCB->signal_changed().connect(sigc::mem_fun(*this, &InitialBlockDialog::on_block_changed));
	
	// append the names of the text blocks
	for (BufferMap::iterator it=m_Blocks.begin();
	     it!=m_Blocks.end(); ++it) {
		m_BlocksCB->append_text((*it).first);
	}
	
	// allocate text view
	m_TextPreview=manage(new Gtk::TextView);
	m_TextPreview->set_editable(false);
	
	// allocate its containing window
	m_SWindow=manage(new Gtk::ScrolledWindow);
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
	add_button("OK", Gtk::RESPONSE_OK);
	add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
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
