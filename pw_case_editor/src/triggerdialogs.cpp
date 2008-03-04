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

#include "intl.h"
#include "triggerdialogs.h"
#include "utilities.h"

// abstract dialog constructor
AbstractDialog::AbstractDialog(const Glib::ustring &trigger) {
	set_title(_("Insert Trigger"));
	
	Gtk::VBox *vb=get_vbox();
	vb->set_spacing(5);
	vb->set_border_width(10);
	
	// allocate label
	m_TriggerLabel=Gtk::manage(new Gtk::Label);
	m_TriggerLabel->set_markup(_("Trigger")+": <i>"+trigger+"</i>");
	
	// pack the default label and separator
	vb->pack_start(*m_TriggerLabel);
	vb->pack_start(*Gtk::manage(new Gtk::HSeparator));
	
	// add buttons
	Gtk::Button *ok=add_button(_("OK"), Gtk::RESPONSE_OK);
	Gtk::Button *cancel=add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
	
	// connect button signals
	ok->signal_clicked().connect(sigc::mem_fun(*this, &AbstractDialog::on_ok_button_clicked));
	cancel->signal_clicked().connect(sigc::mem_fun(*this, &AbstractDialog::on_cancel_button_clicked));
	
	show_all_children();
}

/***************************************************************************/

// constructor
AddCourtRecDialog::AddCourtRecDialog(const EvidenceMap &ev, const CharacterMap &chars):
		AbstractDialog("add_evidence, add_profile") {
	construct(ev, chars);
}

// get the selected evidence or profile
AddCourtRecDialog::Data AddCourtRecDialog::get_data() const {
	AddCourtRecDialog::Data data;
	
	if (m_NB->get_current_page()==0) {
		if (m_EvidenceAnimCB->get_active())
			data.type=TYPE_ADD_EVIDENCE_ANIMATED;
		else
			data.type=TYPE_ADD_EVIDENCE_SILENT;
		
		data.id=m_EvidenceCB->get_selected_internal();
	}
	
	else {
		data.type==TYPE_ADD_PROFILE;
		data.id=m_ProfileCB->get_selected_internal();
	}
	
	return data;
}

// build the dialog
void AddCourtRecDialog::construct(const EvidenceMap &ev, const CharacterMap &chars) {
	Gtk::VBox *vb=get_vbox();
	
	// allocate notebook
	m_NB=Gtk::manage(new Gtk::Notebook);
	
	// add pages
	m_NB->append_page(*build_evidence_page(ev), _("Evidence"));
	m_NB->append_page(*build_profile_page(chars), _("Profile"));
	
	// pack widgets
	vb->pack_start(*m_NB, Gtk::PACK_SHRINK);
	
	show_all_children();
}

// handler for combo box changes
void AddCourtRecDialog::on_combo_box_changed(const Glib::ustring &id) {
	if (id=="evidence")
		m_EvidenceImage->set(m_EvidenceCB->get_selected_evidence()->pixbuf);
	
	else
		m_ProfileImage->set(m_ProfileCB->get_selected_character()->get_headshot());
}

// build the add_evidence page
Gtk::Table* AddCourtRecDialog::build_evidence_page(const EvidenceMap &ev) {
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_EvidenceLabel=Gtk::manage(new Gtk::Label(_("Evidence")));
	m_EvidencePreviewLabel=Gtk::manage(new Gtk::Label(_("Preview")));
	
	// allocate combobox
	m_EvidenceCB=Gtk::manage(new EvidenceComboBox(ev));
	
	// connect signals
	m_EvidenceCB->signal_changed().connect(sigc::bind(
				     sigc::mem_fun(*this, &AddCourtRecDialog::on_combo_box_changed), "evidence"));
	
	// allocate image
	m_EvidenceImage=Gtk::manage(new Gtk::Image);
	m_EvidenceImage->set_size_request(70, 70);
	
	// allocate check button
	m_EvidenceAnimCB=manage(new Gtk::CheckButton(_("Show Animation with Message")));
	m_EvidenceAnimCB->set_active(false);
	
	// place widgets
	table->attach(*m_EvidenceLabel, 0, 1, 0, 1);
	table->attach(*m_EvidenceCB, 1, 2, 0, 1);
	table->attach(*m_EvidencePreviewLabel, 0, 1, 1, 2);
	table->attach(*m_EvidenceImage, 1, 2, 1, 2);
	table->attach(*m_EvidenceAnimCB, 0, 2, 2, 3);
	
	return table;
}

// build the add_profile page
Gtk::Table* AddCourtRecDialog::build_profile_page(const CharacterMap &chars) {
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_ProfileLabel=Gtk::manage(new Gtk::Label(_("Profile")));
	m_ProfilePreviewLabel=Gtk::manage(new Gtk::Label(_("Preview")));
	
	// allocate combobox
	m_ProfileCB=Gtk::manage(new CharComboBox(chars));
	
	// connect signal
	m_ProfileCB->signal_changed().connect(sigc::bind(
				     sigc::mem_fun(*this, &AddCourtRecDialog::on_combo_box_changed), "profile"));
	
	// allocate image
	m_ProfileImage=Gtk::manage(new Gtk::Image);
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
	construct(chars);
	
}

// get the selected speaker
Glib::ustring SpeakerDialog::get_speaker() {
	return m_CharsCB->get_selected_internal();
}

