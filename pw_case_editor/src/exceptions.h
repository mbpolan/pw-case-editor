/***************************************************************************
 *   Copyright (C) 2007 by Mike Polan                                      *
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
// exceptions.h: various classes for C++ exceptions

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

/** Exception thrown when a case element is not acceptable.
  * Used only in MainWindow::check_case_element, where programmer 
  * errors could occur.
*/
class ElementEx: public std::exception {
	public:
		/** Get a string of what went wrong
		  * \return The string description of the error
		*/
		virtual const char* what() const throw() {
			return "Case element provided is not acceptable";
		}
};

#endif
