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
// triggerdialogs.cpp: implementation of triggerdialogs.h classes

#include <gtkmm/buttonbox.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/separator.h>
#include <gtkmm/table.h>

#include "triggerdialogs.h"

// abstract dialog constructor
AbstractDialog::AbstractDialog(const Glib::ustring &trigger) {
	Gtk::VBox *vb=get_vbox();
	vb->set_spacing(5);
	vb->set_border_width(10);
	
	// allocate label
	m_TriggerLabel=manage(new Gtk::Label);
	m_TriggerLabel->set_markup("Trigger: <i>"+trigger+"</i>");
	
	// pack the default label and separator
	vb->pack_start(*m_TriggerLabel);
	vb->pack_start(*manage(new Gtk::HSeparator));
	
	// add buttons
	Gtk::Button *ok=add_button("OK", Gtk::RESPONSE_OK);
	Gtk::Button *cancel=add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	// connect button signals
	ok->signal_clicked().connect(sigc::mem_fun(*this, &AbstractDialog::on_ok_button_clicked));
	cancel->signal_clicked().connect(sigc::mem_fun(*this, &AbstractDialog::on_cancel_button_clicked));
	
	show_all_children();
}

/***************************************************************************/

// constructor
AddCourtRecDialog::AddCourtRecDialog(const EvidenceMap &ev, const CharacterMap &chars):
		AbstractDialog("add_evidence, add_profile") {
	construct();
	
	m_EvidenceMap=ev;
	m_CharMap=chars;
	
	// iterate over evidence map
	for (EvidenceMap::const_iterator it=ev.begin(); it!=ev.end(); ++it) {
		static bool once=true;
		if ((*it).second.pixbuf && once) {
			m_EvidenceImage->set((*it).second.pixbuf);
			once=false;
		}
		
		m_EvidenceCB->append_text((*it).second.name);
	}
	
	m_EvidenceCB->set_active(0);
	
	// iterate over character map
	for (CharacterMap::const_iterator it=chars.begin(); it!=chars.end(); ++it)
		m_ProfileCB->append_text((*it).second.get_name());
	
	m_ProfileCB->set_active(0);
	if (!chars.empty())
		m_ProfileImage->set(m_CharMap[m_ProfileCB->get_active_text()].get_headshot());
}

// get the selected evidence or profile
AddCourtRecDialog::Data AddCourtRecDialog::get_data() const {
	AddCourtRecDialog::Data data;
	
	data.type=(m_NB->get_current_page()==0 ? TYPE_ADD_EVIDENCE : TYPE_ADD_PROFILE);
	
	if (data.type==TYPE_ADD_EVIDENCE) {
		for (EvidenceMap::const_iterator it=m_EvidenceMap.begin(); it!=m_EvidenceMap.end(); ++it) {
			if ((*it).second.name==m_EvidenceCB->get_active_text()) {
				data.id=(*it).first;
				break;
			}
		}
	}
	
	else {
		for (CharacterMap::const_iterator it=m_CharMap.begin(); it!=m_CharMap.end(); ++it) {
			if ((*it).second.get_name()==m_ProfileCB->get_active_text()) {
				data.id=(*it).second.get_internal_name();
				break;
			}
		}
	}
	
	return data;
}

// build the dialog
void AddCourtRecDialog::construct() {
	Gtk::VBox *vb=get_vbox();
	
	// allocate notebook
	m_NB=manage(new Gtk::Notebook);
	
	// add pages
	m_NB->append_page(*build_evidence_page(), "Evidence");
	m_NB->append_page(*build_profile_page(), "Profile");
	
	// pack widgets
	vb->pack_start(*m_NB, Gtk::PACK_SHRINK);
	
	show_all_children();
}

