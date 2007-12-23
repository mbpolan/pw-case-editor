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
// iconmanager.cpp: implementation of IconManager class

#include <gtkmm/iconfactory.h>
#include <gtkmm/iconset.h>
#include <gtkmm/iconsource.h>

#include "iconmanager.h"
#include "iohandler.h"

namespace AppStock {
	Gtk::StockID ADD_CHARACTER("add_character");
	Gtk::StockID AUDIO("audio");
	Gtk::StockID BACKGROUND("background");
	Gtk::StockID BROWSE_CHARS("browse_chars");
	Gtk::StockID EVIDENCE("evidence");
	Gtk::StockID IMAGE("image");
	Gtk::StockID INITBLOCK("init_block");
	Gtk::StockID INSERT_DIALOGUE("insert_dialogue");
	Gtk::StockID LOCATION("location");
	Gtk::StockID TESTIMONY("testimony");
}

// create icons sets from file
bool IconManager::create_from_file(const Glib::ustring &file) {
	// load the icons
	IconMap icons;
	if (!IO::read_icons_from_file(icons))
		return false;
	
	// now create icon sets for these icons
	create_icon_sets(icons);
	
	return true;
}

// look up a stock id based on icon set string
Gtk::StockID IconManager::lookup_from_set(const Glib::ustring &str) {
	if (str=="add-char")
		return AppStock::ADD_CHARACTER;
	else if (str=="audio")
		return AppStock::AUDIO;
	else if (str=="browse-chars")
		return AppStock::BROWSE_CHARS;
	else if (str=="bg")
		return AppStock::BACKGROUND;
	else if (str=="evidence")
		return AppStock::EVIDENCE;
	else if (str=="image")
		return AppStock::IMAGE;
	else if (str=="initblock")
		return AppStock::INITBLOCK;
	else if (str=="insert-dialogue")
		return AppStock::INSERT_DIALOGUE;
	else if (str=="location")
		return AppStock::LOCATION;
	else if (str=="testimony")
		return AppStock::TESTIMONY;
	else
		g_message("IconManager: unknown icon set '%s'!", str.c_str());
}

// create complete icon sets for given icons
void IconManager::create_icon_sets(const IconMap &icons) {
	// create a new icon factory
	Glib::RefPtr<Gtk::IconFactory> factory=Gtk::IconFactory::create();
	
	// default iconsets
	m_DefaultSets["add-char"]=Gtk::IconSet();
	m_DefaultSets["audio"]=Gtk::IconSet();
	m_DefaultSets["browse-chars"]=Gtk::IconSet();
	m_DefaultSets["bg"]=Gtk::IconSet();
	m_DefaultSets["evidence"]=Gtk::IconSet();
	m_DefaultSets["image"]=Gtk::IconSet();
	m_DefaultSets["initblock"]=Gtk::IconSet();
	m_DefaultSets["insert-dialogue"]=Gtk::IconSet();
	m_DefaultSets["location"]=Gtk::IconSet();
	m_DefaultSets["testimony"]=Gtk::IconSet();
	
	// go over each loaded icon
	for (IconMap::const_iterator it=icons.begin(); it!=icons.end(); ++it) {
		Glib::ustring root=(*it).first;
		
		// first, split the string into name and size
		Glib::ustring name=root.substr(0, root.rfind("_"));
		root.erase(0, root.rfind("_")+1);
		Glib::ustring rsize=root.substr(0, root.rfind("."));
		
		// get a numerical value
		int size=atoi(rsize.c_str());
		
		// according to size, generate an icon source
		switch(size) {
			case 16: {
				Gtk::IconSource menu, stoolbar, btn;
				
				menu.set_size(Gtk::ICON_SIZE_MENU);
				stoolbar.set_size(Gtk::ICON_SIZE_SMALL_TOOLBAR);
				btn.set_size(Gtk::ICON_SIZE_BUTTON);
				
				menu.set_pixbuf((*it).second);
				stoolbar.set_pixbuf((*it).second);
				btn.set_pixbuf((*it).second);
				
				m_DefaultSets[name].add_source(menu);
				m_DefaultSets[name].add_source(stoolbar);
				m_DefaultSets[name].add_source(btn);
			}; break;
			
			case 22: {
				Gtk::IconSource ltoolbar;
				
				ltoolbar.set_size(Gtk::ICON_SIZE_LARGE_TOOLBAR);
				ltoolbar.set_pixbuf((*it).second);
				
				m_DefaultSets[name].add_source(ltoolbar);
			}; break;
			
			case 32: {
				Gtk::IconSource diag, dnd;
				
				diag.set_size(Gtk::ICON_SIZE_DIALOG);
				dnd.set_size(Gtk::ICON_SIZE_DND);
				
				diag.set_pixbuf((*it).second);
				dnd.set_pixbuf((*it).second);
				
				m_DefaultSets[name].add_source(diag);
				m_DefaultSets[name].add_source(dnd);
			}; break;
		}
	}
	
	// now add all the icon sources to the factory
	for (SetMap::iterator it=m_DefaultSets.begin(); it!=m_DefaultSets.end(); ++it)
		factory->add(lookup_from_set((*it).first), (*it).second);
	
	// finally, incorporate this icon factory
	factory->add_default();
}
