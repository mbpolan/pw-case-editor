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

#include <iostream>
#include <glibmm/exception.h>
#include <gtkmm/aboutdialog.h>
#include <gtkmm/box.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/menubar.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/recentchoosermenu.h>
#include <gtkmm/separatortoolitem.h>
#include <gtkmm/stock.h>
#include <gtkmm/toolbar.h>
#include <sstream>

#include "customizedialog.h"
#include "dialogs.h"
#include "exceptions.h"
#include "iohandler.h"
#include "mainwindow.h"
#include "spriteeditor.h"
#include "testimonyeditor.h"
#include "textboxdialog.h"
#include "triggerdialogs.h"
#include "utilities.h"

ElementEx g_ElementEx;

MainWindow *g_MainWnd;

// constructor
MainWindow::MainWindow() {
	// set the title and minimum window size
	set_title("Unsaved File - Phoenix Wright Case Editor");
	set_size_request(640, 480);
	
	// register our window as the global pointer
	g_MainWnd=this;
	
	// center it up
	int x, y;
	Utils::calculate_center(640, 480, x, y);
	move(x, y);
	
	// reset save variables
	m_Saved=false;
	m_SavePath="";
	
	construct();
	
	// now we can safely delete the resource temp directory
	Utils::FS::remove_dir(Utils::FS::cwd()+".temp");
}

