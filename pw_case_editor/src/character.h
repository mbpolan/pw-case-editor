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
// character.h: the Character class

#ifndef CHARACTER_H
#define CHARACTER_H

#include <gdkmm/pixbuf.h>
#include <glibmm/ustring.h>

// class representing a character and its associated sprites
class Character {
	public:
		// constructor
		Character(const Glib::ustring &internal="", const Glib::ustring &name="", const Glib::ustring &description="");
		
		// set the internal character name
		void set_internal_name(const Glib::ustring &name) { m_InternalName=name; }
		
		// get the internal character name
		Glib::ustring get_internal_name() const { return m_InternalName; }
		
		// set the displayed name
		void set_name(const Glib::ustring &name) { m_Name=name; }
		
		// get the displayed name
		Glib::ustring get_name() const { return m_Name; }
		
		// set the description
		void set_description(const Glib::ustring &desc) { m_Description=desc; }
		
		// get the description
		Glib::ustring get_description() const { return m_Description; }
		
		// set the name of the sprite file to associated
		void set_sprite_name(const Glib::ustring &spr) { m_SpriteName=spr; }
		
		// get sprite name associated with this character
		Glib::ustring get_sprite_name() const { return m_SpriteName; }
		
		// set if this character has a text box tag
		void set_has_text_box_tag(bool b) { m_HasTextBoxTag=b; }
		
		// check if this character has a text box tag
		bool has_text_box_tag() const { return m_HasTextBoxTag; }
		
		// set the text box pixbuf
		void set_text_box_tag(const Glib::RefPtr<Gdk::Pixbuf> &pixbuf) { m_TextBoxTag=pixbuf; }
		
		// get the text box tag pixbuf
		Glib::RefPtr<Gdk::Pixbuf> get_text_box_tag() const { return m_TextBoxTag; }
		
		// set if this character has a headshot image
		void set_has_headshot(bool b) { m_HasHeadshot=b; }
		
		// check if this character has a headshot image
		bool has_headshot() const { return m_HasHeadshot; }
		
		// set the headshot image
		void set_headshot(const Glib::RefPtr<Gdk::Pixbuf> &pixbuf) { m_Headshot=pixbuf; }
		
		// get the headshot image
		Glib::RefPtr<Gdk::Pixbuf> get_headshot() const { return m_Headshot; }
	
	private:
		// internal name
		Glib::ustring m_InternalName;
		
		// displayed name
		Glib::ustring m_Name;
		
		// description
		Glib::ustring m_Description;
		
		// sprite name
		Glib::ustring m_SpriteName;
		
		// text box tag image
		bool m_HasTextBoxTag;
		Glib::RefPtr<Gdk::Pixbuf> m_TextBoxTag;
		
		// headshot image
		bool m_HasHeadshot;
		Glib::RefPtr<Gdk::Pixbuf> m_Headshot;
};

#endif
