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

// class representing a character and its associated sprites
class Character {
	public:
		// character gender
		enum Gender { GENDER_MALE=0, GENDER_FEMALE, GENDER_UNKNOWN };
		
		// constructor
		Character(const ustring &internal="", const ustring &name="", 
			  const ustring &caption="", const ustring &description="");
		
		// add a talk option
		void addTalkOption(const ustring &viewStr, const ustring &block);
		
		// remove a talk option
		void removeTalkOption(const ustring &id);
		
		// clear talk options
		void clearTalkOptions() { m_TalkOptions.clear(); }
		
		// return talk options
		std::vector<StringPair> getTalkOptions() const { return m_TalkOptions; }
		
		// add a presentable piece of evidence/profile
		void addPresentable(const ustring &id, const ustring &targetBlock);
		
		// remove presentable evidence/profile
		void removePresentable(const ustring &id);
		
		// clear accepted evidence/profiles
		void clearPresentableItems() { m_AcceptedItems.clear(); }
		
		// set the block id to use when a bad item is presented
		void setBadPresentableBlock(const ustring &id) { m_BadPresentableBlock=id; }
		
		// set the block to use when a bad item is presented
		ustring getBadPresentableBlock() const { return m_BadPresentableBlock; }
		
		// return evidence that can be presented
		std::vector<StringPair> getPresentableItems() const { return m_AcceptedItems; }
		
		// set the internal character name
		void setInternalName(const ustring &name) { m_InternalName=name; }
		
		// get the internal character name
		ustring getInternalName() const { return m_InternalName; }
		
		// set the displayed name
		void setName(const ustring &name) { m_Name=name; }
		
		// get the displayed name
		ustring getName() const { return m_Name; }
		
		// set the character's gender
		void setGender(const Gender &g) { m_Gender=g; }
		
		// get the character's gender
		Gender getGender() const { return m_Gender; }
		
		// set the caption
		void setCaption(const ustring &caption) { m_Caption=caption; }
		
		// get the caption
		ustring getCaption() const { return m_Caption; }
		
		// set the description
		void setDescription(const ustring &desc) { m_Description=desc; }
		
		// get the description
		ustring getDescription() const { return m_Description; }
		
		// set the sprite for this character
		void setSprite(const Sprite &spr) { m_Sprite=spr; }
		
		// get the sprite for this animation
		Sprite* getSprite() { return &m_Sprite; }
		
		// set sprite name
		void setSpriteName(const ustring &name) { m_SpriteName=name; }
		
		// get sprite name
		ustring getSpriteName() const { return m_SpriteName; }
		
		// set the root sprite animation (normal, smirk, etc)
		void setRootAnimation(const ustring &anim) { m_RootAnim=anim; }
		
		// get the root sprite animation
		ustring getRootAnimation() const { return m_RootAnim; }
		
		// set if this character has a text box tag
		void setHasTextBoxTag(bool b) { m_HasTextBoxTag=b; }
		
		// check if this character has a text box tag
		bool hasTextBoxTag() const { return m_HasTextBoxTag; }
		
		// set the text box image
		void setTextBoxTag(SDL_Surface *surface) { m_TextBoxTag=surface; }
		
		// get the text box tag image
		SDL_Surface* getTextBoxTag() { return m_TextBoxTag; }
		
		// set if this character has a headshot image
		void setHasHeadshot(bool b) { m_HasHeadshot=b; }
		
		// check if this character has a headshot image
		bool hasHeadshot() const { return m_HasHeadshot; }
		
		// set the headshot image
		void setHeadshot(SDL_Surface *full, SDL_Surface *thumb) { m_Headshot=full; m_HeadshotThumb=thumb; }
		
		// get the headshot image
		SDL_Surface* getHeadshot() { return m_Headshot; }
		
		// get the scaled headshot image
		SDL_Surface* getHeadshotThumb() { return m_HeadshotThumb; }
	
	private:
		// internal name
		ustring m_InternalName;
		
		// displayed name
		ustring m_Name;
		
		// caption
		ustring m_Caption;
		
		// description
		ustring m_Description;
		
		// sprite name
		ustring m_SpriteName;
		ustring m_RootAnim;
		
		// text box image
		bool m_HasTextBoxTag;
		SDL_Surface *m_TextBoxTag;
		
		// sprite
		Sprite m_Sprite;
		
		// character gender
		Gender m_Gender;
		
		// vector of talk options
		std::vector<StringPair> m_TalkOptions;
		
		// vector of presentable evidence/profile ids and target blocks
		std::vector<StringPair> m_AcceptedItems;
		ustring m_BadPresentableBlock;
		
		// headshot image
		bool m_HasHeadshot;
		SDL_Surface *m_Headshot;
		SDL_Surface *m_HeadshotThumb;
};

#endif
