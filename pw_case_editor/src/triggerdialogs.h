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
// triggerdialogs.h: various dialogs for inserting triggers into script

#ifndef TRIGGERDIALOGS_H
#define TRIGGERDIALOGS_H

#include <gtkmm/comboboxtext.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/image.h>
#include <gtkmm/notebook.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/table.h>
#include <gtkmm/textview.h>

#include "case.h"
#include "casecombobox.h"
#include "locationwidget.h"
#include "triplet.h"

// abstract dialog for inserting triggers
class AbstractDialog: public Gtk::Dialog {
	public:
		// constructor
		AbstractDialog(const Glib::ustring &trigger);
		
	protected:
		// handler for ok button clicks
		virtual void on_ok_button_clicked() { };
		
		// handler for cancel button clicks
		virtual void on_cancel_button_clicked() { };
		
		// labels
		Gtk::Label *m_TriggerLabel;
};

/***************************************************************************/

// add_evidence/add_profile triggers
class AddCourtRecDialog: public AbstractDialog {
	public:
		// type of trigger
		enum Type { TYPE_ADD_EVIDENCE, TYPE_ADD_PROFILE };
		
		// data representing selection
		struct _Data {
			Type type;
			Glib::ustring id;
		};
		typedef struct _Data Data;
		
		// constructor
		AddCourtRecDialog(const EvidenceMap &ev, const CharacterMap &chars);
		
		// get the selected evidence or profile
		Data get_data() const;
		
	protected:
		// build the dialog
		void construct(const EvidenceMap &ev, const CharacterMap &chars);
		
		// handler for combo box changes
		void on_combo_box_changed(const Glib::ustring &id);
		
		// build the add_evidence page
		Gtk::Table* build_evidence_page(const EvidenceMap &ev);
		
		// build the add_profile page
		Gtk::Table* build_profile_page(const CharacterMap &chars);
		
		// notebook container
		Gtk::Notebook *m_NB;
		
		// labels
		Gtk::Label *m_EvidenceLabel;
		Gtk::Label *m_ProfileLabel;
		Gtk::Label *m_EvidencePreviewLabel;
		Gtk::Label *m_ProfilePreviewLabel;
		
		// combo boxes
		EvidenceComboBox *m_EvidenceCB;
		CharComboBox *m_ProfileCB;
		
		// images
		Gtk::Image *m_EvidenceImage;
		Gtk::Image *m_ProfileImage;
};

/***************************************************************************/

// speaker trigger
class SpeakerDialog: public AbstractDialog {
	public:
		// constructor
		SpeakerDialog(const CharacterMap &chars);
		
		// get the selected speaker
		Glib::ustring get_speaker();
		
	protected:
		// build the dialog
		void construct(const CharacterMap &chars);
		
		// labels
		Gtk::Label *m_SpeakerLabel;
		
		// combo boxes
		CharComboBox *m_CharsCB;
};

/***************************************************************************/

// goto (and its relatives) trigger
class GotoDialog: public AbstractDialog {
	public:
		// type
		enum Type { GOTO_NORMAL, GOTO_DIRECT, GOTO_TIMED };
		typedef Triplet<Glib::ustring, Type, int> Data;
		
		// constructor
		GotoDialog(const BufferMap &blocks);
		
		// get the goto data
		Data get_data();
		
	protected:
		// build the dialog
		void construct(const BufferMap &blocks);
		
		// radio button toggle handler
		void on_radio_button_toggled(const Glib::ustring &button);
		
		// radio buttons
		Gtk::RadioButtonGroup m_RadioGroup;
		Gtk::RadioButton *m_NormalRB;
		Gtk::RadioButton *m_DirectRB;
		Gtk::RadioButton *m_TimedRB;
		
		// labels
		Gtk::Label *m_TypeLabel;
		Gtk::Label *m_TargetLabel;
		Gtk::Label *m_TimeLabel;
		
		// entries
		Gtk::Entry *m_TimeEntry;
		
		// combo boxes
		BlockComboBox *m_BlockCB;
};

/***************************************************************************/

// show_evidence_* triggers
class ShowEvidenceDialog: public AbstractDialog {
	public:
		// position of evidence
		enum Type { TYPE_POS_LEFT, TYPE_POS_RIGHT };
		typedef std::pair<Glib::ustring, Type> Data;
		
		// constructor
		ShowEvidenceDialog(const EvidenceMap &ev);
		
		// get selected evidence
		Data get_data() const;
		
	protected:
		// build the dialog
		void construct(const EvidenceMap &ev);
		
		// handler for combo box changes
		void on_combo_box_changed();
		
		// radio buttons
		Gtk::RadioButtonGroup m_Group;
		Gtk::RadioButton *m_LeftRB;
		Gtk::RadioButton *m_RightRB;
		
		// labels
		Gtk::Label *m_PositionLabel;
		Gtk::Label *m_EvidenceLabel;
		Gtk::Label *m_PreviewLabel;
		
		// combo boxes
		EvidenceComboBox *m_EvidenceCB;
		
		// images
		Gtk::Image *m_Image;
};

/***************************************************************************/

// hide_evidence_* trigger
class HideEvidenceDialog: public AbstractDialog {
	public:
		enum Type { TYPE_POS_LEFT, TYPE_POS_RIGHT };
		
