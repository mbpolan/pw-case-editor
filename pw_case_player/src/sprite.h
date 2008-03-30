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

#include <GL/gl.h>
#include <iostream>
#include <map>
#include <vector>
#include "SDL.h"

#include "common.h"

/** Struct representing a frame of animation.
  * Each sprite frame is stored in this struct. Multiple 
  * frames are then combined into Animation structs
*/
struct _Frame {
	/// The time delay in milliseconds for this frame
	int time;
	
	/// Sound effect to play, if any
	ustring sfx;
	
	/// The texture for this frame
	GLuint image;
};
typedef struct _Frame Frame;

/** Struct representing a single animation.
  * Each animation is made up of multiple frames, which can be played 
  * with looping enabled or just once through.
*/
struct _Animation {
	/// The internal ID of this particular animation
	ustring id;
	
	/// Flag whether or not to continually loop the animation
	bool loop;
	
	/// Vector of frames
	std::vector<Frame> frames;
};
typedef struct _Animation Animation;

typedef std::map<ustring, Animation> AnimationMap;
typedef std::map<ustring, Animation>::iterator AnimationMapIter;

/** General clss representing a sprite.
  * The Sprite class stores all of the image data needed to function. It also keeps 
  * track of the current frame of animation, as well as management of many animations. In addition, 
  * it provides the utility to create animations from a series of GIF files. 
  * Besides just storing data, the Sprite class is also in charge of playing the 
  * actual animations, including correctly timing each frame.
*/
class Sprite {
	public:
		/// Default constructor
		Sprite();
		
		/** Flag whether or not this sprite should loop the animations
		  * \param b <b>true</b> to enable, <b>false</b> to disable
		*/
		void toggleLoop(bool b);
		
		/** See if looping is enabled
		  * \return <b>true</b> if enabled, <b>false</b> otherwise
		*/
		bool loop();
		
		/** See if this sprite has finished playing
		  * \return <b>true</b> if done, <b>false</b> otherwise
		*/
		bool done();
		
		/** Set the animation to play
		  * \param anim The ID of the animation
		*/
		void setAnimation(const ustring &anim);
		
		/** Animate the sprite
		  * \param p The coordinates at which to draw the sprite
		*/
		void animate(const Point &p);
		
		/** Render a single frame of current animation
		  * \param p The point where to draw
		  * \param frame The frame to draw
		*/
		void renderFrame(const Point &p, int frame=0);
		
		/** Add an entire animation to the sprite
		  * \param anim The animation to add
		*/
		void addAnimation(const Animation &anim) { m_Animations[anim.id]=anim; }
		
		/** Get the ID of the current animation
		  * \return The current animation's ID
		*/
		ustring getCurrentAnimationId() const { return m_CurAnim; }
		
		/** Get an animation sequence
		  * \param id The ID of the animation
		  * \return Pointer to requested Animation struct
		*/
		Animation* getAnimation(const ustring &id);
		
		/** Get the full map of animations
		  * \return All of the animations in the sprite
		*/
		AnimationMap getAnimations() const { return m_Animations; }
		
		/** Get the current frame
		  * \return Pointer to the current frame
		*/
		Frame* getCurrentFrame();
		
		/** Get the amount of animations in this sprite
		  * \return The amount of animations in the sprite
		*/
		int numAnimations() const { return m_Animations.size(); }
		
		/** Add a frame to an animation sequence
		  * \param id The ID of the target animation
		  * \param time The time duration of the frame
		  * \param frame The frame image ID
		*/
		void addFrame(const ustring &id, int time, const GLuint &frame);
		
		/// Reset the sprite
		void reset();
		
	private:
		// the current frame and animation
		ustring m_CurAnim;
		int m_CurFrame;
		int m_LastFrame;
		
		/// Internal map of animations
		AnimationMap m_Animations;
};

#endif

