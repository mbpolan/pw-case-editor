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

#include <iostream>
#include "SDL.h"

#include "common.h"
#include "sprite.h"

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
		Character(const ustring &internal="", const ustring &name="", 
			  const ustring &caption="", const ustring &description="");
		
		/** Add a talk option
		  * \param viewStr The display string for this option
		  * \param block The block to execute if selected
		*/
		void addTalkOption(const ustring &viewStr, const ustring &block);
		
		/** Remove a talk option
		  * \param id The name of the talk option to remove
		*/
		void removeTalkOption(const ustring &id);
		
		/// Clear all talk options
		void clearTalkOptions() { m_TalkOptions.clear(); }
		
		/** Get a vector of all talk options
		  * \return A string vector of all talk options
		*/
		std::vector<StringPair> getTalkOptions() const { return m_TalkOptions; }
		
		/** Add a presentable piece of evidence/profile
		  * \param id The ID of the item
		  * \param targetBlock Block to execute if selected
		*/
		void addPresentable(const ustring &id, const ustring &targetBlock);
		
		/** Remove a presentable evidence/profile
		  * \param id The ID of the item to remove
		*/
		void removePresentable(const ustring &id);
		
		/// Clear all accepted evidence/profiles
		void clearPresentableItems() { m_AcceptedItems.clear(); }
		
		/** Set the block ID to use when a bad item is presented
		  * \param id The ID of the block
		*/
		void setBadPresentableBlock(const ustring &id) { m_BadPresentableBlock=id; }
		
		/** Get the block to use when a bad item is presented
		  * \return The ID of the block
		*/
		ustring getBadPresentableBlock() const { return m_BadPresentableBlock; }
		
		/** Get all the evidence and profiles that can be presented
		  * \return String vector of item IDs
		*/
		std::vector<StringPair> getPresentableItems() const { return m_AcceptedItems; }
		
		/** Set the internal character name
		  * \param name The new internal name
		*/
		void setInternalName(const ustring &name) { m_InternalName=name; }
		
		/** Get the internal character name
		  * \return The internal name
		*/
		ustring getInternalName() const { return m_InternalName; }
		
		/** Set the display name
		  * \param name The character's new display name
		*/
		void setName(const ustring &name) { m_Name=name; }
		
		/** Get the display name
		  * \return The character's display name
		*/
		ustring getName() const { return m_Name; }
		
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
		void setCaption(const ustring &caption) { m_Caption=caption; }
		
		/** Get the caption for this character
		  * \return The character's caption
		*/
		ustring getCaption() const { return m_Caption; }
		
		/** Set the description for this character
		  * \param desc The description
		*/
		void setDescription(const ustring &desc) { m_Description=desc; }
		
		/** Get the description for this character
		  * \return This character's description
		*/
		ustring getDescription() const { return m_Description; }
		
		/** Set the sprite for this character
		  * \param spr The sprite to set
		*/
		void setSprite(const Sprite &spr) { m_Sprite=spr; }
		
		/** Get the sprite for this character
		  * \return Pointer to the sprite, or NULL if there is none
		*/
		Sprite* getSprite() { return &m_Sprite; }
		
		/** Set the name of the sprite file to associate with this character
		  * \param spr Path relative to case file for the sprite
		*/
		void setSpriteName(const ustring &name) { m_SpriteName=name; }
		
		/** Get sprite file name associated with this character
		  * \return The file path
		*/
		ustring getSpriteName() const { return m_SpriteName; }
		
		/** Set the root sprite animation (normal, smirk, etc)
		  * \param anim The ID of the root animation
		*/
		void setRootAnimation(const ustring &anim) { m_RootAnim=anim; }
		
		/** Get the root sprite animation
		  * \return The ID of the root animation
		*/
		ustring getRootAnimation() const { return m_RootAnim; }
		
		/** Set if this character has a textbox tag
		  * \param b <b>true</b> if this character has a tag, <b>false</b> otherwise
		*/
		void setHasTextBoxTag(bool b) { m_HasTextBoxTag=b; }
		
		/** Check if this character has a textbox tag
		  * \return <b>true</b> if yes, <b>false</b> otherwise
		*/
		bool hasTextBoxTag() const { return m_HasTextBoxTag; }
		
		/** Set the textbox tag image
		  * \param surface The textbox tag image
		*/
		void setTextBoxTag(SDL_Surface *surface) { m_TextBoxTag=surface; }
		
		/** Get the textbox tag image
		  * \return The textbox tag image, if it exists
		*/
		SDL_Surface* getTextBoxTag() { return m_TextBoxTag; }
		
		/** Set if this character has a headshot image
		  * \param b <b>true</b> if yes, <b>false</b> otherwise
		*/
		void setHasHeadshot(bool b) { m_HasHeadshot=b; }
		
		/** Check if this character has a headshot image
		  * \return <b>true</b> if yes, <b>false</b> otherwise
		*/
		bool hasHeadshot() const { return m_HasHeadshot; }
		
		/** Set the headshot (profile) image
		  * \param pixbuf The image for the profile headshot
		*/
		void setHeadshot(SDL_Surface *full, SDL_Surface *thumb) { m_Headshot=full; m_HeadshotThumb=thumb; }
		
		/** Get the headshot (profile) image
		  * \return The headshot image, if it exists
		*/
		SDL_Surface* getHeadshot() { return m_Headshot; }
		
		/** Get the scaled headshot image.
		  * The scaled image is used in certain animations, and it automatically generated when ever
		  * the headshot image gets updated or changed
		  * \return The scaled headshot iamge, if it exists
		*/
		SDL_Surface* getHeadshotThumb() { return m_HeadshotThumb; }
	
	private:
		/// The internal name
		ustring m_InternalName;
		
		/// The displayed name
		ustring m_Name;
		
		/// The caption
		ustring m_Caption;
		
		/// The description
		ustring m_Description;
		
		/// The sprite name
		ustring m_SpriteName;
		
		/// The ID of the root animation
		ustring m_RootAnim;
		
		/// Flag whether or not this character has a textbox tag
		bool m_HasTextBoxTag;
		
		/// The textbox tag image
		SDL_Surface *m_TextBoxTag;
		
		/// Sprite associated with this character
		Sprite m_Sprite;
		
		/// Character's gender
		Gender m_Gender;
		
		/// Vector of talk options
		std::vector<StringPair> m_TalkOptions;
		
		/// Vector of presentable evidence/profile IDs and target blocks
		std::vector<StringPair> m_AcceptedItems;
		
		/// Block to execute if a useless item is presented
		ustring m_BadPresentableBlock;
		
		/// Flag whether or not this character has a headshot (profile) image
		bool m_HasHeadshot;
		
		/// The headshot image
		SDL_Surface *m_Headshot;
		
		/// The scaled headshot image
		SDL_Surface *m_HeadshotThumb;
};

#endif