// build the dialog
void SpeakerDialog::construct(const CharacterMap &chars) {
	Gtk::VBox *vb=get_vbox();
	
	// hbox for layout
	Gtk::HBox *hb=Gtk::manage(new Gtk::HBox);
	hb->set_spacing(5);
	
	// allocate labels
	m_SpeakerLabel=Gtk::manage(new Gtk::Label(_("Speaker")));
	
	// allocate combo box
	m_CharsCB=Gtk::manage(new CharComboBox(chars));
	
	// pack widgets
	hb->pack_start(*m_SpeakerLabel, Gtk::PACK_SHRINK);
	hb->pack_start(*m_CharsCB, Gtk::PACK_SHRINK);
	
	vb->pack_start(*hb, Gtk::PACK_SHRINK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
GotoDialog::GotoDialog(const BufferMap &blocks): AbstractDialog("goto") {
	construct(blocks);
	
	// toggle some widgets
	m_TimeLabel->set_sensitive(false);
	m_TimeEntry->set_sensitive(false);
}

// get the goto trigger data
GotoDialog::Data GotoDialog::get_data() {
	// fill in values
	GotoDialog::Type type;
	Glib::ustring id=m_BlockCB->get_selected_internal();
	int time=atoi(m_TimeEntry->get_text().c_str());
	
	if (m_NormalRB->get_active())
		type=GOTO_NORMAL;
	else if (m_DirectRB->get_active())
		type=GOTO_DIRECT;
	else
		type=GOTO_TIMED;
	
	return make_triplet<Glib::ustring, GotoDialog::Type, int>(id, type, time);
}

// build the dialog
void GotoDialog::construct(const BufferMap &blocks) {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate button box
	Gtk::HButtonBox *hbb=Gtk::manage(new Gtk::HButtonBox);
	
	// allocate radio buttons
	m_NormalRB=Gtk::manage(new Gtk::RadioButton(m_RadioGroup, _("Standard")));
	m_DirectRB=Gtk::manage(new Gtk::RadioButton(m_RadioGroup, _("Direct")));
	m_TimedRB=Gtk::manage(new Gtk::RadioButton(m_RadioGroup, _("Timed")));
	
	// allocate entries
	m_TimeEntry=Gtk::manage(new Gtk::Entry);
	
	// connect signals
	m_NormalRB->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &GotoDialog::on_radio_button_toggled), "standard"));
	m_DirectRB->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &GotoDialog::on_radio_button_toggled), "direct"));
	m_TimedRB->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &GotoDialog::on_radio_button_toggled), "timed"));
	
	// allocate labels
	m_TypeLabel=Gtk::manage(new Gtk::Label(_("Goto Type")));
	m_TargetLabel=Gtk::manage(new Gtk::Label(_("Target Block")));
	m_TimeLabel=Gtk::manage(new Gtk::Label(_("Delay Time (ms)")));
	
	// allocate combo boxes
	m_BlockCB=Gtk::manage(new BlockComboBox(blocks));
	
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
	construct(ev);
}

// get the selection
ShowEvidenceDialog::Data ShowEvidenceDialog::get_data() const {
	ShowEvidenceDialog::Type type=(m_LeftRB->get_active() ? ShowEvidenceDialog::TYPE_POS_LEFT : ShowEvidenceDialog::TYPE_POS_RIGHT);
	Glib::ustring id=m_EvidenceCB->get_selected_internal();
	
	return std::make_pair<Glib::ustring, ShowEvidenceDialog::Type>(id, type);
}

// build the dialog
void ShowEvidenceDialog::construct(const EvidenceMap &ev) {
	// get vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_PositionLabel=Gtk::manage(new Gtk::Label(_("Position")));
	m_EvidenceLabel=Gtk::manage(new Gtk::Label(_("Evidence")));
	m_PreviewLabel=Gtk::manage(new Gtk::Label(_("Preview")));
	
	// allocate radio buttons
	m_LeftRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Left")));
	m_RightRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Right")));
	
	// allocate combo box
	m_EvidenceCB=Gtk::manage(new EvidenceComboBox(ev));
	m_EvidenceCB->signal_changed().connect(sigc::mem_fun(*this, &ShowEvidenceDialog::on_combo_box_changed));
	
	// allocate image
	m_Image=Gtk::manage(new Gtk::Image);
	m_Image->set_size_request(70, 70);
	
	// button box for radio buttons
	Gtk::HButtonBox *hbb=Gtk::manage(new Gtk::HButtonBox);
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
	m_Image->set(m_EvidenceCB->get_selected_evidence()->pixbuf);
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
	m_LeftRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Left")));
	m_RightRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Right")));
	
	// allocate labels
	m_PositionLabel=Gtk::manage(new Gtk::Label(_("Position")));
	
	// hbox for widgets
	Gtk::HBox *hb=Gtk::manage(new Gtk::HBox);
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
	construct(loc);
}

// get the selected location
Glib::ustring SetLocationDialog::get_location() const {
	return m_LocWidget->get_selected_location();
}