// build the ui
void MainWindow::construct() {
	// allocate vbox
	Gtk::VBox *vb=manage(new Gtk::VBox);
	
	// create our application icons
	m_IconMgr.create_from_file(Utils::FS::cwd()+"dat.dpkg");
	
	// allocate action group
	m_ActionGroup=Gtk::ActionGroup::create();
	
	// populate it
	m_ActionGroup->add(Gtk::Action::create("FileNew", Gtk::Stock::NEW, "_New", "Create a new case"),
			   sigc::mem_fun(*this, &MainWindow::on_new));
	m_ActionGroup->add(Gtk::Action::create("FileSave", Gtk::Stock::SAVE, "_Save"),
			   sigc::mem_fun(*this, &MainWindow::on_save));
	m_ActionGroup->add(Gtk::Action::create("FileSaveAs", Gtk::Stock::SAVE_AS, "_Save As"),
			   Gtk::AccelKey("<control><shift>s", "<MainWindow>/FileMenu/FileSaveAs"),
			   sigc::mem_fun(*this, &MainWindow::on_save_as));
	m_ActionGroup->add(Gtk::Action::create("FileOpen", Gtk::Stock::OPEN, "_Open"),
			   sigc::mem_fun(*this, &MainWindow::on_open));
	m_ActionGroup->add(Gtk::Action::create("FileExport", Gtk::Stock::CONVERT, "_Export"),
			   Gtk::AccelKey("<control>e", "<MainWindow>/FileMenu/FileExport"),
			   sigc::mem_fun(*this, &MainWindow::on_export));
	m_ActionGroup->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT, "_Quit"),
			   sigc::mem_fun(*this, &MainWindow::on_quit));
	
	m_ActionGroup->add(Gtk::Action::create("ScriptInsertDialogue", AppStock::INSERT_DIALOGUE, "_Insert Dialogue"),
			   sigc::mem_fun(*this, &MainWindow::on_script_insert_dialogue));
	
	m_ActionGroup->add(Gtk::Action::create("CaseAddChar", AppStock::ADD_CHARACTER, "_Add Character"),
			   sigc::mem_fun(*this, &MainWindow::on_case_add_char));
	m_ActionGroup->add(Gtk::Action::create("CaseBrowseChar", AppStock::BROWSE_CHARS, "_Browse Characters"),
			   sigc::mem_fun(*this, &MainWindow::on_case_browse_chars));
	m_ActionGroup->add(Gtk::Action::create("CaseManageTestimonies", AppStock::TESTIMONY, "_Manage Testimonies"),
			   sigc::mem_fun(*this, &MainWindow::on_case_manage_testimonies));
	m_ActionGroup->add(Gtk::Action::create("CaseEditLocations", AppStock::LOCATION, "_Edit Locations"),
			   sigc::mem_fun(*this, &MainWindow::on_case_edit_locations));
	m_ActionGroup->add(Gtk::Action::create("CaseEditOverview", Gtk::Stock::PROPERTIES, "_Edit Overview"),
			   sigc::mem_fun(*this, &MainWindow::on_case_edit_overview));
	m_ActionGroup->add(Gtk::Action::create("CaseInitialBlock", AppStock::INITBLOCK, "_Initial Text Block"),
			   sigc::mem_fun(*this, &MainWindow::on_case_change_initial_block));
	m_ActionGroup->add(Gtk::Action::create("CaseCustomize", "_Customize"),
			   sigc::mem_fun(*this, &MainWindow::on_case_customize));
	
	m_ActionGroup->add(Gtk::Action::create("AssetsManageAudio", AppStock::AUDIO, "Manage _Audio"),
			   sigc::mem_fun(*this, &MainWindow::on_assets_manage_audio));
	m_ActionGroup->add(Gtk::Action::create("AssetsManageBG", AppStock::BACKGROUND, "Manage _Backgrounds"),
			   sigc::mem_fun(*this, &MainWindow::on_assets_manage_bg));
	m_ActionGroup->add(Gtk::Action::create("AssetsManageEvidence", AppStock::EVIDENCE, "Manage _Evidence"),
			   sigc::mem_fun(*this, &MainWindow::on_assets_manage_evidence));
	m_ActionGroup->add(Gtk::Action::create("AssetsManageImages", AppStock::IMAGE, "Manage _Images"),
			   sigc::mem_fun(*this, &MainWindow::on_assets_manage_images));
	
	m_ActionGroup->add(Gtk::Action::create("ToolsSpriteEditor", "_Sprite Editor"),
			   sigc::mem_fun(*this, &MainWindow::on_tools_sprite_editor));
	
	m_ActionGroup->add(Gtk::Action::create("HelpAbout", Gtk::Stock::ABOUT, "_About"),
			   sigc::mem_fun(*this, &MainWindow::on_help_about));
	
	m_ActionGroup->add(Gtk::Action::create("FileMenu", "_File"));
	m_ActionGroup->add(Gtk::Action::create("ScriptMenu", "_Script"));
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
			"		<menuitem action='FileSaveAs'/>"
			"		<menuitem action='FileOpen'/>"
			"		<separator/>"
			"		<separator/>"
			"		<menuitem action='FileExport'/>"
			"		<separator/>"
			"		<menuitem action='FileQuit'/>"
			"	</menu>"
			"	<menu action='ScriptMenu'>"
			"		<menuitem action='ScriptInsertDialogue'/>"
			"	</menu>"
			"	<menu action='CaseMenu'>"
			"		<menuitem action='CaseAddChar'/>"
			"		<menuitem action='CaseBrowseChar'/>"
			"		<separator/>"
			"		<menuitem action='CaseManageTestimonies'/>"
			"		<separator/>"
			"		<menuitem action='CaseInitialBlock'/>"
			"		<menuitem action='CaseEditLocations'/>"
			"		<menuitem action='CaseEditOverview'/>"
			"		<separator/>"
			"		<menuitem action='CaseCustomize'/>"
			"	</menu>"
			"	<menu action='AssetsMenu'>"
			"		<menuitem action='AssetsManageAudio'/>"
			"		<menuitem action='AssetsManageBG'/>"
			"		<menuitem action='AssetsManageEvidence'/>"
			"		<menuitem action='AssetsManageImages'/>"
			"	</menu>"
			"	<menu action='ToolsMenu'>"
			"		<menuitem action='ToolsSpriteEditor'/>"
			"	</menu>"
			"	<menu action='HelpMenu'>"
			"		<menuitem action='HelpAbout'/>"
			"	</menu>"
			"</menubar>"
			"<toolbar name='Standard'>"
			"	<toolitem action='FileNew'/>"
			"	<toolitem action='FileSave'/>"
			"	<toolitem action='FileSaveAs'/>"
			"	<toolitem action='FileOpen'/>"
			"	<toolitem action='FileExport'/>"
			"</toolbar>"
			"</ui>";
	
	// add this ui
	m_UIManager->add_ui_from_string(ui);
	
	// get the menu bar widget
	Gtk::MenuBar *menuBar=dynamic_cast<Gtk::MenuBar*> (m_UIManager->get_widget("/MenuBar"));
	if (menuBar) {
		vb->pack_start(*manage(menuBar), Gtk::PACK_SHRINK);
		
		Gtk::Menu *tmenu=new Gtk::Menu;
		
		// we need to manually add a trigger submenu to the Script menu
		Gtk::Menu *refMenu=menuBar->items()[1].get_submenu();
		
		// grab the list of items and add a new submenu
		Gtk::Menu_Helpers::MenuList &list=refMenu->items();
		Gtk::Menu_Helpers::MenuList::iterator it=list.begin(); it++;
		list.insert(it, Gtk::Menu_Helpers::MenuElem("Insert Trigger", *manage(tmenu)));
		list.insert(it, Gtk::Menu_Helpers::SeparatorElem());
		
		// finally, create the menu
		create_trigger_submenu(tmenu);
		
		m_RecentMenu=manage(new Gtk::Menu);
		
		// get the file menu
		refMenu=menuBar->items()[0].get_submenu();
		
		// get the list of items, and increment the iterator into position
		list=refMenu->items();
		it=list.begin();
		for (int i=0; i<5; i++)
			it++; // puts us into the 5th row, below Open
		
		// now insert a separator, and the actual recent files menu
		list.insert(it, Gtk::Menu_Helpers::SeparatorElem());
		list.insert(it, Gtk::Menu_Helpers::MenuElem("Open Recent", *m_RecentMenu));
		
		// read the recent files record
		if (IO::read_recent_files(m_RecentFiles)) {
			list=m_RecentMenu->items();
			
			// go over each file
			for (int i=0; i<m_RecentFiles.size(); i++) {
				// create an image for the menu from stock
				Gtk::Image *image=manage(new Gtk::Image);
				Gtk::Stock::lookup(Gtk::StockID(Gtk::Stock::FILE), Gtk::IconSize(Gtk::ICON_SIZE_MENU), *image);
				
				// and add it as a recent file with said image
				list.push_back(Gtk::Menu_Helpers::ImageMenuElem(m_RecentFiles[i].second, *image,
					       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_open_recent), m_RecentFiles[i].first)));
			}
		}
	}
	
	// get the toolbar widgets
	Gtk::Toolbar *standardTB=(Gtk::Toolbar*) m_UIManager->get_widget("/Standard");
	if (standardTB) {
		standardTB->set_toolbar_style(Gtk::TOOLBAR_ICONS);
		standardTB->set_tooltips(true);
		
		// allocate separator between standard buttons and script buttons
		Gtk::SeparatorToolItem *item=manage(new Gtk::SeparatorToolItem);
		standardTB->append(*item);
		
		// allocate script buttons
		Gtk::ToolButton *insertDiagButton=manage(new Gtk::ToolButton(*manage(new Gtk::Image(AppStock::INSERT_DIALOGUE, Gtk::ICON_SIZE_LARGE_TOOLBAR))));
		
		// append script buttons
		standardTB->append(*insertDiagButton, sigc::mem_fun(*this, &MainWindow::on_script_insert_dialogue));
		
		vb->pack_start(*manage(standardTB), Gtk::PACK_SHRINK);
	}
	
	// allocate script widget
	m_ScriptWidget=manage(new ScriptWidget);
	
	// allocate status bar
	m_Statusbar=manage(new Gtk::Statusbar);
	
	// pack widgets
	vb->pack_start(*m_ScriptWidget);
	vb->pack_start(*m_Statusbar, Gtk::PACK_SHRINK);
	
	// add the containers
	add(*vb);
	show_all_children();
}

