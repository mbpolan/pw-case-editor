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
// mainwindow.cpp: implementation of MainWindow class

#include <gtkmm/aboutdialog.h>
#include <gtkmm/box.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/stock.h>

#include "dialogs.h"
#include "iohandler.h"
#include "mainwindow.h"
#include "spriteeditor.h"

// constructor
MainWindow::MainWindow() {
	// set the title and minimum window size
	set_title("Unsaved File - Phoenix Wright Case Editor");
	set_size_request(640, 480);
	
	// reset save variables
	m_Saved=false;
	m_SavePath="";
	
	construct();
}

// build the ui
void MainWindow::construct() {
	// allocate vbox
	Gtk::VBox *vb=manage(new Gtk::VBox);
	
	// allocate action group
	m_ActionGroup=Gtk::ActionGroup::create();
	
	// populate it
	m_ActionGroup->add(Gtk::Action::create("FileNew", Gtk::Stock::NEW, "_New"),
			   sigc::mem_fun(*this, &MainWindow::on_new));
	m_ActionGroup->add(Gtk::Action::create("FileSave", Gtk::Stock::SAVE, "_Save"),
			   sigc::mem_fun(*this, &MainWindow::on_save));
	m_ActionGroup->add(Gtk::Action::create("FileOpen", Gtk::Stock::OPEN, "_Open"),
			   sigc::mem_fun(*this, &MainWindow::on_open));
	m_ActionGroup->add(Gtk::Action::create("FileExport", "_Export"),
			   sigc::mem_fun(*this, &MainWindow::on_export));
	m_ActionGroup->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT, "_Quit"),
			   sigc::mem_fun(*this, &MainWindow::on_quit));
	
	m_ActionGroup->add(Gtk::Action::create("CaseAddChar", "_Add Character"),
			   sigc::mem_fun(*this, &MainWindow::on_case_add_char));
	m_ActionGroup->add(Gtk::Action::create("CaseBrowseChar", "_Browse Characters"),
			   sigc::mem_fun(*this, &MainWindow::on_case_browse_chars));
	m_ActionGroup->add(Gtk::Action::create("CaseEditLocations", "_Edit Locations"),
			   sigc::mem_fun(*this, &MainWindow::on_case_edit_locations));
	m_ActionGroup->add(Gtk::Action::create("CaseEditOverview", "_Edit Overview"),
			   sigc::mem_fun(*this, &MainWindow::on_case_edit_overview));
	m_ActionGroup->add(Gtk::Action::create("CaseInitialBlock", "_Initial Text Block"),
			   sigc::mem_fun(*this, &MainWindow::on_case_change_initial_block));
	
	m_ActionGroup->add(Gtk::Action::create("AssetsManageAudio", "Manage _Audio"),
			   sigc::mem_fun(*this, &MainWindow::on_assets_manage_audio));
	m_ActionGroup->add(Gtk::Action::create("AssetsManageBG", "Manage _Backgrounds"),
			   sigc::mem_fun(*this, &MainWindow::on_assets_manage_bg));
	m_ActionGroup->add(Gtk::Action::create("AssetsManageEvidence", "Manage _Evidence"),
			   sigc::mem_fun(*this, &MainWindow::on_assets_manage_evidence));
	
	m_ActionGroup->add(Gtk::Action::create("ToolsSpriteEditor", "_Sprite Editor"),
			   sigc::mem_fun(*this, &MainWindow::on_tools_sprite_editor));
	
	m_ActionGroup->add(Gtk::Action::create("HelpAbout", Gtk::Stock::ABOUT, "_About"),
			   sigc::mem_fun(*this, &MainWindow::on_help_about));
	
	m_ActionGroup->add(Gtk::Action::create("FileMenu", "_File"));
	m_ActionGroup->add(Gtk::Action::create("CaseMenu", "_Case"));
	m_ActionGroup->add(Gtk::Action::create("AssetsMenu", "_Assets"));
	m_ActionGroup->add(Gtk::Action::create("ToolsMenu", "_Tools"));
	m_ActionGroup->add(Gtk::Action::create("HelpMenu", "_Help"));
	
	// allocate ui manager
	m_UIManager=Gtk::UIManager::create();
	m_UIManager->insert_action_group(m_ActionGroup);
	m_UIManager->set_add_tearoffs(true);
	
	// accelerate the menus
	add_accel_group(m_UIManager->get_accel_group());
	
	// create the ui from a string
	Glib::ustring ui="<ui>"
			 "<menubar name='MenuBar'>"
			"	<menu action='FileMenu'>"
			"		<menuitem action='FileNew'/>"
			"		<menuitem action='FileSave'/>"
			"		<menuitem action='FileOpen'/>"
			"		<separator/>"
			"		<menuitem action='FileExport'/>"
			"		<separator/>"
			"		<menuitem action='FileQuit'/>"
			"	</menu>"
			"	<menu action='CaseMenu'>"
			"		<menuitem action='CaseAddChar'/>"
			"		<menuitem action='CaseBrowseChar'/>"
			"		<separator/>"
			"		<menuitem action='CaseInitialBlock'/>"
			"		<menuitem action='CaseEditLocations'/>"
			"		<menuitem action='CaseEditOverview'/>"
			"	</menu>"
			"	<menu action='AssetsMenu'>"
			"		<menuitem action='AssetsManageAudio'/>"
			"		<menuitem action='AssetsManageBG'/>"
			"		<menuitem action='AssetsManageEvidence'/>"
			"	</menu>"
			"	<menu action='ToolsMenu'>"
			"		<menuitem action='ToolsSpriteEditor'/>"
			"	</menu>"
			"	<menu action='HelpMenu'>"
			"		<menuitem action='HelpAbout'/>"
			"	</menu>"
			"</menubar>"
			"</ui>";
	
	// add this ui
	m_UIManager->add_ui_from_string(ui);
	
	// get the menu bar widget
	Gtk::Widget *menuBar=m_UIManager->get_widget("/MenuBar");
	if (menuBar)
		vb->pack_start(*manage(menuBar), Gtk::PACK_SHRINK);
	
	// allocate script widget
	m_ScriptWidget=manage(new ScriptWidget);
	
	// pack widgets
	vb->pack_start(*m_ScriptWidget);
	
	// add the containers
	add(*vb);
	show_all_children();
}

