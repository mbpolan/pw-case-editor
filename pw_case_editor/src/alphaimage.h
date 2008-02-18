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

/** Image widget with optional alpha value.
  * This widget displays an image with a customizable alpha value 
  * for transparency
*/
class AlphaImage: public Gtk::DrawingArea {
	public:
		/** Constructor
		  * \param file Path to image file
		*/
		AlphaImage(const std::string &file);
		
		/** Set the alpha value for the image
		  * \param alpha The alpha value, should be between 0.0 to 1.0
		*/
		void set_alpha(double alpha);
		
		/** Get the alpha value
		  * \return The image's alpha value
		*/
		double get_alpha() const { return m_Alpha; }
		
	private:
		// expose event handler
		virtual bool on_expose_event(GdkEventExpose*);
		
		// our background image
		Cairo::RefPtr<Cairo::ImageSurface> m_Image;
		
		/// The internal alpha value
		double m_Alpha;
};

#endif
