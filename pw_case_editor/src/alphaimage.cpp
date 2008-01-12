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
// alphaimage.cpp: implementations of AlphaImage class

#include "alphaimage.h"

// constructor
AlphaImage::AlphaImage(const std::string &file) {
	m_Image=Cairo::ImageSurface::create_from_png(file);
	m_Alpha=1.0;
	
	set_size_request(m_Image->get_width(), m_Image->get_height());
	
	add_events(Gdk::EXPOSURE_MASK);
}

// set the alpha value
void AlphaImage::set_alpha(double alpha) {
	m_Alpha=alpha;
	queue_draw();
}

// expose event handler
bool AlphaImage::on_expose_event(GdkEventExpose *e) {
	bool ret=Gtk::DrawingArea::on_expose_event(e);
	
	// get our window
	Glib::RefPtr<Gdk::Window> window=get_window();
	if (!window)
		return ret;
	
	// create a new cairo drawing context
	Cairo::RefPtr<Cairo::Context> cr=window->create_cairo_context();
	
	// set the image as the source
	cr->set_source(m_Image, 0, 0);
	
	// move to the top left corner
	cr->move_to(0, 0);
	
	// and draw the image, converting the char alpha to double
	cr->paint_with_alpha(m_Alpha);
	
	return ret;
}
