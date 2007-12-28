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
// testimonyeditor.cpp: the TestimonyEditor class

#include <gtkmm/buttonbox.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/table.h>
#include <sstream>

#include "testimonyeditor.h"
#include "textboxdialog.h"
#include "tooltips.h"

// constructor
TestimonyEditor::TestimonyEditor(const CharacterMap &chars, const LocationMap &locations, 
				 const BufferMap &buffers, const StringVector &testimonyIds) {
	construct(chars, locations, buffers);
	
	// add a default piece
	m_Testimony.pieces.push_back(create_testimony_piece());
	
	// set default values
	m_CurPiece=0;
	m_Ids=testimonyIds;
	
	update();
}

// set a testimony to use
void TestimonyEditor::set_testimony(const Case::Testimony &testimony) {
	// copy the testimony struct
	m_Testimony=testimony;
	
	// set entry values
	m_IdEntry->set_text(testimony.id);
	m_TitleEntry->set_text(testimony.title);
	m_SpeakerCB->set_active_internal(testimony.speaker);
	m_NextBlockCB->set_active_text(testimony.nextBlock);
	m_FollowLocCB->set_active_internal(testimony.followLoc);
	m_XExamineCB->set_active_text(testimony.xExamineEndBlock);
	
	// since we set a testimony, that must mean we're editing it
	// so disable the id entry
	m_IdEntry->set_sensitive(false);
	
	// and enable ok button
	m_OKButton->set_sensitive(true);
	
	// update the dialog
	update();
}

// return the current testimony data
Case::Testimony TestimonyEditor::get_testimony_data() {
	// serialize values
	m_Testimony.id=m_IdEntry->get_text();
	m_Testimony.title=m_TitleEntry->get_text();
	m_Testimony.speaker=m_SpeakerCB->get_selected_internal();
	m_Testimony.nextBlock=m_NextBlockCB->get_active_text();
	m_Testimony.followLoc=m_FollowLocCB->get_selected_internal();
	m_Testimony.xExamineEndBlock=m_XExamineCB->get_selected_internal();
	
	return m_Testimony;
}