// handler for combo box changes
void AddCourtRecDialog::on_combo_box_changed(const Glib::ustring &id) {
	if (id=="evidence") {
		// find the id
		for (EvidenceMap::const_iterator it=m_EvidenceMap.begin(); it!=m_EvidenceMap.end(); ++it) {
			if ((*it).second.name==m_EvidenceCB->get_active_text()) {
				m_EvidenceImage->set(m_EvidenceMap[(*it).first].pixbuf);
				break;
			}
		}
	}
	
	else
		m_ProfileImage->set(m_CharMap[m_ProfileCB->get_active_text()].get_headshot());
}

// build the add_evidence page
Gtk::Table* AddCourtRecDialog::build_evidence_page() {
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_EvidenceLabel=manage(new Gtk::Label("Evidence"));
	m_EvidencePreviewLabel=manage(new Gtk::Label("Preview"));
	
	// allocate combobox
	m_EvidenceCB=manage(new Gtk::ComboBoxText);
	
	// connect signals
	m_EvidenceCB->signal_changed().connect(sigc::bind(
				     sigc::mem_fun(*this, &AddCourtRecDialog::on_combo_box_changed), "evidence"));
	
	// allocate image
	m_EvidenceImage=manage(new Gtk::Image);
	m_EvidenceImage->set_size_request(70, 70);
	
	// place widgets
	table->attach(*m_EvidenceLabel, 0, 1, 0, 1);
	table->attach(*m_EvidenceCB, 1, 2, 0, 1);
	table->attach(*m_EvidencePreviewLabel, 0, 1, 1, 2);
	table->attach(*m_EvidenceImage, 1, 2, 1, 2);
	
	return table;
}

// build the add_profile page
Gtk::Table* AddCourtRecDialog::build_profile_page() {
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_ProfileLabel=manage(new Gtk::Label("Profile"));
	m_ProfilePreviewLabel=manage(new Gtk::Label("Preview"));
	
	// allocate combobox
	m_ProfileCB=manage(new Gtk::ComboBoxText);
	
	// connect signal
	m_ProfileCB->signal_changed().connect(sigc::bind(
				     sigc::mem_fun(*this, &AddCourtRecDialog::on_combo_box_changed), "profile"));
	
	// allocate image
	m_ProfileImage=manage(new Gtk::Image);
	m_ProfileImage->set_size_request(70, 70);
	
	// place widgets
	table->attach(*m_ProfileLabel, 0, 1, 0, 1);
	table->attach(*m_ProfileCB, 1, 2, 0, 1);
	table->attach(*m_ProfilePreviewLabel, 0, 1, 1, 2);
	table->attach(*m_ProfileImage, 1, 2, 1, 2);
	
	return table;
}

/***************************************************************************/

// constructor
SpeakerDialog::SpeakerDialog(const CharacterMap &chars): AbstractDialog("speaker") {
	m_CharMap=chars;
	
	construct();
	
	for (CharacterMap::const_iterator it=chars.begin(); it!=chars.end(); ++it) {
		Character ch=(*it).second;
		m_CharsCB->append_text(ch.get_name());
	}
	
	m_CharsCB->set_active(0);
	
}

// get the selected speaker
Glib::ustring SpeakerDialog::get_speaker() {
	return m_CharMap[m_CharsCB->get_active_text()].get_internal_name();
}

