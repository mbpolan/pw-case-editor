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
// triplet.h: the Triplet template class

#ifndef TRIPLET_H
#define TRIPLET_H

#include <glibmm/ustring.h>

// class that extends std::pair; stores three elements
template <class X, class Y, class Z>
class Triplet {
	public:
		// constructor
		Triplet(const X &x, const Y &y, const Z &z):
			first(x), second(y), third(z) { };
		
		// elements
		X first;
		Y second;
		Z third;
};

// create a triplet
template <class X, class Y, class Z>
Triplet<X, Y, Z> make_triplet(const X &x, const Y &y, const Z&z) {
	Triplet<X, Y, Z> t(x, y, z);
	return t;
}

// common triplet types
typedef Triplet<Glib::ustring, Glib::ustring, Glib::ustring> StringTriplet;

#endif
