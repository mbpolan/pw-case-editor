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

#include "SDL_gfxPrimitives.h"

#include "renderer.h"
#include "texture.h"
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

// register a fade out effect
void UI::Manager::registerFadeOut(const std::string &id, int speed, const AnimType &type) {
	Animation anim;
	
	// fill in values
	anim.alpha=0;
	anim.speed=speed;
	anim.lastDraw=0;
	anim.type=type;
	
	// add animatiom
	m_Animations[id]=anim;
}

// register a flash effect
void UI::Manager::registerFlash(const std::string &id, int speed) {
	Animation anim;
	
	// fill in values
	anim.speed=speed;
	anim.lastDraw=0;
	anim.ticks=0;
	
	// add animation
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

// fade out the current scene to black
bool UI::Manager::fadeOut(const std::string &id) {
	if (m_Animations.find(id)==m_Animations.end()) {
		std::cout << "UIManager: animation '" << id << "' not registered\n";
		return true;
	}
	
	// get the animation
	UI::Animation &anim=m_Animations[id];
	
	// if the alpha is 255, then we're done
	if (anim.alpha>=255) {
		// reset for next time this function is called
		anim.alpha=0;
		return false;
	}
	
	// see if it's time to increase the alpha
	int now=SDL_GetTicks();
	if (now-anim.lastDraw>=anim.speed) {
		anim.lastDraw=now;
		
		// increment the alpha value by 1
		anim.alpha+=1;
	}
	
	// get the opaque surface
	SDL_Surface *opaque=Textures::queryTexture("opaque_black");
	SDL_SetAlpha(opaque, SDL_SRCALPHA, anim.alpha);
	
	// draw this surface, depending on screen
	switch(anim.type) {
		case UI::ANIM_FADE_OUT_TOP: Renderer::drawImage(0, 0, opaque); break;
		case UI::ANIM_FADE_OUT_BOTTOM: Renderer::drawImage(0, 197, opaque); break;
		case UI::ANIM_FADE_OUT_BOTH: {
			Renderer::drawImage(0, 0, opaque);
			Renderer::drawImage(0, 197, opaque);
		} break;
	}
	
	return true;
}

// perform a flash effect
bool UI::Manager::flash(const std::string &id) {
	// get the animation
	if (m_Animations.find(id)==m_Animations.end()) {
		std::cout << "UIManager: animation '" << id << "' not registered\n";
		return false;
	}
	Animation &anim=m_Animations[id];
	bool ret;
	
	// increment tick count
	anim.ticks++;
	
	// we're done
	if (anim.ticks>=anim.speed) {
		anim.ticks=0;
		anim.alpha=0;
		ret=false;
	}
	else {
		anim.alpha=255;
		ret=true;
	}
	
	// only draw the filled in rectangle if this animation isn't complete
	if (anim.alpha!=0)
		boxRGBA(SDL_GetVideoSurface(), 0, 0, 256, 192, 255, 255, 255, anim.alpha);
	
	return ret;
}
