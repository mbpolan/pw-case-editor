/***************************************************************************
 *   Copyright (C) 2008 by Mike Polan                                      *
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

#include <QPixmap>

/** Class representing a character and its associated sprites.
  * Each character in the case has his or her own set of attributes, and an associated 
  * sprite. All of this data is condensed in this class.
*/
class Character {
	public:
		/// The character's gender
		enum Gender { GENDER_MALE=0, GENDER_FEMALE, GENDER_UNKNOWN };
		
		/** Constructor
		  * \param internal The internal name
		  * \param name The display name
		  * \param caption The text displayed in the green box in the Court Record
		  * \param description Text displayed below the caption in the Court Record
		*/
		Character(const QString &internal="", const QString &name="", 
			    const QString &caption="", const QString &description="");
		
		/** Set the internal character name
		  * \param name The new internal name
		*/
		void setInternalName(const QString &name) { m_InternalName=name; }
		
		/** Get the internal character name
		  * \return The internal name
		*/
		QString getInternalName() const { return m_InternalName; }
		
		/** Set the display name
		  * \param name The character's new display name
		*/
		void setName(const QString &name) { m_Name=name; }
		
		/** Get the display name
		  * \return The character's display name
		*/
		QString getName() const { return m_Name; }
		
		/** Set gender of this character
		  * \param g The new gender type
		*/
		void setGender(const Gender &g) { m_Gender=g; }
		
		/** Get the gender of this character
		  * \return The character's gender
		*/
		Gender getGender() const { return m_Gender; }
		
		/** Set the caption for this character
		  * \param cap The caption
		*/
		void setCaption(const QString &cap) { m_Caption=cap; }
		
		/** Get the caption for this character
		  * \return The character's caption
		*/
		QString getCaption() const { return m_Caption; }
		
		/** Set the description for this character
		  * \param desc The description
		*/
		void setDescription(const QString &desc) { m_Description=desc; }
		
		/** Get the description for this character
		  * \return This character's description
		*/
		QString getDescription() const { return m_Description; }
		
		/** Set the base name of the sprite file to associate with this character
		  * \param spr Path relative to case file for the sprite
		*/
		void setSpriteName(const QString &spr) { m_SpriteName=spr; }
		
		/** Get sprite base file name associated with this character
		  * \return The file path
		*/
		QString getSpriteName() const { return m_SpriteName; }
		
		/** Set if this character has a textbox tag
		  * \param b <b>true</b> if this character has a tag, <b>false</b> otherwise
		*/
		void setHasTextBoxTag(bool b) { m_HasTextBoxTag=b; }
		
		/** Check if this character has a textbox tag
		  * \return <b>true</b> if yes, <b>false</b> otherwise
		*/
		bool hasTextBoxTag() const { return m_HasTextBoxTag; }
		
		/** Set the textbox tag image
		  * \param pixbuf The Gdk::Pixbuf containing textbox tag image data
		*/
		void setTextBoxTag(const QPixmap &pixbuf) { m_TextBoxTag=pixbuf; }
		
		/** Get the textbox tag image
		  * \return The textbox tag image, if it exists
		*/
		QPixmap getTextBoxTag() const { return m_TextBoxTag; }
		
		/** Set if this character has a headshot image
		  * \param b <b>true</b> if yes, <b>false</b> otherwise
		*/
		void setHasHeadshot(bool b) { m_HasHeadshot=b; }
		
		/** Check if this character has a headshot image
		  * \return <b>true</b> if yes, <b>false</b> otherwise
		*/
		bool hasHeadshot() const { return m_HasHeadshot; }
		
		/** Set the headshot image
		  * \param pixbuf The Gdk::Pixbuf containing the image data
		*/
		void setHeadshot(const QPixmap &pixbuf) { m_Headshot=pixbuf; }
		
		/** Get the headshot image
		  * \return The headshot image, if it exists
		*/
		QPixmap getHeadshot() const { return m_Headshot; }
	
	private:
		/// Internal name
		QString m_InternalName;
		
		/// Displayed name
		QString m_Name;
		
		/// Caption
		QString m_Caption;
		
		/// Description
		QString m_Description;
		
		/// Character's sprite name
		QString m_SpriteName;
		
		/// Character's gender
		Gender m_Gender;
		
		/// Flag if this character has a textbox tag
		bool m_HasTextBoxTag;
		
		/// The textbox tag image
		QPixmap m_TextBoxTag;
		
		/// Flag if this character has a headshot image
		bool m_HasHeadshot;
		
		/// The headshot image
		QPixmap m_Headshot;
};

#endif