// build the ui
void TestimonyEditor::construct(const CharacterMap &chars, const LocationMap &locations, const BufferMap &buffers) {
	// get vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate layout table
	Gtk::Table *table=manage(new Gtk::Table);
	Gtk::Table *fTable=manage(new Gtk::Table);
	table->set_spacings(5);
	
	fTable->set_spacings(5);
	fTable->set_border_width(10);
	
	// allocate buttons
	m_FormatTitleButton=manage(new Gtk::Button("Format"));
	m_PrevButton=manage(new Gtk::Button("<<"));
	m_NextButton=manage(new Gtk::Button(">>"));
	m_AppendButton=manage(new Gtk::Button("Append"));
	m_InsertButton=manage(new Gtk::Button("Insert"));
	m_DeleteButton=manage(new Gtk::Button("Delete"));
	m_AmendButton=manage(new Gtk::Button("Amend"));
	
	// connect signals
	m_FormatTitleButton->signal_clicked().connect(sigc::mem_fun(*this, &TestimonyEditor::on_format_title_button_clicked));
	m_PrevButton->signal_clicked().connect(sigc::mem_fun(*this, &TestimonyEditor::on_prev_button_clicked));
	m_NextButton->signal_clicked().connect(sigc::mem_fun(*this, &TestimonyEditor::on_next_button_clicked));
	m_AppendButton->signal_clicked().connect(sigc::mem_fun(*this, &TestimonyEditor::on_append_button_clicked));
	m_InsertButton->signal_clicked().connect(sigc::mem_fun(*this, &TestimonyEditor::on_insert_button_clicked));
	m_DeleteButton->signal_clicked().connect(sigc::mem_fun(*this, &TestimonyEditor::on_delete_button_clicked));
	m_AmendButton->signal_clicked().connect(sigc::mem_fun(*this, &TestimonyEditor::on_amend_button_clicked));
	
	// allocate labels
	m_IdLabel=manage(new Gtk::Label("Testimony ID"));
	m_TitleLabel=manage(new Gtk::Label("Title"));
	m_SpeakerLabel=manage(new Gtk::Label("Speaker"));
	m_NextBlockLabel=manage(new Gtk::Label("Testimony End Block"));
	m_FollowLocLabel=manage(new Gtk::Label("Testimony End Location"));
	m_XExamineBlockLabel=manage(new Gtk::Label("Cross Examination End Block"));
	m_PieceLabel=manage(new Gtk::Label("1/1"));
	m_PressLabel=manage(new Gtk::Label("Witness Pressed"));
	
	// allocate entries
	m_IdEntry=manage(new Gtk::Entry);
	m_TitleEntry=manage(new Gtk::Entry);
	m_PresentIdEntry=manage(new Gtk::Entry);
	
	// set tooltips for entries
	m_IdEntry->set_tooltip_text(Tooltips::lookup("TestimonyEditor::m_IdEntry"));
	m_TitleEntry->set_tooltip_text(Tooltips::lookup("TestimonyEditor::m_TitleEntry"));
	
	// allocate combo boxes
	m_SpeakerCB=manage(new CharComboBox(chars));
	m_PresentTargetCB=manage(new BlockComboBox(buffers));
	m_PressCB=manage(new BlockComboBox(buffers));
	m_XExamineCB=manage(new BlockComboBox(buffers));
	m_NextBlockCB=manage(new BlockComboBox(buffers));
	m_FollowLocCB=manage(new LocationComboBox(locations));
	
	// connect signals
	m_IdEntry->signal_changed().connect(sigc::mem_fun(*this, &TestimonyEditor::on_id_entry_changed));
	
	// allocate check buttons
	m_HiddenCB=manage(new Gtk::CheckButton("Piece is Hidden"));
	m_PresentLabelCB=manage(new Gtk::CheckButton("Evidence Presented"));
	
	// connect signals
	m_PresentLabelCB->signal_toggled().connect(sigc::mem_fun(*this, &TestimonyEditor::on_present_toggled));
	
	// allocate text view
	m_TextView=manage(new Gtk::TextView);
	
	// connect signals
	m_TextView->signal_populate_popup().connect(sigc::mem_fun(*this, &TestimonyEditor::on_text_view_populate_menu));
	
	// allocate containers
	m_PieceFrame=manage(new Gtk::Frame("Testimony Piece"));
	
	m_SWindow=manage(new Gtk::ScrolledWindow);
	m_SWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
	m_SWindow->set_size_request(200, 200);
	m_SWindow->add(*m_TextView);
	
	// hbox and table for testimony data
	Gtk::HBox *thb=manage(new Gtk::HBox);
	Gtk::Table *ttable=manage(new Gtk::Table);
	ttable->set_spacings(5);
	
	// hbox for testimony piece control
	Gtk::HBox *hb=manage(new Gtk::HBox);
	hb->set_spacing(5);
	
	// vbox for frame
	Gtk::VBox *fvb=manage(new Gtk::VBox);
	fvb->set_spacing(5);
	
	// pack widgets
	hb->pack_start(*m_PrevButton, Gtk::PACK_SHRINK);
	hb->pack_start(*m_PieceLabel);
	hb->pack_start(*m_NextButton, Gtk::PACK_SHRINK);
	
	// hbuttonbox for buttons
	Gtk::HButtonBox *buttons=manage(new Gtk::HButtonBox);
	buttons->set_layout(Gtk::BUTTONBOX_SPREAD);
	
	buttons->pack_start(*m_InsertButton);
	buttons->pack_start(*m_AppendButton);
	buttons->pack_start(*m_DeleteButton);
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// place frame widgets
	fTable->attach(*m_SWindow, 0, 3, 0, 1, xops, yops);
	fTable->attach(*m_HiddenCB, 0, 3, 1, 2, xops, yops);
	
	// allocate table for testimony piece widgets
	Gtk::Table *tpTable=manage(new Gtk::Table);
	tpTable->set_spacings(5);
	tpTable->set_homogeneous(true);
	
	// place testimony piece widgets
	tpTable->attach(*m_PresentLabelCB, 0, 1, 0, 1, xops, yops);
	tpTable->attach(*m_PresentIdEntry, 1, 2, 0, 1, xops, yops);
	tpTable->attach(*m_PresentTargetCB, 2, 3, 0, 1, xops, yops);
	tpTable->attach(*m_PressLabel, 0, 1, 1, 2, xops, yops);
	tpTable->attach(*m_PressCB, 1, 2, 1, 2, xops, yops);
	tpTable->attach(*m_AmendButton, 2, 3, 1, 2, xops, yops);
	
	fTable->attach(*tpTable, 0, 3, 2, 3, xops, yops);
	
	// pack widgets
	fvb->pack_start(*hb);
	fvb->pack_start(*buttons);
	fvb->pack_start(*fTable);
	
	// add vbox to frame
	m_PieceFrame->add(*fvb);
	
	// place widgets
	ttable->attach(*m_IdLabel, 0, 1, 0, 1, xops, yops);
	ttable->attach(*m_IdEntry, 1, 2, 0, 1, xops, yops);
	ttable->attach(*m_TitleLabel, 0, 1, 1, 2, xops, yops);
	ttable->attach(*m_TitleEntry, 1, 2, 1, 2, xops, yops);
	ttable->attach(*m_FormatTitleButton, 2, 3, 1, 2, xops, yops);
	ttable->attach(*m_SpeakerLabel, 0, 1, 2, 3, xops, yops);
	ttable->attach(*m_SpeakerCB, 1, 2, 2, 3, xops, yops);
	ttable->attach(*m_NextBlockLabel, 0, 1, 3, 4, xops, yops);
	ttable->attach(*m_NextBlockCB, 1, 2, 3, 4, xops, yops);
	ttable->attach(*m_FollowLocLabel, 0, 1, 4, 5, xops, yops);
	ttable->attach(*m_FollowLocCB, 1, 2, 4, 5, xops, yops);
	ttable->attach(*m_XExamineBlockLabel, 0, 1, 5, 6, xops, yops);
	ttable->attach(*m_XExamineCB, 1, 2, 5, 6, xops, yops);
	
	thb->pack_start(*ttable, Gtk::PACK_SHRINK);
	
	table->attach(*thb, 0, 3, 0, 1, xops, yops);
	table->attach(*m_PieceFrame, 0, 3, 1, 2, xops, yops);
	
	vb->pack_start(*table);
	
	// add buttons
	m_OKButton=add_button("OK", Gtk::RESPONSE_OK);
	add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	m_OKButton->set_sensitive(false);
	
	show_all_children();
}

