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
// mainwindow.cpp: implementations of MainWindow class

#include <iostream>
#include <QCloseEvent>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>

#include "dialogs.h"
#include "mainwindow.h"

#ifndef __MAC__
const QString g_SrcName="PWCaseEditor";
#else
const QString g_SrcName="pw-case-editor.sf.net";
#endif

const QString g_AppName="Case Editor";

// constructor
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
	// set title and minimum size
	setWindowTitle(tr("Unsaved File")+" - Phoenix Wright Case Editor");
	setMinimumSize(640, 480);
	
	construct();
	
	// read application settings at this point
	readSettings();
}

// handler for File|New
void MainWindow::onFileNew() {
	// present the dialog
	NewCaseDialog nd(this);
	if (nd.exec()==QDialog::Accepted) {
	}
}

// handler for File|Open
void MainWindow::onFileOpen() {
	// get the path
	QString path=QFileDialog::getOpenFileName(this, tr("Open Case"), "", tr("Case Project Files (*.cprjt)"));
	if (!path.isNull()) {
		// see if we should add this file to the recent list
		QSettings settings(g_SrcName, g_AppName);
		std::vector<QString> files;
		
		// get amount of files
		int amount=settings.value("numRecentFiles", 0).toInt();
		
		// add this one in
		files.push_back(path);
		
		// collect each file
		std::vector<QString> temp;
		for (int i=0; i<amount; i++)
			temp.push_back(settings.value("recentFile%1").toString().arg(i));
			
		// now add the other files
		for (int i=0; i<amount; i++) {
			if (path!=temp[i])
				files.push_back(temp[i]);
		}
		
		// we added a new file, increment counter
		amount+=1;
		
		// populate the recent files menu and update settings
		settings.setValue("numRecentFiles", (amount>5 ? 5 : amount));
		for (int i=0; i<settings.value("numRecentFiles").toInt(); i++) {
			// create an action
			QAction *act=new QAction(files[i], this);
			connect(act, SIGNAL(triggered()), this, SLOT(onRecentFile()));
			m_Menus["File|Recent"]->addAction(act);
			
			// and set the settings key
			settings.setValue(QString("recentFile%1").arg(i), files[i]);
		}
	}
}

// handler for File|Save
void MainWindow::onFileSave() {
}

// handler for File|Save As
void MainWindow::onFileSaveAs() {
}

// handler for File|Export
void MainWindow::onFileExport() {
}

// handler for File|Recent Files
void MainWindow::onRecentFile() {
}

// overloaded onCloseEvent
void MainWindow::onCloseEvent(QCloseEvent *e) {
	e->accept();
}

// build the interface
void MainWindow::construct() {
	// create menus and other basic gui elements
	createActions();
	createMenus();
	createToolBars();
	
	// allocate script widget
	m_ScriptWidget=new ScriptWidget(this);
	
	// allocate list manager
	m_Lists=new ListManager(this);
	
	// connect signals
	connect(m_ScriptWidget, SIGNAL(contentsUpdated(QString, QString)), m_Lists, SLOT(updateBlock(QString, QString)));
	connect(m_Lists, SIGNAL(blockChanged(QString, QString)), m_ScriptWidget, SLOT(setActiveBlock(QString, QString)));
	
	// allocate splitter
	QSplitter *split=new QSplitter(this);
	
	// add our widgets
	split->addWidget(m_Lists);
	split->addWidget(m_ScriptWidget);
	
	// set stretching factors
	split->setStretchFactor(0, 1);
	split->setStretchFactor(1, 3);
	
	setCentralWidget(split);
	
	// set the status bar widgets
	m_StatusLabel=new QLabel(this);
	statusBar()->addWidget(m_StatusLabel);
	statusBar()->showMessage(tr("Ready"));
}