		// constructor
		HideEvidenceDialog();
		
		// get the position
		Type get_position() const;
		
	protected:
		// build the dialog
		void construct();
		
		// radio buttons
		Gtk::RadioButtonGroup m_Group;
		Gtk::RadioButton *m_LeftRB;
		Gtk::RadioButton *m_RightRB;
		
		// labels
		Gtk::Label *m_PositionLabel;
};

/***************************************************************************/

// set_location trigger
class SetLocationDialog: public AbstractDialog {
	public:
		// constructor
		SetLocationDialog(const LocationMap &locations);
		
		// get the selected location
		Glib::ustring get_location() const;
		
	protected:
		// build the dialog
		void construct(const LocationMap &locations);
		
		// location widget
		LocationWidget *m_LocWidget;
};

/***************************************************************************/

// add_location trigger
class AddLocationDialog: public AbstractDialog {
	public:
		// constructor
		AddLocationDialog(const LocationMap &locations);
		
		// get the selected location
		StringPair get_location_pair() const;
		
	protected:
		// build the dialog
		void construct(const LocationMap &locations);
		
		// overridden handler for ok button
		virtual void on_ok_button_clicked();
		
		// labels
		Gtk::Label *m_TargetLabel;
		Gtk::Label *m_LocationLabel;
		
		// combo boxes
		LocationComboBox *m_TargetCB;
		LocationComboBox *m_LocationCB;
};

/***************************************************************************/

// set location trigger
class LocationTriggerDialog: public AbstractDialog {
	public:
		// constructor
		LocationTriggerDialog(const LocationMap &locations, const BufferMap &buffers);
		
		// get the selection
		StringPair get_selection() const;
		
	protected:
		// build the dialog
		void construct(const LocationMap &locations, const BufferMap &buffers);
		
		// handler for text block combo box changes
		void on_text_block_combo_box_changed();
		
		// scrolled windows
		Gtk::ScrolledWindow *m_SWindow;
		
		// labels
		Gtk::Label *m_LocLabel;
		Gtk::Label *m_BlockLabel;
		
		// combo boxes
		LocationComboBox *m_LocationCB;
		BlockComboBox *m_BlockCB;
		
		// text views
		Gtk::TextView *m_BlockView;
};

/***************************************************************************/

// set_animation trigger
class SetAnimationDialog: public AbstractDialog {
	public:
		// constructor
		SetAnimationDialog(const CharacterMap &chars);
		
		// get the selection
		StringPair get_selection() const;
		
	protected:
		// build the dialog
		void construct(const CharacterMap &chars);
		
		// labels
		Gtk::Label *m_CharLabel;
		Gtk::Label *m_AnimLabel;
		
		// entries
		Gtk::Entry *m_AnimEntry;
		
		// combo boxes
		CharComboBox *m_CharCB;
};

/***************************************************************************/

// put_character trigger
class PutCharDialog: public AbstractDialog {
	public:
		// constructor
		PutCharDialog(const CharacterMap &chars, const LocationMap &locations);
		
		// get the selection
		StringPair get_selection() const;
		
	protected:
		// build the dialog
		void construct(const CharacterMap &chars, const LocationMap &locations);
		
		// labels
		Gtk::Label *m_CharLabel;
		
		// combo boxes
		CharComboBox *m_CharCB;
		
		// location widget
		LocationWidget *m_LocWidget;
};

/***************************************************************************/

// add_talk_option trigger
class AddTalkDialog: public AbstractDialog {
	public:
		// constructor
		AddTalkDialog(const CharacterMap &chars, const BufferMap &buffers);
		
		// get the data
		StringTriplet get_data() const;
		
	protected:
		// build the dialog
		void construct(const CharacterMap &chars, const BufferMap &buffers);
		
		// labels
		Gtk::Label *m_CharLabel;
		Gtk::Label *m_ViewLabel;
		Gtk::Label *m_BlockLabel;
		
		// combo boxes
		CharComboBox *m_CharCB;
		BlockComboBox *m_BlockCB;
		
		// entries
		Gtk::Entry *m_ViewEntry;
};

/***************************************************************************/

// remove_talk_option trigger
class RemoveTalkDialog: public AbstractDialog {
	public:
		// constructor
		RemoveTalkDialog(const CharacterMap &chars);
		
		// get data
		StringPair get_data() const;
		
	protected:
		// build the dialog
		void construct(const CharacterMap &chars);
		
		// labels
		Gtk::Label *m_CharLabel;
		Gtk::Label *m_ViewLabel;
		
		// combo boxes
		CharComboBox *m_CharCB;
		
		// entries
		Gtk::Entry *m_ViewEntry;
};

/***************************************************************************/

// clear_* triggers
class ClearCharDialog: public AbstractDialog {
	public:
		// constructor
		ClearCharDialog(const Glib::ustring &trigger, const CharacterMap &chars);
		
		// get the selected character
		Glib::ustring get_character() const;
		
	protected:
		// build the dialog
		void construct(const CharacterMap &chars);
		
		// labels
		Gtk::Label *m_CharLabel;
		
		// combo box
		CharComboBox *m_CharCB;
};

#endif
