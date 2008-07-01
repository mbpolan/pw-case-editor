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
// highlighter.cpp: implementation of Highlighter class

#include "highlighter.h"

// constructor
Highlighter::Highlighter(QTextEdit *parent): QSyntaxHighlighter(parent) {
	// control chars are red
	QTextCharFormat ccFmt;
	ccFmt.setForeground(Qt::red);
	
	// list of regular control characters
	QStringList cc;
	cc << "\\\\b" << "\\\\d" << "\\\\=" << "\\\\\\*" << "\\\\p[0-9][0-9]";
	cc << "\\\\\\+[0-9]" << "\\\\-[0-9]" << "\\\\n";
	
	// add each as a rule
	foreach(QString exp, cc)
		m_Rules.push_back(Rule(QRegExp(exp), ccFmt));
	
	QTextCharFormat ccolor;
	
	// trigger format
	ccolor.setForeground(Qt::darkGreen);
	ccolor.setFontItalic(true);
	m_Rules.push_back(Rule(QRegExp("\\{\\*[A-Za-z]*\\:[A-Za-z0-9,_]*;\\*\\}"), ccolor));
	
	// blue color control char
	ccolor.setFontItalic(false);
	ccolor.setForeground(Qt::cyan);
	m_Rules.push_back(Rule(QRegExp("\\\\c"), ccolor));
	
	// green color control char
	ccolor.setForeground(Qt::green);
	m_Rules.push_back(Rule(QRegExp("\\\\g"), ccolor));
	
	// orange color control char
	ccolor.setForeground(QColor::fromRgb(255, 153, 0));
	m_Rules.push_back(Rule(QRegExp("\\\\o"), ccolor));
	
	// white color control char
	ccolor.setForeground(Qt::gray);
	m_Rules.push_back(Rule(QRegExp("\\\\w"), ccolor));
}

// overloaded function to highlight text
void Highlighter::highlightBlock(const QString &text) {
	// simply apply each rule
	foreach(Rule rule, m_Rules) {
		QRegExp exp(rule.regex);
		int index=text.indexOf(exp);
		while(index>=0) {
			int length=exp.matchedLength();
			setFormat(index, length, rule.format);
			index=text.indexOf(exp, index+length);
		}
	}
}
