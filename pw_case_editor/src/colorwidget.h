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
// colorwidget.h: the ColorWidget class

#ifndef COLORWIDGET_H
#define COLORWIDGET_H

#include <gtkmm/drawingarea.h>

/** Widget that displays a color.
  * The given color is drawn in a small rectangle area. This widget is 
  * intended to provide a preview sample of a color.
*/
class ColorWidget: public Gtk::DrawingArea {
	public:
		/** Constructor taking string representation of color
		  * \param color String representing a color
		*/
		ColorWidget(const Glib::ustring &color);
		
		/** Constructor taking an RGB color
		  * \param r The red component
		  * \param g The green component
		  * \param b The blue component
		*/
		ColorWidget(char r, char g, char b);
		
	private:
		/// Initialize the widget
		void init();
		
		// realize event handler
		virtual void on_realize();
		
		// exposure event handler
		virtual bool on_expose_event(GdkEventExpose *e);
		
		// drawing stuff
		Glib::RefPtr<Gdk::GC> m_GC;
		
		// color
		Gdk::Color m_Color;
};

#endif
