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

#include <gdkmm/pixbuf.h>
#include <glibmm/ustring.h>
#include <map>

// struct representing a frame of animation
struct _Frame {
	int time;
	Glib::RefPtr<Gdk::Pixbuf> pixbuf;
};
typedef struct _Frame Frame;

// struct representing a single animation
struct _Animation {
	Glib::ustring id;
	bool loop;
	std::vector<Frame> frames;
};
typedef struct _Animation Animation;

typedef std::map<Glib::ustring, Animation> AnimationMap;
typedef std::map<Glib::ustring, Animation>::iterator AnimationMapIter;

// clss representing a sprite
class Sprite {
	public:
		// constructor
		Sprite() { }
		
		// add an entire animation
		void add_animation(const Animation &anim) { m_Animations[anim.id]=anim; }
		
		// get an animation sequence
		Animation& get_animation(const Glib::ustring &id) { return m_Animations[id]; }
		
		// get full map of animations
		AnimationMap get_animations() const { return m_Animations; }
		
		// get the amount of animations in this sprite
		int num_animations() const { return m_Animations.size(); }
		
		// remove an animation based on id
		void remove_animation(const Glib::ustring &id) { m_Animations.erase(id); }
		
		// add a frame to an animation sequence
		void add_frame(const Glib::ustring &id, int time, const Glib::RefPtr<Gdk::Pixbuf> &pixbuf) {
			Frame fr;
			fr.time=time;
			fr.pixbuf=pixbuf;
			
			m_Animations[id].frames.push_back(fr);
		}
		
		// remove a frame from an animation sequence
		void remove_frame(const Glib::ustring &id, int index) {
			Animation &anim=m_Animations[id];
			int c=0;
			for (std::vector<Frame>::iterator it=anim.frames.begin(); it!=anim.frames.end(); ++it) {
				// erase this element if it matches the index
				if (c==index) {
					it=anim.frames.erase(it);
					break;
				}
				c+=1;
			}
		}
		
		// clear out all animations and other data
		void clear() { m_Animations.clear(); }
		
	private:
		// map of animations
		AnimationMap m_Animations;
};

#endif