// new case handler
void MainWindow::on_new() {
	// ask to save
	if (m_ScriptWidget->get_buffers().size()>2) {
		// display dialog
		Gtk::MessageDialog md(*this, "Would you like to save your current case project?", 
				       false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
		if (md.run()==Gtk::RESPONSE_YES)
			on_save();
	}
	
	// display new case dialog
	NewDialog nd;
	if (nd.run()==Gtk::RESPONSE_OK) {
		// clear out any previous case data
		m_Case.clear();
		m_ScriptWidget->clear();
		
		// reset save variables
		m_Saved=false;
		m_SavePath="";
		
		// reset title bar
		set_title("Unsaved File - Phoenix Wright Case Editor");
		
		// get the updated case overview
		Case::Overview overview=nd.get_overview();
		
		// and apply this new overview
		m_Case.set_overview(overview);
	}
}

// save case handler
void MainWindow::on_save() {
	// save automatically if we have a path set
	if (m_Saved) {
		// save this case
		std::map<Glib::ustring, Glib::ustring> bdescs=m_ScriptWidget->get_buffer_descriptions();
		if (!IO::save_case_to_file(m_SavePath, m_Case, m_ScriptWidget->get_buffers(), bdescs)) {
			// TODO: this should be more informative...
			Gtk::MessageDialog md(*this, "An error prevented a complete save of your case!", false, Gtk::MESSAGE_ERROR);
			md.run();
			
			return;
		}
		
		return;
	}
	
	// prepare file chooser dialog
	Gtk::FileChooserDialog fcd(*this, "Save Case", Gtk::FILE_CHOOSER_ACTION_SAVE);
	fcd.add_button("Save", Gtk::RESPONSE_OK);
	fcd.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	// add filter
	Gtk::FileFilter filter;
	filter.add_pattern("*.cprjt");
	filter.set_name("Case project file (*.cprjt)");
	fcd.add_filter(filter);
	
	// run the dialog
	if (fcd.run()==Gtk::RESPONSE_OK) {
		// get pathname
		Glib::ustring path=fcd.get_filename();
		
		// check for extension
		Glib::ustring ext=path.substr(path.size()-6, path.size()-1);
		ext=ext.lowercase();
		if (ext!=".cprjt")
			path+=".cprjt";
		
		// save this case
		std::map<Glib::ustring, Glib::ustring> bdescs=m_ScriptWidget->get_buffer_descriptions();
		if (!IO::save_case_to_file(path, m_Case, m_ScriptWidget->get_buffers(), bdescs)) {
			// TODO: this should be more informative...
			Gtk::MessageDialog md(*this, "An error prevented a complete save of your case!", false, Gtk::MESSAGE_ERROR);
			md.run();
			
			return;
		}
		
		// name this document
		if (!m_Saved) {
			m_Saved=true;
			m_SavePath=path;
			
			Glib::ustring title=path.substr(path.find("/")+1, path.size()-1);
			title+=" - Phoenix Wright Case Editor";
			set_title(title);
		}
	}
}

// export case handler
void MainWindow::on_export() {
	// prepare file chooser dialog
	Gtk::FileChooserDialog fcd(*this, "Export Case", Gtk::FILE_CHOOSER_ACTION_SAVE);
	fcd.add_button("Export", Gtk::RESPONSE_OK);
	fcd.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	// add filter
	Gtk::FileFilter filter;
	filter.add_pattern("*.pwt");
	filter.set_name("Phoenix Wright Trials (*.pwt)");
	fcd.add_filter(filter);
	
	// run the dialog
	if (fcd.run()==Gtk::RESPONSE_OK) {
		// get pathname
		Glib::ustring path=fcd.get_filename();
		
		// check for extension
		Glib::ustring ext=path.substr(path.size()-4, path.size()-1);
		ext=ext.lowercase();
		if (ext!=".pwt")
			path+=".pwt";
		
		// export this case
		if (!IO::export_case_to_file(path, m_Case, m_ScriptWidget->get_buffers())) {
			// TODO: more info has to be added here
			Gtk::MessageDialog md(*this, "An error prevented an export of your case!", false, Gtk::MESSAGE_ERROR);
			md.run();
		}
	}
}

// load case handler
void MainWindow::on_open() {
	// ask to save
	if (m_ScriptWidget->get_buffers().size()>2) {
		// display dialog
		Gtk::MessageDialog md(*this, "Would you like to save your current case project?", 
				       false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
		if (md.run()==Gtk::RESPONSE_YES)
			on_save();
	}
	
	// clear out the current case
	m_Case.clear();
	m_ScriptWidget->clear();
	
	// prepare file chooser dialog
	Gtk::FileChooserDialog fcd(*this, "Open Case", Gtk::FILE_CHOOSER_ACTION_OPEN);
	fcd.add_button("Open", Gtk::RESPONSE_OK);
	fcd.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	// add filter
	Gtk::FileFilter filter;
	filter.add_pattern("*.cprjt");
	filter.set_name("Case project file (*.cprjt)");
	fcd.add_filter(filter);
	
	// run the dialog
	if (fcd.run()==Gtk::RESPONSE_OK) {
		// get pathname
		Glib::ustring path=fcd.get_filename();
		
		// load the case
		BufferMap buffers;
		std::map<Glib::ustring, Glib::ustring> bufferDescriptions;
		if (!IO::load_case_from_file(path.c_str(), m_Case, buffers, bufferDescriptions)) {
			// gee, another vague error message; make it more detailed in the future
			Gtk::MessageDialog md(*this, "Unable to load case due to an unknown error.", false, Gtk::MESSAGE_ERROR);
			md.run();
		}
		
		// case information is set during load, but we need to make the rest of the widgets aware
		// first, add characters to the list view
		std::map<Glib::ustring, Character> characters=m_Case.get_characters();
		for (CharacterMapIter it=characters.begin(); it!=characters.end(); ++it)
			m_ScriptWidget->add_character((*it).first, (*it).second.get_internal_name());
		
		// now add buffers
		int i=0;
		for (BufferMap::iterator it=buffers.begin(); it!=buffers.end(); ++it) {
			// we have the block id, and we can use that to map it to a parent node
			Glib::ustring id=(*it).first;
			
			// find the first underscore from the right
			int npos=id.rfind("_");
			
			// everything before the first underscore is the id of the parent
			Glib::ustring parent=id.substr(0, npos);
			
			// now add the text block
			m_ScriptWidget->add_text_block(parent, id, bufferDescriptions[id], (*it).second);
		}
		
		// cache save path
		m_Saved=true;
		m_SavePath=path;
		
		// update title bar
		Glib::ustring title=path.substr(path.rfind("/")+1, path.size()-1);
		title+=" - Phoenix Wright Case Editor";
		set_title(title);
	}
}

// quit handler
void MainWindow::on_quit() {
	// ask to save
	Gtk::MessageDialog md(*this, "Would you like to save your current case project?", 
			       false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
	if (md.run()==Gtk::RESPONSE_YES)
		on_save();
	
	hide();
}

// add character
void MainWindow::on_case_add_char() {
	// display the appropriate character dialog
	NewCharDialog ncd(m_Case.get_character_names());
	if (ncd.run()==Gtk::RESPONSE_OK) {
		// get character data
		Character newChar=ncd.get_character_data();
		
		// add it to the case
		m_Case.add_character(newChar);
		
		// and also add it to the script widget
		m_ScriptWidget->add_character(newChar.get_name(), newChar.get_internal_name());
	}
}

// browse characters handler
void MainWindow::on_case_browse_chars() {
	// run browser dialog
	CharBrowser cb(m_Case.get_characters());
	if (cb.run()==Gtk::RESPONSE_OK) {
		// if something was changed, we need to update some data
		if (!cb.was_changed())
			return;
		
		// otherwise, get the character map
		CharacterMap cmap=cb.get_characters();
		
		// clear out old characters
		m_Case.clear_characters();
		
		// set the new ones
		for (CharacterMapIter it=cmap.begin(); it!=cmap.end(); ++it)
			m_Case.add_character((*it).second);
	}
}

// edit locations handler
void MainWindow::on_case_edit_locations() {
	// run appropriate dialog
	LocationsDialog ld(m_Case.get_locations(), m_Case.get_backgrounds(), m_Case.get_location_ids());
	if (ld.run()==Gtk::RESPONSE_OK) {
		// get updated list of locations
		LocationMap locations=ld.get_locations();
		
		// clear out location map from case
		m_Case.clear_locations();
		
		// add these locations
		for (LocationMapIter it=locations.begin(); it!=locations.end(); ++it)
			m_Case.add_location((*it).second);
	}
}

// edit case overview information
void MainWindow::on_case_edit_overview() {
	// run the case dialog
	NewDialog nd;
	
	// set our current overview
	nd.set_overview(m_Case.get_overview());
	
	// run it
	if (nd.run()==Gtk::RESPONSE_OK) {
		// update the overview
		m_Case.set_overview(nd.get_overview());
	}
}

// change initial case text block
void MainWindow::on_case_change_initial_block() {
	// make sure there are any text blocks at all
	if (m_ScriptWidget->get_buffers().empty()) {
		Gtk::MessageDialog md(*this, "There are no registered text blocks in this case!", false, Gtk::MESSAGE_ERROR);
		md.run();
		return;
	}
	
	// run the appropriate dialog
	InitialBlockDialog ibd(m_Case.get_initial_block_id(), m_ScriptWidget->get_buffers());
	if (ibd.run()==Gtk::RESPONSE_OK)
		m_Case.set_initial_block_id(ibd.get_selected_block_id());
}

// manage audio assets handler
void MainWindow::on_assets_manage_audio() {
}

// manage background assets handler
void MainWindow::on_assets_manage_bg() {
	// prepare background manager
	BackgroundsDialog bgd(m_Case.get_backgrounds(), m_Case.get_background_ids());
	
	// run this dialog
	if (bgd.run()==Gtk::RESPONSE_OK) {
		// get updated backgrounds
		BackgroundMap backgrounds=bgd.get_backgrounds();
		
		// clear out whatever backgrounds might have been saved
		m_Case.clear_backgrounds();
		
		// go over and add these backgrounds
		for (BackgroundMapIter it=backgrounds.begin(); it!=backgrounds.end(); ++it)
			m_Case.add_background((*it).second);
	}
}

// manage evidence assets handler
void MainWindow::on_assets_manage_evidence() {
	// prepare evidence manager
	EvidenceDialog ed(m_Case.get_evidence(), m_Case.get_evidence_ids());
	
	// run this dialog
	if (ed.run()==Gtk::RESPONSE_OK) {
		// get updated evidence
		EvidenceMap evidence=ed.get_evidence();
		
		// clear out previous evidence stored
		m_Case.clear_evidence();
		
		// iterate over map and add these pieces of evidence
		for (EvidenceMapIter it=evidence.begin(); it!=evidence.end(); ++it)
			m_Case.add_evidence((*it).second);
		
	}
}

// display sprite editor handler
void MainWindow::on_tools_sprite_editor() {
	// first, run the sprite chooser dialog
	SpriteChooserDialog scd;
	if (scd.run()==Gtk::RESPONSE_OK) {
		// this dialog is not needed anymore
		scd.hide();
		
		// get sprite mode
		SpriteChooserDialog::SpriteMode mode=scd.get_sprite_mode();
		
		// prepare sprite editor
		SpriteEditor se;
		
		// if we are opening a sprite, specify so now
		if (mode==SpriteChooserDialog::SPRITE_EXISTING) {
			// load the sprite from file
			Sprite spr;
			if (IO::load_sprite_from_file(scd.get_path(), spr))
				se.set_sprite_data(spr);
			else {
				Gtk::MessageDialog md(*this, "Unable to open sprite.", false, Gtk::MESSAGE_ERROR);
				md.run();
				return;
			}
		}
		
		// run the dialog
		int ret=se.run();
		if (ret==Gtk::RESPONSE_OK) {
			// get the sprite
			Sprite sprite=se.get_sprite_data();
			
			// prepare file chooser
			Gtk::FileChooserDialog fcd(*this, "Save Sprite", Gtk::FILE_CHOOSER_ACTION_SAVE);
			fcd.add_button("Save", Gtk::RESPONSE_OK);
			fcd.add_button("Cancel", Gtk::RESPONSE_CANCEL);
			
			// add a filter
			Gtk::FileFilter filter;
			filter.add_pattern("*.spr");
			filter.set_name("Sprites (*.spr)");
			fcd.add_filter(filter);
			
			// run the dialog
			if (fcd.run()==Gtk::RESPONSE_OK) {
				// get the path
				Glib::ustring path=fcd.get_filename();
				
				// check extension
				Glib::ustring ext=path.substr(path.size()-4, path.size()-1);
				ext=ext.lowercase();
				if (ext!=".spr")
					path+=".spr";
				
				// save this sprite
				if (!IO::save_sprite_to_file(path, sprite)) {
					// yet another vague error
					Gtk::MessageDialog md(*this, "Unable to save sprite.", false, Gtk::MESSAGE_ERROR);
					md.run();
				}
			}
		}
		
		// export request
		else if (ret==Gtk::RESPONSE_YES) {
			// get the sprite
			Sprite sprite=se.get_sprite_data();
			
			// prepare file chooser
			Gtk::FileChooserDialog fcd(*this, "Export Sprite", Gtk::FILE_CHOOSER_ACTION_SAVE);
			fcd.add_button("Export", Gtk::RESPONSE_OK);
			fcd.add_button("Cancel", Gtk::RESPONSE_CANCEL);
			
			// add a filter
			Gtk::FileFilter filter;
			filter.add_pattern("*.pws");
			filter.set_name("Phoenix Wright sprites (*.pws)");
			fcd.add_filter(filter);
			
			// run the dialog
			if (fcd.run()==Gtk::RESPONSE_OK) {
				// get the path
				Glib::ustring path=fcd.get_filename();
				
				// check extension
				Glib::ustring ext=path.substr(path.size()-4, path.size()-1);
				ext=ext.lowercase();
				if (ext!=".pws")
					path+=".pws";
				
				// save this sprite
				if (!IO::export_sprite_to_file(path, sprite)) {
					// yet another vague error
					Gtk::MessageDialog md(*this, "Unable to export sprite.", false, Gtk::MESSAGE_ERROR);
					md.run();
				}
			}
		}
	}
}

// display info about this application
void MainWindow::on_help_about() {
	Gtk::AboutDialog ad;
	
	// set data
	std::vector<std::string> authors;
	authors.push_back("Mike Polan (kanadakid@gmail.com)");
	ad.set_authors(authors);
	
	ad.set_name("Phoenix Wright Case Editor");
	ad.set_version("1.0");
	
	ad.set_comments("This is an editor that creates cases/trials capable of being played by the Phoenix Wright Case Player.");
	
	ad.run();
}
