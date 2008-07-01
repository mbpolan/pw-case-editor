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
// listmanager.h: the ListManager class

#ifndef LISTMANAGER_H
#define LISTMANAGER_H

#include <QComboBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "clistview.h"

/** Core handler for managing block lists.
  * This class is responsible for handling and managing
  * the various lists of blocks found in the editor. It
  * also provides some convenience functions for
  * generating unique block IDs, as well as block aggregation
*/
class ListManager: public QWidget {
	Q_OBJECT
	
	public:
		/// Constructor
		ListManager(QWidget *parent=NULL);
		
		/** Return an instance of this object
		  * \return Instance of this object
		*/
		static ListManager* instance();
		
		/** Generate a unique ID for a given root block
		  * \param id The ID of the root block
		  * \return A unique ID string
		*/
		QString uniqueID(const QString &id);
	
	signals:
		/** Signal emitted when a list's active block is changed
		  * \param id The block's ID
		*/
		void blockChanged(QString id, QString contents);
	
	public slots:
		/** Update the contents of a block
		  * \param id The block's ID
		  * \param contents The new contents to set
		*/
		void updateBlock(const QString &id, const QString &contents);
	
	private slots:
		/// Handler for list block changes
		void onListBlockChanged(QString id, QString contents);
		
	private:
		/// Create a CListView object ready for use
		CListView* addList();
		
		// layouts
		QVBoxLayout *m_Layout;
		
		// tab widgets
		QTabWidget *m_TabWidget;
		
		// combo boxes
		QComboBox *m_DayCB;
		
		// blank widgets to store tabs
		QWidget *m_InvTab;
		QWidget *m_CaseTab;
		
		/// Vector of available lists
		std::vector<CListView*> m_Lists;
		
};

#endif
