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
// dialogs.h: various small dialogs

#ifndef DIALOGS_H
#define DIALOGS_H

#include <gtkmm/button.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include <gtkmm/listviewtext.h>
#include <gtkmm/notebook.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/textview.h>

#include "case.h"
#include "character.h"
#include "hotspotwidget.h"

// dialog used to add a hotspot to a location
class NewHotspotDialog: public Gtk::Dialog {
	public:
		// constructor
		NewHotspotDialog();
		
		// set pixbuf for background
		void set_pixbuf(const Glib::RefPtr<Gdk::Pixbuf> &pixbuf) { m_HSWidget->set_image(pixbuf); }
		
		// return the hotspot
		Case::Hotspot get_hotspot();
		
	private:
		// build the ui
		void construct();
		
		// signal handler for point changes
		void on_point_changed(int x, int y);
		
		// signal handler for dimension changes
		void on_dimensions_changed(int w, int h);
		
		// handler for coordinate entry changes
		void on_coord_entry_changed();
		
		// handler for dimension entry changes
		void on_dimension_entry_changed();
		
		// labels
		Gtk::Label *m_XLabel;
		Gtk::Label *m_YLabel;
		Gtk::Label *m_WLabel;
		Gtk::Label *m_HLabel;
		Gtk::Label *m_BlockLabel;
		
		// entries
		Gtk::Entry *m_XEntry;
		Gtk::Entry *m_YEntry;
		Gtk::Entry *m_WEntry;
		Gtk::Entry *m_HEntry;
		Gtk::Entry *m_BlockEntry;
		
		// widget to visually edit the hotspot
		HotspotWidget *m_HSWidget;
};

/***************************************************************************/

// dialog used to manage locations
class LocationsDialog: public Gtk::Dialog {
	public:
		// constructor
		LocationsDialog(const LocationMap &locations, const BackgroundMap &bgs, const StringVector &usedIds);
		
		// return the stored location map
		LocationMap get_locations() const { return m_Locations; }
		
	private:
		// build the ui
		void construct();
		
		// add a location
		void on_add();
		
		// remove a location
		void on_delete();
		
		// add a hotspot
		void on_add_hotspot();
		
		// remove a hotspot
		void on_delete_hotspot();
		
		// amend button click handler
		void on_amend_button_clicked();
		
		// row changes handler
		void on_selection_changed();
		
		// buttons
		Gtk::Button *m_AddButton;
		Gtk::Button *m_DeleteButton;
		Gtk::Button *m_AddHSButton;
		Gtk::Button *m_DeleteHSButton;
		Gtk::Button *m_AmendButton;
		
		// labels
		Gtk::Label *m_LocationsLabel;
		Gtk::Label *m_DetailsLabel;
		Gtk::Label *m_IdLabel;
		Gtk::Label *m_NameLabel;
		Gtk::Label *m_BGLabel;
		Gtk::Label *m_HotspotsLabel;
		
		// entries
		Gtk::Entry *m_IdEntry;
		Gtk::Entry *m_NameEntry;
		Gtk::Entry *m_BGEntry;
		
		// scrolled windows
		Gtk::ScrolledWindow *m_SWindow;
		Gtk::ScrolledWindow *m_HotspotSWindow;
		
		// tree model and view
		Gtk::TreeView *m_TreeView;
		Glib::RefPtr<Gtk::ListStore> m_Model;
		
		// column record
		class ColumnRec: public Gtk::TreeModel::ColumnRecord {
			public:
				// constructor
				ColumnRec() {
					add(m_Column);
				}
				
				// columns
				Gtk::TreeModelColumn<Glib::ustring> m_Column;
		};
		
		// column record instance
		ColumnRec m_ColumnRec;
		
		// list view for hotspots
		Gtk::ListViewText *m_HotspotList;
		
		// stored location data
		LocationMap m_Locations;
		BackgroundMap m_Backgrounds;
		StringVector m_UsedIds;
};

/***************************************************************************/

// dialog displayed before viewing sprite editor
class SpriteChooserDialog: public Gtk::Dialog {
	public:
		enum SpriteMode { SPRITE_NEW=0, SPRITE_EXISTING };
		
