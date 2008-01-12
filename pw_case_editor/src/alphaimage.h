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
// alphaimge.h: the AlphaImage class

#ifndef ALPHAIMAGE_H
#define ALPHAIMAGE_H

#include <cairomm/surface.h>
#include <gtkmm/drawingarea.h>

// image widget with optional alpha value
class AlphaImage: public Gtk::DrawingArea {
	public:
		// constructor
		AlphaImage(const std::string &file);
		
		// set the alpha value
		void set_alpha(double alpha);
		
		// get the alpha value
		double get_alpha() const { return m_Alpha; }
		
	private:
		// expose event handler
		virtual bool on_expose_event(GdkEventExpose*);
		
		// our background image
		Cairo::RefPtr<Cairo::ImageSurface> m_Image;
		
		// alpha value
		double m_Alpha;
};

#endif