// build the dialog
void SpeakerDialog::construct() {
	Gtk::VBox *vb=get_vbox();
	
	// hbox for layout
	Gtk::HBox *hb=manage(new Gtk::HBox);
	hb->set_spacing(5);
	
	// allocate labels
	m_SpeakerLabel=manage(new Gtk::Label("Speaker"));
	
	// allocate combo box
	m_CharsCB=manage(new Gtk::ComboBoxText);
	
	// pack widgets
	hb->pack_start(*m_SpeakerLabel, Gtk::PACK_SHRINK);
	hb->pack_start(*m_CharsCB, Gtk::PACK_SHRINK);
	
	vb->pack_start(*hb, Gtk::PACK_SHRINK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
GotoDialog::GotoDialog(BufferMap blocks): AbstractDialog("goto") {
	construct();
	
	// toggle some widgets
	m_TimeLabel->set_sensitive(false);
	m_TimeEntry->set_sensitive(false);
	
	// fill in combo box
	for (BufferMap::iterator it=blocks.begin(); it!=blocks.end(); ++it) {
		// remove list id
		Glib::ustring id=(*it).first;
		id.erase(id.rfind('_'), id.size());
		
		// add the block
		m_BlockCB->append_text(id);
	}
	
	m_BlockCB->set_active(0);
}

// get the goto trigger data
GotoDialog::Data GotoDialog::get_data() {
	GotoDialog::Data data;
	
	// fill in values
	data.target=m_BlockCB->get_active_text();
	data.time=atoi(m_TimeEntry->get_text().c_str());
	
	if (m_NormalRB->get_active())
		data.type=GOTO_NORMAL;
	else if (m_DirectRB->get_active())
		data.type=GOTO_DIRECT;
	else
		data.type=GOTO_TIMED;
	
	return data;
}

// build the dialog
void GotoDialog::construct() {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate button box
	Gtk::HButtonBox *hbb=manage(new Gtk::HButtonBox);
	
	// allocate radio buttons
	m_NormalRB=manage(new Gtk::RadioButton(m_RadioGroup, "Standard"));
	m_DirectRB=manage(new Gtk::RadioButton(m_RadioGroup, "Direct"));
	m_TimedRB=manage(new Gtk::RadioButton(m_RadioGroup, "Timed"));
	
	// allocate entries
	m_TimeEntry=manage(new Gtk::Entry);
	
	// connect signals
	m_NormalRB->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &GotoDialog::on_radio_button_toggled), "standard"));
	m_DirectRB->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &GotoDialog::on_radio_button_toggled), "direct"));
	m_TimedRB->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &GotoDialog::on_radio_button_toggled), "timed"));
	
	// allocate labels
	m_TypeLabel=manage(new Gtk::Label("Goto Type"));
	m_TargetLabel=manage(new Gtk::Label("Target Block"));
	m_TimeLabel=manage(new Gtk::Label("Delay Time (ms)"));
	
	// allocate combo boxes
	m_BlockCB=manage(new Gtk::ComboBoxText);
	
	// pack buttons
	hbb->pack_start(*m_NormalRB, Gtk::PACK_SHRINK);
	hbb->pack_start(*m_DirectRB, Gtk::PACK_SHRINK);
	hbb->pack_start(*m_TimedRB, Gtk::PACK_SHRINK);
	
	// place widgets
	table->attach(*m_TypeLabel, 0, 1, 0, 1);
	table->attach(*hbb, 1, 2, 0, 1);
	table->attach(*m_TargetLabel, 0, 1, 1, 2);
	table->attach(*m_BlockCB, 1, 2, 1, 2);
	table->attach(*m_TimeLabel, 0, 1, 2, 3);
	table->attach(*m_TimeEntry, 1, 2, 2, 3);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	show_all_children();
}

// radio button toggle handler
void GotoDialog::on_radio_button_toggled(const Glib::ustring &button) {
	bool b;
	
	// toggle widgets
	if (button=="standard" || button=="direct")
		b=false;
	
	else
		b=true;
	
	// time stuff is only relavent for timed goto
	m_TimeLabel->set_sensitive(b);
	m_TimeEntry->set_sensitive(b);
}

/***************************************************************************/

// constructor
ShowEvidenceDialog::ShowEvidenceDialog(const EvidenceMap &ev): AbstractDialog("show_evidence_*") {
	construct();
	
	m_EvidenceMap=ev;
	
	// iterate over evidence
	for (EvidenceMap::const_iterator it=ev.begin(); it!=ev.end(); ++it)
		m_EvidenceCB->append_text((*it).second.name);
	m_EvidenceCB->set_active(0);
}

