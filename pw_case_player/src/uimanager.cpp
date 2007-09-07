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
// uimanager.cpp: implementations of UI namespace

#include "renderer.h"
#include "uimanager.h"

// constructor
UI::Manager::Manager() {
}

// reverse the velocity of a registered animation
void UI::Manager::reverseVelocity(const std::string &id) {
	// if this animation exists, multiply its velocity by -1
	if (m_Animations.find(id)!=m_Animations.end()) {
		UI::Animation &anim=m_Animations[id];
		anim.velocity*=-1;
	}
}

// register a ui animation that bounces the image from side to side
void UI::Manager::registerSideBounceAnimation(const std::string &id, const std::string &texture, bool horizontal, 
					      const Point &origin, int limitA, int limitB, int speed) {
	// create an animation struct
	UI::Animation anim;
	
	// set current position and velocity
	anim.velocity=1;
	anim.current=origin;
	
	// fill in limits and type based on direction
	if (horizontal) {
		// set limits
		anim.leftLimit=origin.x()+limitA;
		anim.rightLimit=origin.x()+limitB;
		
		// set type
		anim.type=UI::ANIM_SIDE_HBOUNCE;
	}
	else {
		// set limits
		anim.topLimit=origin.y()+limitA;
		anim.bottomLimit=origin.y()+limitB;
		
		// set type
		anim.type=UI::ANIM_SIDE_VBOUNCE;
	}
	
	// set other data
	anim.speed=speed;
	anim.lastDraw=0;
	anim.texture=texture;
	
	// add this animation
	m_Animations[id]=anim;
}

// draw an animation
void UI::Manager::drawAnimation(const std::string &id) {
	// get the requested animation
	if (m_Animations.find(id)==m_Animations.end()) {
		std::cout << "UIManager: animation '" << id << "' not registered\n";
		return;
	}
	
	UI::Animation &anim=m_Animations[id];
	
	// see if it's time to progress the animation
	int now=SDL_GetTicks();
	if (now-anim.lastDraw>=anim.speed) {
		// record this time
		anim.lastDraw=now;
		
		// see if we crossed the limit moving on x axis
		if (anim.type==ANIM_SIDE_HBOUNCE) {
			// moving left
			if (anim.velocity==-1 && (anim.current.x()+anim.velocity<anim.leftLimit))
					anim.velocity*=-1;
			
			// moving right
			else if (anim.velocity==1 && (anim.current.x()+anim.velocity>anim.rightLimit))
					anim.velocity*=-1;
			
			// modify position
			anim.current.setX(anim.current.x()+anim.velocity);
		}
	}
	
	// draw the texture
	Renderer::drawImage(anim.current.x(), anim.current.y(), anim.texture);
}
