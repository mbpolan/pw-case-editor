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
// mainwindow.h: the MainWindow class

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm/actiongroup.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/label.h>
#include <gtkmm/table.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/window.h>

// data gathered from frontend
struct _PlayerParams {
	// the path to the case file
	Glib::ustring path;
	
	// switches to pass to player
	Glib::ustring params;
};
typedef struct _PlayerParams PlayerParams;

// the core window of the frontend
class MainWindow: public Gtk::Window {
	public:
		// constructor
		MainWindow();
		
		// get the selected options and case path
		PlayerParams* get_params();
		
	private:
		// build the interface
		void construct();
		
		// play button clicked
		void on_play_clicked();
		
		// open handler
		void on_open();
		
		// quit handler
		void on_quit();
		
		// about handler
		void on_about();
		
		// menu stuff
		Glib::RefPtr<Gtk::ActionGroup> m_ActionGroup;
		Glib::RefPtr<Gtk::UIManager> m_UIManager;
		
		// main layout box
		Gtk::VBox *m_VB;
		
		// buttons
		Gtk::Button *m_PlayButton;
		
		// labels
		Gtk::Label *m_PathLabel;
		Gtk::Label *m_EntryLabel;
		
		// check buttons
		Gtk::CheckButton *m_NoSoundCB;
		Gtk::CheckButton *m_DebugModeCB;
		Gtk::CheckButton *m_FullscreenCB;
		
		// flag if the window was closed without clicking
		// the run button
		bool m_Closed;
};

#endif