// create a default testimony piece
Case::TestimonyPiece TestimonyEditor::create_testimony_piece() {
	Case::TestimonyPiece piece;
	
	// set default values
	piece.presentId="null";
	piece.presentBlock="null";
	piece.pressBlock="null";
	piece.hidden=false;
	
	return piece;
}

// update the dialog state
void TestimonyEditor::update() {
	// update piece label
	std::stringstream ss;
	ss << m_CurPiece+1 << "/" << m_Testimony.pieces.size();
	
	m_PieceLabel->set_text(ss.str());
	
	// get the current piece
	Case::TestimonyPiece piece=m_Testimony.pieces[m_CurPiece];
	
	// update other entries
	m_PresentIdEntry->set_text(piece.presentId);
	
	// we need to toggle certain widgets
	bool b=piece.presentBlock=="null";
	if (b)
		m_PresentLabelCB->set_active(false);
	else {
		m_PresentLabelCB->set_active(true);
		m_PresentTargetCB->set_active_text(piece.presentBlock);
	}
	
	on_present_toggled();
	
	m_PressCB->set_active_text(piece.pressBlock);
	m_HiddenCB->set_active(piece.hidden);
	
	// set text in buffer
	m_TextView->get_buffer()->set_text(piece.text);
}

// format title button click handler
void TestimonyEditor::on_format_title_button_clicked() {
	// prepare text box dialog formatted with testimony title
	TextBoxDialog tbd;
	tbd.get_editor()->set_format(TextBoxEditor::FORMAT_TESTIMONY_TITLE);
	
	// run the dialog, and insert text into entry
	if (tbd.run()==Gtk::RESPONSE_OK) {
		Glib::ustring text=tbd.get_text();
		
		// remove useless characters
		Glib::ustring reformatted="";
		for (int i=0; i<text.size(); i++) {
			if (text[i]!='\n')
				reformatted+=text[i];
		}
		
		// now set the text
		m_TitleEntry->set_text(reformatted);
	}
}

