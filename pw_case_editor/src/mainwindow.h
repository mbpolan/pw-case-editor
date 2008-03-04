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
#include <gtkmm/menu.h>
#include <gtkmm/recentfilter.h>
#include <gtkmm/recentmanager.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/window.h>
#include <gtkmm/uimanager.h>

#include "case.h"
#include "iconmanager.h"
#include "scriptwidget.h"
#include "spriteeditor.h"

/** The central window for the editor.
  * This is the main window that displays all of the editor's 
  * components, along with the usual menus and implementations of 
  * basic functionality.
*/
class MainWindow: public Gtk::Window {
	public:
		/// Default constructor
		MainWindow();
		
		/** Get a pointer to the single case object
		  * \return A pointer to the internal Case
		*/
		Case::Case* get_case() { return &m_Case; }
		
		/** Get the buffers used in this case
		  * \return Map of buffers
		*/
		BufferMap get_case_buffers() const { return m_ScriptWidget->get_buffers(); }
	
	private:
		/// Build the window's UI
		void construct();
		
		/** Check to see if there are enough elements of a case.
		  * This function will check the given element, and determine if there is 
		  * at least <i>amount</i> of it.<br/>
		  * Valid elements are: characters, locations, evidence, blocks, audio, images, and testimonies
		  * \warning If element is not one of the above strings, then an exception is thrown
		  * \param element The element of a case
		  * \param amount Amount of element to check
		  * \return <b>true</b> if check passes, <b>false</b> otherwise
		*/
		bool check_case_element(const Glib::ustring &element, int amount);
		
		/** Create the trigger submenu
		  * Internal function to create and populate the "Insert Trigger" submenu
		  * \param menu A pointer to the parent menu
		*/
		void create_trigger_submenu(Gtk::Menu *menu);
		
		/** Set an icon for a menu item
		  * \param path GTK valid path to the menu item
		  * \param id Stock icon ID to set
		*/
		void set_menuitem_icon(const Glib::ustring &path, const Gtk::StockID &id);
		
		/** Process and handle a loaded case from file
		  * \param path Path to case file
		  * \return <b>true</b> if everything is fine, <b>false</b> otherwise
		*/
		bool process_load_case(const Glib::ustring &path);
		
		/** Process and configure a case, then export it
		  * \param path Path to export the case to
		  * \return <b>true</b> if export was successful, <b>false</b> otherwise
		*/
		bool process_export(const Glib::ustring &path);
		
		/// Handler for creating a new case
		void on_new();
		
		/// Handler to save a case
		void on_save();
		
		/// Handler to save a case under a new file
		void on_save_as();
		
		/// Handler to export a case
		void on_export();
		
		/** Handler to conveniently export a case
		  * \param path The path to export the case to
		*/
		void on_quick_export(const Glib::ustring &path);
		
		/// Handler to open a case
		void on_open();
		
		/** Handler to open a recent case file
		  * \param path Path to the case file
		*/
		void on_open_recent(const Glib::ustring &path);
		
		/// Handler to quit the editor
		void on_quit();
		
		/// Handler to find text in blocks
		void on_edit_find_in_blocks();
		
		/// Handler to add formatted dialogue to script
		void on_script_insert_dialogue();
		
		/// Handler to insert a trigger into block
		void on_script_insert_trigger(const Glib::ustring &trigger);
		
		/// Handler to change the text color
		void on_script_change_text_color();
		
		/// Handler to change text speed
		void on_script_change_text_speed();
		
		/// Handler to add character
		void on_case_add_char();
		
		/// Handler to browse characters
		void on_case_browse_chars();
		
		/// Handler to manage testimonies
		void on_case_manage_testimonies();
		
		/// Handler to edit locations
		void on_case_edit_locations();
		
		/// Handler to edit case overview information
		void on_case_edit_overview();
		
		/// Handler to change initial case text block
		void on_case_change_initial_block();
		
		/// Handler to customize case elements
		void on_case_customize();
		
		/// Handler to manage audio assets
		void on_assets_manage_audio();
		
		/// Handler to manage background assets
		void on_assets_manage_bg();
		
		/// Handler to manage evidence assets
		void on_assets_manage_evidence();
		
		/// Handler to manage images assets
		void on_assets_manage_images();
		
		/// Handler to set program language
		void on_tools_set_language();
		
		/// Handler to display sprite editor
		void on_tools_sprite_editor();
		
		/// Handler to display info about this application
		void on_help_about();
		
		// menu stuff
		Glib::RefPtr<Gtk::ActionGroup> m_ActionGroup;
		Glib::RefPtr<Gtk::UIManager> m_UIManager;
		Gtk::Menu *m_RecentMenu;
		
		/// Status bar in window
		Gtk::Statusbar *m_Statusbar;
		
		/// Instance of ScriptWidget
		ScriptWidget *m_ScriptWidget;
		
		/// Instance of IconManager
		IconManager m_IconMgr;
		
		/// Pointer to quick export menu
		Gtk::Menu *m_QExportMenu;
		
		/// Instance of SpriteEditor
		SpriteEditor m_SprEditor;
		
		/// Internal case data
		Case::Case m_Case;
		
		/// Flag whether or not the case was already saved
		bool m_Saved;
		
		/// Path to automatically save case to
		Glib::ustring m_SavePath;
		
		/// Vector of recent files
		std::vector<StringPair> m_RecentFiles;
};

/// Global pointer to the only MainWindow object
extern MainWindow *g_MainWnd;

#endif
