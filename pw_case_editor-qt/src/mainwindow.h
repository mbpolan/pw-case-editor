/***************************************************************************
 *   Copyright (C) 2008 by Mike Polan                                      *
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

#include <QAction>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QToolBar>

#include "case.h"
#include "common.h"
#include "listmanager.h"
#include "scriptwidget.h"

extern const QString g_SrcName;
extern const QString g_AppName;

/** The central window for the editor.
  * This is the main window that displays all of the editor's 
  * components, along with the usual menus and implementations of 
  * basic functionality.
*/
class MainWindow: public QMainWindow {
	Q_OBJECT
	
	public:
		/// Constructor
		MainWindow(QWidget *parent=NULL);
	
	private slots:
		/// Handler for modified document status
		void onDocModified(bool state);
	
		/// Handler for File|New
		void onFileNew();
		
		/// Handler for File|Open
		void onFileOpen();
		
		/// Handler for File|Save
		void onFileSave();
		
		/// Handler for File|Save As
		void onFileSaveAs();
		
		/// Handler for File|Export
		void onFileExport();
		
		/// Handler for File|Recent Files actions
		void onRecentFile();
		
		/// Handler for Case|Manage Character
		void onManageCharacters();
		
		/// Handler for Case|Manage Locations
		void onManageLocations();
		
		/// Handler for Assets|Manage Evidence
		void onManageEvidence();
		
		/// Handler for Assets|Manage Backgrounds
		void onManageBackgrounds();
	
	protected:
		/// Overloaded closeEvent handler
		virtual void closeEvent(QCloseEvent *e);
		
		/// Build the interface
		void construct();
		
		/** Toggle the status of the window contents
		  * \param enabled <b>true</b> to enable, <b>false</b> to disable
		*/
		void setWindowEnabled(bool enabled);
		
		/// Read saved application settings
		void readSettings();
		
		/// Create all needed menu actions
		void createActions();
		
		/// Create all menus
		void createMenus();
		
		/// Create toolbars
		void createToolBars();
		
		/** Convenience function to create a ready to go QAction
		  * \param icon The icon to use
		  * \param name Visible string name
		  * \param shortcut Shortcut key combination
		  * \param tip Brief description of action
		  * \return Pointer to the QAction object
		*/
		QAction* createQAction(const QString &icon, const QString &name, const QString &shortcut, 
						const QString &tip);
		
		// labels
		QLabel *m_StatusLabel;
		
		// toolbars
		QToolBar *m_MainTB;
		
		/// Map of QAction objects for menus
		ActionMap m_Actions;
		
		/// Map of QMenu objects for menus
		std::map<QString, QMenu*> m_Menus;
		
		// widget for script editing
		ScriptWidget *m_ScriptWidget;
		
		// widget for managing block lists
		ListManager *m_Lists;
		
		/// The current case object we are editing
		Case::Case m_Case;
};

#endif