// previous piece button click handler
void TestimonyEditor::on_prev_button_clicked() {
	if (m_CurPiece==0)
		return;
	
	// go back a testimony piece
	m_CurPiece--;
	
	update();
}

// next piece button click handler
void TestimonyEditor::on_next_button_clicked() {
	if (m_CurPiece+1>m_Testimony.pieces.size()-1)
		return;
	
	// go forward a testimony piece
	m_CurPiece++;
	
	update();
}

// append piece button click handler
void TestimonyEditor::on_append_button_clicked() {
	// create a new testimony piece first
	Case::TestimonyPiece piece=create_testimony_piece();
	
	// add it to the end of the vector
	m_Testimony.pieces.push_back(piece);
	
	update();
}

// insert piece button click handler
void TestimonyEditor::on_insert_button_clicked() {
	// replacement vector
	std::vector<Case::TestimonyPiece> newVec;
	
	// iterate over elements
	for (int i=0; i<m_Testimony.pieces.size(); i++) {
		newVec.push_back(m_Testimony.pieces[i]);
		
		// if the index matches the current piece, add a new piece after
		if (i==m_CurPiece)
			newVec.push_back(create_testimony_piece());
	}
	
	// replace vector
	m_Testimony.pieces=newVec;
	
	update();
}

// delete piece button click handler
void TestimonyEditor::on_delete_button_clicked() {
	// we need at least one piece in this testimony
	if (m_Testimony.pieces.size()==1) {
		Gtk::MessageDialog md("A testimony must have at least one piece!", false, Gtk::MESSAGE_ERROR);
		md.run();
		
		return;
	}
	
	// this is our new pieces vector
	std::vector<Case::TestimonyPiece> newVec;
	
	// iterate over elements
	for (int i=0; i<m_Testimony.pieces.size(); i++) {
		// if our current piece matches the index, skip it
		if (i!=m_CurPiece)
			newVec.push_back(m_Testimony.pieces[i]);
	}
	
	// update the internal vector
	m_Testimony.pieces=newVec;
	
	// reset counter
	m_CurPiece=0;
	
	update();
}

// amend button click handler
void TestimonyEditor::on_amend_button_clicked() {
	Case::TestimonyPiece &piece=m_Testimony.pieces[m_CurPiece];
	
	// serialize all values
	piece.text=m_TextView->get_buffer()->get_text();
	piece.presentId=(m_PresentLabelCB->get_active() ? "null" : m_PresentIdEntry->get_text());
	piece.presentBlock=m_PresentTargetCB->get_selected_internal();
	piece.pressBlock=m_PressCB->get_selected_internal();
	piece.hidden=m_HiddenCB->get_active();
}

// id entry change handler
void TestimonyEditor::on_id_entry_changed() {
	// compare the given id to present ones
	Glib::ustring id=m_IdEntry->get_text();
	
	bool found=false;
	
	// id can't have certain characters
	if (id.empty() || id[0]==' ')
		found=true;
	
	// iterate over ids
	for (int i=0; i<m_Ids.size(); i++) {
		if (id==m_Ids[i])
			found=true;
	}
	
	// update ok button
	m_OKButton->set_sensitive(!found);
}

// handler for populate signal from text view
void TestimonyEditor::on_text_view_populate_menu(Gtk::Menu *menu) {
	Gtk::Menu::MenuList &list=menu->items();
	
	// add more items to popup
	list.push_back(Gtk::Menu_Helpers::SeparatorElem());
	list.push_back(Gtk::Menu_Helpers::MenuElem("Insert Dialogue", 
		       sigc::mem_fun(*this, &TestimonyEditor::on_list_button_pressed)));
}

// present check button toggled
void TestimonyEditor::on_present_toggled() {
	bool b=m_PresentLabelCB->get_active();
	m_PresentIdEntry->set_sensitive(b);
	m_PresentTargetCB->set_sensitive(b);
}

// handler for right clicks on list
void TestimonyEditor::on_list_button_pressed() {
	// run text box dialog
	TextBoxDialog tbd;
	if (tbd.run()==Gtk::RESPONSE_OK) {
		Glib::ustring text=tbd.get_text();
		
		// insert this text into the testimony
		m_TextView->get_buffer()->insert_at_cursor(text);
	}
}
