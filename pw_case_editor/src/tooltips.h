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
// tooltips.h: string map of tooltips used within the program

#ifndef TOOLTIPS_H
#define TOOLTIPS_H

#include <glibmm/ustring.h>
#include <map>

namespace Tooltips {

// our map of tooltips
extern std::map<Glib::ustring, Glib::ustring> g_TipMap;

// lookup a tooltip
static Glib::ustring lookup(const Glib::ustring &widgetID) {
	return g_TipMap[widgetID];
}

// register tooltips for widgets
void register_tooltips();

}

#endif
