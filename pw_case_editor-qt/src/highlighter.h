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
// highlighter.h: the Highlighter class

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QRegExp>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <vector>

/** Class that is responsible for highlighting script syntax
*/
class Highlighter: public QSyntaxHighlighter {
	public:
		/// Constructor
		Highlighter(QTextEdit *parent);
	
	private:
		/// Overloaded function to highlight text
		void highlightBlock(const QString &text);
		
		/// Class to store highlighting rules
		class Rule {
			public:
				/** Constructor
				* \param regex The regular expression to test for
				* \param format The formatting to apply
				* \param cstart Amount of characters to ignore from the expression start
				* \param cend Amount of characters to ignore from expression end
				*/
				Rule(const QRegExp &exp, const QTextCharFormat &fmt, int cstart=0, int cend=0):
					regex(exp), format(fmt), start(cstart), end(cend) {
				}
			
				QRegExp regex;
				QTextCharFormat format;
				int start, end;
		};
		
		/// Vector of highlighting rules
		std::vector<Rule> m_Rules;
};

#endif
