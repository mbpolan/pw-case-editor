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
#include <gtkmm/progressbar.h>
#include <gtkmm/scale.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/textview.h>
#include <gtkmm/togglebutton.h>

#include "case.h"
#include "casecombobox.h"
#include "character.h"
#include "colorwidget.h"
#include "hotspotwidget.h"

/** Dialog to let the user change text speed
*/
class ChangeSpeedDialog: public Gtk::Dialog {
	public:
		/// Constructor
		ChangeSpeedDialog();
		
		/** Get the text speed
		  * \return String representation of text speed
		*/
		Glib::ustring get_text_speed();
		
	private:
		// build the ui
		void construct();
		
		// handler for value changes in slider
		void on_value_changed();
		
		// buttons
		Gtk::Button *m_OKButton;
		
		// labels
		Gtk::Label *m_SlowerLabel;
		Gtk::Label *m_FasterLabel;
		
		// scale for selecting value
		Gtk::HScale *m_Scale;
};

/***************************************************************************/

/** Dialog to let the user change the dialogue text color
*/
class ChangeColorDialog: public Gtk::Dialog {
	public:
		/// Constructor
		ChangeColorDialog();
		
		/** Get the selected color
		  * \return String representation of selected color
		*/
		Glib::ustring get_color();
		
	private:
		// build the ui
		void construct();
		
		// buttons containing colors
		Gtk::RadioButton *m_BlueButton;
		Gtk::RadioButton *m_GreenButton;
		Gtk::RadioButton *m_OrangeButton;
		Gtk::RadioButton *m_WhiteButton;
		
		// radio buttons
		Gtk::RadioButtonGroup m_Group;
};

/***************************************************************************/

/** Dialog to display progress of a task
*/
class ProgressDialog: public Gtk::Dialog {
	public:
		/** Constructor
		  * \param label The text to display in the dialog
		*/
		ProgressDialog(const Glib::ustring &label);
		
		/** Set the amount of progress done
		  * \param amount Percentage of task completed
		*/
		void set_progress(double amount);
		
	private:
		// build the ui
		void construct();
		
		// label
		Gtk::Label *m_Label;
		
		// progress bar
		Gtk::ProgressBar *m_ProgBar;
};

/***************************************************************************/

/** Dialog used to manage testimonies
*/
class TestimonyManager: public Gtk::Dialog {
	public:
		/** Constructor
		  * \param tmap Map of testimonies
		  * \param testimonyIds String vector of testimony internal IDs
		*/
		TestimonyManager(const TestimonyMap &tmap, const StringVector &testimonyIds);
		
		/** Get updated map of testimonies
		  * \return Map of testimonies
		*/
		TestimonyMap get_testimonies() const { return m_Testimonies; }
		
	private:
		// build the ui
		void construct();
		
		// add button click handler
		void on_add_button_clicked();
		
		// edit button click handler
		void on_edit_button_clicked();
		
		// delete button click handler
		void on_delete_button_clicked();
		
		// selection change handler
		void on_selection_changed();
		
		// labels
		Gtk::Label *m_TitleLabel;
		Gtk::Label *m_PreviewLabel;
		
		// buttons
		Gtk::Button *m_AddButton;
		Gtk::Button *m_EditButton;
		Gtk::Button *m_DeleteButton;
		
		// list view for list and its container
		Gtk::ScrolledWindow *m_SWindow;
		Gtk::ListViewText *m_ListView;
		
		// internal record of testimonies
		TestimonyMap m_Testimonies;
		StringVector m_Ids;
};

/***************************************************************************/

/** Dialog used to manage images
*/
class ImageDialog: public Gtk::Dialog {
	public:
		/** Constructor
		  * \param imap Map of images
		  * \param imgIds String vector of image internal IDs
		*/
		ImageDialog(const ImageMap &imap, const StringVector &imgIds);
		
		/** Get updated map of images
		  * \return New map of images
		*/
		ImageMap get_images() const { return m_Images; }
		
	private:
		// build the ui
		void construct();
		
		// add image handler
		void on_add_clicked();
		
		// remove image handler
		void on_delete_clicked();
		
		// selection change handler
		void on_selection_changed();
		
		// labels
		Gtk::Label *m_ImageLabel;
		Gtk::Label *m_PreviewLabel;
		
		// buttons
		Gtk::Button *m_AddButton;
		Gtk::Button *m_DeleteButton;
		
