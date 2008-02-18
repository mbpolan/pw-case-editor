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

#include <cairomm/surface.h>
#include <gtkmm/drawingarea.h>
#include <map>

/** Widget that allows for editing a character's textbox dialogue.
  * In order to provide a good tool for users to insert formatted dialogue, a preview 
  * should be shown. This widget draws up to three lines of text, and sizes them so 
  * they look as though they were drawn in the player.
*/
class TextBoxEditor: public Gtk::DrawingArea {
	public:
		/// Predefined formats for text
		enum Format { FORMAT_PLAIN=0, FORMAT_BLUE, FORMAT_DATE, FORMAT_TESTIMONY_TITLE };
		
		/// Default constructor
		TextBoxEditor();
		
		/** Set text for a given line
		  * \param line The line number, a value between [1,3], inclusive
		  * \param text The text to set
		*/
		void set_text(int line, const Glib::ustring &text);
		
		/** Set the format to use for text
		  * \param format A predefined format
		*/
		void set_format(const Format &format) { m_Format=format; }
		
	private:
		// expose event handler
		virtual bool on_expose_event(GdkEventExpose *e);
		
		// mouse movement handler
		virtual bool on_motion_notify_event(GdkEventMotion *e);
		
		// draw a single line of text
		void draw_line(Cairo::RefPtr<Cairo::Context> cr, const std::string &text, int x, int y);
		
		// background image
		Cairo::RefPtr<Cairo::ImageSurface> m_BG;
		
		/// Record of text format
		Format m_Format;
		
		/// Largest height of a line of text
		int m_TextHeight;
		
		/// X-coordinate of mouse
		int m_MouseX;
		
		/// Y-coordinate of mouse
		int m_MouseY;
		
		/// Map of lines of text
		std::map<int, Glib::ustring> m_Text;
};

#endif
