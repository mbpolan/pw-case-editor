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
// uimanager.h: the UIManager class

#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <iostream>
#include <map>
#include "SDL.h"

#include "common.h"

// the UI namespace
namespace UI {

// animation types
enum AnimType { ANIM_SIDE_HBOUNCE=0,
		ANIM_SIDE_VBOUNCE,
		ANIM_FADE_OUT_TOP,
		ANIM_FADE_OUT_BOTTOM,
		ANIM_FADE_OUT_BOTH };

// a struct containing animation data
struct _Animation {
	// texture image associated with this animation
	std::string texture;
	
	// current position of element
	Point current;
	
	// type of animation
	AnimType type;
	
	// for sideways bounce limitations
	int leftLimit;
	int rightLimit;
	int topLimit;
	int bottomLimit;
	
	// the velocity of the animation
	int velocity;
	
	// current alpha value
	int alpha;
	
	// speed of the animation in milliseconds
	int speed;
	int lastDraw;
};
typedef struct _Animation Animation;

// class that manages ui state
class Manager {
	public:
		// constructor
		Manager();
		
		// reverse the velocity of a registered animation
		void reverseVelocity(const std::string &id);
		
		// register a ui animation that bounces the image from side to side
		// limits are relative to origin; that is, if origin is (100, 100), and if the animation
		// should bounce sideways 10 pixels in each direction, limits should be -10 and 10, respectively
		void registerSideBounceAnimation(const std::string &id, const std::string &texture, bool horizontal, 
						 const Point &origin, int limitA, int limitB, int speed);
		
		// register a fade out effect
		void registerFadeOut(const std::string &id, int speed, const AnimType &type);
		
		// draw an animation
		void drawAnimation(const std::string &id);
		
		// fade out the current scene to black
		// returns true if fade out is done, false otherwise
		bool fadeOut(const std::string &id);
		
	private:
		// map of registered animations
		std::map<std::string, Animation> m_Animations;
};

}; // namespace UI

#endif