// alert the user is there are not enough items in an element to perform an action
bool MainWindow::check_case_element(const Glib::ustring &element, int amount) {
	// format an error string, if needed
	std::stringstream ss;
	ss << "You need at least " << amount;
	
	// flag if we failed the check
	bool fail=false;
	
	// check amount of characters
	try {
	if (element=="characters") {
		if (m_Case.get_characters().size()<amount) {
			ss << " character(s)";
			fail=true;
		}
	}
	
	// check amount of locations
	else if (element=="locations") {
		if (m_Case.get_locations().size()<amount) {
			ss << " location(s)";
			fail=true;
		}
	}
	
	// check amount of evidence
	else if (element=="evidence") {
		if (m_Case.get_evidence().size()<amount) {
			ss << " piece(s) of evidence";
			fail=true;
		}
	}
	
	// check amount of text blocks
	else if (element=="blocks") {
		if (m_ScriptWidget->get_buffers().size()<amount) {
			ss << " text block(s)";
			fail=true;
		}
	}
	
	// check amount of audio
	else if (element=="audio") {
		if (m_Case.get_audio().size()<amount) {
			ss << " audio sample(s)";
			fail=true;
		}
	}
	
	// check images
	else if (element=="images") {
		if (m_Case.get_images().size()<amount) {
			ss << " image(s)";
			fail=true;
		}
	}
	
	// check amount of testimonies
	else if (element=="testimonies") {
		if (m_Case.get_testimonies().size()<amount) {
			if (amount==1)
				ss << " testimony";
			else
				ss << " testimonies";
			
			fail=true;
		}
	}
	
	// not a real element? O_o
	else
		throw g_ElementEx;
	
	} // try
	
	// catch the one and only exception that can possibly occur
	catch (ElementEx &ex) {
		Gtk::MessageDialog md(*this, Utils::exception_string(ex.what(), __FILE__, __LINE__), false, Gtk::MESSAGE_ERROR);
		md.run();
		abort();
	}
	
	// looks like we failed the check, show an error dialog
	if (fail) {
		ss << " in your case.";
		Gtk::MessageDialog md(*this, ss.str(), false, Gtk::MESSAGE_ERROR);
		md.run();
	}
	
	return !fail; // correct return value
}

