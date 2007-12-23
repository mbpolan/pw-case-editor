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
// splashscreen.cpp: implementation of SplashScreen class

#include <gtkmm/box.h>

#include "splashscreen.h"

// constructor
SplashScreen::SplashScreen(const Glib::ustring &file) {
	Gtk::VBox *vb=manage(new Gtk::VBox);
	
	// allocate the image
	m_Image=manage(new Gtk::Image(file));
	
	// pack widgets
	vb->pack_start(*m_Image, Gtk::PACK_SHRINK);
	add(*vb);
	
	// configure this window
	set_keep_above(true);
	set_decorated(false);
	
	show_all_children();
}
