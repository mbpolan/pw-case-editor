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
// hotspotwidget.h: the HotspotWidget class

#ifndef HOTSPOTWIDGET_H
#define HOTSPOTWIDGET_H

#include <gtkmm/drawingarea.h>
#include <gtkmm/image.h>
#include <vector>

#include "case.h"

// widget that allows the user to select areas on an image
class HotspotWidget: public Gtk::DrawingArea {
	public:
		// constructor
		HotspotWidget();
		
		// update (x,y) coordinate pair
		void update_coords(int x, int y);
		
		// update dimensions
		void update_dimensions(int w, int h);
		
		// set the pixbuf to draw
		void set_image(const Glib::RefPtr<Gdk::Pixbuf> &pb) { m_Pixbuf=pb; }
		
		// return the inputted hotspot
		Case::Hotspot get_hotspot() const { return m_Hotspot; }
		
		// signal emitted when (x,y) pair has changed
		sigc::signal<void, int, int> signal_point_changed() const { return m_SigPointChanged; }
		
		// signal emitted when width,height have changed
		sigc::signal<void, int, int> signal_dimensions_changed() const { return m_SigDimensionsChanged; }
		
	private:
		// signals
		sigc::signal<void, int, int> m_SigPointChanged;
		sigc::signal<void, int, int> m_SigDimensionsChanged;
		
		// realize handler
		virtual void on_realize();
		
		// expose event handler
		virtual bool on_expose_event(GdkEventExpose *e);
		
		// button press handler
		virtual bool on_button_press_event(GdkEventButton *e);
		
		// motion event handler
		virtual bool on_motion_notify_event(GdkEventMotion *e);
		
		// pixbuf to draw
		Glib::RefPtr<Gdk::Pixbuf> m_Pixbuf;
		
		// drawing gc
		Glib::RefPtr<Gdk::GC> m_GC;
		
		// colors we use
		Gdk::Color m_Red;
		
		// flag if the button is down
		bool m_MouseDown;
		
		// the hotspot we are editing
		Case::Hotspot m_Hotspot;
};

#endif