// get the selection
ShowEvidenceDialog::Data ShowEvidenceDialog::get_data() const {
	ShowEvidenceDialog::Data data;
	
	data.type=(m_LeftRB->get_active() ? ShowEvidenceDialog::TYPE_POS_LEFT : ShowEvidenceDialog::TYPE_POS_RIGHT);
	
	// find the id
	for (EvidenceMap::const_iterator it=m_EvidenceMap.begin(); it!=m_EvidenceMap.end(); ++it) {
		if ((*it).second.name==m_EvidenceCB->get_active_text()) {
			data.id=(*it).first;
			break;
		}
	}
	
	return data;
}

// build the dialog
void ShowEvidenceDialog::construct() {
	// get vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_PositionLabel=manage(new Gtk::Label("Position"));
	m_EvidenceLabel=manage(new Gtk::Label("Evidence"));
	m_PreviewLabel=manage(new Gtk::Label("Preview"));
	
	// allocate radio buttons
	m_LeftRB=manage(new Gtk::RadioButton(m_Group, "Left"));
	m_RightRB=manage(new Gtk::RadioButton(m_Group, "Right"));
	
	// allocate combo box
	m_EvidenceCB=manage(new Gtk::ComboBoxText);
	m_EvidenceCB->signal_changed().connect(sigc::mem_fun(*this, &ShowEvidenceDialog::on_combo_box_changed));
	
	// allocate image
	m_Image=manage(new Gtk::Image);
	m_Image->set_size_request(70, 70);
	
	// button box for radio buttons
	Gtk::HButtonBox *hbb=manage(new Gtk::HButtonBox);
	hbb->pack_start(*m_LeftRB);
	hbb->pack_start(*m_RightRB);
	
	// place widgets
	table->attach(*m_PositionLabel, 0, 1, 0, 1);
	table->attach(*hbb, 1, 2, 0, 1);
	table->attach(*m_EvidenceLabel, 0, 1, 1, 2);
	table->attach(*m_EvidenceCB, 1, 2, 1, 2);
	table->attach(*m_PreviewLabel, 0, 1, 2, 3);
	table->attach(*m_Image, 1, 2, 2, 3);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	show_all_children();
}

// handler for combo box changes
void ShowEvidenceDialog::on_combo_box_changed() {
	// find the id
	for (EvidenceMap::const_iterator it=m_EvidenceMap.begin(); it!=m_EvidenceMap.end(); ++it) {
		if ((*it).second.name==m_EvidenceCB->get_active_text()) {
			m_Image->set(m_EvidenceMap[(*it).first].pixbuf);
			break;
		}
	}
}

/***************************************************************************/

// constructor
HideEvidenceDialog::HideEvidenceDialog(): AbstractDialog("hide_evidence") {
	construct();
}

// get selected position
HideEvidenceDialog::Type HideEvidenceDialog::get_position() const {
	if (m_LeftRB->get_active())
		return HideEvidenceDialog::TYPE_POS_LEFT;
	else
		return HideEvidenceDialog::TYPE_POS_RIGHT;
}

