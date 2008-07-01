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
// scriptwidget.cpp: implementation of ScriptWidget class

#include <QVBoxLayout>

#include "highlighter.h"
#include "scriptwidget.h"

// constructor
ScriptWidget::ScriptWidget(QWidget *parent): QWidget(parent) {
	// allocate labels
	m_CurBlockLabel=new QLabel(tr("Current Text Block")+": "+tr("none"), this);
	m_CurBlockLabel->setAlignment(Qt::AlignHCenter);
	
	// allocate text edit
	m_BlockEdit=new QTextEdit(this);
	
	// allocate highlighter
	Highlighter *hilite=new Highlighter(m_BlockEdit);
	
	// connect signals
	connect(m_BlockEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
	
	// create layout and populate it
	QVBoxLayout *vb=new QVBoxLayout;
	vb->addWidget(m_CurBlockLabel);
	vb->addWidget(m_BlockEdit);
	
	setLayout(vb);
	show();
}

// set the active block
void ScriptWidget::setActiveBlock(const QString &id, const QString &contents) {
	// update label and text edit
	m_CurBlockLabel->setText(tr("Current Text Block")+": "+id);
	m_BlockEdit->setPlainText(contents);
}

// handler for text changes from editor
void ScriptWidget::onTextChanged() {
	QString id=m_CurBlockLabel->text();
	int npos=id.lastIndexOf(": ");
	id=id.mid(npos+2, id.size()-npos);
	
	emit contentsUpdated(id, m_BlockEdit->toPlainText());
}
