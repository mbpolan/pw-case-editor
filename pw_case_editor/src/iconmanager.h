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

/// Namespace for stock icons for this application
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

/** Class that is responsible for managing custom icons.
  * Since the editor uses a set of custom icons, this class is needed 
  * to simplify handling the stock IDs and looking up specific icons.
*/
class IconManager {
	public:
		/** Create icons sets from file
		  * \param file The path to the resource file
		  * \return <b>true</b> if there were no errors, <b>false</b> otherwise
		*/
		bool create_from_file(const Glib::ustring &file);
		
		/** Look up a stock ID based on icon set string
		  * \param str ID of a particular icons set
		  * \return The stock ID of that icon
		*/
		Gtk::StockID lookup_from_set(const Glib::ustring &str);
		
	private:
		/** Create complete icon sets for given icons
		  * \param icons The map of icons to generate a set for
		*/
		void create_icon_sets(const IconMap &icons);
		
		/// Default icon sets
		SetMap m_DefaultSets;
};

#endif