		// scrolled window container
		Gtk::ScrolledWindow *m_SWindow;
		
		// list of images
		Gtk::ListViewText *m_ImageList;
		
		// preview image
		Gtk::Image *m_Image;
		
		// record of images
		ImageMap m_Images;
		StringVector m_ImageIds;
};

/***************************************************************************/

/** Dialog used to add a hotspot to a location
*/
class NewHotspotDialog: public Gtk::Dialog {
	public:
		/// Constructor
		NewHotspotDialog();
		
		/** Set pixbuf containing image data for background
		  * \param pixbuf The image data for the background
		*/
		void set_pixbuf(const Glib::RefPtr<Gdk::Pixbuf> &pixbuf) { m_HSWidget->set_image(pixbuf); }
		
		/** Get the hotspot
		  * \return Completed Case::Hotspot struct
		*/
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

/** Dialog used to manage locations
*/
class LocationsDialog: public Gtk::Dialog {
	public:
		/** Constructor
		  * \param locations Map of locations
		  * \param bgs Map of backgrounds
		  * \param usedIds String vector of used location internal IDs
		*/
		LocationsDialog(const LocationMap &locations, const BackgroundMap &bgs, const StringVector &usedIds);
		
		/** Get updated location map
		  * \return New map of locations
		*/
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

/** Dialog displayed before viewing sprite editor
*/
class SpriteChooserDialog: public Gtk::Dialog {
	public:
		/// Specifies what type of sprite action is requested by the user
		enum SpriteMode { SPRITE_NEW=0, SPRITE_EXISTING, SPRITE_FROM_GIFS };
		
		/// Constructor
		SpriteChooserDialog();
		
		/** Get the selected sprite option
		  * \return The corresponding SpriteMode as requested by the user
		*/
		SpriteMode get_sprite_mode() const;
		
		/** Get the path to open a sprite or directory of GIFs
		  * \return The path in the entry
		*/
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
		
		// from gif files radio buttons toggle handler
		void on_from_gifs_toggled();
		
		// radio buttons
		Gtk::RadioButtonGroup m_Group;
		Gtk::RadioButton *m_NewSpriteRB;
		Gtk::RadioButton *m_OpenSpriteRB;
		Gtk::RadioButton *m_CreateFromGifsRB;
		
		// labels
		Gtk::Label *m_SpriteLabel;
		Gtk::Label *m_PathLabel;
		
		// entry
		Gtk::Entry *m_PathEntry;
		
		// buttons
		Gtk::Button *m_BrowseButton;
};

/***************************************************************************/

/** Generic input dialog for text.
  * This could be used where ever simple text input is needed
*/
class TextInputDialog: public Gtk::Dialog {
	public:
		/** Constructor
		  * \param label Text to display in the label
		  * \param defaultValue Default text to display in entry
		*/
		TextInputDialog(const Glib::ustring &label="Input text:", const Glib::ustring &defaultValue="");
		
		/** Get the inputted text
		  * \return Text from the entry
		*/
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

/** Dialog used to manage audio samples
*/
class AudioDialog: public Gtk::Dialog {
	public:
		/// Constructor
		AudioDialog();
		
		/** Set the audio samples to display
		  * \param audio Map of audio samples
		*/
		void set_audio(const AudioMap &audio);
		
		/** Get updated map of audio samples
		  * \return New map of audio samples
		*/
		AudioMap get_audio_data();
		
	private:
		// build the ui
		void construct();
		
		// add button handler
		void on_add_audio();
		
		// delete button handler
		void on_remove_audio();
		
		// labels
		Gtk::Label *m_AudioLabel;
		
		// containers
		Gtk::ScrolledWindow *m_SWindow;
		
		// buttons
		Gtk::Button *m_AddButton;
		Gtk::Button *m_DeleteButton;
		
		// list of audio files
		Glib::RefPtr<Gtk::ListStore> m_Model;
		Gtk::TreeView *m_AudioList;
		
		// column record
		class ColumnRec: public Gtk::TreeModel::ColumnRecord {
			public:
				ColumnRec() {
					add(m_NameCol);
					add(m_IdCol);
				}
				
				// columns
				Gtk::TreeModelColumn<Glib::ustring> m_NameCol;
				Gtk::TreeModelColumn<Glib::ustring> m_IdCol;
		};
		
