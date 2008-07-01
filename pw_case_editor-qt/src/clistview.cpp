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
// clistview.cpp: implementation of CListView class

#include <QContextMenuEvent>
#include <QInputDialog>
#include <QMenu>
#include <QTreeWidgetItem>

#include "clistview.h"
#include "listmanager.h"
#include "utilities.h"

// constructor
CListView::CListView(QWidget *parent): QTreeWidget(parent) {
	setColumnCount(1);
	setHeaderLabel(tr("Component"));
	
	// add default columns
	addTopLevelItem(new QTreeWidgetItem(this, QStringList(tr("Script"))));
	addTopLevelItem(new QTreeWidgetItem(this, QStringList(tr("Narration"))));
	addTopLevelItem(new QTreeWidgetItem(this, QStringList(tr("Dates"))));
	
	// create actions for context menu
	m_Actions["AddBlock"]=new QAction(tr("Add Text Block"), this);
	connect(m_Actions["AddBlock"], SIGNAL(triggered()), this, SLOT(onAddTextBlock()));
	
	m_Actions["RemoveBlock"]=new QAction(tr("Remove Text Block"), this);
	connect(m_Actions["RemoveBlock"], SIGNAL(triggered()), this, SLOT(onDeleteTextBlock()));
	
	m_Actions["EditDesc"]=new QAction(tr("Edit Description"), this);
	connect(m_Actions["EditDesc"], SIGNAL(triggered()), this, SLOT(onEditDescription()));
	
	// connect signals
	connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemActivated(QTreeWidgetItem*, int)));
	
}

// serialize all blocks into one vector
StringVector CListView::serializeBlocks() {
	StringVector vec;
	
	// walk the map
	for (StringMap::iterator it=m_Blocks.begin(); it!=m_Blocks.end(); ++it)
		vec.push_back((*it).first);
	
	return vec;
}

// see if a block exists
bool CListView::hasBlock(const QString &id) {
	// walk the tree
	for (int i=0; i<topLevelItemCount(); i++) {
		QTreeWidgetItem *top=topLevelItem(i);
		for (int j=0; j<top->childCount(); j++) {
			QTreeWidgetItem *c=top->child(j);
			if (Utils::stripDescription(c->text(0))==id)
				return true;
		}
	}
	
	return false;
}

// set the contents of a block
void CListView::setBlockContents(const QString &id, const QString &contents) {
	m_Blocks[id]=contents;
}

// handler for adding a text block
void CListView::onAddTextBlock() {
	// get our current node
	QTreeWidgetItem *item=currentItem();
	
	// generate an id for our new node
	QString id=ListManager::instance()->uniqueID(item->text(0));
	m_Blocks[id]="";
	
	// format the rest of the string
	id+=" ()";
	
	// and append a new item
	QTreeWidgetItem *node=new QTreeWidgetItem(item);
	node->setText(0, id);
	item->addChild(node);
}

// handler for deleting a text block
void CListView::onDeleteTextBlock() {
	// save id and get rid of the node
	QTreeWidgetItem *item=currentItem();
	QString id=Utils::stripDescription(item->text(0));
	item->parent()->removeChild(item);
	
	// now delete it from the map
	m_Blocks.erase(id);
}

// handler for editing text block descriptions
void CListView::onEditDescription() {
	QTreeWidgetItem *item=currentItem();
	QString desc=Utils::extractDescription(item->text(0));
	
	// present a dialog to edit this data
	bool ok;
	QString str=QInputDialog::getText(this, tr("Edit Description"), tr("Enter new description"),
						    QLineEdit::Normal, desc, &ok);
	if (ok)
		item->setText(0, Utils::stripDescription(item->text(0))+" ("+str+")");
}

// handler for item changes
void CListView::onItemActivated(QTreeWidgetItem *item, int col) {
	// worry only about child nodes
	if (item && item->parent()) {
		QString id=Utils::stripDescription(item->text(0));
		emit blockChanged(id, m_Blocks[id]);
	}
}

// overloaded handler for context menus
void CListView::contextMenuEvent(QContextMenuEvent *event) {
	// by default all actions are enabled
	m_Actions["AddBlock"]->setEnabled(true);
	m_Actions["RemoveBlock"]->setEnabled(true);
	m_Actions["EditDesc"]->setEnabled(true);
	
	// if nothing is selected, disable all actions
	if (!currentItem()) {
		m_Actions["AddBlock"]->setEnabled(false);
		m_Actions["RemoveBlock"]->setEnabled(false);
		m_Actions["EditDesc"]->setEnabled(false);
	}
	
	// for toplevel items, only enable adding blocks
	else if (!currentItem()->parent()) {
		m_Actions["RemoveBlock"]->setEnabled(false);
		m_Actions["EditDesc"]->setEnabled(false);
	}
	
	// create and populate a popup menu
	QMenu popup(this);
	popup.addAction(m_Actions["AddBlock"]);
	popup.addAction(m_Actions["RemoveBlock"]);
	popup.addAction(m_Actions["EditDesc"]);
	
	popup.exec(event->globalPos());
}