// create the trigger submenu
void MainWindow::create_trigger_submenu(Gtk::Menu *menu) {
	// get the list of items
	Gtk::Menu_Helpers::MenuList &list=menu->items();
	
	// populate the menu
	list.push_back(Gtk::Menu_Helpers::MenuElem("Speaker", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "speaker")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Goto", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "goto")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Add Evidence/Profile", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "add_evidence/profile")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Show Evidence", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "show_evidence")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Hide Evidence", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "hide_evidence")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Set Location", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "set_location")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Add Location", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "add_location")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Set Location Trigger", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "set_location_trigger")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Set Animation", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "set_animation")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Put Character", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "put_character")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Add Talk Option", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "add_talk_option")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Remove Talk Option", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "remove_talk_option")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Clear Talk Options", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "clear_talk_options")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Add Presentable", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "add_presentable")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Remove Presentable", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "remove_presentable")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Clear Presentables", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "clear_presentables")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Set Bad Presentable Block", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "set_bad_presentable_block")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Set Location Music", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "set_location_music")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Clear Location Music", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "clear_location_music")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Request Evidence", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "request_evidence")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Request Answer", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "request_answer")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Play Music", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "play_music")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Halt Music", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "halt_music")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Sound Effect", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "sfx")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Fade Out", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "fade_out")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Flash", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "flash")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Special Effect", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "special_effect")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Move Court Camera", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "move_court_camera")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Set Court Overview Image", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "set_court_overview_image")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Set Temp Image", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "set_temp_image")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Hide Temp Image", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "hide_temp_image")));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Display Testimony", 
		       sigc::bind(sigc::mem_fun(*this, &MainWindow::on_script_insert_trigger), "display_testimony")));
}

// set an icon for a menu item
void MainWindow::set_menuitem_icon(const Glib::ustring &path, const Gtk::StockID &id) {
	// get the menu item
	Gtk::ImageMenuItem *item=dynamic_cast<Gtk::ImageMenuItem*> (m_UIManager->get_widget(path));
	if (item) {
		// create the icon
		Gtk::Image *img=manage(new Gtk::Image(id, Gtk::ICON_SIZE_MENU));
		item->set_image(*img);
	}
}

// process a loaded case
bool MainWindow::process_load_case(const Glib::ustring &path) {
	// prepare variables
	BufferMap buffers;
	std::map<Glib::ustring, Glib::ustring> bufferDescriptions;
	
	// and load the case
	IO::Code code;
	if ((code=IO::load_case_from_file(path.c_str(), m_Case, buffers, bufferDescriptions))!=IO::CODE_OK) {
		// format error message
		Glib::ustring msg="Unable to load requested case.\nReason:";
		msg+=Utils::io_error_to_str(code);
		
		// show a dialog
		Gtk::MessageDialog md(*this, msg, false, Gtk::MESSAGE_ERROR);
		md.run();
		
		m_Statusbar->push("Unable to open case file");
		return false;
	}
	
	// clear out the current case
	m_ScriptWidget->clear(m_Case.get_overview().lawSys);
	
	// case information is set during load, but we need to make the rest of the widgets aware
	// first, add characters to the list view
	CharacterMap characters=m_Case.get_characters();
	for (CharacterMap::iterator it=characters.begin(); it!=characters.end(); ++it)
		m_ScriptWidget->add_character(-1, -1, (*it).first, (*it).second.get_internal_name());
		
	// now add buffers
	for (BufferMap::iterator it=buffers.begin(); it!=buffers.end(); ++it) {
		// we have the block id, and we can use that to map it to a parent node
		Glib::ustring id=(*it).first;
		
		// extract the real id
		Glib::ustring realId=id.substr(0, id.rfind("_"));
		
		// find the first underscore from the right
		int npos=id.rfind("_");
		Glib::ustring rootId=id.substr(npos+1, id.size()-npos);
		id.erase(npos, id.size()-npos);
		
		// get the stage and day identifiers
		int stage=(rootId[0]==(char) 'i' ? 0 : 1);
		int day=atoi(rootId.substr(1, rootId.size()).c_str());
		
		// find the first underscore from the right
		npos=id.rfind("_");
		
		// everything before the first underscore is the id of the parent
		Glib::ustring parent=id.substr(0, npos);
		
		// now add the text block
		m_ScriptWidget->add_text_block(stage, day, parent, id, bufferDescriptions[realId], (*it).second);
	}
	
	return true;
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
		// reset save variables
		m_Saved=false;
		m_SavePath="";
		
		// reset title bar
		set_title("Unsaved File - Phoenix Wright Case Editor");
		
		// get the updated case overview
		Case::Overview overview=nd.get_overview();
		
		// clear out any previous case data
		m_Case.clear();
		m_ScriptWidget->clear(overview.lawSys);
		
		// and apply this new overview
		m_Case.set_overview(overview);
	}
}

