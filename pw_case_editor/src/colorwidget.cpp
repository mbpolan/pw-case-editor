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
// colorwidget.cpp: implementation of ColorWidget

#include "colorwidget.h"

// constructor
ColorWidget::ColorWidget(const Glib::ustring &color) {
	m_Color=Gdk::Color(color);
	
	init();
}

// constructor taking rgb values
ColorWidget::ColorWidget(char r, char g, char b) {
	m_Color.set_red(r);
	m_Color.set_green(g);
	m_Color.set_blue(b);
	
	init();
}

// initialize the widget
void ColorWidget::init() {
	// allocate our default color
	get_default_colormap()->alloc_color(m_Color);
	
	// default size
	set_size_request(32, 32);
	
	add_events(Gdk::EXPOSURE_MASK);
}

// realize event handler
void ColorWidget::on_realize() {
	Gtk::DrawingArea::on_realize();
	
	Glib::RefPtr<Gdk::Window> window=get_window();
	
	// create our graphics context
	m_GC=Gdk::GC::create(window);
	
	// configure the gc
	m_GC->set_foreground(m_Color);
	window->clear();
}

// expose event handler
bool ColorWidget::on_expose_event(GdkEventExpose *e) {
	bool ret=Gtk::DrawingArea::on_expose_event(e);
	
	// make sure our window is valid
	Glib::RefPtr<Gdk::Window> window=get_window();
	if (!window)
		return ret;
	
	window->clear();
	
	// get our allocation
	Gtk::Allocation allocation=get_allocation();
	
	// draw a rectangle covering this area
	window->draw_rectangle(m_GC, true, 0, 0, allocation.get_width(), allocation.get_height());
	
	return ret;
}
