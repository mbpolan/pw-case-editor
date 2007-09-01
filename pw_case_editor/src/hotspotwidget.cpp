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
// hotspotwidget.cpp: implementation of HotspotWidget class

#include <sstream>

#include "hotspotwidget.h"

// constructor
HotspotWidget::HotspotWidget() {
	set_size_request(256, 192);
	
	// zero out the hotspot
	m_Hotspot.rect.x=0;
	m_Hotspot.rect.y=0;
	m_Hotspot.rect.w=0;
	m_Hotspot.rect.h=0;
	
	m_MouseDown=false;
	
	// create colormap
	Glib::RefPtr<Gdk::Colormap> cm=get_default_colormap();
	
	// set colors
	m_Red=Gdk::Color("red");
	cm->alloc_color(m_Red);
	
	// make sure events are received
	add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_MOTION_MASK | Gdk::BUTTON_RELEASE_MASK | 
			Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK);
}

// update (x,y) coordinate pair
void HotspotWidget::update_coords(int x, int y) {
	m_Hotspot.rect.x=x;
	m_Hotspot.rect.y=y;
	
	queue_draw();
}

// update dimensions
void HotspotWidget::update_dimensions(int w, int h) {
	m_Hotspot.rect.w=w;
	m_Hotspot.rect.h=h;
	
	queue_draw();
}

// realize handler
void HotspotWidget::on_realize() {
	Gtk::DrawingArea::on_realize();
	
	// get the window
	Glib::RefPtr<Gdk::Window> window=get_window();
	
	// create the gc
	m_GC=Gdk::GC::create(window);
}

// expose event handler
bool HotspotWidget::on_expose_event(GdkEventExpose *e) {
	// call base function
	bool ret=Gtk::DrawingArea::on_expose_event(e);
	
	// get the window
	Glib::RefPtr<Gdk::Window> window=get_window();
	
	// get its geometry
	int wx, wy, ww, wh, wd;
	window->get_geometry(wx, wy, ww, wh, wd);
	
	// clear it out
	window->clear();
	
	// if we have a pixbuf set, commence drawing
	if (m_Pixbuf) {
		// draw the pixbuf first
		window->draw_pixbuf(m_GC, m_Pixbuf, 0, 0, 0, 0, 
				    m_Pixbuf->get_width(), m_Pixbuf->get_height(), Gdk::RGB_DITHER_NONE, 0, 0);
		
		// now draw the hotspot rectangle
		m_GC->set_foreground(m_Red);
		window->draw_rectangle(m_GC, false, m_Hotspot.rect.x, m_Hotspot.rect.y, m_Hotspot.rect.w, m_Hotspot.rect.h);
	}
	
	return ret;
}

// button press handler
bool HotspotWidget::on_button_press_event(GdkEventButton *e) {
	bool ret=Gtk::DrawingArea::on_button_press_event(e);
	
	// only the first button is applicable
	if (e->button!=1)
		return ret;
	
	// flag that the button was pressed
	static bool once=false;
	if (!once) {
		m_MouseDown=true;
		once=true;
		
		// mark where the button was pressed
		m_Hotspot.rect.x=int(e->x);
		m_Hotspot.rect.y=int(e->y);
		
		// emit signal
		m_SigPointChanged.emit(m_Hotspot.rect.x, m_Hotspot.rect.y);
	}
	
	else {
		m_MouseDown=false;
		once=false;
	}
	
	queue_draw();
	
	return ret;
}

// motion event handler
bool HotspotWidget::on_motion_notify_event(GdkEventMotion *e) {
	bool ret=Gtk::DrawingArea::on_motion_notify_event(e);
	
	// mouse button has to be down to draw
	if (!m_MouseDown)
		return ret;
	
	// update the width and height
	m_Hotspot.rect.w=(int(e->x)-m_Hotspot.rect.x);
	if (m_Hotspot.rect.w<0)
		m_Hotspot.rect.w=0;
	
	m_Hotspot.rect.h=(int(e->y)-m_Hotspot.rect.y);
	if (m_Hotspot.rect.h<0)
		m_Hotspot.rect.h=0;
	
	// emit signal
	m_SigDimensionsChanged.emit(m_Hotspot.rect.w, m_Hotspot.rect.h);
	
	queue_draw();
	
	return ret;
}
