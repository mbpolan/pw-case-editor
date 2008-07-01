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
// sprite.h: the Sprite class

#ifndef SPRITE_H
#define SPRITE_H

#include <QPixmap>
#include <map>

/** Struct representing a frame of animation.
  * Each sprite frame is stored in this struct. Multiple 
  * frames are then combined into Animation structs
*/
struct _Frame {
	/// The time delay in milliseconds for this frame
	int time;
	
	/// Sound effect to play, if any
	QString sfx;
	
	/// Image data for this frame
	QPixmap pixbuf;
};
typedef struct _Frame Frame;

/** Struct representing a single animation.
  * Each animation is made up of multiple frames, which can be played 
  * with looping enabled or just once through.
*/
struct _Animation {
	/// The internal ID of this particular animation
	QString id;
	
	/// Flag whether or not to continually loop the animation
	bool loop;
	
	/// Vector of frames
	std::vector<Frame> frames;
};
typedef struct _Animation Animation;

typedef std::map<QString, Animation> AnimationMap;
typedef std::map<QString, Animation>::iterator AnimationMapIter;

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
		
		/** Set the ID of the sprite's default animation
		  * \param id The animation ID
		*/
		void setDefaultAnimation(const QString &id) { m_DefAnim=id; }
		
		/** Get the ID of the default animation
		  * \return ID of the default animation
		*/
		QString getDefaultAnimation() const { return m_DefAnim; }
		
		/** Create a sprite from GIFs in a directory
		  * \param path The path to the directory
		  * \return <b>true</b> if successful, <b>false</b> otherwise
		*/
		bool createFromGIFs(const QString &path);
		
		/** Add an entire animation to the sprite
		  * \param anim The animation
		*/
		void addAnimation(const Animation &anim) { m_Animations[anim.id]=anim; }
		
		/** Add an animation from a GIF image
		  * \param id The new ID for this animation
		  * \param path The path to the GIF
		*/
		void addAnimationFromGIF(const QString &id, const QString &path);
		
		/** Get an animation from the sprite
		  * \param id The ID of the animation
		  * \return The requested animation, if found
		*/
		Animation& getAnimation(const QString &id) { return m_Animations[id]; }
		
		/** Get the full map of animations
		  * \return A map of all sprite animations
		*/
		AnimationMap getAnimations() const { return m_Animations; }
		
		/** Get the amount of animations in this sprite
		  * \return The amount of animations
		*/
		int numAnimations() const { return m_Animations.size(); }
		
		/** Remove an animation based on ID
		  * \param id The ID of the animation
		*/
		void removeAnimation(const QString &id) { m_Animations.erase(id); }
		
		/** Add a frame to an animation
		  * \param id The ID of the target animation
		  * \param time Time delay of the frame
		  * \param pixbuf Image data for this frame
		*/
		void addFrame(const QString &id, int time, const QPixmap &pixbuf);
		
		/** Remove a frame from an animation
		  * \param id The target animation's ID
		  * \param index The 0 based index of the frame to remove
		*/
		void removeFrame(const QString &id, int index);
		
		/// Clear out all animations and other data
		void clear() { m_Animations.clear(); }
		
	private:
		/// Map of animations
		AnimationMap m_Animations;
		
		/// ID of default animation
		QString m_DefAnim;
};

#endif
