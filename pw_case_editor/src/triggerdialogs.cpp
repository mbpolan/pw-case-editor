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
	construct(ev, chars);
}

// get the selected evidence or profile
AddCourtRecDialog::Data AddCourtRecDialog::get_data() const {
	AddCourtRecDialog::Data data;
	
	data.type=(m_NB->get_current_page()==0 ? TYPE_ADD_EVIDENCE : TYPE_ADD_PROFILE);
	
	if (data.type==TYPE_ADD_EVIDENCE)
		data.id=m_EvidenceCB->get_selected_internal();
	
	else
		data.id=m_ProfileCB->get_selected_internal();
	
	return data;
}

// build the dialog
void AddCourtRecDialog::construct(const EvidenceMap &ev, const CharacterMap &chars) {
	Gtk::VBox *vb=get_vbox();
	
	// allocate notebook
	m_NB=manage(new Gtk::Notebook);
	
	// add pages
	m_NB->append_page(*build_evidence_page(ev), "Evidence");
	m_NB->append_page(*build_profile_page(chars), "Profile");
	
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
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_EvidenceLabel=manage(new Gtk::Label("Evidence"));
	m_EvidencePreviewLabel=manage(new Gtk::Label("Preview"));
	
	// allocate combobox
	m_EvidenceCB=manage(new EvidenceComboBox(ev));
	
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
Gtk::Table* AddCourtRecDialog::build_profile_page(const CharacterMap &chars) {
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_ProfileLabel=manage(new Gtk::Label("Profile"));
	m_ProfilePreviewLabel=manage(new Gtk::Label("Preview"));
	
	// allocate combobox
	m_ProfileCB=manage(new CharComboBox(chars));
	
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
	Gtk::HBox *hb=manage(new Gtk::HBox);
	hb->set_spacing(5);
	
	// allocate labels
	m_SpeakerLabel=manage(new Gtk::Label("Speaker"));
	
	// allocate combo box
	m_CharsCB=manage(new CharComboBox(chars));
	
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
	m_BlockCB=manage(new BlockComboBox(blocks));
	
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
	m_EvidenceCB=manage(new EvidenceComboBox(ev));
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
	m_LocWidget=manage(new LocationWidget(locations));
	
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
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_TargetLabel=manage(new Gtk::Label("Add"));
	m_LocationLabel=manage(new Gtk::Label("To"));
	
	// allocate combo boxes
	m_LocationCB=manage(new LocationComboBox(locations));
	m_TargetCB=manage(new LocationComboBox(locations));
	
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
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_LocLabel=manage(new Gtk::Label("Location"));
	m_BlockLabel=manage(new Gtk::Label("Block Trigger"));
	
	// allocate combo boxes
	m_LocationCB=manage(new LocationComboBox(locations));
	m_BlockCB=manage(new BlockComboBox(buffers));
	
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
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_CharLabel=manage(new Gtk::Label("Character"));
	m_AnimLabel=manage(new Gtk::Label("Animation"));
	
	// allocate entries
	m_AnimEntry=manage(new Gtk::Entry);
	
	// allocate combo box
	m_CharCB=manage(new CharComboBox(chars));
	
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
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate location widget
	m_LocWidget=manage(new LocationWidget(locations));
	
	// allocate labels
	m_CharLabel=manage(new Gtk::Label("Character"));
	
	// allocat combo boxes
	m_CharCB=manage(new CharComboBox(chars));
	
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
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate combo boxes
	m_CharCB=manage(new CharComboBox(chars));
	m_BlockCB=manage(new BlockComboBox(buffers));
	
	// allocate labels
	m_CharLabel=manage(new Gtk::Label("Character"));
	m_ViewLabel=manage(new Gtk::Label("Display String"));
	m_BlockLabel=manage(new Gtk::Label("Block"));
	
	// allocate entries
	m_ViewEntry=manage(new Gtk::Entry);
	
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
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_CharLabel=manage(new Gtk::Label("Character"));
	m_ViewLabel=manage(new Gtk::Label("Display String"));
	
	// combo boxes
	m_CharCB=manage(new CharComboBox(chars));
	
	// entries
	m_ViewEntry=manage(new Gtk::Entry);
	
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
	Gtk::HBox *hb=manage(new Gtk::HBox);
	hb->set_spacing(5);
	
	// allocate label
	m_CharLabel=manage(new Gtk::Label("Character"));
	
	// allocate combo box
	m_CharCB=manage(new CharComboBox(chars));
	
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
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_CharLabel=manage(new Gtk::Label("Character"));
	m_ItemLabel=manage(new Gtk::Label("Court Record Item ID"));
	m_BlockLabel=manage(new Gtk::Label("Target Block"));
	
	// allocate combo boxes
	m_CharCB=manage(new CharComboBox(chars));
	m_BlockCB=manage(new BlockComboBox(buffers));
	
	// allocate entries
	m_ItemEntry=manage(new Gtk::Entry);
	
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
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_CharLabel=manage(new Gtk::Label("Character"));
	m_ItemLabel=manage(new Gtk::Label("Court Record Item ID"));
	
	// allocate combo boxes
	m_CharCB=manage(new CharComboBox(chars));
	
	// allocate entries
	m_ItemEntry=manage(new Gtk::Entry);
	
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
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_CharLabel=manage(new Gtk::Label("Character"));
	m_BlockLabel=manage(new Gtk::Label("Target Block"));
	
	// allocate combo boxes
	m_CharCB=manage(new CharComboBox(chars));
	m_BlockCB=manage(new BlockComboBox(buffers));
	
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
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	Gtk::Label *m_MusicLabel=manage(new Gtk::Label("Music"));
	
	// allocate location widget
	m_LocWidget=manage(new LocationWidget(locations));
	
	// allocate combo boxes
	m_AudioCB=manage(new AudioComboBox(audio, AudioComboBox::FILTER_MUSIC));
	
	// place widgets
	table->attach(*m_MusicLabel, 0, 1, 0, 1);
	table->attach(*m_AudioCB, 1, 2, 0, 1);
	table->attach(*manage(new Gtk::HSeparator), 0, 2, 1, 2);
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
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	Gtk::Label *m_MusicLabel=manage(new Gtk::Label("Audio"));
	
	// allocate combo boxes
	m_AudioCB=manage(new AudioComboBox(audio, AudioComboBox::FILTER_MUSIC));
	
	// place widgets
	table->attach(*m_MusicLabel, 0, 1, 0, 1);
	table->attach(*m_AudioCB, 1, 2, 0, 1);
	
	vb->pack_start(*table);
	
	show_all_children();
}