		// column record instance
		ColumnRec m_ColRec;
};

/***************************************************************************/

/** Dialog used for adding new evidence
*/
class NewEvidenceDialog: public Gtk::Dialog {
	public:
		/** Constructor
		  * \param ids String vector of used evidence internal IDs
		*/
		NewEvidenceDialog(const StringVector &ids);
		
		/** Get the inputted evidence data
		  * \return Completed Case::Evidence struct with values filled
		*/
		Case::Evidence get_evidence_data();
		
		/** Get the inputted path to evidence image
		  * \return The provided path to the image
		*/
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

/** Dialog used to manage evidence
*/
class EvidenceDialog: public Gtk::Dialog {
	public:
		/** Constructor
		  * \param evidence Map of evidence
		  * \param images Map of images
		  * \param evidenceIds String vector of used evidence internal IDs
		*/
		EvidenceDialog(const EvidenceMap &evidence,
			       const ImageMap &images,
			       const StringVector &evidenceIds);
		
		/** Get the evidence map stored in the dialog
		  * \return New map of evidence
		*/
		EvidenceMap get_evidence() const { return m_Evidence; }
		
	private:
		// build the ui
		void construct(const ImageMap &images);
		
		// has check image button toggled
		void on_check_img_toggled();
		
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
		
		// check buttons
		Gtk::CheckButton *m_HasImgCB;
		
		// combo boxes
		ImgComboBox *m_ImgCB;
		
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

/** Dialog used for managing backgrounds
*/
class BackgroundsDialog: public Gtk::Dialog {
	public:
		/** Constructor
		  * \param bgs Map of backgrounds
		  * \param bgIds String vector of used background internal IDs
		*/
		BackgroundsDialog(const BackgroundMap &bgs,
				  const StringVector &bgIds);
		
		/** Get the backgrounds stored in the dialog
		  * \return New map of updated backgrounds
		*/
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

/** Dialog used for adding new backgrounds
*/
class NewBackgroundDialog: public Gtk::Dialog {
	public:
		/** Constructor
		  * \param bgIds String vector of used background internal IDs
		*/
		NewBackgroundDialog(const StringVector &bgIds);
		
		/** Get the data in the dialog
		  * \return Completed Case::Background struct
		*/
		Case::Background get_background_data();
		
		/** Get the path to the requested image
		  * \return Path to background image
		*/
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

/** Dialog displayed for making new cases
*/
class NewDialog: public Gtk::Dialog {
	public:
		/// Constructor
		NewDialog();
		
		/** Set the case overview to use
		  * \param overview Case::Overview struct to default values to
		*/
		void set_overview(const Case::Overview &overview);
		
		/** Get updated or new case information
		  * \return Completed Case::Overview struct
		*/
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

/** Dialog displayed to browse characters
*/
class CharBrowser: public Gtk::Dialog {
	public:
		/** Constructor
		  * \param cmap Map of characters
		*/
		CharBrowser(const CharacterMap &cmap);
		
		/** Check if a character was changed
		  * \return <b>true</b> if the current character needs to be updated, <b>false</b> otherwise
		*/
		bool was_changed() const { return m_CharChanged; }
		
		/** Get the updated character map
		  * \return Updated map of characters
		*/
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

/** Dialog displayed when adding a new character
*/
class NewCharDialog: public Gtk::Dialog {
	public:
		/** Constructor
		  * \param names String vector of used character display names
		*/
		NewCharDialog(const StringVector &names);
		
		/** Set the character data to default dialog values to
		  * \param ch The character to set
		*/
		void set_character_data(const Character &ch);
		
		/** Get the data in the dialog
		  * \return Completed Character object
		*/
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
		
		// radio buttons
		Gtk::RadioButtonGroup m_Group;
		Gtk::RadioButton *m_MaleRB;
		Gtk::RadioButton *m_FemaleRB;
		Gtk::RadioButton *m_UnknownRB;
		
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
		Gtk::Label *m_GenderLabel;
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

/** Dialog displayed to select initial text block
*/
class InitialBlockDialog: public Gtk::Dialog {
	public:
		/** Constructor
		  * \param id Internal ID of the block
		  * \param blocks Map of blocks
		*/
		InitialBlockDialog(const Glib::ustring &id, BufferMap blocks);
		
		/** Get the selected text block ID
		  * \return The selected block's internal ID
		*/
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
