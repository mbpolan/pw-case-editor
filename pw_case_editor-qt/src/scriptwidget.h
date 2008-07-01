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
// scriptwidget.h: the ScriptWidget class

#ifndef SCRIPTWIDGET_H
#define SCRIPTWIDGET_H

#include <QLabel>
#include <QTextEdit>
#include <QWidget>

/** Widget that implements the text editor and other relevant widgets.
  * The body of a MainWindow class consists primarily of this widget. It implements 
  * the text editor where the text blocks are displayed, along with the series of 
  * lists where the blocks themselves are organized.
*/
class ScriptWidget: public QWidget {
	Q_OBJECT
	
	public:
		/// Constructor
		ScriptWidget(QWidget *parent=NULL);
	
	signals:
		/** Signal emitted when the contents of a block change
		  * \param id The block's ID
		  * \param contents Updated string contents
		*/
		void contentsUpdated(QString id, QString contents);
	
	public slots:
		/** Set the active block
		  * \param id The block's ID
		  * \param contents The block's contents
		*/
		void setActiveBlock(const QString &id, const QString &contents);
	
	private slots:
		/// Handler for text changes from editor
		void onTextChanged();
	
	protected:
		// labels
		QLabel *m_CurBlockLabel;
		
		// text edits
		QTextEdit *m_BlockEdit;
};

#endif
