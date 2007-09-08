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
// textboxeditor.cpp: implementation of TextBoxEditor widget

#include "textboxeditor.h"

#include "textbox.xpm"

// constructor
TextBoxEditor::TextBoxEditor() {
	// create colormap
	Glib::RefPtr<Gdk::Colormap> cm=get_default_colormap();
	
	// allocate colors
	m_White=Gdk::Color("white");
	cm->alloc_color(m_White);
	
	// create the background image
	m_BG=Gdk::Pixbuf::create_from_xpm_data(textbox_xpm);
	
	// set the size request of this widget
	set_size_request(m_BG->get_width(), m_BG->get_height());
	
	add_events(Gdk::EXPOSURE_MASK);
}

// set the line text
void TextBoxEditor::set_text(int line, const Glib::ustring &text) {
	m_Text[line]=text;
}

// realize event handler
void TextBoxEditor::on_realize() {
	Gtk::DrawingArea::on_realize();
	
	// get the window
	Glib::RefPtr<Gdk::Window> window=get_window();
	
	// create a graphics context
	m_GC=Gdk::GC::create(window);
}

// expose event handler
bool TextBoxEditor::on_expose_event(GdkEventExpose *e) {
	// call base function
	bool ret=Gtk::DrawingArea::on_expose_event(e);
	
	// get the window
	Glib::RefPtr<Gdk::Window> window=get_window();
	if (!window)
		return ret;
	
	// clear the window out
	window->clear();
	
	// draw the background image
	window->draw_pixbuf(m_GC, m_BG, 0, 0, 0, 0, -1, -1, Gdk::RGB_DITHER_NONE, 0, 0);
	
	// draw lines of text
	m_GC->set_foreground(m_White);
	
	// create pango layouts
	Glib::RefPtr<Pango::Layout> l1c=create_pango_layout(m_Text[1]);
	Glib::RefPtr<Pango::Layout> l2c=create_pango_layout(m_Text[2]);
	Glib::RefPtr<Pango::Layout> l3c=create_pango_layout(m_Text[3]);
	
	// make sure to unify the fonts
	Pango::FontDescription fd1, fd2, fd3;
	fd1.set_size(10*PANGO_SCALE); fd1.set_family("arial");
	fd2.set_size(10*PANGO_SCALE); fd2.set_family("arial");
	fd3.set_size(10*PANGO_SCALE); fd3.set_family("arial");
	
	l1c->set_font_description(fd1);
	l2c->set_font_description(fd2);
	l3c->set_font_description(fd3);
	
	// draw the layouts
	window->draw_layout(m_GC, 8, 4, l1c);
	window->draw_layout(m_GC, 8, 21, l2c);
	window->draw_layout(m_GC, 8, 38, l3c);
	
	return ret;
}