// build the dialog
void SetLocationDialog::construct(const LocationMap &locations) {
	// get vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate special location widget
	m_LocWidget=Gtk::manage(new LocationWidget(locations));
	
	vb->pack_start(*m_LocWidget, Gtk::PACK_SHRINK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
AddLocationDialog::AddLocationDialog(const LocationMap &loc): AbstractDialog("add_location") {
	construct(loc);
}

// get the selected location
std::pair<Glib::ustring, Glib::ustring> AddLocationDialog::get_location_pair() const {
	return std::make_pair<Glib::ustring, Glib::ustring> 
			(m_TargetCB->get_selected_internal(), m_LocationCB->get_selected_internal());
}

// build the dialog
void AddLocationDialog::construct(const LocationMap &locations) {
	// get vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_TargetLabel=Gtk::manage(new Gtk::Label(_("Add")));
	m_LocationLabel=Gtk::manage(new Gtk::Label(_("To")));
	
	// allocate combo boxes
	m_LocationCB=Gtk::manage(new LocationComboBox(locations));
	m_TargetCB=Gtk::manage(new LocationComboBox(locations));
	
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
		Gtk::MessageDialog md(*this, _("You must select two different locations."), false, Gtk::MESSAGE_ERROR);
		md.run();
	}
}

/***************************************************************************/

// constructor
LocationTriggerDialog::LocationTriggerDialog(const LocationMap &locations, const BufferMap &buffers):
		AbstractDialog("set_location_trigger") {
	construct(locations, buffers);
}

// get the selection
StringPair LocationTriggerDialog::get_selection() const {
	return std::make_pair<Glib::ustring, Glib::ustring> (m_LocationCB->get_selected_internal(), m_BlockCB->get_active_text());
}

// build the dialog
void LocationTriggerDialog::construct(const LocationMap &locations, const BufferMap &buffers) {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_LocLabel=Gtk::manage(new Gtk::Label(_("Location")));
	m_BlockLabel=Gtk::manage(new Gtk::Label(_("Block Trigger")));
	
	// allocate combo boxes
	m_LocationCB=Gtk::manage(new LocationComboBox(locations));
	m_BlockCB=Gtk::manage(new BlockComboBox(buffers));
	
	// connect signals
	m_BlockCB->signal_changed().connect(sigc::mem_fun(*this, &LocationTriggerDialog::on_text_block_combo_box_changed));
	
	// allocate text views
	m_BlockView=Gtk::manage(new Gtk::TextView);
	m_BlockView->set_editable(false);
	m_BlockView->set_size_request(200, 200);
	
	// allocate scrolled window
	m_SWindow=Gtk::manage(new Gtk::ScrolledWindow);
	m_SWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_SWindow->add(*m_BlockView);
	
	// place widgets
	table->attach(*m_LocLabel, 0, 1, 0, 1), Gtk::FILL | Gtk::EXPAND, Gtk::SHRINK;
	table->attach(*m_LocationCB, 1, 2, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::SHRINK);
	table->attach(*m_BlockLabel, 0, 1, 1, 2, Gtk::FILL | Gtk::EXPAND, Gtk::SHRINK);
	table->attach(*m_BlockCB, 1, 2, 1, 2, Gtk::FILL | Gtk::EXPAND, Gtk::SHRINK);
	table->attach(*Gtk::manage(new Gtk::VSeparator), 2, 3, 0, 2);
	table->attach(*m_SWindow, 3, 4, 0, 2);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	show_all_children();
}

// handler for text block combo box changes
void LocationTriggerDialog::on_text_block_combo_box_changed() {
	// simply find the block, and set its contents in the textview
	m_BlockView->set_buffer(m_BlockCB->get_selected_block());
}

/***************************************************************************/

// constructor
SetAnimationDialog::SetAnimationDialog(const CharacterMap &chars): AbstractDialog("set_animation") {
	construct(chars);
}

// get the selection
StringPair SetAnimationDialog::get_selection() const {	
	return std::make_pair<Glib::ustring, Glib::ustring> (m_CharCB->get_selected_internal(), m_AnimEntry->get_text());
}

// build the dialog
void SetAnimationDialog::construct(const CharacterMap &chars) {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_CharLabel=Gtk::manage(new Gtk::Label(_("Character")));
	m_AnimLabel=Gtk::manage(new Gtk::Label(_("Animation")));
	
	// allocate entries
	m_AnimEntry=Gtk::manage(new Gtk::Entry);
	
	// allocate combo box
	m_CharCB=Gtk::manage(new CharComboBox(chars));
	
	// place widgets
	table->attach(*m_CharLabel, 0, 1, 0, 1);
	table->attach(*m_CharCB, 1, 2, 0, 1);
	table->attach(*m_AnimLabel, 0, 1, 1, 2);
	table->attach(*m_AnimEntry, 1, 2, 1, 2);
	
	vb->pack_start(*table);
	
	show_all_children();
}

/***************************************************************************/

// constructor
PutCharDialog::PutCharDialog(const CharacterMap &chars, const LocationMap &locations):
		AbstractDialog("put_character") {
	construct(chars, locations);
}

// get the selection
StringPair PutCharDialog::get_selection() const {	
	return std::make_pair<Glib::ustring, Glib::ustring> 
			(m_CharCB->get_selected_internal(), m_LocWidget->get_selected_location());
}

// build the dialog
void PutCharDialog::construct(const CharacterMap &chars, const LocationMap &locations) {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate location widget
	m_LocWidget=Gtk::manage(new LocationWidget(locations));
	
	// allocate labels
	m_CharLabel=Gtk::manage(new Gtk::Label(_("Character")));
	
	// allocat combo boxes
	m_CharCB=Gtk::manage(new CharComboBox(chars));
	
	// place widgets
	table->attach(*m_CharLabel, 0, 1, 0, 1);
	table->attach(*m_CharCB, 1, 2, 0, 1);
	table->attach(*m_LocWidget, 0, 2, 1, 2);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
AddTalkDialog::AddTalkDialog(const CharacterMap &chars, const BufferMap &buffers):
		AbstractDialog("add_talk_option") {
	construct(chars, buffers);
}

// get the data
StringTriplet AddTalkDialog::get_data() const {
	return make_triplet<Glib::ustring, Glib::ustring, Glib::ustring>(m_CharCB->get_selected_internal(),
									 m_ViewEntry->get_text(),
									 m_BlockCB->get_selected_internal());
}

// build the dialog
void AddTalkDialog::construct(const CharacterMap &chars, const BufferMap &buffers) {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate combo boxes
	m_CharCB=Gtk::manage(new CharComboBox(chars));
	m_BlockCB=Gtk::manage(new BlockComboBox(buffers));
	
	// allocate labels
	m_CharLabel=Gtk::manage(new Gtk::Label(_("Character")));
	m_ViewLabel=Gtk::manage(new Gtk::Label(_("Display String")));
	m_BlockLabel=Gtk::manage(new Gtk::Label(_("Block")));
	
	// allocate entries
	m_ViewEntry=Gtk::manage(new Gtk::Entry);
	
	// place widgets
	table->attach(*m_CharLabel, 0, 1, 0, 1);
	table->attach(*m_CharCB, 1, 2, 0, 1);
	table->attach(*m_ViewLabel, 0, 1, 1, 2);
	table->attach(*m_ViewEntry, 1, 2, 1, 2);
	table->attach(*m_BlockLabel, 0, 1, 2, 3);
	table->attach(*m_BlockCB, 1, 2, 2, 3);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
RemoveTalkDialog::RemoveTalkDialog(const CharacterMap &chars): AbstractDialog("remove_talk_option") {
	construct(chars);
}

// get data
StringPair RemoveTalkDialog::get_data() const {
	return std::make_pair<Glib::ustring, Glib::ustring>(m_CharCB->get_selected_internal(), m_ViewEntry->get_text());
}

// build the dialog
void RemoveTalkDialog::construct(const CharacterMap &chars) {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_CharLabel=Gtk::manage(new Gtk::Label(_("Character")));
	m_ViewLabel=Gtk::manage(new Gtk::Label(_("Display String")));
	
	// combo boxes
	m_CharCB=Gtk::manage(new CharComboBox(chars));
	
	// entries
	m_ViewEntry=Gtk::manage(new Gtk::Entry);
	
	// place widgets
	table->attach(*m_CharLabel, 0, 1, 0, 1);
	table->attach(*m_CharCB, 1, 2, 0, 1);
	table->attach(*m_ViewLabel, 0, 1, 1, 2);
	table->attach(*m_ViewEntry, 1, 2, 1, 2);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
ClearCharDialog::ClearCharDialog(const Glib::ustring &trigger, const CharacterMap &chars):
		AbstractDialog(trigger) {
	construct(chars);
}

// get the selected character
Glib::ustring ClearCharDialog::get_character() const {
	return m_CharCB->get_selected_internal();
}

// build the dialog
void ClearCharDialog::construct(const CharacterMap &chars) {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate hbox
	Gtk::HBox *hb=Gtk::manage(new Gtk::HBox);
	hb->set_spacing(5);
	
	// allocate label
	m_CharLabel=Gtk::manage(new Gtk::Label(_("Character")));
	
	// allocate combo box
	m_CharCB=Gtk::manage(new CharComboBox(chars));
	
	// pack widgets
	hb->pack_start(*m_CharLabel);
	hb->pack_start(*m_CharCB);
	
	vb->pack_start(*hb, Gtk::PACK_SHRINK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
AddPresentDialog::AddPresentDialog(const CharacterMap &chars, const BufferMap &buffers):
		AbstractDialog("add_presentable") {
	construct(chars, buffers);
}

// get the selected data
StringTriplet AddPresentDialog::get_data() const {
	return make_triplet<Glib::ustring, Glib::ustring, Glib::ustring> (m_CharCB->get_selected_internal(),
									  m_ItemEntry->get_text(),
									  m_BlockCB->get_selected_internal());
}

// build the dialog
void AddPresentDialog::construct(const CharacterMap &chars, const BufferMap &buffers) {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_CharLabel=Gtk::manage(new Gtk::Label(_("Character")));
	m_ItemLabel=Gtk::manage(new Gtk::Label(_("Court Record Item ID")));
	m_BlockLabel=Gtk::manage(new Gtk::Label(_("Target Block")));
	
	// allocate combo boxes
	m_CharCB=Gtk::manage(new CharComboBox(chars));
	m_BlockCB=Gtk::manage(new BlockComboBox(buffers));
	
	// allocate entries
	m_ItemEntry=Gtk::manage(new Gtk::Entry);
	
	// place widgets
	table->attach(*m_CharLabel, 0, 1, 0, 1);
	table->attach(*m_CharCB, 1, 2, 0, 1);
	table->attach(*m_ItemLabel, 0, 1, 1, 2);
	table->attach(*m_ItemEntry, 1, 2, 1, 2);
	table->attach(*m_BlockLabel, 0, 1, 2, 3);
	table->attach(*m_BlockCB, 1, 2, 2, 3);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
RemovePresentDialog::RemovePresentDialog(const CharacterMap &chars):
		AbstractDialog("remove_presentable") {
	construct(chars);
}

// get the selected data
StringPair RemovePresentDialog::get_data() const {
	return std::make_pair<Glib::ustring, Glib::ustring> (m_CharCB->get_selected_internal(),
							     m_ItemEntry->get_text());
}

// build the dialog
void RemovePresentDialog::construct(const CharacterMap &chars) {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_CharLabel=Gtk::manage(new Gtk::Label(_("Character")));
	m_ItemLabel=Gtk::manage(new Gtk::Label(_("Court Record Item ID")));
	
	// allocate combo boxes
	m_CharCB=Gtk::manage(new CharComboBox(chars));
	
	// allocate entries
	m_ItemEntry=Gtk::manage(new Gtk::Entry);
	
	// place widgets
	table->attach(*m_CharLabel, 0, 1, 0, 1);
	table->attach(*m_CharCB, 1, 2, 0, 1);
	table->attach(*m_ItemLabel, 0, 1, 1, 2);
	table->attach(*m_ItemEntry, 1, 2, 1, 2);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
BadPresentDialog::BadPresentDialog(const CharacterMap &chars, const BufferMap &buffers):
		AbstractDialog("set_bad_presentable_block") {
	construct(chars, buffers);
}

// get the selected data
StringPair BadPresentDialog::get_data() const {
	return std::make_pair<Glib::ustring, Glib::ustring> (m_CharCB->get_selected_internal(),
							     m_BlockCB->get_selected_internal());
}

// build the dialog
void BadPresentDialog::construct(const CharacterMap &chars, const BufferMap &buffers) {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_CharLabel=Gtk::manage(new Gtk::Label(_("Character")));
	m_BlockLabel=Gtk::manage(new Gtk::Label(_("Target Block")));
	
	// allocate combo boxes
	m_CharCB=Gtk::manage(new CharComboBox(chars));
	m_BlockCB=Gtk::manage(new BlockComboBox(buffers));
	
	// place widgets
	table->attach(*m_CharLabel, 0, 1, 0, 1);
	table->attach(*m_CharCB, 1, 2, 0, 1);
	table->attach(*m_BlockLabel, 0, 1, 1, 2);
	table->attach(*m_BlockCB, 1, 2, 1, 2);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
LocMusicDialog::LocMusicDialog(const LocationMap &locations, const AudioMap &audio):
		AbstractDialog("set_location_music") {
	construct(locations, audio);
}

// get the selected data
StringPair LocMusicDialog::get_data() const {
	// FIXME: the location widget returns a specific court location; we need to generalize
	// this return value as 'court'
	return std::make_pair<Glib::ustring, Glib::ustring> (m_AudioCB->get_selected_internal(),
							     m_LocWidget->get_selected_location());
}

// build the dialog
void LocMusicDialog::construct(const LocationMap &locations, const AudioMap &audio) {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	Gtk::Label *m_MusicLabel=Gtk::manage(new Gtk::Label(_("Music")));
	
	// allocate location widget
	m_LocWidget=Gtk::manage(new LocationWidget(locations));
	
	// allocate combo boxes
	m_AudioCB=Gtk::manage(new AudioComboBox(audio, AudioComboBox::FILTER_MUSIC));
	
	// place widgets
	table->attach(*m_MusicLabel, 0, 1, 0, 1);
	table->attach(*m_AudioCB, 1, 2, 0, 1);
	table->attach(*Gtk::manage(new Gtk::HSeparator), 0, 2, 1, 2);
	table->attach(*m_LocWidget, 0, 2, 2, 3);
	
	vb->pack_start(*table);
	
	show_all_children();
}

/***************************************************************************/

// constructor
MusicDialog::MusicDialog(const AudioMap &audio):
		AbstractDialog("play_music") {
	construct(audio);
}

// get the selected audio
Glib::ustring MusicDialog::get_audio() const {
	return m_AudioCB->get_selected_internal();
}

// build the dialog
void MusicDialog::construct(const AudioMap &audio) {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	Gtk::Label *m_MusicLabel=Gtk::manage(new Gtk::Label(_("Audio")));
	
	// allocate combo boxes
	m_AudioCB=Gtk::manage(new AudioComboBox(audio, AudioComboBox::FILTER_MUSIC));
	
	// place widgets
	table->attach(*m_MusicLabel, 0, 1, 0, 1);
	table->attach(*m_AudioCB, 1, 2, 0, 1);
	
	vb->pack_start(*table);
	
	show_all_children();
}

/***************************************************************************/

// constructor
ReqEvidenceDialog::ReqEvidenceDialog(const BufferMap &buffers):
		AbstractDialog("request_evidence") {
	construct(buffers);
}

// get the data
StringTriplet ReqEvidenceDialog::get_data() const {
	return make_triplet<Glib::ustring, Glib::ustring, Glib::ustring> (m_EvidenceEntry->get_text(),
									  m_CorrectCB->get_selected_internal(),
									  m_WrongCB->get_selected_internal());
}

// build the dialog
void ReqEvidenceDialog::construct(const BufferMap &buffers) {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate combo boxes
	m_CorrectCB=Gtk::manage(new BlockComboBox(buffers));
	m_WrongCB=Gtk::manage(new BlockComboBox(buffers));
	
	// allocate labels
	m_EvidenceLabel=Gtk::manage(new Gtk::Label(_("Court Record Item ID")));
	m_CorrectLabel=Gtk::manage(new Gtk::Label(_("Target Correct Block")));
	m_WrongLabel=Gtk::manage(new Gtk::Label(_("Target Incorrect Block")));
	
	// allocate entries
	m_EvidenceEntry=Gtk::manage(new Gtk::Entry);
	
	// place widgets
	table->attach(*m_EvidenceLabel, 0, 1, 0, 1);
	table->attach(*m_EvidenceEntry, 1, 2, 0, 1);
	table->attach(*m_CorrectLabel, 0, 1, 1, 2);
	table->attach(*m_CorrectCB, 1, 2, 1, 2);
	table->attach(*m_WrongLabel, 0, 1, 2, 3);
	table->attach(*m_WrongCB, 1, 2, 2, 3);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
ReqAnswerDialog::ReqAnswerDialog(): AbstractDialog("request_answer") {
	construct();
}

// get the talk options
std::vector<StringPair> ReqAnswerDialog::get_talk_options() const {
	std::vector<StringPair> vec;
	
	for (int i=0; i<m_TalkList->size(); i++) {
		Glib::ustring a=m_TalkList->get_text(i, 0);
		Glib::ustring b=m_TalkList->get_text(i, 1);
		
		vec.push_back(std::make_pair<Glib::ustring, Glib::ustring> (a, b));
	}
	
	return vec;
}

// build the dialog
void ReqAnswerDialog::construct() {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate buttons
	m_AddButton=Gtk::manage(new Gtk::Button(_("Add")));
	m_RemoveButton=Gtk::manage(new Gtk::Button(_("Remove")));
	
	// connect signals
	m_AddButton->signal_clicked().connect(sigc::mem_fun(*this, &ReqAnswerDialog::on_add_button_clicked));
	m_RemoveButton->signal_clicked().connect(sigc::mem_fun(*this, &ReqAnswerDialog::on_remove_button_clicked));
	
	// allocate scrolled window
	m_SWindow=Gtk::manage(new Gtk::ScrolledWindow);
	m_SWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_SWindow->set_size_request(200, 250);
	
	// allocate list view
	m_TalkList=Gtk::manage(new Gtk::ListViewText(2, true));
	m_SWindow->add(*m_TalkList);
	
	// configure list
	m_TalkList->set_column_title(0, _("Display String"));
	m_TalkList->set_column_title(1, _("Target Block"));
	
	// button box for buttons
	Gtk::HButtonBox *hbb=Gtk::manage(new Gtk::HButtonBox);
	
	// pack widgets
	hbb->pack_start(*m_AddButton, Gtk::PACK_SHRINK);
	hbb->pack_start(*m_RemoveButton, Gtk::PACK_SHRINK);
	
	table->attach(*hbb, 0, 1, 0, 1);
	table->attach(*m_SWindow, 0, 1, 1, 2);
	
	vb->pack_start(*table);
	
	show_all_children();
}

// add button handler
void ReqAnswerDialog::on_add_button_clicked() {
	// insert a new row
	int i=m_TalkList->append_text(_("New Talk Option"));
	
	// we need to generate a unique id
	int c=1;
	Glib::ustring newID="block0";
	while(1) {
		bool b=true;
		
		// given the id, iterate over items, and see if there is already a similar one
		for (int i=0; i<m_TalkList->size(); i++) {
			if (m_TalkList->get_text(i, 1)==newID) {
				newID="block";
				newID+=Utils::to_string(c++);
				b=false;
			}
		}
		
		if (b)
			break;
	}
	
	m_TalkList->set_text(i, 1, newID);
}

// remove button handler
void ReqAnswerDialog::on_remove_button_clicked() {
	// get our selection
	Glib::ustring selected=m_TalkList->get_text(m_TalkList->get_selected()[0], 1);
	
	// iterate over items
	std::vector<StringPair> items;
	for (int i=0; i<m_TalkList->size(); i++) {
		Glib::ustring id=m_TalkList->get_text(i, 1);
		if (id!=selected)
			items.push_back(std::make_pair<Glib::ustring, Glib::ustring> (m_TalkList->get_text(i, 0), id));
	}
	
	// we're done with this list's current contents
	m_TalkList->clear_items();
	
	// now set the new row contents
	for (int i=0; i<items.size(); i++) {
		int row=m_TalkList->append_text(items[i].first);
		m_TalkList->set_text(row, 1, items[i].second);
	}
}

/***************************************************************************/

// constructor
CourtCamDialog::CourtCamDialog(): AbstractDialog("move_court_camera") {
	construct();
}

// get the data
StringPair CourtCamDialog::get_data() const {
	// convert to actual location id
	Glib::ustring a=m_FromCB->get_active_text();
	if (a==_("Prosecution Stand")) a="prosecutor_stand";
	else if (a==_("Witness Stand")) a="witness_stand";
	else a="defense_stand";
	
	// likewise as above
	Glib::ustring b=m_ToCB->get_active_text();
	if (b==_("Prosecution Stand")) b="prosecutor_stand";
	else if (b==_("Witness Stand")) b="witness_stand";
	else b="defense_stand";
	
	return std::make_pair<Glib::ustring, Glib::ustring> (a, b);
}

// build the dialog
void CourtCamDialog::construct() {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_FromLabel=Gtk::manage(new Gtk::Label(_("From")));
	m_ToLabel=Gtk::manage(new Gtk::Label(_("To")));
	
	// allocate arrow
	m_Arrow=Gtk::manage(new Gtk::Arrow(Gtk::ARROW_RIGHT, Gtk::SHADOW_NONE));
	
	// allocate combo boxes
	m_FromCB=Gtk::manage(new Gtk::ComboBoxText);
	m_ToCB=Gtk::manage(new Gtk::ComboBoxText);
	
	// populate combo boxes
	for (int i=0; i<3; i++) {
		Glib::ustring str;
		switch(i) {
			case 0: str=_("Prosecution Stand"); break;
			case 1: str=_("Witness Stand"); break;
			case 2: str=_("Defense Stand"); break;
		}
		
		m_FromCB->append_text(str);
		m_ToCB->append_text(str);
	}
	
	// set defaults
	m_FromCB->set_active(0);
	m_ToCB->set_active(1);
	
	// place widgets
	table->attach(*m_FromLabel, 0, 1, 0, 1);
	table->attach(*m_ToLabel, 2, 3, 0, 1);
	table->attach(*m_FromCB, 0, 1, 1, 2);
	table->attach(*m_Arrow, 1, 2, 1, 2);
	table->attach(*m_ToCB, 2, 3, 1, 2);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
FadeDialog::FadeDialog(): AbstractDialog("fade_out") {
	construct();
}

// get the selection
Glib::ustring FadeDialog::get_selection() const {
	if (m_TopRB->get_active())
		return "top";
	else if (m_BottomRB->get_active())
		return "bottom";
	else
		return "both";
}

// build the dialog
void FadeDialog::construct() {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	
	// button box
	Gtk::VButtonBox *vbb=Gtk::manage(new Gtk::VButtonBox);
	vbb->set_spacing(5);
	
	// allocate radio button
	m_TopRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Upper Screen")));
	m_BottomRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Lower Screen")));
	m_BothRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Both Screens")));
	
	// pack widgets
	vbb->pack_start(*m_TopRB);
	vbb->pack_start(*m_BottomRB);
	vbb->pack_start(*m_BothRB);
	
	vb->pack_start(*vbb, Gtk::PACK_SHRINK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
OverImageDialog::OverImageDialog(const ImageMap &images): AbstractDialog("set_court_overview_image") {
	construct(images);
}

// get the data
StringPair OverImageDialog::get_data() const {
	Glib::ustring area;
	if (m_ProsecutorRB->get_active())
		area="prosecutor";
	else if (m_DefenseRB->get_active())
		area="defense";
	else
		area="witness";
	
	return std::make_pair<Glib::ustring, Glib::ustring> (area, m_ImgCB->get_selected_internal());
}

// build the dialog
void OverImageDialog::construct(const ImageMap &images) {
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_ImageLabel=Gtk::manage(new Gtk::Label(_("Image")));
	m_PreviewLabel=Gtk::manage(new Gtk::Label(_("Preview")));
	m_LocLabel=Gtk::manage(new Gtk::Label(_("Set Image for Court Location")));
	
	// allocate image combo box
	m_ImgCB=Gtk::manage(new ImgComboBox(images));
	
	// allocate image
	m_Image=Gtk::manage(new Gtk::Image);
	
	// allocate scrolled window for image
	m_SWindow=Gtk::manage(new Gtk::ScrolledWindow);
	m_SWindow->set_size_request(100, 100);
	m_SWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_SWindow->add(*m_Image);
	
	// connect signals
	m_ImgCB->signal_changed().connect(sigc::mem_fun(*this, &OverImageDialog::on_combo_box_changed));
	on_combo_box_changed(); // call initially to set image
	
	// allocate radio buttons
	m_ProsecutorRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Prosecutor Stand")));
	m_DefenseRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Defense Stand")));
	m_WitnessRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Witness Stand")));
	m_JudgeRB=Gtk::manage(new Gtk::RadioButton(m_Group, _("Judge's Stand")));
	
	// place widgets
	table->attach(*m_ImageLabel, 0, 1, 0, 1);
	table->attach(*m_ImgCB, 1, 2, 0, 1);
	table->attach(*m_PreviewLabel, 0, 1, 1, 2);
	table->attach(*m_SWindow, 1, 2, 1, 2);
	table->attach(*Gtk::manage(new Gtk::HSeparator), 0, 2, 2, 3);
	table->attach(*m_LocLabel, 0, 1, 3, 4);
	table->attach(*m_ProsecutorRB, 1, 2, 3, 4);
	table->attach(*m_DefenseRB, 1, 2, 4, 5);
	table->attach(*m_WitnessRB, 1, 2, 5, 6);
	table->attach(*m_JudgeRB, 1, 2, 6, 7);
	
	get_vbox()->pack_start(*table, Gtk::PACK_SHRINK);
	
	show_all_children();
}

// handler for combo box selection changes
void OverImageDialog::on_combo_box_changed() {
	m_Image->set(m_ImgCB->get_selected_image()->pixbuf);
}

/***************************************************************************/

// constructor
TempImgDialog::TempImgDialog(const ImageMap &map): AbstractDialog("set_temp_image") {
	construct(map);
}

// get the selected image
Glib::ustring TempImgDialog::get_image() const {
	return m_ImgCB->get_selected_internal();
}

// build the dialog
void TempImgDialog::construct(const ImageMap &map) {
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_ImgLabel=Gtk::manage(new Gtk::Label(_("Image")));
	m_PreviewLabel=Gtk::manage(new Gtk::Label(_("Preview")));
	
	// allocate image combo box
	m_ImgCB=Gtk::manage(new ImgComboBox(map));
	m_ImgCB->signal_changed().connect(sigc::mem_fun(*this, &TempImgDialog::on_combo_box_changed));
	
	// allocate image
	m_Image=Gtk::manage(new Gtk::Image);
	
	// allocate scrolled window for image
	m_SWindow=Gtk::manage(new Gtk::ScrolledWindow);
	m_SWindow->set_size_request(100, 100);
	m_SWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_SWindow->add(*m_Image);
	
	on_combo_box_changed();
	
	// pack widgets
	table->attach(*m_ImgLabel, 0, 1, 0, 1);
	table->attach(*m_ImgCB, 1, 2, 0, 1);
	table->attach(*m_PreviewLabel, 0, 1, 1, 2);
	table->attach(*m_SWindow, 1, 2, 1, 2);
	
	get_vbox()->pack_start(*table, Gtk::PACK_SHRINK);
	
	show_all_children();
}

// handler for combo box selection changes
void TempImgDialog::on_combo_box_changed() {
	m_Image->set(m_ImgCB->get_selected_image()->pixbuf);
}

/***************************************************************************/

// constructor
DisplayTestimonyDialog::DisplayTestimonyDialog(const TestimonyMap &map):
		AbstractDialog("display_testimony") {
	construct(map);
}

// get testimony selected
Glib::ustring DisplayTestimonyDialog::get_testimony() const {
	return m_TestimonyCB->get_active_text();
}

// build the dialog
void DisplayTestimonyDialog::construct(const TestimonyMap &map) {
	// allocate hbox
	Gtk::HBox *hb=Gtk::manage(new Gtk::HBox);
	hb->set_spacing(5);
	
	// allocate labels
	m_TestimonyLabel=Gtk::manage(new Gtk::Label(_("Testimony to Display")));
	
	// allocate combo box
	m_TestimonyCB=Gtk::manage(new Gtk::ComboBoxText);
	for (TestimonyMap::const_iterator it=map.begin(); it!=map.end(); ++it)
		m_TestimonyCB->append_text((*it).first);
	m_TestimonyCB->set_active(0);
	
	// pack widgets
	hb->pack_start(*m_TestimonyLabel);
	hb->pack_start(*m_TestimonyCB);
	
	get_vbox()->pack_start(*hb, Gtk::PACK_SHRINK);
	
	show_all_children();
}

/***************************************************************************/

// constructor
EditRecItemDialog::EditRecItemDialog(const CharacterMap &chars): AbstractDialog("change_character_*") {
	m_Mode=MODE_CHARACTER;
	
	construct(chars);
}

// constructor
EditRecItemDialog::EditRecItemDialog(const EvidenceMap &evidence): AbstractDialog("change_evidence_*") {
	m_Mode=MODE_EVIDENCE;
	
	construct(evidence);
}

// get the inputted data
EditRecItemDialog::Data EditRecItemDialog::get_data() const {
	Data data;
	
	data.name=(m_NameCB->get_active() ? m_NameEntry->get_text() : "null");
	data.caption=(m_CaptionCB->get_active() ? m_CaptionEntry->get_text() : "null");
	data.desc=(m_DescCB->get_active() ? m_DescEntry->get_text() : "null");
	
	if (m_Mode==MODE_CHARACTER) {
		data.id=m_CharCB->get_selected_internal();
		
		if (m_CGenderCB->get_active()) {
			// make sure to bring the string to lowercase
			char *str=const_cast<char*> (m_GenderCB->get_active_text().c_str());
			str[0]=tolower(str[0]);
			data.gender=str;
		}
		else
			data.gender="null";
	}
	
	else
		data.id=m_EvidenceCB->get_selected_internal();
	
	return data;
}

// build the dialog
void EditRecItemDialog::construct(const CharacterMap &chars) {
	// allocate labels
	m_ItemLabel=manage(new Gtk::Label(_("Character")));
	
	// allocate combo boxes
	m_CharCB=manage(new CharComboBox(chars));
	m_GenderCB=manage(new Gtk::ComboBoxText);
	
	// allocate gender check button
	m_CGenderCB=manage(new Gtk::CheckButton(_("Gender")));
	
	// append text to the combo box
	m_GenderCB->append_text(_("Male"));
	m_GenderCB->append_text(_("Female"));
	m_GenderCB->append_text(_("Unknown"));
	m_GenderCB->set_active(0);
	
	construct_internal();
}

// build the dialog with evidence map
void EditRecItemDialog::construct(const EvidenceMap &evidence) {
	// allocate labels
	m_ItemLabel=manage(new Gtk::Label(_("Evidence")));
	
	// allocate combo boxes
	m_EvidenceCB=manage(new EvidenceComboBox(evidence));
	
	construct_internal();
}

// build the other portions of the dialog
void EditRecItemDialog::construct_internal() {
	// allocate table
	Gtk::Table *m_Table=manage(new Gtk::Table);
	m_Table->set_spacings(5);
	
	// allocate entries
	m_NameEntry=manage(new Gtk::Entry);
	m_CaptionEntry=manage(new Gtk::Entry);
	m_DescEntry=manage(new Gtk::Entry);
	
	// allocate check buttons
	m_NameCB=manage(new Gtk::CheckButton(_("Name")));
	m_CaptionCB=manage(new Gtk::CheckButton(_("Caption")));
	m_DescCB=manage(new Gtk::CheckButton(_("Description")));
	
	// keep proper alignment in case of missing widgets
	int spacer=(m_Mode==MODE_EVIDENCE ? 1 : 0);
	
	// place widgets
	m_Table->attach(*m_ItemLabel, 0, 1, 0, 1);
	
	// get a pointer to the right combo box
	Gtk::Widget *cb;
	if (m_Mode==MODE_CHARACTER)
		cb=(Gtk::Widget*) m_CharCB;
	else
		cb=(Gtk::Widget*) m_EvidenceCB;
	
	m_Table->attach(*cb, 1, 2, 0, 1);
	m_Table->attach(*m_NameCB, 0, 1, 1, 2);
	m_Table->attach(*m_NameEntry, 1, 2, 1, 2);
	
	if (m_Mode==MODE_CHARACTER) {
		m_Table->attach(*m_CGenderCB, 0, 1, 2, 3);
		m_Table->attach(*m_GenderCB, 1, 2, 2, 3);
	}
	
	m_Table->attach(*m_CaptionCB, 0, 1, 3-spacer, 4-spacer);
	m_Table->attach(*m_CaptionEntry, 1, 2, 3-spacer, 4-spacer);
	m_Table->attach(*m_DescCB, 0, 1, 4-spacer, 5-spacer);
	m_Table->attach(*m_DescEntry, 1, 2, 4-spacer, 5-spacer);
	
	get_vbox()->pack_start(*m_Table, Gtk::PACK_SHRINK);
	
	// connect signals for check buttons
	m_NameCB->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &EditRecItemDialog::on_button_toggled), "m_NameCB"));
	m_CaptionCB->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &EditRecItemDialog::on_button_toggled), "m_CaptionCB"));
	m_DescCB->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &EditRecItemDialog::on_button_toggled), "m_DescCB"));
	
	// disable check buttons by default
	m_NameCB->set_active(false);
	m_CaptionCB->set_active(false);
	m_DescCB->set_active(false);
	
	// make our changes take effect
	on_button_toggled("m_NameCB");
	on_button_toggled("m_CaptionCB");
	on_button_toggled("m_DescCB");
	
	// deal with certain widgets only in certain modes
	if (m_Mode==MODE_CHARACTER) {
		// connect gender combo box signal
		m_CGenderCB->signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &EditRecItemDialog::on_button_toggled), "m_CGenderCB"));
		m_CGenderCB->set_active(false);
		on_button_toggled("m_CGenderCB");
		
		// connect character combo box signal
		m_CharCB->signal_changed().connect(sigc::mem_fun(*this, &EditRecItemDialog::on_combo_box_changed));
		m_CharCB->set_active(0);
	}
	
	else {
		// connect evidence combo box signal
		m_EvidenceCB->signal_changed().connect(sigc::mem_fun(*this, &EditRecItemDialog::on_combo_box_changed));
		m_EvidenceCB->set_active(0);
	}
	
	on_combo_box_changed();
	
	show_all_children();
}

// character combo box changed
void EditRecItemDialog::on_combo_box_changed() {
	if (m_Mode==MODE_CHARACTER) {
		Character *c=m_CharCB->get_selected_character();
		
		// update the fields
		m_NameEntry->set_text(c->get_name());
		m_GenderCB->set_active(c->get_gender());
		m_CaptionEntry->set_text(c->get_caption());
		m_DescEntry->set_text(c->get_description());
	}
	
	else {
		Case::Evidence *e=m_EvidenceCB->get_selected_evidence();
		
		// update the fields
		m_NameEntry->set_text(e->name);
		m_CaptionEntry->set_text(e->caption);
		m_DescEntry->set_text(e->description);
	}
}

// check button toggle handler
void EditRecItemDialog::on_button_toggled(const Glib::ustring &button) {
	if (button=="m_NameCB")
		m_NameEntry->set_sensitive(m_NameCB->get_active());
	else if (button=="m_CGenderCB")
		m_GenderCB->set_sensitive(m_CGenderCB->get_active());
	else if (button=="m_CaptionCB")
		m_CaptionEntry->set_sensitive(m_CaptionCB->get_active());
	else if (button=="m_DescCB")
		m_DescEntry->set_sensitive(m_DescCB->get_active());
	else
		g_error("Unrecognized button ID sent in signal: %s", button.c_str());
}