		// constructor
		SpriteChooserDialog();
		
		// get the selected sprite option
		SpriteMode get_sprite_mode() const { return (m_NewSpriteRB->get_active() ? SPRITE_NEW : SPRITE_EXISTING); }
		
		// get path name
		Glib::ustring get_path() const { return m_PathEntry->get_text(); }
		
	private:
		// build the ui
		void construct();
		
		// browse button click handler
		void on_browse_button_clicked();
		
		// new sprite radio button toggle handler
		void on_new_sprite_toggled();
		
		// open sprite radio button toggle handler
		void on_open_sprite_toggled();
		
		// radio buttons
		Gtk::RadioButtonGroup m_Group;
		Gtk::RadioButton *m_NewSpriteRB;
		Gtk::RadioButton *m_OpenSpriteRB;
		
		// labels
		Gtk::Label *m_SpriteLabel;
		Gtk::Label *m_PathLabel;
		
		// entry
		Gtk::Entry *m_PathEntry;
		
		// buttons
		Gtk::Button *m_BrowseButton;
};

/***************************************************************************/

// generic input dialog for text
class TextInputDialog: public Gtk::Dialog {
	public:
		// constructor
		TextInputDialog(const Glib::ustring &label="Input text:", const Glib::ustring &defaultValue="");
		
		// get inputted text
		Glib::ustring get_text() const { return m_Entry->get_text(); }
		
	private:
		// build the ui
		void construct(const Glib::ustring &label, const Glib::ustring &defaultValue);
		
		// labels
		Gtk::Label *m_Label;
		
		// entries
		Gtk::Entry *m_Entry;
};

/***************************************************************************/
/*
// dialog used to manage audio clips
class AudioDialog: public Gtk::Dialog {
	public:
		// constructor
		AudioDialog();
};
*/

/***************************************************************************/

// dialog used for adding new evidence
class NewEvidenceDialog: public Gtk::Dialog {
	public:
		// constructor
		NewEvidenceDialog(const StringVector &ids);
		
		// get the inputted evidence data
		Case::Evidence get_evidence_data();
		
		// get inputted path to image
		Glib::ustring get_image_path() const { return m_PathEntry->get_text(); }
		
	private:
		// build the ui
		void construct();
		
		// id text changes handler
		void on_id_changed();
		
		// browse button clicks
		void on_browse_button_clicked();
		
		// labels
		Gtk::Label *m_PathLabel;
		Gtk::Label *m_IdLabel;
		
		// entries
		Gtk::Entry *m_PathEntry;
		Gtk::Entry *m_IdEntry;
		
		// buttons
		Gtk::Button *m_BrowseButton;
		Gtk::Button *m_OKButton;
		
		// record of used evidence ids
		StringVector m_UsedIds;
};

/***************************************************************************/

// dialog used to manage evidence
class EvidenceDialog: public Gtk::Dialog {
	public:
		// constructor
		EvidenceDialog(const EvidenceMap &evidence,
			       const StringVector &evidenceIds);
		
		// get the evidence data stored in the dialog
		EvidenceMap get_evidence() const { return m_Evidence; }
		
	private:
		// build the ui
		void construct();
		
		// add a background
		void on_add();
		
		// remove a background
		void on_delete();
		
		// amend button click handler
		void on_amend_button_clicked();
		
		// row changes handler
		void on_selection_changed();
		
		// tree view and model for managing assets
		Gtk::TreeView *m_TreeView;
		Glib::RefPtr<Gtk::ListStore> m_Model;
		
		// labels
		Gtk::Label *m_EvidenceLabel;
		Gtk::Label *m_PreviewLabel;
		Gtk::Label *m_InternalLabel;
		Gtk::Label *m_NameLabel;
		Gtk::Label *m_CaptionLabel;
		Gtk::Label *m_DescLabel;
		
		// scrolled window container
		Gtk::ScrolledWindow *m_SWindow;
		
		// image for previews
		Gtk::Image *m_Image;
		
		// entries
		Gtk::Entry *m_InternalEntry;
		Gtk::Entry *m_NameEntry;
		Gtk::Entry *m_CaptionEntry;
		Gtk::Entry *m_DescEntry;
		
