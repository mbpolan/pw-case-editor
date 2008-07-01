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
// listmanager.cpp: implementation of ListManager class

#include "listmanager.h"
#include "utilities.h"

ListManager *g_ListManager=NULL;

// constructor
ListManager::ListManager(QWidget *parent): QWidget(parent) {
	g_ListManager=this;
	
	// allocate layout
	m_Layout=new QVBoxLayout;
	
	// allocate combo box
	m_DayCB=new QComboBox(this);
	m_DayCB->addItem(tr("Day")+QString("1"));
	m_DayCB->addItem(tr("Day")+QString("2"));
	m_DayCB->addItem(tr("Day")+QString("3"));
	
	// allocate tab widget for trial/investigation 
	m_TabWidget=new QTabWidget(this);
	
	// allocate some lists
	m_Lists.push_back(addList());
	m_Lists.push_back(addList());
	
	// add tabs
	m_TabWidget->addTab(m_Lists[0], tr("Investigation"));
	m_TabWidget->addTab(m_Lists[1], tr("Trial"));
	
	// place widgets
	m_Layout->addWidget(m_DayCB, 1);
	m_Layout->addWidget(m_TabWidget, 5);
	
	setLayout(m_Layout);
}

// return an instance of this class
ListManager* ListManager::instance() {
	return g_ListManager;
}

// generate a unique id for a block
QString ListManager::uniqueID(const QString &id) {
	StringVector blocks;
	
	// iterate over all lists
	for (int i=0; i<m_Lists.size(); i++) {
		StringVector b=m_Lists[i]->serializeBlocks();
		
		// add only the pertinent ones
		for (int j=0; j<b.size(); j++) {
			if (Utils::extractID(b[j])==id)
				blocks.push_back(b[j]);
		}
	}
	
	// set an initial try
	QString tID=QString(id)+QString("_%1");
	
	// now make sure its not already taken
	int n=blocks.size()+1;
	while(1) {
		int orig=n;
		QString tmp=tID.arg(orig);
		for (int i=0; i<blocks.size(); i++) {
			if (tmp==blocks[i]) {
				n++;
				break;
			}
		}
		
		if (orig==n)
			break;
	}
	
	return tID.arg(n);
}

// update block contents
void ListManager::updateBlock(const QString &id, const QString &contents) {
	// see which list has this block
	for (int i=0; i<m_Lists.size(); i++) {
		if (m_Lists[i]->hasBlock(id)) {
			m_Lists[i]->setBlockContents(id, contents);
		}
	}
}

// handler for list block changes
void ListManager::onListBlockChanged(QString id, QString contents) {
	emit blockChanged(id, contents);
}

// create a CListView object ready for use
CListView* ListManager::addList() {
	CListView *list=new CListView(m_TabWidget);
	
	// connect all signals
	connect(list, SIGNAL(blockChanged(QString, QString)), this, SLOT(onListBlockChanged(QString, QString)));
	
	return list;
}
