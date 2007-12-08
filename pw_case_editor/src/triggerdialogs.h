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
#include <gtkmm/table.h>

#include "case.h"

// abstract dialog for inserting triggers
class AbstractDialog: public Gtk::Dialog {
	public:
		// constructor
		AbstractDialog(const Glib::ustring &trigger);
		
	protected:
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
		void construct();
		
		// handler for combo box changes
		void on_combo_box_changed(const Glib::ustring &id);
		
		// build the add_evidence page
		Gtk::Table* build_evidence_page();
		
		// build the add_profile page
		Gtk::Table* build_profile_page();
		
		// notebook container
		Gtk::Notebook *m_NB;
		
		// labels
		Gtk::Label *m_EvidenceLabel;
		Gtk::Label *m_ProfileLabel;
		Gtk::Label *m_EvidencePreviewLabel;
		Gtk::Label *m_ProfilePreviewLabel;
		
		// combo boxes
		Gtk::ComboBoxText *m_EvidenceCB;
		Gtk::ComboBoxText *m_ProfileCB;
		
		// images
		Gtk::Image *m_EvidenceImage;
		Gtk::Image *m_ProfileImage;
		
		// records of maps
		CharacterMap m_CharMap;
		EvidenceMap m_EvidenceMap;
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
		void construct();
		
		// labels
		Gtk::Label *m_SpeakerLabel;
		
		// combo boxes
		Gtk::ComboBoxText *m_CharsCB;
		
		CharacterMap m_CharMap;
};

/***************************************************************************/

// goto (and its relatives) trigger
class GotoDialog: public AbstractDialog {
	public:
		// type
		enum Type { GOTO_NORMAL, GOTO_DIRECT, GOTO_TIMED };
		
		// data related to a goto trigger
		struct _Data {
			Glib::ustring target;
			int time;
			Type type;
		};
		typedef struct _Data Data;
		
		
		// constructor
		GotoDialog(BufferMap blocks);
		
		// get the goto data
		Data get_data();
		
	protected:
		// build the dialog
		void construct();
		
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
		Gtk::ComboBoxText *m_BlockCB;
};

/***************************************************************************/

// show_evidence_* triggers
class ShowEvidenceDialog: public AbstractDialog {
	public:
		// position of evidence
		enum Type { TYPE_POS_LEFT, TYPE_POS_RIGHT };
		
		// data representing selection
		struct _Data {
			Type type;
			Glib::ustring id;
		};
		typedef struct _Data Data;
		
		// constructor
		ShowEvidenceDialog(const EvidenceMap &ev);
		
		// get selected evidence
		Data get_data() const;
		
	protected:
		// build the dialog
		void construct();
		
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
		Gtk::ComboBoxText *m_EvidenceCB;
		
		// images
		Gtk::Image *m_Image;
		
		EvidenceMap m_EvidenceMap;
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

#endif