// build the dialog
void HideEvidenceDialog::construct() {
	// get vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocat radio buttons
	m_LeftRB=manage(new Gtk::RadioButton(m_Group, "Left"));
	m_RightRB=manage(new Gtk::RadioButton(m_Group, "Right"));
	
	// allocate labels
	m_PositionLabel=manage(new Gtk::Label("Position"));
	
	// hbox for widgets
	Gtk::HBox *hb=manage(new Gtk::HBox);
	hb->set_spacing(5);
	
	// pack widgets
	hb->pack_start(*m_PositionLabel);
	hb->pack_start(*m_LeftRB);
	hb->pack_start(*m_RightRB);
	
	vb->pack_start(*hb, Gtk::PACK_SHRINK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
SetLocationDialog::SetLocationDialog(const LocationMap &loc): AbstractDialog("set_location") {
	construct();
	
	m_Locations=loc;
	
	// iterate over locations
	for (LocationMap::const_iterator it=loc.begin(); it!=loc.end(); ++it)
		m_LocationCB->append_text((*it).second.name);
	
	m_LocationCB->set_active(0);
}

// get the selected location
Glib::ustring SetLocationDialog::get_location() const {
	for (LocationMap::const_iterator it=m_Locations.begin(); it!=m_Locations.end(); ++it) {
		if ((*it).second.name==m_LocationCB->get_active_text())
			return (*it).first;
	}
}

// build the dialog
void SetLocationDialog::construct() {
	// get vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate labels
	m_LocLabel=manage(new Gtk::Label("Location"));
	
	// allocate combo boxes
	m_LocationCB=manage(new Gtk::ComboBoxText);
	
	// hbox for widgets
	Gtk::HBox *hb=manage(new Gtk::HBox);
	hb->set_spacing(5);
	
	// pack widgets
	hb->pack_start(*m_LocLabel);
	hb->pack_start(*m_LocationCB);
	
	vb->pack_start(*hb, Gtk::PACK_SHRINK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
AddLocationDialog::AddLocationDialog(const LocationMap &loc): AbstractDialog("add_location") {
	construct();
	
	m_Locations=loc;
	
	// iterate over locations
	for (LocationMap::const_iterator it=loc.begin(); it!=loc.end(); ++it) {
		m_LocationCB->append_text((*it).second.name);
		m_TargetCB->append_text((*it).second.name);
	}
	
	m_LocationCB->set_active(0);
	m_TargetCB->set_active(1);
}

// get the selected location
std::pair<Glib::ustring, Glib::ustring> AddLocationDialog::get_location_pair() const {
	Glib::ustring a, b;
	
	// iterate over locations, and fill in the variables
	for (LocationMap::const_iterator it=m_Locations.begin(); it!=m_Locations.end(); ++it) {
		if ((*it).second.name==m_TargetCB->get_active_text())
			a=(*it).first;
		
		else if ((*it).second.name==m_LocationCB->get_active_text())
			b=(*it).first;
	}
	
	return std::make_pair<Glib::ustring, Glib::ustring> (a, b);
}

// build the dialog
void AddLocationDialog::construct() {
	// get vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_TargetLabel=manage(new Gtk::Label("Add"));
	m_LocationLabel=manage(new Gtk::Label("To"));
	
	// allocate combo boxes
	m_LocationCB=manage(new Gtk::ComboBoxText);
	m_TargetCB=manage(new Gtk::ComboBoxText);
	
	// place widgets
	table->attach(*m_TargetLabel, 0, 1, 0, 1);
	table->attach(*m_TargetCB, 1, 2, 0, 1);
	table->attach(*m_LocationLabel, 0, 1, 1, 2);
	table->attach(*m_LocationCB, 1, 2, 1, 2);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	show_all_children();
}

// overridden handler for ok button
void AddLocationDialog::on_ok_button_clicked() {
	// make sure the user didn't select two of the same locations
	if (m_LocationCB->get_active_text()==m_TargetCB->get_active_text()) {
		Gtk::MessageDialog md(*this, "You must select two different locations.", false, Gtk::MESSAGE_ERROR);
		md.run();
	}
}

/***************************************************************************/

// constructor
LocationTriggerDialog::LocationTriggerDialog(const LocationMap &locations, const BufferMap &buffers):
		AbstractDialog("set_location_trigger") {
	construct();
	
	m_Locations=locations;
	m_Blocks=buffers;
	
	// iterate over locations
	for (LocationMap::const_iterator it=locations.begin(); it!=locations.end(); ++it)
		m_LocationCB->append_text((*it).second.name);
	
	// iterate over blocks
	for (BufferMap::const_iterator it=buffers.begin(); it!=buffers.end(); ++it) {
		Glib::ustring id=(*it).first;
		id.erase(id.rfind("_"), id.size());
		
		m_BlockCB->append_text(id);
	}
	
	m_LocationCB->set_active(0);
	m_BlockCB->set_active(0);
}

// get the selection
StringPair LocationTriggerDialog::get_selection() const {
	Glib::ustring location;
	
	// iterate over locations and find the id
	for (LocationMap::const_iterator it=m_Locations.begin(); it!=m_Locations.end(); ++it) {
		if ((*it).second.name==m_LocationCB->get_active_text()) {
			location=(*it).first;
			break;
		}
	}
	
	return std::make_pair<Glib::ustring, Glib::ustring> (location, m_BlockCB->get_active_text());
}

// build the dialog
void LocationTriggerDialog::construct() {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_LocLabel=manage(new Gtk::Label("Location"));
	m_BlockLabel=manage(new Gtk::Label("Block Trigger"));
	
	// allocate combo boxes
	m_LocationCB=manage(new Gtk::ComboBoxText);
	m_BlockCB=manage(new Gtk::ComboBoxText);
	
	// connect signals
	m_BlockCB->signal_changed().connect(sigc::mem_fun(*this, &LocationTriggerDialog::on_text_block_combo_box_changed));
	
	// allocate text views
	m_BlockView=manage(new Gtk::TextView);
	m_BlockView->set_editable(false);
	m_BlockView->set_size_request(200, 200);
	
	// allocate scrolled window
	m_SWindow=manage(new Gtk::ScrolledWindow);
	m_SWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_SWindow->add(*m_BlockView);
	
	// place widgets
	table->attach(*m_LocLabel, 0, 1, 0, 1), Gtk::FILL | Gtk::EXPAND, Gtk::SHRINK;
	table->attach(*m_LocationCB, 1, 2, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::SHRINK);
	table->attach(*m_BlockLabel, 0, 1, 1, 2, Gtk::FILL | Gtk::EXPAND, Gtk::SHRINK);
	table->attach(*m_BlockCB, 1, 2, 1, 2, Gtk::FILL | Gtk::EXPAND, Gtk::SHRINK);
	table->attach(*manage(new Gtk::VSeparator), 2, 3, 0, 2);
	table->attach(*m_SWindow, 3, 4, 0, 2);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	show_all_children();
}

// handler for text block combo box changes
void LocationTriggerDialog::on_text_block_combo_box_changed() {
	// first, get the text block that's selected
	Glib::ustring id=m_BlockCB->get_active_text();
	
	// now, simply find the block, and set its contents in the textview
	for (BufferMap::iterator it=m_Blocks.begin(); it!=m_Blocks.end(); ++it) {
		if ((*it).first.find(id)!=-1) {
			m_BlockView->set_buffer(m_Blocks[(*it).first]);
			break;
		}
	}
}

/***************************************************************************/

// constructor
SetAnimationDialog::SetAnimationDialog(const CharacterMap &chars): AbstractDialog("set_animation") {
	construct();
	
	m_CharMap=chars;
	
	// iterate over characters
	for (CharacterMap::const_iterator it=chars.begin(); it!=chars.end(); ++it)
		m_CharCB->append_text((*it).second.get_name());
	
	m_CharCB->set_active(0);
}

// get the selection
StringPair SetAnimationDialog::get_selection() const {
	Glib::ustring ch;
	
	// first, find the character's id
	for (CharacterMap::const_iterator it=m_CharMap.begin(); it!=m_CharMap.end(); ++it) {
		if ((*it).second.get_name()==m_CharCB->get_active_text()) {
			ch=(*it).second.get_internal_name();
			break;
		}
	}
	
	return std::make_pair<Glib::ustring, Glib::ustring> (ch, m_AnimEntry->get_text());
}

// build the dialog
void SetAnimationDialog::construct() {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_CharLabel=manage(new Gtk::Label("Character"));
	m_AnimLabel=manage(new Gtk::Label("Animation"));
	
	// allocate entries
	m_AnimEntry=manage(new Gtk::Entry);
	
	// allocate combo box
	m_CharCB=manage(new Gtk::ComboBoxText);
	
	// place widgets
	table->attach(*m_CharLabel, 0, 1, 0, 1);
	table->attach(*m_CharCB, 1, 2, 0, 1);
	table->attach(*m_AnimLabel, 0, 1, 1, 2);
	table->attach(*m_AnimEntry, 1, 2, 1, 2);
	
	vb->pack_start(*table);
	
	show_all_children();
}