		// buttons
		Gtk::Button *m_AddButton;
		Gtk::Button *m_DeleteButton;
		Gtk::Button *m_AmendButton;
		
		// column record
		class ColumnRec: public Gtk::TreeModel::ColumnRecord {
			public:
				// constructor
				ColumnRec() {
					add(m_Column);
				}
				
				// columns
				Gtk::TreeModelColumn<Glib::ustring> m_Column;
		};
		
		// column record instance
		ColumnRec m_ColumnRec;
		
		// record of evidence data
		EvidenceMap m_Evidence;
		StringVector m_EvidenceIds;
};

/***************************************************************************/

// dialog used for managing backgrounds
class BackgroundsDialog: public Gtk::Dialog {
	public:
		// constructor
		BackgroundsDialog(const BackgroundMap &bgs,
				  const StringVector &bgIds);
		
		// get the backgrounds stored in the dialog
		BackgroundMap get_backgrounds() const { return m_Backgrounds; }
		
	private:
		// build the ui
		void construct();
		
		// add a background
		void on_add();
		
		// remove a background
		void on_delete();
		
		// row changes handler
		void on_selection_changed();
		
		// scrolled window container
		Gtk::ScrolledWindow *m_SWindow;
		
		// list view for managing backgrounds
		Gtk::TreeView *m_ListView;
		Glib::RefPtr<Gtk::ListStore> m_ListModel;
		
		// image
		Gtk::Image *m_Image;
		
		// labels
		Gtk::Label *m_BGLabel;
		Gtk::Label *m_PreviewLabel;
		
		// buttons
		Gtk::Button *m_AddButton;
		Gtk::Button *m_DeleteButton;
		
		// column record
		class ColumnRec: public Gtk::TreeModel::ColumnRecord {
			public:
				// constructor
				ColumnRec() {
					add(m_Column);
				}
				
				// columns
				Gtk::TreeModelColumn<Glib::ustring> m_Column;
		};
		
		// column record instance
		ColumnRec m_ColumnRec;
		
		// record of background data
		BackgroundMap m_Backgrounds;
		StringVector m_BGIds;
};

/***************************************************************************/

// dialog used for adding new backgrounds
class NewBackgroundDialog: public Gtk::Dialog {
	public:
		// constructor
		NewBackgroundDialog(const StringVector &bgIds);
		
		// get the data in the dialog compiled in Background struct
		Case::Background get_background_data();
		
		// get the path to the requested image
		Glib::ustring get_path() const { return m_PathEntry->get_text(); }
		
	private:
		// build the ui
		void construct();
		
		// browse button click handler
		void on_browse_button_clicked();
		
		// id entry text change handler
		void on_id_changed();
		
		// labels
		Gtk::Label *m_PathLabel;
		Gtk::Label *m_IdLabel;
		Gtk::Label *m_TypeLabel;
		
		// entries
		Gtk::Entry *m_PathEntry;
		Gtk::Entry *m_IdEntry;
		
		// radio buttons
		Gtk::RadioButton::Group m_RBGroup;
		Gtk::RadioButton *m_SingleScreenRB;
		Gtk::RadioButton *m_DoubleScreenRB;
		
		// buttons
		Gtk::Button *m_OKButton;
		Gtk::Button *m_BrowseButton;
		
		// record of background ids
		StringVector m_UsedBGs;
};

/***************************************************************************/

// dialog displayed for making new cases
class NewDialog: public Gtk::Dialog {
	public:
		// constructor
		NewDialog();
		
		// set case overview
		void set_overview(const Case::Overview &overview);
		
		// get case information
		Case::Overview get_overview();
	
	private:
		// build the ui
		void construct();
		
		// labels
		Gtk::Label *m_CaseNameLabel;
		Gtk::Label *m_CaseAuthorLabel;
		Gtk::Label *m_LawSysLabel;
		
		// entries
		Gtk::Entry *m_CaseNameEntry;
		Gtk::Entry *m_CaseAuthorEntry;
		
		// combo boxes
		Gtk::ComboBoxText *m_LawSysCB;
};

