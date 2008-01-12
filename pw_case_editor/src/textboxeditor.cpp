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

// constructor
TextBoxEditor::TextBoxEditor() {
	// default format
	m_Format=FORMAT_PLAIN;
	
	// allocate colors
	//m_Blue=Gdk::Color("#6BC6F7");
	//m_Green=Gdk::Color("#00F700"); cm->alloc_color(m_Green);
	//m_Orange=Gdk::Color("#F77339"); cm->alloc_color(m_Orange);
	//m_White=Gdk::Color("white"); cm->alloc_color(m_White);
	
	m_BG=Cairo::ImageSurface::create_from_png("textbox.png");
	
	// set the size request of this widget
	//set_size_request(m_BG->get_width(), m_BG->get_height());
	set_size_request(256, 64);
	
	add_events(Gdk::EXPOSURE_MASK);
}

// set the line text
void TextBoxEditor::set_text(int line, const Glib::ustring &text) {
	m_Text[line]=text;
}

// expose event handler
bool TextBoxEditor::on_expose_event(GdkEventExpose *e) {
	// call base function
	bool ret=Gtk::DrawingArea::on_expose_event(e);
	
	
	// get the window
	Glib::RefPtr<Gdk::Window> window=get_window();
	if (!window)
		return ret;
	
	// create a new context for drawing
	Cairo::RefPtr<Cairo::Context> cr=window->create_cairo_context();
	
	// only draw the region that was exposed
	cr->rectangle(e->area.x, e->area.y, e->area.width, e->area.height);
	cr->clip();
	
	// first, draw the textbox background
	cr->set_source(m_BG, 0, 0);
	cr->paint();
	
	// calculate x based on format
	int x1, x2, x3;
	if (m_Format==FORMAT_DATE || m_Format==FORMAT_TESTIMONY_TITLE) {
		// TODO: centering
	}
	else
		x1=x2=x3=8;
	
	// configure our font
	cr->set_source_rgb(1.0, 1.0, 1.0);
	cr->select_font_face("Arial", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
	cr->set_font_size(13);
	
	// only once, calculate static text height
	static bool once=true;
	if (once) {
		Cairo::TextExtents te;
		cr->get_text_extents("W", te); // use largest character
		m_TextHeight=te.height;
		once=!once;
	}
	
	// cache the lines
	Glib::ustring line1=m_Text[1];
	Glib::ustring line2=m_Text[2];
	Glib::ustring line3=m_Text[3];
	
	int y=m_TextHeight+2;
	int y2=y+m_TextHeight+8;
	
	draw_line(cr, m_Text[1], x1, y+4);
	draw_line(cr, m_Text[2], x1, y2);
	draw_line(cr, m_Text[3], x1, y2+m_TextHeight+2);
	
	return ret;
}

// draw a single line of text
void TextBoxEditor::draw_line(Cairo::RefPtr<Cairo::Context> cr, const std::string &text, int x, int y) {
	cr->move_to(x, y);
	for (int i=0; i<text.size(); i++) {
		Glib::ustring tmp="";
		tmp+=text[i];
		
		cr->show_text(tmp);
		
		Cairo::TextExtents te;
		cr->get_text_extents(tmp, te);
		
		// move the context along
		if (text[i]==' ')
			x+=10;
		else
			x+=te.width+2;
		
		cr->move_to(x, y);
	}
}
