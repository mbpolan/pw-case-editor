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
#include <gtkmm/filefilter.h>
#include <gtkmm/frame.h>
#include <gtkmm/stock.h>
#include <gtkmm/table.h>

#include "mainwindow.h"

// constructor
MainWindow::MainWindow(): m_Closed(false) {
	set_title("Phoenix Wright Case Player");
	set_resizable(false);
	
	construct();
	
	set_size_request(200, 200);
	
	m_VB->set_sensitive(false);
	
	// since the window should only be closed via the run button, make sure
	// to handle the delete-event signal
	signal_delete_event().connect(sigc::bind_return(sigc::hide(sigc::mem_fun(*this, &MainWindow::on_quit)), true));
}

// get the selected options and case path
PlayerParams* MainWindow::get_params() {
	if (m_Closed)
		return NULL;
	
	PlayerParams *p=new PlayerParams;
	
	// first, set the case path
	p->path=m_EntryLabel->get_text();
	
	// now for each parameter checked, add an argument
	p->params=" ";
	if (m_NoSoundCB->get_active())
		p->params+="-ns ";
	if (m_DebugModeCB->get_active())
		p->params+="-d ";
	if (m_FullscreenCB->get_active())
		p->params+="-fs ";
	
	return p;
}

// build the interface
void MainWindow::construct() {
	// allocate action group
	m_ActionGroup=Gtk::ActionGroup::create();
	
	m_ActionGroup->add(Gtk::Action::create("FileOpen", Gtk::Stock::OPEN, "_Open"),
			   sigc::mem_fun(*this, &MainWindow::on_open));
	m_ActionGroup->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT, "_Quit"),
			   sigc::mem_fun(*this, &MainWindow::on_quit));
	
	m_ActionGroup->add(Gtk::Action::create("HelpAbout", Gtk::Stock::ABOUT, "_About"),
			   sigc::mem_fun(*this, &MainWindow::on_about));
	
	m_ActionGroup->add(Gtk::Action::create("FileMenu", "_File"));
	m_ActionGroup->add(Gtk::Action::create("HelpMenu", "_Help"));
	
	// create ui manager
	m_UIManager=Gtk::UIManager::create();
	m_UIManager->insert_action_group(m_ActionGroup);
	
	// don't set tearoffs for Windows
#ifndef __WIN32__
	m_UIManager->set_add_tearoffs(true);
#endif
	
	Glib::ustring ui="<ui>"
			"<menubar name='MainMenu'>"
			"	<menu action='FileMenu'>"
			"		<menuitem action='FileOpen'/>"
			"		<separator/>"
			"		<menuitem action='FileQuit'/>"
			"	</menu>"
			"	<menu action='HelpMenu'>"
			"		<menuitem action='HelpAbout'/>"
			"	</menu>"
			"</menubar>"
			"</ui>";
	
	// now just add the ui
	m_UIManager->add_ui_from_string(ui);
	
	// allocate vbox
	Gtk::VBox *vb=manage(new Gtk::VBox);
	vb->set_spacing(5);
	
	// get the main menu bar
	Gtk::Widget *menuBar=m_UIManager->get_widget("/MainMenu");
	if (menuBar)
		vb->pack_start(*manage(menuBar), Gtk::PACK_SHRINK);
	
	// allocate main vbox
	m_VB=manage(new Gtk::VBox);
	m_VB->set_spacing(5);
	
	// allocate frames
	Gtk::Frame *fileFrame=manage(new Gtk::Frame("File"));
	Gtk::Frame *paramFrame=manage(new Gtk::Frame("Parameters"));
	
	// allocate frame tables
	Gtk::Table *fileTable=manage(new Gtk::Table);
	Gtk::Table *paramTable=manage(new Gtk::Table);
	
	// labels
	m_PathLabel=manage(new Gtk::Label("Path to File: "));
	m_EntryLabel=manage(new Gtk::Label);
	
	// allocate check buttons
	m_NoSoundCB=manage(new Gtk::CheckButton("No Sound"));
	m_DebugModeCB=manage(new Gtk::CheckButton("Debug Mode"));
	m_FullscreenCB=manage(new Gtk::CheckButton("Fullscreen"));
	
	// allocate buttons
	m_PlayButton=manage(new Gtk::Button(Gtk::Stock::MEDIA_PLAY));
	m_PlayButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::hide));
	
	// place widgets
	fileTable->attach(*m_PathLabel, 0, 1, 0, 1);
	fileTable->attach(*m_EntryLabel, 1, 2, 0, 1);
	
	paramTable->attach(*m_NoSoundCB, 0, 1, 0, 1);
	paramTable->attach(*m_DebugModeCB, 0, 1, 1, 2);
	paramTable->attach(*m_FullscreenCB, 0, 1, 2, 3);
	
	fileFrame->add(*fileTable);
	paramFrame->add(*paramTable);
	
	m_VB->pack_start(*fileFrame);
	m_VB->pack_start(*paramFrame);
	m_VB->pack_start(*m_PlayButton, Gtk::PACK_SHRINK);
	
	vb->pack_start(*m_VB, Gtk::PACK_SHRINK);
	
	add(*vb);
	
	show_all_children();
}

// open handler
void MainWindow::on_open() {
	Gtk::FileChooserDialog fcd(*this, "Select Case", Gtk::FILE_CHOOSER_ACTION_OPEN);
	Gtk::FileFilter filter;
	filter.set_name("Phoenix Wright Trials (*.pwt)");
	filter.add_pattern("*.pwt");
	fcd.add_filter(filter);
	
	fcd.add_button("Open", Gtk::RESPONSE_OK);
	fcd.add_button("Close", Gtk::RESPONSE_CANCEL);
	
	// run the dialog
	if (fcd.run()==Gtk::RESPONSE_OK) {
		Glib::ustring path=fcd.get_filename();
		
		m_EntryLabel->set_text(path);
		m_VB->set_sensitive(true);
	}
}

// quit handler
void MainWindow::on_quit() {
	m_Closed=true;
	hide();
}

// about handler
void MainWindow::on_about() {
	Gtk::AboutDialog ad;
	ad.set_name("Phoenix Wright Case Player");
	ad.set_version("1.0");
	ad.set_comments("The player program to run fan-made cases, which were exported from the editor.");
	ad.set_website("http://pw-case-player.sf.net");
	
	std::vector<Glib::ustring> authors;
	authors.push_back("Mike Polan (kanadakid@gmail.com)");
	
	ad.set_authors(authors);
	
	ad.run();
}