// read stored application settings
void MainWindow::readSettings() {
	// supply the required parameters
	QSettings settings(g_SrcName, g_AppName);
	
	// first we check the amount of recent files
	int amount=settings.value("numRecentFiles", 0).toInt();
	
	// load each file as a recent file
	for (int i=0; i<amount; i++) {
		QString path=settings.value(QString("recentFile%1").arg(i)).toString();
		QAction *act=new QAction(path, this);
		connect(act, SIGNAL(triggered()), this, SLOT(onRecentFile()));
		
		// now just add this action to the menu
		m_Menus["File|Recent"]->addAction(act);
	}
}

// create all needed menu actions
void MainWindow::createActions() {
	m_Actions["File|New"]=createQAction(":/icons/new.png", tr("&New"), tr("Ctrl+N"), tr("Create a new case"));
	connect(m_Actions["File|New"], SIGNAL(triggered()), this, SLOT(onFileNew()));
	
	m_Actions["File|Open"]=createQAction(":/icons/open.png", tr("&Open"), tr("Ctrl+O"), tr("Open an existing case"));
	connect(m_Actions["File|Open"], SIGNAL(triggered()), this, SLOT(onFileOpen()));
	
	m_Actions["File|Save"]=createQAction(":/icons/save.png", tr("&Save"), tr("Ctrl+S"), tr("Save the current case"));
	connect(m_Actions["File|Save"], SIGNAL(triggered()), this, SLOT(onFileSave()));
	
	m_Actions["File|SaveAs"]=createQAction(":/icons/saveas.png", tr("Save As"), tr("Ctrl+Shift+S"), "");
	connect(m_Actions["File|SaveAs"], SIGNAL(triggered()), this, SLOT(onFileSaveAs()));
	
	m_Actions["File|Export"]=createQAction(":/icons/export.png", tr("Export"), tr("Ctrl+E"), tr("Export a playable case"));
	connect(m_Actions["File|Export"], SIGNAL(triggered()), this, SLOT(onFileExport()));
}

// create all menus
void MainWindow::createMenus() {
	m_Menus["File"]=menuBar()->addMenu(tr("&File"));
	m_Menus["File"]->addAction(m_Actions["File|New"]);
	m_Menus["File"]->addAction(m_Actions["File|Open"]);
	m_Menus["File"]->addAction(m_Actions["File|Save"]);
	m_Menus["File"]->addAction(m_Actions["File|SaveAs"]);
	m_Menus["File"]->addSeparator();
	m_Menus["File"]->addAction(m_Actions["File|Export"]);
	m_Menus["File"]->addSeparator();
	
	m_Menus["File|Recent"]=m_Menus["File"]->addMenu(tr("Recent Files"));
	
	m_Menus["File"]->addSeparator();
	m_Menus["File"]->addAction(m_Actions["File|Quit"]);
	
	m_Menus["Edit"]=menuBar()->addMenu(tr("&Edit"));
	
	m_Menus["Case"]=menuBar()->addMenu(tr("&Case"));
	
	m_Menus["Assets"]=menuBar()->addMenu(tr("&Assets"));
	
	m_Menus["Tools"]=menuBar()->addMenu(tr("&Tools"));
	
	m_Menus["Help"]=menuBar()->addMenu(tr("&Help"));
}

// create toolbars
void MainWindow::createToolBars() {
	m_MainTB=addToolBar(tr("File"));
	m_MainTB->addAction(m_Actions["File|New"]);
	m_MainTB->addAction(m_Actions["File|Open"]);
	m_MainTB->addAction(m_Actions["File|Save"]);
	m_MainTB->addAction(m_Actions["File|SaveAs"]);
	m_MainTB->addAction(m_Actions["File|Export"]);
}

// create a QAction ready for use
QAction* MainWindow::createQAction(const QString &icon, const QString &name, const QString &shortcut, const QString &tip) {
	// allocate either an action with or without an icon
	QAction *act;
	if (icon!="null")
		act=new QAction(QIcon(icon), name, this);
	else
		act=new QAction(name, this);
		
	// set details
	if (shortcut!="null")
		act->setShortcut(shortcut);
	
	if (tip!="null")
		act->setStatusTip(tip);
	return act;
}
