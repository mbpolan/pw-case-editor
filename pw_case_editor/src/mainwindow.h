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
#include <gtkmm/recentfilter.h>
#include <gtkmm/recentmanager.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/window.h>
#include <gtkmm/uimanager.h>

#include "case.h"
#include "iconmanager.h"
#include "scriptwidget.h"

// the core application window
class MainWindow: public Gtk::Window {
	public:
		// constructor
		MainWindow();
		
		// get the case object
		Case::Case* get_case() { return &m_Case; }
		
		// return the buffers used in this case
		BufferMap get_case_buffers() const { return m_ScriptWidget->get_buffers(); }
	
	private:
		// build the ui
		void construct();
		
		// alert the user is there are not enough items of an element to perform an action
		bool check_case_element(const Glib::ustring &element, int amount);
		
		// create the trigger submenu
		void create_trigger_submenu(Gtk::Menu *menu);
		
		// create icons for menu items
		void create_icons();
		
		// set an icon for a menu item
		void set_menuitem_icon(const Glib::ustring &path, const Gtk::StockID &id);
		
		// process a loaded case from file
		bool process_load_case(const Glib::ustring &path);
		
		// new case handler
		void on_new();
		
		// save case handler
		void on_save();
		
		// save as handler
		void on_save_as();
		
		// export case handler
		void on_export();
		
		// load case handler
		void on_open();
		
		// open a recent file
		void on_open_recent(const Glib::ustring &path);
		
		// quit handler
		void on_quit();
		
		// add a formatted dialogue to script
		void on_script_insert_dialogue();
		
		// insert a trigger into block
		void on_script_insert_trigger(const Glib::ustring &trigger);
		
		// add character
		void on_case_add_char();
		
		// browse characters handler
		void on_case_browse_chars();
		
		// manage testimonies
		void on_case_manage_testimonies();
		
		// edit locations handler
		void on_case_edit_locations();
		
		// edit case overview information
		void on_case_edit_overview();
		
		// change initial case text block
		void on_case_change_initial_block();
		
		// manage audio assets handler
		void on_assets_manage_audio();
		
		// manage background assets handler
		void on_assets_manage_bg();
		
		// manage evidence assets handler
		void on_assets_manage_evidence();
		
		// manage images assets handler
		void on_assets_manage_images();
		
		// display sprite editor handler
		void on_tools_sprite_editor();
		
		// display info about this application
		void on_help_about();
		
		// menu stuff
		Glib::RefPtr<Gtk::ActionGroup> m_ActionGroup;
		Glib::RefPtr<Gtk::UIManager> m_UIManager;
		Gtk::Menu *m_RecentMenu;
		
		// status bar
		Gtk::Statusbar *m_Statusbar;
		
		// script widget
		ScriptWidget *m_ScriptWidget;
		
		// icon manager
		IconManager m_IconMgr;
		
		// internal case data
		Case::Case m_Case;
		
		// save file details
		bool m_Saved;
		Glib::ustring m_SavePath;
		
		// recent files
		std::vector<StringPair> m_RecentFiles;
};

// global pointer to the only mainwindow object
extern MainWindow *g_MainWnd;

#endif
