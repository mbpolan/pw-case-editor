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
#include "utilities.h"

using namespace Utils;

// constructor
SplashScreen::SplashScreen(const Glib::ustring &file) {
	// ask to receive button presses
	add_events(Gdk::BUTTON_PRESS_MASK);
	
	Gtk::VBox *vb=Gtk::manage(new Gtk::VBox);
	
	// allocate the image
	m_Image=Gtk::manage(new Gtk::Image(file));
	
	// pack widgets
	vb->pack_start(*m_Image, Gtk::PACK_SHRINK);
	add(*vb);
	
	// configure this window
	set_keep_above(true);
	set_decorated(false);
	
	// calculate center of screen for splash image
	int x, y;
	calculate_center(m_Image->get_pixbuf()->get_width(), m_Image->get_pixbuf()->get_height(), x, y);
	
	// and move it there
	move(x, y);
	
	show_all_children();
}

// mouse button click handler
bool SplashScreen::on_button_press_event(GdkEventButton *e) {
	bool ret=Gtk::Window::on_button_press_event(e);
	
	hide();

	return ret;
}
