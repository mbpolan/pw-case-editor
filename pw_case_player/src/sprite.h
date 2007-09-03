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
// sprite.h: the Sprite class

#ifndef SPRITE_H
#define SPRITE_H

#include <iostream>
#include <map>
#include <vector>
#include "SDL.h"

// struct representing data about a frame
struct _Frame {
	int time;
	SDL_Surface *image;
};
typedef struct _Frame Frame;

// struct representing a single animation
struct _Animation {
	std::string id;
	std::vector<Frame> frames;
};
typedef struct _Animation Animation;

typedef std::map<std::string, Animation> AnimationMap;
typedef std::map<std::string, Animation>::iterator AnimationMapIter;

// clss representing a sprite
class Sprite {
	public:
		// constructor
		Sprite();
		
		// set animation to play
		void setAnimation(const std::string &anim) { m_CurAnim=anim; }
		
		// animate the sprite
		void animate(int x, int y);
		
		// add an entire animation
		void addAnimation(const Animation &anim) { m_Animations[anim.id]=anim; }
		
		// get an animation sequence
		Animation* getAnimation(const std::string &id);
		
		// get full map of animations
		AnimationMap getAnimations() const { return m_Animations; }
		
		// get the current frame
		Frame* getCurrentFrame();
		
		// get the amount of animations in this sprite
		int numAnimations() const { return m_Animations.size(); }
		
		// add a frame to an animation sequence
		void addFrame(const std::string &id, int time, SDL_Surface *frame);
		
		// reset the sprite
		void reset();
		
	private:
		// the current frame and animation
		std::string m_CurAnim;
		int m_CurFrame;
		int m_LastFrame;
		
		// map of animations
		AnimationMap m_Animations;
};

#endif