/***************************************************************************/

// dialog displayed to browse characters
class CharBrowser: public Gtk::Dialog {
	public:
		// constructor
		CharBrowser(const CharacterMap &cmap);
		
		// check if a character was changed
		bool was_changed() const { return m_CharChanged; }
		
		// return the character map
		CharacterMap get_characters() const { return m_CharacterMap; }
		
	private:
		// build the ui
		void construct();
		
		// edit button handler
		void on_edit_button_clicked();
		
		// combo box selectio changes
		void on_combo_box_changed();
		
		// see if a character was changed
		bool m_CharChanged;
		
		// labels
		Gtk::Label *m_CharacterLabel;
		Gtk::Label *m_InternalLabel;
		Gtk::Label *m_NameLabel;
		Gtk::Label *m_DescLabel;
		
		// entries
		Gtk::Entry *m_InternalEntry;
		Gtk::Entry *m_NameEntry;
		Gtk::Entry *m_DescEntry;
		
		// buttons
		Gtk::Button *m_EditButton;
		
		// combo boxes
		Gtk::ComboBoxText *m_CharCB;
		
		// internal record of characters
		CharacterMap m_CharacterMap;
};

/***************************************************************************/

// dialog displayed when adding a new character
class NewCharDialog: public Gtk::Dialog {
	public:
		// constructor
		NewCharDialog(const StringVector &names);
		
		// set character data
		void set_character_data(const Character &ch);
		
		// return the data in the dialog
		Character get_character_data();
		
	private:
		// build the ui
		void construct();
		
		// signal handler for internal name text changes
		void on_internal_name_changed();
		
		// signal handler for check button clicks
		void on_check_button_clicked(const Glib::ustring &buttonId);
		
		// signal handler for browse button clicks
		void on_browse_button_clicked(const Glib::ustring &buttonId);
		
		// flag if internal names should be checked
		bool m_CheckInternals;
		
		// build the general page
		Gtk::Container* build_general_page();
		
		// build the graphics page
		Gtk::Container* build_graphics_page();
		
		// check buttons
		Gtk::CheckButton *m_HasTagCB;
		Gtk::CheckButton *m_HasHeadshotCB;
		
		// notebook for tabs
		Gtk::Notebook *m_NB;
		
		// buttons
		Gtk::Button *m_OKButton;
		Gtk::Button *m_TagBrowseButton;
		Gtk::Button *m_HeadshotBrowseButton;
		
		// labels
		Gtk::Label *m_CodeNameLabel;
		Gtk::Label *m_NameLabel;
		Gtk::Label *m_CapLabel;
		Gtk::Label *m_DescLabel;
		Gtk::Label *m_SpriteLabel;
		Gtk::Label *m_TextBoxLabel;
		Gtk::Label *m_HeadshotLabel;
		
		// entries
		Gtk::Entry *m_CodeNameEntry;
		Gtk::Entry *m_NameEntry;
		Gtk::Entry *m_CapEntry;
		Gtk::Entry *m_DescEntry;
		Gtk::Entry *m_SpriteEntry;
		
		// images
		Gtk::Image *m_TextBoxImage;
		Gtk::Image *m_HeadshotImage;
		
		// record of already present character internal names (used for validation)
		StringVector m_UsedNames;
};

/***************************************************************************/

// dialog displayed to select initial text block
class InitialBlockDialog: public Gtk::Dialog {
	public:
		// constructor
		InitialBlockDialog(const Glib::ustring &id, BufferMap blocks);
		
		// get the selected text block id
		Glib::ustring get_selected_block_id() const { return m_BlocksCB->get_active_text(); }
		
	private:
		// build the ui
		void construct();
		
		// block cb changes
		void on_block_changed();
		
		// scrolled window for text view
		Gtk::ScrolledWindow *m_SWindow;
		
		// text view
		Gtk::TextView *m_TextPreview;
		
		// combo box
		Gtk::ComboBoxText *m_BlocksCB;
		
		// labels
		Gtk::Label *m_BlockLabel;
		Gtk::Label *m_PreviewLabel;
		
		// record of text blocks
		BufferMap m_Blocks;
};

#endif
