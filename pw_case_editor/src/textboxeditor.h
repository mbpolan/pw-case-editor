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
// textboxeditor.h: the TextboxEditor widget

#ifndef TEXTBOXWIDGET_H
#define TEXTBOXWIDGET_H

#include <gdkmm/color.h>
#include <gdkmm/pixbuf.h>
#include <gtkmm/drawingarea.h>
#include <map>

// widget that allows for editing a character's text box dialogue
class TextBoxEditor: public Gtk::DrawingArea {
	public:
		// constructor
		TextBoxEditor();
		
		// set text for a given line
		void set_text(int line, const Glib::ustring &text);
		
	private:
		// colors
		Gdk::Color m_White;
		
		// background image
		Glib::RefPtr<Gdk::Pixbuf> m_BG;
		
		// graphics context
		Glib::RefPtr<Gdk::GC> m_GC;
		
		// realize event handler
		virtual void on_realize();
		
		// expose event handler
		virtual bool on_expose_event(GdkEventExpose *e);
		
		// lines of text
		std::map<int, Glib::ustring> m_Text;
};

#endif
