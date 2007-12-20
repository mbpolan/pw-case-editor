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
// iconmanager.h: class to manage application icons

#ifndef ICONMANAGER_H
#define ICONMANAGER_H

#include <gdkmm/pixbuf.h>
#include <glibmm/ustring.h>
#include <gtkmm/stockid.h>
#include <map>

// stock icons for this application
namespace AppStock {
	extern Gtk::StockID ADD_CHARACTER;
	extern Gtk::StockID AUDIO;
	extern Gtk::StockID BACKGROUND;
	extern Gtk::StockID BROWSE_CHARS;
	extern Gtk::StockID EVIDENCE;
	extern Gtk::StockID IMAGE;
	extern Gtk::StockID INITBLOCK;
	extern Gtk::StockID INSERT_DIALOGUE;
	extern Gtk::StockID LOCATION;
	extern Gtk::StockID TESTIMONY;
}

// typedef'd map of icons
typedef std::map<Glib::ustring, Glib::RefPtr<Gdk::Pixbuf> > IconMap;
typedef std::map<Glib::ustring, Gtk::IconSet> SetMap;

// manager for application icons
class IconManager {
	public:
		// create icons sets from file
		bool create_from_file(const Glib::ustring &file);
		
		// look up a stock id based on icon set string
		Gtk::StockID lookup_from_set(const Glib::ustring &str);
		
	private:
		// create complete icon sets for given icons
		void create_icon_sets(const IconMap &icons);
		
		// default icon sets
		SetMap m_DefaultSets;
};

#endif
