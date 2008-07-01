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
// clistview.h: the CListView class

#ifndef CLISTVIEW_H
#define CLISTVIEW_H

#include <map>
#include <QAction>
#include <QTreeWidget>

#include "common.h"

/** Implements a customized QTreeWidget interface.
  * Normally, Qt's list widget provides the usual functions and API needed
  * to take care of most editor requirements. This class extends that API 
  * with custom functions that deal specifically with the editor's unique 
  * needs for handling text block nodes, descriptions, and everything else.
*/
class CListView: public QTreeWidget {
	Q_OBJECT
	
	public:
		/// Constructor
		CListView(QWidget *parent=NULL);
		
		/** Serialize all blocks into a single vector
		  * \return A vector containing all blocks in this list
		*/
		StringVector serializeBlocks();
		
		/** See if a block is in this list
		  * \param id The block's ID
		  * \return <b>true</b> if found, <b>false</b> otherwise
		*/
		bool hasBlock(const QString &id);
		
		/** Set the contents of a block
		  * \param id The block's ID
		  * \param contents The string to set
		*/
		void setBlockContents(const QString &id, const QString &contents);
	
	signals:
		/** Signal emitted when a new block has been selected
		  * \param id The block's ID
		*/
		void blockChanged(QString id, QString contents);
	
	private slots:
		/// Handler for adding a text block
		void onAddTextBlock();
		
		/// Handler for deleting a text block
		void onDeleteTextBlock();
		
		/// Handler for editing text block descriptions
		void onEditDescription();
		
		/// Handler for item changes on list
		void onItemActivated(QTreeWidgetItem *item, int col);
	
	protected:
		/// Overloaded context menu member
		void contextMenuEvent(QContextMenuEvent *event);
		
		/// Map of actions for context menu
		ActionMap m_Actions;
		
		/// Map of blocks and contents
		StringMap m_Blocks;
};

#endif
