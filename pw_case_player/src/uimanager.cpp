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
UI::Manager::Manager(Case::Case *pcase): m_Case(pcase) {
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

// register a court camera effect
void UI::Manager::registerCourtCameraMovement(const std::string &id) {
	Animation anim;
	
	// fill in values
	anim.speed=1;
	anim.lastDraw=0;
	anim.velocity=0;
	anim.multiplier=85;
	anim.current=Point(0, 0);
	
	// add animation
	m_Animations[id]=anim;
}

// register a testimony sprite sequence animation
void UI::Manager::registerTestimonySequence(const std::string &id) {
	Animation anim;
	
	// fill in values
	anim.speed=5;
	anim.lastDraw=0;
	anim.topLimit=0;
	anim.bottomLimit=256;
	anim.velocity=1;
	anim.multiplier=3;
	
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
	Renderer::drawImage(anim.current, anim.texture);
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
		case UI::ANIM_FADE_OUT_TOP: Renderer::drawImage(Point(0, 0), opaque); break;
		case UI::ANIM_FADE_OUT_BOTTOM: Renderer::drawImage(Point(0, 197), opaque); break;
		case UI::ANIM_FADE_OUT_BOTH: {
			Renderer::drawImage(Point(0, 0), opaque);
			Renderer::drawImage(Point(0, 197), opaque);
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

// perform a court camera movement
bool UI::Manager::moveCourtCamera(const std::string &id, SDL_Surface *panorama, UI::Limit start, UI::Limit end) {
	// make sure the animation is valid
	if (m_Animations.find(id)==m_Animations.end()) {
		std::cout << "UIManager: animation '" << id << "' not registered\n";
		return true;
	}
	
	// make sure the panorama is valid
	if (!panorama) {
		std::cout << "UIMananger: supplied panorama is invalid!\n";
		return true;
	}
	
	// get the animation
	Animation &anim=m_Animations[id];
	Point &cur=anim.current;
	
	// modify velocity based on direction once
	if (anim.velocity==0) {
		// we're starting from the prosecutor stand
		if (start==UI::LIMIT_PROSECUTOR_STAND) {
			anim.velocity=-1;
			anim.current.setX(panorama->w-256);
		}
		
		// starting from defense stand
		else if (start==UI::LIMIT_DEFENSE_STAND) {
			anim.velocity=1;
			anim.current.setX(0);
		}
		
		// starting from witness stand
		else if (start==UI::LIMIT_WITNESS_STAND) {
			anim.current.setX(472);
			
			// see where we are going
			if (end==UI::LIMIT_PROSECUTOR_STAND)
				anim.velocity=1;
			else if (end==UI::LIMIT_DEFENSE_STAND)
				anim.velocity=-1;
		}
	}
	
	// move the panorama
	int now=SDL_GetTicks();
	if (now-anim.lastDraw>1) {
		anim.lastDraw=now;
		
		// calculate real velocity based on multiplier value
		int velocity=anim.velocity*anim.multiplier;
		
		// calculate the end point
		int endPt;
		if (end==UI::LIMIT_DEFENSE_STAND)
			endPt=0;
		else if (end==UI::LIMIT_PROSECUTOR_STAND)
			endPt=panorama->w-256;
		else if (end==UI::LIMIT_WITNESS_STAND)
			endPt=472;
		
		// cache our x coordinate
		int x=cur.x();
		
		// moving the camera to the left
		if (velocity<0) {
			// if we are still not done, move the camera
			if (x>endPt) {
				// make sure not to overshoot the end point
				if (x+velocity<=endPt)
					cur.setX(endPt);
				else
					cur.setX(x+velocity);
			}
			
			// we're reached the end point
			else
				return true;
		}
		
		// moving camera to the right
		else if (velocity>0) {
			// if we are still not done, move the camera
			if (x<endPt) {
				// once again, make sure not to overshoot the far right point
				if (x+velocity>=endPt)
					cur.setX(endPt);
				else
					cur.setX(x+velocity);
			}
			
			// we're done
			else
				return true;
		}
	}
	
	// draw the panorama
	Renderer::drawImage(Point(cur.x(), 0), 256, 192, panorama, SDL_GetVideoSurface());
	
	return false;
}

// animate the testimony sprite sequence
bool UI::Manager::animateTestimonySequence(const std::string &id) {
	// make sure the animation is valid
	if (m_Animations.find(id)==m_Animations.end()) {
		std::cout << "UIManager: animation '" << id << "' not registered\n";
		return true;
	}
	
	Animation &anim=m_Animations[id];
	
	// check for existance of sprites
	Character *tt=m_Case->getCharacter("testimony_top");
	Character *tb=m_Case->getCharacter("testimony_bottom");
	if (!tt || !tb) {
		std::cout << "UIManager: needed sprites 'testimony_top' and 'testimony_bottom' not found\n";
		return true;
	}
	
	// now get sprites
	Sprite *stt=tt->getSprite();
	Sprite *stb=tb->getSprite();
	
	// disable looping
	stt->toggleLoop(false);
	stb->toggleLoop(false);
	
	// calculate center points for both
	static int centerxTop=128-(stt->getCurrentFrame()->image->w/2);
	static int centerxBottom=128-(stb->getCurrentFrame()->image->w/2);
	
	// calculate y values
	static int yTop=5;
	static int yBottom=stt->getCurrentFrame()->image->h+10;
	
	// we are not animating the sprites
	if (anim.topLimit!=centerxTop || anim.bottomLimit!=centerxBottom) {
		// we're still progressing towards the center
		if (anim.velocity==1) {
			// move sprites across the screen
			anim.topLimit+=1*anim.multiplier;
			anim.bottomLimit-=3*anim.multiplier;
			
			// center the top
			if (anim.topLimit>=centerxTop)
				anim.topLimit=centerxTop;
			
			// center the bottom
			if (anim.bottomLimit<=centerxBottom)
				anim.bottomLimit=centerxBottom;
		}
		
		// otherwise, we have already centered both sprites, and
		// now must move away
		else {
			// move sprites across the screen
			anim.topLimit+=1*anim.multiplier;
			anim.bottomLimit-=1*anim.multiplier;
			
			// clamp the top limit
			if (anim.topLimit>=256)
				anim.topLimit=256;
			
			// clamp the bottom limit
			if (anim.bottomLimit<=-stb->getCurrentFrame()->image->w)
				anim.bottomLimit=-stb->getCurrentFrame()->image->w;
			
			// only if both sprites have been removed from the screen, can the
			// animation end
			if (anim.topLimit==256 && anim.bottomLimit==-stb->getCurrentFrame()->image->w)
				return true;
		}
		
		// draw a single frame of the sprites
		stt->renderFrame(anim.topLimit, yTop);
		stb->renderFrame(anim.bottomLimit, yBottom);
	}
	
	// the two sprites have reached their center x points
	else if (anim.topLimit==centerxTop && anim.bottomLimit==centerxBottom) {
		static int ticks=0;
		
		// just as soon as the sprites have reached their center positions,
		// draw a white rectangle to simulate a flash effect
		if (ticks<5) {
			SDL_Surface *screen=SDL_GetVideoSurface();
			
			// draw the rectangle
			Renderer::drawRect(screen, Point(0, 0), 256, 192, SDL_MapRGB(screen->format, 255, 255, 255));
			ticks++;
		}
		
		// once that's done, proceed to animate the sprites
		else {
			stt->animate(centerxTop, yTop);
			stb->animate(centerxBottom, yBottom);
			
			if (stt->done() && stt->done()) {
				//stt->reset();
				//stb->reset();
				
				// flag that we're continuing moving the sprites
				anim.velocity=0;
				
				// start an initial movement
				anim.topLimit+=3*anim.multiplier;
				anim.bottomLimit-=1*anim.multiplier;
			}
		}
	}
	
	return false;
}
