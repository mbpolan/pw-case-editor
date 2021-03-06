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
// splashscreen.h: the SplashScreen window class

#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <gtkmm/image.h>
#include <gtkmm/window.h>

/** Borderless window that functions like a splash screen.
  * Since GTK doesn't provide a stock widget to do splash screens, 
  * this window serves to emulate one.
*/
class SplashScreen: public Gtk::Window {
	public:
		/** Constructor
		  * \param file The path to the image
		*/
		SplashScreen(const Glib::ustring &file);
		
	private:
		// mouse button click handler
		virtual bool on_button_press_event(GdkEventButton *e);
		
		// image
		Gtk::Image *m_Image;
};

#endif