// save case handler
void MainWindow::on_save() {
	// save automatically if we have a path set
	if (m_Saved) {
		m_Statusbar->push("Saving to "+m_SavePath);
		
		// save this case
		std::map<Glib::ustring, Glib::ustring> bdescs=m_ScriptWidget->get_buffer_descriptions();
		IO::Code code;
		if ((code=IO::save_case_to_file(m_SavePath, m_Case, m_ScriptWidget->get_buffers(), bdescs))!=IO::CODE_OK) {
			// format error string
			Glib::ustring msg="An error prevented a complete save of your case!\nReason:";
			msg+=Utils::io_error_to_str(code);
			
			// show dialog
			Gtk::MessageDialog md(*this, msg, false, Gtk::MESSAGE_ERROR);
			md.run();
			
			return;
		}
		
		m_Statusbar->push("Case saved successfully");
		
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
		
		m_Statusbar->push("Saving...");
		
		// save this case
		std::map<Glib::ustring, Glib::ustring> bdescs=m_ScriptWidget->get_buffer_descriptions();
		IO::Code code;
		if ((code=IO::save_case_to_file(path, m_Case, m_ScriptWidget->get_buffers(), bdescs))!=IO::CODE_OK) {
			// format the error message
			Glib::ustring msg="An error prevented a complete save of your case!\nReason:";
			msg+=Utils::io_error_to_str(code);
			
			// show dialog
			Gtk::MessageDialog md(*this, msg, false, Gtk::MESSAGE_ERROR);
			md.run();
			
			m_Statusbar->push("Unable to save file");
			
			return;
		}
		
		m_Statusbar->push("File saved");
		
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

// save as handler
void MainWindow::on_save_as() {
	// remove the saved flag
	m_Saved=false;
	
	// save the case
	on_save();
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
		
		m_Statusbar->push("Exporting case to file...");
		
		// export this case
		IO::Code code;
		if ((code=IO::export_case_to_file(path, m_Case, m_ScriptWidget->get_buffers()))!=IO::CODE_OK) {
			// format the error message
			Glib::ustring msg="An error prevented an export of your case!\n";
			msg+="Reason: ";
			msg+=Utils::io_error_to_str(code);
			
			// show a dialog
			Gtk::MessageDialog md(*this, msg, false, Gtk::MESSAGE_ERROR);
			md.run();
			
			m_Statusbar->push("Unable to export case");
			return;
		}
		
		m_Statusbar->push("Exported case successfully");
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
		
		// clear the case out
		m_Case.clear();
		
		m_Statusbar->push("Opening file...");
		
		// load the case
		if (!process_load_case(path))
			return;
		
		m_Statusbar->push("Case opened successfully");
		
		// extract file name
		Glib::ustring title=path.substr(path.rfind("/")+1, path.size()-1);
		
		// add this file as a recent file
		IO::add_recent_file(path, title);
		
		// cache save path
		m_Saved=true;
		m_SavePath=path;
		
		// update title bar
		title+=" - Phoenix Wright Case Editor";
		set_title(title);
	}
}

// open a recent file
void MainWindow::on_open_recent(const Glib::ustring &path) {
	if (process_load_case(path)) {
		// update the title bar
		Glib::ustring title=path.substr(path.rfind("/")+1, path.size()-1);
		title+=" - Phoenix Wright Case Editor";
		set_title(title);
		
		// cache save path
		m_Saved=true;
		m_SavePath=path;
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

// add a formatted dialogue to script
void MainWindow::on_script_insert_dialogue() {
	// run the dialog
	TextBoxDialog tbd;
	
	// if the user clicked ok, insert the text
	if (tbd.run()==Gtk::RESPONSE_OK) {
		// get the text
		Glib::ustring text=tbd.get_text();
		
		// get the current buffer
		Glib::RefPtr<Gtk::TextBuffer> buffer=m_ScriptWidget->get_current_buffer();
		if (buffer)
			buffer->insert_at_cursor(text);
	}
}

// insert a trigger into block
void MainWindow::on_script_insert_trigger(const Glib::ustring &trigger) {
	if (trigger=="goto") {
		if (!check_case_element("blocks", 1))
			return;
		
		// run the dialog, and get data
		GotoDialog diag(m_ScriptWidget->get_buffers());
		if (diag.run()==Gtk::RESPONSE_OK) {
			// get the data
			GotoDialog::Data data=diag.get_data();
			
			// begin trigger string
			Glib::ustring trig="{*";
			
			// complete is based on goto type
			switch(data.second) {
				case GotoDialog::GOTO_NORMAL: {
					trig+="goto:";
					trig+=data.first;
					trig+=";*}";
				} break;
				
				case GotoDialog::GOTO_DIRECT: {
					trig+="direct_goto:";
					trig+=data.first;
					trig+=";*}";
				} break;
				
				case GotoDialog::GOTO_TIMED: {
					trig+="timed_goto:";
					trig+=data.first;
					trig+=",";
					trig+=Utils::to_string(data.third);
					trig+=";*}";
				} break;
			}
			
			// finally, insert it
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// add_evidence/add_profile triggers
	else if (trigger=="add_evidence/profile") {
		AddCourtRecDialog diag(m_Case.get_evidence(), m_Case.get_characters());
		if (diag.run()==Gtk::RESPONSE_OK) {
			AddCourtRecDialog::Data data=diag.get_data();
			
			Glib::ustring trig="{*add_";
			
			if (data.type==AddCourtRecDialog::TYPE_ADD_EVIDENCE)
				trig+="evidence:";
			else
				trig+="profile:";
			
			trig+=data.id;
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// show evidence
	else if (trigger=="show_evidence") {
		if (!check_case_element("evidence", 1))
			return;
		
		// prepare dialog
		ShowEvidenceDialog diag(m_Case.get_evidence());
		if (diag.run()==Gtk::RESPONSE_OK) {
			ShowEvidenceDialog::Data data=diag.get_data();
			
			Glib::ustring trig="{*show_evidence:";
			trig+=data.first;
			trig+=",";
			
			if (data.second==ShowEvidenceDialog::TYPE_POS_LEFT)
				trig+="left";
			else
				trig+="right";
			
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// hide evidence
	else if (trigger=="hide_evidence") {
		HideEvidenceDialog diag;
		if (diag.run()==Gtk::RESPONSE_OK) {
			Glib::ustring trig="{*hide_evidence:";
			
			if (diag.get_position()==HideEvidenceDialog::TYPE_POS_LEFT)
				trig+="left";
			else
				trig+="right";
			
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// set location
	else if (trigger=="set_location") {
		if (!check_case_element("locations", 1))
			return;
		
		SetLocationDialog diag(m_Case.get_locations());
		if (diag.run()==Gtk::RESPONSE_OK) {
			Glib::ustring trig="{*set_location:";
			trig+=diag.get_location();
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// add location
	else if (trigger=="add_location") {
		if (!check_case_element("locations", 2))
			return;
		
		// prepare dialog and run it
		AddLocationDialog diag(m_Case.get_locations());
		if (diag.run()==Gtk::RESPONSE_OK) {
			// get the selected locations
			StringPair p=diag.get_location_pair();
			
			// do some error checking
			if (p.first==p.second)
				return;
			
			Glib::ustring trig="{*add_location:";
			trig+=p.first;
			trig+=",";
			trig+=p.second;
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// set location trigger
	else if (trigger=="set_location_trigger") {
		LocationMap locations=m_Case.get_locations();
		BufferMap blocks=m_ScriptWidget->get_buffers();
		
		// make sure we have at least one location
		if (!check_case_element("locations", 1))
			return;
		
		// and at least one block
		if (!check_case_element("blocks", 1))
			return;
		
		// prepare and run the dialog
		LocationTriggerDialog diag(locations, blocks);
		if (diag.run()==Gtk::RESPONSE_OK) {
			// get the selected location and buffer
			StringPair p=diag.get_selection();
			
			Glib::ustring trig="{*set_location_trigger:";
			trig+=p.first;
			trig+=",";
			trig+=p.second;
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// set animation
	else if (trigger=="set_animation") {
		if (!check_case_element("characters", 1))
			return;
		
		// prepare the dialog
		SetAnimationDialog diag(m_Case.get_characters());
		if (diag.run()==Gtk::RESPONSE_OK) {
			// get the selected character and animation
			StringPair p=diag.get_selection();
			
			Glib::ustring trig="{*set_animation:";
			trig+=p.first;
			trig+=",";
			trig+=p.second;
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// put character
	else if (trigger=="put_character") {
		if (!check_case_element("locations", 1) || !check_case_element("characters", 1))
			return;
		
		// prepare dialog
		PutCharDialog diag(m_Case.get_characters(), m_Case.get_locations());
		if (diag.run()==Gtk::RESPONSE_OK) {
			// get the selected character and location
			StringPair p=diag.get_selection();
			
			Glib::ustring trig="{*put_character:";
			trig+=p.first;
			trig+=",";
			trig+=p.second;
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// add talk option
	else if (trigger=="add_talk_option") {
		if (!check_case_element("characters", 1))
			return;
		
		// prepare dialog
		AddTalkDialog diag(m_Case.get_characters(), m_ScriptWidget->get_buffers());
		if (diag.run()==Gtk::RESPONSE_OK) {
			// get the inputted data
			StringTriplet t=diag.get_data();
			
			Glib::ustring trig="{*add_talk_option:";
			trig+=t.first;
			trig+=",";
			trig+=t.second;
			trig+=",";
			trig+=t.third;
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// remove talk option
	else if (trigger=="remove_talk_option") {
		if (!check_case_element("characters", 1))
			return;
		
		// prepare dialog
		RemoveTalkDialog diag(m_Case.get_characters());
		if (diag.run()==Gtk::RESPONSE_OK) {
			// get the inputted data
			StringPair p=diag.get_data();
			
			Glib::ustring trig="{*remove_talk_option:";
			trig+=p.first;
			trig+=",";
			trig+=p.second;
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// clear talk options or presentables
	else if (trigger=="clear_talk_options" || trigger=="clear_presentables") {
		if (!check_case_element("characters", 1))
			return;
		
		// prepare dialog
		ClearCharDialog diag(trigger, m_Case.get_characters());
		if (diag.run()==Gtk::RESPONSE_OK) {
			Glib::ustring trig="{*";
			trig+=trigger;
			trig+=":";
			trig+=diag.get_character();
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// add presentable trigger
	else if (trigger=="add_presentable") {
		if (!check_case_element("characters", 1) || !check_case_element("blocks", 1))
			return;
		
		// prepare dialog
		AddPresentDialog diag(m_Case.get_characters(), m_ScriptWidget->get_buffers());
		if (diag.run()==Gtk::RESPONSE_OK) {
			// get the inputted data
			StringTriplet t=diag.get_data();
			
			Glib::ustring trig="{*add_presentable:";
			trig+=t.first;
			trig+=",";
			trig+=t.second;
			trig+=",";
			trig+=t.third;
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// remove presentable trigger
	else if (trigger=="remove_presentable") {
		if (!check_case_element("characters", 1))
			return;
		
		// prepare the dialog
		RemovePresentDialog diag(m_Case.get_characters());
		if(diag.run()==Gtk::RESPONSE_OK) {
			// get the data
			StringPair p=diag.get_data();
			
			Glib::ustring trig="{*remove_presentable:";
			trig+=p.first;
			trig+=",";
			trig+=p.second;
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// set bad presentable block trigger
	else if (trigger=="set_bad_presentable_block") {
		if (!check_case_element("characters", 1) || !check_case_element("blocks", 1))
			return;
		
		// prepare the dialog
		BadPresentDialog diag(m_Case.get_characters(), m_ScriptWidget->get_buffers());
		if (diag.run()==Gtk::RESPONSE_OK) {
			// get the data
			StringPair p=diag.get_data();
			
			Glib::ustring trig="{*set_bad_presentable_block:";
			trig+=p.first;
			trig+=",";
			trig+=p.second;
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// set location music trigger
	else if (trigger=="set_location_music") {
		if (!check_case_element("locations", 1) || !check_case_element("audio", 1))
			return;
		
		// prepare dialog
		LocMusicDialog diag(m_Case.get_locations(), m_Case.get_audio());
		if (diag.run()==Gtk::RESPONSE_OK) {
			// get selection data
			StringPair p=diag.get_data();
			
			Glib::ustring trig="{*set_location_music:";
			trig+=p.first;
			trig+=",";
			trig+=p.second;
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// clear location music trigger
	else if (trigger=="clear_location_music") {
		if (!check_case_element("locations", 1))
			return;
		
		// prepare dialog
		SetLocationDialog diag(m_Case.get_locations());
		if (diag.run()==Gtk::RESPONSE_OK) {
			Glib::ustring trig="{*clear_location_music:";
			trig+=diag.get_location();
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// speaker trigger
	else if (trigger=="speaker") {
		if (!check_case_element("characters", 1))
			return;
		
		// prepare dialog
		SpeakerDialog diag(m_Case.get_characters());
		if (diag.run()==Gtk::RESPONSE_OK) {
			Glib::ustring trig="{*speaker:";
			trig+=diag.get_speaker();
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// play music trigger
	else if (trigger=="play_music") {
		if (!check_case_element("audio", 1))
			return;
		
		// prepare dialog
		MusicDialog diag(m_Case.get_audio());
		if (diag.run()==Gtk::RESPONSE_OK) {
			Glib::ustring trig="{*play_music:";
			trig+=diag.get_audio();
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// halt_music trigger
	else if (trigger=="halt_music")
		m_ScriptWidget->insert_trigger_at_cursor("{*halt_music:true;*}");
	
	// request evidence trigger
	else if (trigger=="request_evidence") {
		if (!check_case_element("blocks", 2))
			return;
		
		// prepare dialog
		ReqEvidenceDialog diag(m_ScriptWidget->get_buffers());
		if (diag.run()==Gtk::RESPONSE_OK) {
			// get the data
			StringTriplet t=diag.get_data();
			
			Glib::ustring trig="{*request_evidence:";
			trig+=t.first;
			trig+=",";
			trig+=t.second;
			trig+=",";
			trig+=t.third;
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// request answer trigger
	else if (trigger=="request_answer") {
		ReqAnswerDialog diag;
		if (diag.run()==Gtk::RESPONSE_OK) {
			// get talk options
			std::vector<StringPair> vec=diag.get_talk_options();
			
			Glib::ustring trig="{*request_answer:";
			
			// iterate over talk options and format each
			for (int i=0; i<vec.size(); i++) {
				if (i!=0)
					trig+=",";
				trig+="(";
				trig+=vec[i].first;
				trig+=",";
				trig+=vec[i].second;
				trig+=")";
			}
			
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// move court camera trigger
	else if (trigger=="move_court_camera") {
		CourtCamDialog diag;
		if (diag.run()==Gtk::RESPONSE_OK) {
			// get data
			StringPair p=diag.get_data();
			
			Glib::ustring trig="{*move_court_camera:";
			trig+=p.first;
			trig+=",";
			trig+=p.second;
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// fade out trigger
	else if (trigger=="fade_out") {
		FadeDialog diag;
		if (diag.run()==Gtk::RESPONSE_OK) {
			Glib::ustring trig="{*fade_out:";
			trig+=diag.get_selection();
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// flash
	else if (trigger=="flash")
		m_ScriptWidget->insert_trigger_at_cursor("{*flash:top;*}");
	
	// set an image for court overview
	else if (trigger=="set_court_overview_image") {
		if (!check_case_element("images", 1))
			return;
		
		// prepare dialog
		OverImageDialog diag(m_Case.get_images());
		if (diag.run()==Gtk::RESPONSE_OK) {
			// get the data
			StringPair p=diag.get_data();
			
			Glib::ustring trig="{*set_court_overview_image:";
			trig+=p.first;
			trig+=",";
			trig+=p.second;
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// show temporary image
	else if (trigger=="set_temp_image") {
		if (!check_case_element("images", 1))
			return;
		
		// prepare dialog
		TempImgDialog diag(m_Case.get_images());
		if (diag.run()==Gtk::RESPONSE_OK) {
			Glib::ustring trig="{*set_temp_image:";
			trig+=diag.get_image();
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
	
	// hide temporary image
	else if (trigger=="hide_temp_image")
		m_ScriptWidget->insert_trigger_at_cursor("{*hide_temp_image:true;*}");
	
	// display a testimony
	else if (trigger=="display_testimony") {
		if (!check_case_element("testimonies", 1))
			return;
		
		// prepare dialog
		DisplayTestimonyDialog diag(m_Case.get_testimonies());
		if (diag.run()==Gtk::RESPONSE_OK) {
			Glib::ustring trig="{*display_testimony:";
			trig+=diag.get_testimony();
			trig+=";*}";
			
			m_ScriptWidget->insert_trigger_at_cursor(trig);
		}
	}
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
		m_ScriptWidget->add_character(-1, -1, newChar.get_name(), newChar.get_internal_name());
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
		for (CharacterMap::iterator it=cmap.begin(); it!=cmap.end(); ++it)
			m_Case.add_character((*it).second);
	}
}

// manage testimonies
void MainWindow::on_case_manage_testimonies() {
	// run testimony manager
	TestimonyManager tm(m_Case.get_testimonies(), m_Case.get_testimony_ids());
	if (tm.run()==Gtk::RESPONSE_OK) {
		// get the updated testimony map from editor
		TestimonyMap tmap=tm.get_testimonies();
		
		// replace map in case with this one
		m_Case.clear_testimonies();
		for (TestimonyMap::iterator it=tmap.begin(); it!=tmap.end(); ++it)
			m_Case.add_testimony((*it).second);
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
		for (LocationMap::iterator it=locations.begin(); it!=locations.end(); ++it)
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

// customize case elements
void MainWindow::on_case_customize() {
	// bring up the customize dialog
	CustomizeDialog cd(m_Case.get_images());
	if (cd.run()==Gtk::RESPONSE_OK) {
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
	// prepare dialog
	AudioDialog ad;
	
	// set previous audio data
	ad.set_audio(m_Case.get_audio());
	
	// run the dialog
	if (ad.run()==Gtk::RESPONSE_OK) {
		AudioMap amap=ad.get_audio_data();
		
		// clear out previous audio map
		m_Case.clear_audio();
		
		// iterate over audio map
		for (AudioMap::iterator it=amap.begin(); it!=amap.end(); ++it)
			m_Case.add_audio((*it).second);
	}
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
		for (BackgroundMap::iterator it=backgrounds.begin(); it!=backgrounds.end(); ++it)
			m_Case.add_background((*it).second);
	}
}

// manage evidence assets handler
void MainWindow::on_assets_manage_evidence() {
	// prepare evidence manager
	EvidenceDialog ed(m_Case.get_evidence(), m_Case.get_images(), m_Case.get_evidence_ids());
	
	// run this dialog
	if (ed.run()==Gtk::RESPONSE_OK) {
		// get updated evidence
		EvidenceMap evidence=ed.get_evidence();
		
		// clear out previous evidence stored
		m_Case.clear_evidence();
		
		// iterate over map and add these pieces of evidence
		for (EvidenceMap::iterator it=evidence.begin(); it!=evidence.end(); ++it)
			m_Case.add_evidence((*it).second);
		
	}
}

// manage images assets handler
void MainWindow::on_assets_manage_images() {
	// prepare dialog
	ImageDialog diag(m_Case.get_images(), m_Case.get_image_ids());
	
	// run the dialog
	if (diag.run()==Gtk::RESPONSE_OK) {
		// get updated map of images
		ImageMap imap=diag.get_images();
		
		// clear out any previous images
		m_Case.clear_images();
		
		// now add these
		for (ImageMap::iterator it=imap.begin(); it!=imap.end(); ++it)
			m_Case.add_image((*it).second);
	}
}

// display sprite editor handler
void MainWindow::on_tools_sprite_editor() {
	// first, run the sprite chooser dialog
	SpriteChooserDialog scd;
	if (scd.run()==Gtk::RESPONSE_OK) {
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
		
		// or if we are creating a sprite from files, do that now
		else if (mode==SpriteChooserDialog::SPRITE_FROM_GIFS) {
			Sprite spr;
			
			// create this sprite
			if (!spr.create_from_gifs(scd.get_path())) {
				Gtk::MessageDialog md(*this, "Unable to create sprite from files.", false, Gtk::MESSAGE_ERROR);
				md.run();
				return;
			}
			
			else
				se.set_sprite_data(spr);
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
