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

#include <cstdarg>
#include "SDL_gfxPrimitives.h"

#include "audio.h"
#include "game.h"
#include "renderer.h"
#include "texture.h"
#include "theme.h"
#include "uimanager.h"
#include "utilities.h"

UI::Manager *g_Manager;

// constructor for text button
UI::Button::Button(const ustring &text, int width, const Point &p, Callback slot, const ustring &sfx) {
	initAnim(p, text, width, STR_NULL, STR_NULL, slot, sfx);
}

// constructor for image button
UI::Button::Button(const ustring &idle, const ustring &active, int blinkTime, const Point &p, Callback slot, const ustring &sfx) {
	Textures::Texture idleTex=Textures::queryTexture(idle);
	initAnim(p, STR_NULL, idleTex.w, idle, active, NULL, sfx);
}

// prepare the button's internal animation data
void UI::Button::initAnim(const Point &p, const ustring &text, int width, const ustring &idle, const ustring &active, 
			  Callback slot, const ustring &sfx) {
	m_Anim.lastDraw=0;
	m_Anim.w=width;
	m_Anim.txt=text;
	m_Anim.type=ANIM_GUI_BUTTON;
	m_Anim.current=p;
	m_Anim.callback=slot;
	m_Anim.texture1Active=true;
	m_Anim.speed=100;
	m_Anim.ticks=50;
	m_Anim.sfx=sfx;
	m_Anim.velocity=0; // 0 for idle, 1 for clicked
}

// draw the button
void UI::Button::draw() {
	// draw the correct type: text or image
	if (m_Anim.txt!=STR_NULL) {
		// if velocity is 1, then the button was clicked
		if (m_Anim.velocity==1) {
			int now=SDL_GetTicks();
			if (now-m_Anim.lastDraw>m_Anim.speed) {
				m_Anim.lastDraw=now;
				m_Anim.texture1Active=!m_Anim.texture1Active;
			}
		
			m_Anim.ticks-=1;
			if (m_Anim.ticks<=0) {
				initAnim(m_Anim.current, m_Anim.txt, m_Anim.w, m_IdleID, m_ActiveID, m_Anim.callback, m_Anim.sfx);
			
				// if a callback is registered, call it now
				if (m_Anim.callback!=NULL)
					(Game::instance()->*m_Anim.callback)(m_ID);
			}
		}
	
		// draw the button
		if (m_Anim.texture1Active)
			Renderer::drawButton(m_Anim.current, m_Anim.w, m_Anim.txt);
	}
}

// activate the button
void UI::Button::click() {
	m_Anim.velocity=1;
	if (m_Anim.sfx!=STR_NULL)
		Audio::playEffect(m_Anim.sfx, Audio::CHANNEL_GUI);
}

/**************************************************************************************************/

// constructor
UI::Manager::Manager(Case::Case *pcase): m_Case(pcase) {
	g_Manager=this;
}

// return an instance of the ui manager
UI::Manager* UI::Manager::instance() {
	return g_Manager;
}

// get a pointer to an animation struct
UI::Animation* UI::Manager::getAnimation(const ustring &id) {
	if (m_Animations.find(id)==m_Animations.end())
		return NULL;
	
	return &m_Animations[id];
}

// handle any mouse events on a gui element
void UI::Manager::handleGUIClick(const Point &mouse, const ustring &id) {
	StringVector vec;
	vec.push_back(id);
	
	handleGUIClick(mouse, vec);
}

// handle any mouse events on gui elements
void UI::Manager::handleGUIClick(const Point &p, const StringVector &ids) {
	// iterate over provided buttons
	for (int i=0; i<ids.size(); i++) {
		// see if this button was clicked, and if it was, flag it
		if (mouseOverButton(ids[i], p)) {
			UI::Button *button=&m_Buttons[ids[i]];
			button->click();
			
			// don't check anything afterwards if this was clicked
			return;
		}
	}
}

// reverse the velocity of a registered animation
void UI::Manager::reverseVelocity(const ustring &id) {
	// if this animation exists, multiply its velocity by -1
	if (m_Animations.find(id)!=m_Animations.end()) {
		UI::Animation &anim=m_Animations[id];
		anim.velocity*=-1;
	}
}

// disable one texture of a synchronized bounce animation
void UI::Manager::unsyncBounceTexture(const ustring &id, bool left) {
	Animation &anim=m_Animations[id];
	if (left)
		anim.texture1Active=false;
	else
		anim.texture2Active=false;
}

// enable one texture of a synchronized bounce animation
void UI::Manager::resyncBounceTexture(const ustring &id, bool left) {
	Animation &anim=m_Animations[id];
	if (left)
		anim.texture1Active=true;
	else
		anim.texture2Active=true;
}

// see if any gui animations are still occurring
bool UI::Manager::isGUIBusy() {
	for (std::map<ustring, UI::Button>::iterator it=m_Buttons.begin(); it!=m_Buttons.end(); ++it) {
		if ((*it).second.isAnimating())
			return true;
	}
	
	return false;
}

// check to see if the mouse is over a button
bool UI::Manager::mouseOverButton(const ustring &id, const Point &p) {
	UI::Button &button=m_Buttons[id];
	
	Rect rect(button.getOrigin(), button.getWidth(), 26);
	return Utils::pointInRect(p, rect);
}

// register a gui button
void UI::Manager::registerGUIButton(const ustring &id, const Button &b) {
	m_Buttons[id]=b;
	UI::Button *button=&m_Buttons[id];
	button->setID(id);
}

// register a ui animation that bounces the image from side to side
void UI::Manager::registerSideBounceAnimation(const ustring &id, const ustring &texture, bool horizontal, 
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
void UI::Manager::registerFadeOut(const ustring &id, int speed, const AnimType &type) {
	Animation anim;
	
	// fill in values
	anim.type=type;
	anim.alpha=0;
	anim.speed=speed;
	anim.lastDraw=0;
	anim.type=type;
	anim.velocity=1;
	
	// add animatiom
	m_Animations[id]=anim;
}

// register a flash effect
void UI::Manager::registerFlash(const ustring &id, int speed) {
	Animation anim;
	
	// fill in values
	anim.type=ANIM_FLASH;
	anim.speed=speed;
	anim.lastDraw=0;
	anim.ticks=0;
	
	// add animation
	m_Animations[id]=anim;
}

// register a court camera effect
void UI::Manager::registerCourtCameraMovement(const ustring &id) {
	Animation anim;
	
	// fill in values
	anim.type=ANIM_COURT_CAMERA;
	anim.speed=1;
	anim.lastDraw=0;
	anim.velocity=0;
	anim.multiplier=50;
	anim.current=Point(0, 0);
	
	// add animation
	m_Animations[id]=anim;
}

// register a testimony sprite sequence animation
void UI::Manager::registerTestimonySequence(const ustring &id) {
	Animation anim;
	
	// fill in values
	anim.type=ANIM_TESTIMONY_SPR;
	anim.speed=10;
	anim.lastDraw=0;
	anim.topLimit=0;
	anim.bottomLimit=256;
	anim.velocity=1;
	anim.multiplier=3;
	
	// add animation
	m_Animations[id]=anim;
}

// register a cross examination sprite sequence animation
void UI::Manager::registerCrossExamineSequence(const ustring &id) {
	Animation anim;
	
	// fill in values
	anim.type=ANIM_CROSS_EXAMINE_SPR;
	anim.speed=10;
	anim.lastDraw=0;
	anim.rightLimit=256; // x value of right lawyer image
	anim.leftLimit=0; // x value of left lawyer image
	anim.topLimit=0; // keeps track of top sprite
	anim.bottomLimit=256; // keeps track of bottom sprite
	anim.velocity=1;
	anim.multiplier=3;
	
	// add the animation
	m_Animations[id]=anim;
}

// register a blinking animation
void UI::Manager::registerBlink(const ustring &id, const ustring &texture, const Point &p, int speed) {
	Animation anim;
	
	// fill in values
	anim.type=ANIM_BLINK;
	anim.speed=speed;
	anim.lastDraw=0;
	anim.current=p;
	anim.ticks=0;
	anim.velocity=1; // velocity here specifies either draw or hide
	anim.texture=texture;
	
	// add the animation
	m_Animations[id]=anim;
}

// register a synchronized bounce animation
void UI::Manager::registerSyncBounce(const ustring &id, const ustring &tex1, const ustring &tex2,
				     const Point &p1, const Point &p2, int limA, int limB, int speed) {
	Animation anim;
	
	// fill in values
	anim.type=ANIM_SYNC_BOUNCE;
	anim.speed=speed;
	anim.lastDraw=0;
	anim.velocity=1;
	anim.delta=Point(0, 0);
	anim.p1=p1;
	anim.p2=p2;
	anim.leftLimit=limA;
	anim.rightLimit=limB;
	anim.texture=tex1;
	anim.texture2=tex2;
	anim.texture1Active=true;
	anim.texture2Active=true;
	
	// add the animation
	m_Animations[id]=anim;
}

// register a green bar control animation
void UI::Manager::registerGreenBarControl(const ustring &id, const ustring &texture, const Point &p) {
	Animation anim;
	
	// fill values
	anim.type=ANIM_GREEN_BAR;
	anim.lastDraw=0;
	anim.velocity=1;
	anim.speed=10;
	anim.texture=texture;
	anim.current=p;
	anim.rightLimit=100; // current amount fill in
	anim.leftLimit=0; // amount to decrease
	
	// add the animation
	m_Animations[id]=anim;
}

// register an exclamation animation ("Objection!", "Hold It!", and "Take That!")
void UI::Manager::registerExclamation(const ustring &id, const ustring &texture, const Point &p) {
	Animation anim;
	
	// fill in value
	anim.type=ANIM_EXCLAMATION;
	anim.speed=75; // 1.25 seconds at 60 fps
	anim.ticks=0;
	anim.texture=texture;
	anim.current=p;
	
	// add the animation
	m_Animations[id]=anim;
}

// register a sliding background animation
void UI::Manager::registerBGSlide(const ustring &id) {
	Animation anim;
	
	// fill in values
	anim.type=ANIM_BG_SLIDE;
	anim.speed=5;
	anim.current=Point(0, 0);
	anim.bottomLimit=192;
	
	// add the animation
	m_Animations[id]=anim;
}

// register an animation to handle adding evidence
void UI::Manager::registerAddEvidenceSequence(const ustring &id) {
	Animation anim;
	
	// fill in values
	anim.type=ANIM_ADD_EVIDENCE;
	anim.lastDraw=0;
	anim.velocity=1;
	anim.speed=5;
	anim.current=Point(256, 0, Z_ANIM_SPRITE);
	
	// add the animation
	m_Animations[id]=anim;
}

// register an alpha fade animation
void UI::Manager::registerAlphaDecay(const ustring &id, int alpha) {
	Animation anim;
	
	// fill in values
	anim.type=ANIM_ALPHA_DECAY;
	anim.lastDraw=0;
	anim.speed=2;
	anim.multiplier=2;
	anim.alpha=alpha;
	
	// add the animation
	m_Animations[id]=anim;
}

// register a white flash animation
void UI::Manager::registerWhiteFlash(const ustring &id) {
	Animation anim;
	
	// fill in values
	anim.type=ANIM_WHITE_FLASH;
	anim.lastDraw=0;
	anim.speed=10;
	anim.multiplier=2;
	anim.velocity=1;
	anim.alpha=0;
	
	// add the animation
	m_Animations[id]=anim;
}

// draw an animation
void UI::Manager::drawAnimation(const ustring &id) {
	// get the requested animation
	if (!getAnimation(id)) {
		Utils::debugMessage("UIManager: animation '"+id+"' not registered.");
		return;
	}
	
	UI::Animation &anim=*getAnimation(id);
	
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

// draw a button
void UI::Manager::drawButton(const ustring &id) {
	UI::Button *button=&m_Buttons[id];
	button->draw();
}

// fade out the current scene to black
UI::AnimStage UI::Manager::fadeOut(const ustring &id) {
	if (!getAnimation(id)) {
		Utils::debugMessage("UIManager: animation '"+id+"' not registered.");
		return UI::STAGE_ANIM_END;
	}
	
	// get the animation
	UI::Animation &anim=*getAnimation(id);
	
	// see if it's time to increase the alpha
	int now=SDL_GetTicks();
	if (now-anim.lastDraw>=anim.speed) {
		anim.lastDraw=now;
		
		// increment the alpha value by 1
		anim.alpha+=(2*anim.speed)*anim.velocity;
		
		// clamp values
		if (anim.alpha>255) anim.alpha=255;
		else if (anim.alpha<0) anim.alpha=0;
	}
	
	// set the color
	Color color(0, 0, 0, anim.alpha);
	
	// draw this surface, depending on screen
	switch(anim.type) {
		case UI::ANIM_FADE_OUT_TOP_HALF:
		case UI::ANIM_FADE_OUT_TOP: Renderer::drawRect(Rect(Point(0, 0, Z_FADE), 256, 192), color); break;
		
		case UI::ANIM_FADE_OUT_BOTTOM_HALF:
		case UI::ANIM_FADE_OUT_BOTTOM: Renderer::drawRect(Rect(Point(0, 197, Z_FADE), 256, 192), color); break;
		
		case UI::ANIM_FADE_OUT_BOTH_HALF:
		case UI::ANIM_FADE_OUT_BOTH: Renderer::drawRect(Rect(Point(0, 0, Z_FADE), 256, 389), color); break;
		
		case UI::ANIM_FADE_OUT_BOTTOM_GUI: Renderer::drawRect(Rect(Point(0, 215, Z_FADE), 256, 156), color); break;
	}
	
	// if the alpha is 255, then begin fading in
	if (anim.alpha>=255 && anim.velocity==1) {
		anim.alpha=255;
		anim.velocity=-1;
		
		// for half animations, end here
		if (anim.type==UI::ANIM_FADE_OUT_TOP_HALF ||
		    anim.type==UI::ANIM_FADE_OUT_BOTTOM_HALF ||
		    anim.type==UI::ANIM_FADE_OUT_BOTH_HALF) {
			UI::Manager::registerFadeOut(id, anim.speed, anim.type);
			return UI::STAGE_ANIM_END;
		}
		
		else
			return UI::STAGE_ANIM_MID;
	}
	
	// we're done with this special effect
	else if (anim.alpha<=0 && anim.velocity==-1) {
		// reset for next time
		UI::Manager::registerFadeOut(id, anim.speed, anim.type);
		
		return UI::STAGE_ANIM_END;
	}
	
	return UI::STAGE_ANIM_INITIAL;
}

// animate the add evidence animation
UI::AnimStage UI::Manager::animateAddEvidence(const ustring &id, const Case::Evidence *evidence) {
	// get the animation
	if (!getAnimation(id)) {
		Utils::debugMessage("UIManager: animation '"+id+"' not registered.");
		return UI::STAGE_ANIM_END;
	}
	
	Animation &anim=*getAnimation(id);
	
	// draw the info strip across the top of the screen
	Renderer::drawInfoStrip(anim.current, Textures::queryTexture(evidence->texture), 
				evidence->name, evidence->caption, evidence->description, false);
	
	// moving the info strip to the left
	int x=anim.current.x();
	if (x!=0) {
		anim.current.setX(x-anim.speed);
		
		// still moving left initially
		if (anim.velocity==1) {
			// midpoint at x=0
			if (x<0) {
				anim.current.setX(0);
				return UI::STAGE_ANIM_MID;
			}
			
			return UI::STAGE_ANIM_INITIAL;
		}
		
		// moving left after midpoint
		else {
			// we're done if we hit -256
			if (x<-256) {
				anim.current.setX(-256);
				return UI::STAGE_ANIM_END;
			}
			
			return UI::STAGE_ANIM_INITIAL;
		}
	}
	
	else
		return UI::STAGE_ANIM_MID;
}

// fade out to white, then slowly fade back in
UI::AnimStage UI::Manager::whiteFlash(const ustring &id) {
	// get the animation
	if (!getAnimation(id)) {
		Utils::debugMessage("UIManager: animation '"+id+"' not registered.");
		return UI::STAGE_ANIM_END;
	}
	
	Animation &anim=*getAnimation(id);
	
	// first, fade out into white
	int now=SDL_GetTicks();
	if (anim.velocity==1) {
		// play the initial wooshing sound effect
		if (anim.sfx=="0") {
			Audio::playEffect("sfx_flashback", Audio::CHANNEL_SCRIPT);
			anim.sfx="1";
		}
		
		// increase the alpha
		if (now-anim.lastDraw>=anim.speed) {
			anim.lastDraw=now;
			anim.alpha+=anim.velocity*anim.multiplier;
		}
		
		// we've reached the mid point
		if (anim.alpha>255) {
			anim.velocity=-1;
			anim.alpha=255;
			
			// in order to avoid flicker, draw the rectangle
			Renderer::drawRect(Rect(Point(0, 0, Z_FADE), 256, 192), Color(255, 255, 255, anim.alpha));
			
			return UI::STAGE_ANIM_MID;
		}
	}
	
	else {
		// decrease the alpha at a much faster pace
		if (now-anim.lastDraw>=anim.speed) {
			anim.lastDraw=now;
			anim.alpha+=anim.velocity;
		}
		
		// we're done if the alpha is less than or equal to zero
		if (anim.alpha<=0)
			return UI::STAGE_ANIM_END;
	}
	
	// draw the opaque surface with alpha
	Renderer::drawRect(Rect(Point(0, 0, Z_FADE), 256, 192), Color(255, 255, 255, anim.alpha));
	
	return UI::STAGE_ANIM_INITIAL;
}

// perform a flash effect
bool UI::Manager::flash(const ustring &id) {
	// get the animation
	if (!getAnimation(id)) {
		Utils::debugMessage("UIManager: animation '"+id+"' not registered.");
		return true;
	}
	Animation &anim=*getAnimation(id);
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
		Renderer::drawRect(Rect(Point(0, 0, Z_FADE), 256, 192), Color(255, 255, 255, anim.alpha));
	
	return ret;
}

// perform a blinking animation
bool UI::Manager::blink(const ustring &id) {
	// make sure the animation is valid
	if (!getAnimation(id)) {
		Utils::debugMessage("UIManager: animation '"+id+"' not registered.");
		return true;
	}
	
	Animation &anim=*getAnimation(id);
	
	// get the texture
	Textures::Texture tex=Textures::queryTexture(anim.texture);
	
	// see if it's time to display the image
	int now=SDL_GetTicks();
	if (now-anim.lastDraw>anim.speed) {
		anim.lastDraw=now;
		anim.velocity=(anim.velocity ? 0 : 1);
	}
	
	// if our velocity (in this context, to draw or not) is positive, we draw the texture
	if (anim.velocity)
		Renderer::drawImage(anim.current, tex);
	
	// blinking animations never end
	return false;
}

// decay an alpha value
bool UI::Manager::decayAlpha(const ustring &id, int &alpha) {
	// make sure this animation is valid
	if (!getAnimation(id)) {
		Utils::debugMessage("UIManager: animation '"+id+"' not registered.");
		return true;
	}
	Animation &anim=*getAnimation(id);
	
	// decrement the alpha value
	int now=SDL_GetTicks();
	if (now-anim.lastDraw>anim.speed) {
		if (anim.alpha-anim.multiplier<=0) {
			anim.alpha=0;
			return true;
		}
		
		else
			anim.alpha-=anim.multiplier;
		
		anim.lastDraw=now;
	}
	
	// update the parameter
	alpha=anim.alpha;
	
	return false;
}

// perform a court camera movement
bool UI::Manager::moveCourtCamera(const ustring &id, UI::Limit start, UI::Limit end) {
	// make sure the animation is valid
	if (!getAnimation(id)) {
		Utils::debugMessage("UIManager: animation '"+id+"' not registered.");
		return true;
	}
	
	// store our return value
	bool ret=false;
	
	// get the animation
	Animation &anim=*getAnimation(id);
	Point &cur=anim.current;
	
	// get the panorama texture
	Textures::Texture panorama=Textures::queryTexture("court_panorama");
	
	// modify velocity based on direction once
	if (anim.velocity==0) {
		// we're starting from the prosecutor stand
		if (start==UI::LIMIT_PROSECUTOR_STAND) {
			anim.velocity=-1;
			anim.current.setX(panorama.w-256);
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
	if (now-anim.lastDraw>7) {
		anim.lastDraw=now;
		
		// calculate real velocity based on multiplier value
		int velocity=anim.velocity*anim.multiplier;
		
		// calculate the end point
		int endPt;
		if (end==UI::LIMIT_DEFENSE_STAND)
			endPt=0;
		else if (end==UI::LIMIT_PROSECUTOR_STAND)
			endPt=panorama.w-256;
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
				ret=true;
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
				ret=true;
		}
	}
	
	// now use the panorama texture
	glBindTexture(GL_TEXTURE_2D, panorama.id);
	
	// save our current matrix
	glPushMatrix();
	glLoadIdentity();
	
	// and translate our texture by however much pixels the current point is at
	glTranslatef(-cur.x(), 0, 0);
	
	Case::Case *pCase=Game::instance()->getCase();
	
	// finally, draw the panorama as the background
	Renderer::drawImage(Point(0, 0, 0.1f), panorama);
	
	// draw all three characters: prosecutor, defense, and witness
	Case::Location *pStand=pCase->getLocation("prosecutor_stand");
	Case::Location *dStand=pCase->getLocation("defense_stand");
	Case::Location *wStand=pCase->getLocation("witness_stand");
	
	Character *prosecutor=pCase->getCharacter(pStand->character);
	Character *defense=pCase->getCharacter(dStand->character);
	Character *witness=pCase->getCharacter(wStand->character);
	
	// draw the benches for each character
	Renderer::drawImage(Point(0, 0, Z_SPRITE+0.1f), "defense_bench");
	Renderer::drawImage(Point(520, 0, Z_SPRITE+0.1f), "witness_bench");
	Renderer::drawImage(Point(panorama.w-256, 0, Z_SPRITE+0.1f), "prosecutor_bench");
	
	if (defense) defense->getSprite()->renderFrame(Point(0, 0, Z_SPRITE));
	if (witness) witness->getSprite()->renderFrame(Point(520, 0, Z_SPRITE));
	if (prosecutor) prosecutor->getSprite()->renderFrame(Point(panorama.w-256, 0, Z_SPRITE));
	
	glPopMatrix();
	
	return ret;
}

// animate the testimony sprite sequence
bool UI::Manager::animateTestimonySequence(const ustring &id) {
	// make sure the animation is valid
	if (!getAnimation(id)) {
		Utils::debugMessage("UIManager: animation '"+id+"' not registered.");
		return true;
	}
	
	Animation &anim=*getAnimation(id);
	
	// check for existance of sprites
	Character *tt=m_Case->getCharacter("testimony_top");
	Character *tb=m_Case->getCharacter("testimony_bottom");
	if (!tt || !tb) {
		Utils::debugMessage("UIManager: needed sprites 'testimony_top' and 'testimony_bottom' not found for testimony sequence.");
		return true;
	}
	
	// now get sprites
	Sprite *stt=tt->getSprite();
	Sprite *stb=tb->getSprite();
	
	// disable looping
	stt->toggleLoop(false);
	stb->toggleLoop(false);
	
	// cache current frames
	Textures::Texture topFr=Textures::queryTexture(stt->getCurrentFrame()->image);
	Textures::Texture btmFr=Textures::queryTexture(stb->getCurrentFrame()->image);
	
	// calculate center points for both
	static int centerxTop=128-(topFr.w/2);
	static int centerxBottom=128-(btmFr.w/2);
	
	// calculate y values
	static int yTop=5;
	static int yBottom=topFr.h+10;
	
	// we are not animating the sprites
	if (anim.topLimit!=centerxTop || anim.bottomLimit!=centerxBottom) {
		// we're still progressing towards the center
		if (anim.velocity==1) {
			static bool once=true;
			if (once) {
				Audio::playEffect("sfx_fly_in", Audio::CHANNEL_SCRIPT);
				once=!once;
			}
			
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
			static bool once=true;
			if (once) {
				Audio::playEffect("sfx_fly_out", Audio::CHANNEL_SCRIPT);
				once=!once;
			}
			
			// move sprites across the screen
			anim.topLimit+=1*anim.multiplier;
			anim.bottomLimit-=1*anim.multiplier;
			
			// clamp the top limit
			if (anim.topLimit>=256)
				anim.topLimit=256;
			
			// clamp the bottom limit
			if (anim.bottomLimit<=-btmFr.w)
				anim.bottomLimit=-btmFr.w;
			
			// only if both sprites have been removed from the screen, can the
			// animation end
			if (anim.topLimit==256 && anim.bottomLimit==-btmFr.w)
				return true;
		}
		
		// draw a single frame of the sprites
		stt->renderFrame(Point(anim.topLimit, yTop, Z_ANIM_SPRITE));
		stb->renderFrame(Point(anim.bottomLimit, yBottom, Z_ANIM_SPRITE));
	}
	
	// the two sprites have reached their center x points
	else if (anim.topLimit==centerxTop && anim.bottomLimit==centerxBottom) {
		static int ticks=0;
		
		// just as soon as the sprites have reached their center positions,
		// draw a white rectangle to simulate a flash effect
		if (ticks<5) {
			// draw the rectangle
			Renderer::drawRect(Rect(Point(0, 0, Z_FADE), 256, 192), Color(255, 255, 255));
			ticks++;
		}
		
		// once that's done, proceed to animate the sprites
		else {
			stt->animate(Point(centerxTop, yTop, Z_ANIM_SPRITE));
			stb->animate(Point(centerxBottom, yBottom, Z_ANIM_SPRITE));
			
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

// FIXME: the following function is annoyingly similar to the above testimony one
//        maybe find a way to merge them together in the future
// animate the cross examination sprite sequence
bool UI::Manager::animateCrossExamineSequence(const ustring &id, const Textures::Texture &leftImg, const Textures::Texture &rightImg) {
	// make sure the animation is valid
	if (!getAnimation(id)) {
		Utils::debugMessage("UIManager: animation '"+id+"' not registered.");
		return true;
	}
	
	Animation &anim=*getAnimation(id);
	
	// check for existance of sprites
	Character *xt=m_Case->getCharacter("cross_examine_top");
	Character *xb=m_Case->getCharacter("cross_examine_bottom");
	if (!xt || !xb) {
		Utils::debugMessage("UIManager: needed sprites 'cross_examine_top' and 'cross_examine_bottom' not found.");
		return true;
	}
	
	// now get sprites
	Sprite *xtt=xt->getSprite();
	Sprite *xtb=xb->getSprite();
	
	// disable looping
	xtt->toggleLoop(false);
	xtb->toggleLoop(false);
	
	// cache bottom and top frames
	Textures::Texture topFr=Textures::queryTexture(xtt->getCurrentFrame()->image);
	Textures::Texture btmFr=Textures::queryTexture(xtb->getCurrentFrame()->image);
	
	// calculate center points for both
	static int centerxTop=128-(topFr.w/2);
	static int centerxBottom=128-(btmFr.w/2);
	
	// calculate y values
	static int yTop=5;
	static int yBottom=topFr.h+10;
	
	// for the lower screen, always draw the colorized court overview
	Renderer::drawImage(Point(0, 197, 1.0f), "court_overview_c");
	
	// now draw bounding rectangles for containment of lawyer images
	Renderer::drawRect(Rect(Point(0, 227, 1.1f), 256, 2), Color(0, 0, 0));
	Renderer::drawRect(Rect(Point(0, 277, 1.1f), 256, 2), Color(0, 0, 0));
	
	Renderer::drawRect(Rect(Point(0, 307, 1.1f), 256, 2), Color(0, 0, 0));
	Renderer::drawRect(Rect(Point(0, 357, 1.1f), 256, 2), Color(0, 0, 0));
	
	// then draw the blue line images
	Renderer::drawImage(Point(0, 229, 1.2f), "blueline");
	Renderer::drawImage(Point(0, 309, 1.2f), "blueline");
	
	// and finally, draw the two lawyers' images
	Renderer::drawImage(Point(anim.leftLimit, 229, 1.3f), leftImg);
	Renderer::drawImage(Point(anim.rightLimit, 309, 1.3f), rightImg);
	
	// we are not animating the sprites
	if (anim.topLimit!=centerxTop || anim.bottomLimit!=centerxBottom) {
		// we're still progressing towards the center
		if (anim.velocity==1) {
			// play an introductory sound effect
			static bool once=true;
			if (once) {
				// play a sound effect
				Audio::playEffect("sfx_fly_in", Audio::CHANNEL_SCRIPT);
				
				// also preset the starting positions for right lawyer's image
				anim.rightLimit=256-rightImg.w;
				
				once=!once;
			}
			
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
		
		// we're moving up
		else {
			// once more, play another sound effect
			bool once=true; 
			if (once) {
				// play a sound effect
				Audio::playEffect("sfx_fly_out", Audio::CHANNEL_SCRIPT);
				once=!once;
			}
			
			// once the sprites are moved out of the way, we're done
			if (yTop-anim.delta.y()<=0 && yBottom+anim.delta.y()>=192)
				return true;
			
			// increase the y delta
			anim.delta.setY(anim.delta.y()+2*anim.multiplier);
		}
		
		// draw top sprite frame
		xtt->renderFrame(Point(anim.topLimit, yTop-anim.delta.y(), Z_ANIM_SPRITE));
		Textures::Texture lFrame=Textures::queryTexture(xtb->getCurrentFrame()->image);
		
		// the important part about the bottom sprite, is that it has the possibility
		// of moving below into the lower screen when the animation is in its final stages
		// we first need to test if that has already occurred...
		if ((yBottom+anim.delta.y())+lFrame.h<=192)
			// simply draw the frame in its entirety
			xtb->renderFrame(Point(anim.bottomLimit, yBottom+anim.delta.y(), Z_ANIM_SPRITE));
		
		// well, this sucks; we need to draw only the visible portion
		else {
			// FIXME
			/*
			// fill in the source rectangle, with the cutoff region noted
			SDL_Rect src;
			src.x=0;
			src.y=0;
			src.w=lFrame->w;
			src.h=192-(yBottom+anim.delta.y()); // here's the magic
			
			// for the destination rectangle, fill in values for
			// where the sprite is located
			SDL_Rect dest;
			dest.x=anim.bottomLimit;
			dest.y=yBottom+anim.delta.y();
			
			// now manually blit the frame
			SDL_BlitSurface(lFrame, &src, SDL_GetVideoSurface(), &dest);
			*/
		}
	}
	
	// we reached the midpoint: animate the sprites
	else {
		static int ticks=0;
		
		// just as soon as the sprites have reached their center positions,
		// draw a white rectangle to simulate a flash effect
		if (ticks<8) {
			// draw the rectangle
			Renderer::drawRect(Rect(Point(0, 0, Z_FADE), 256, 192), Color(255, 255, 255));
			ticks++;
		}
		
		// once that's done, proceed to animate the sprites
		else {
			xtt->animate(Point(centerxTop, yTop, Z_ANIM_SPRITE));
			xtb->animate(Point(centerxBottom, yBottom, Z_ANIM_SPRITE));
			
			if (xtt->done() && xtt->done()) {
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
	
	// the two lawyer images should move ever so slightly as well
	static int ticks=0;
	if (SDL_GetTicks()-ticks>300) {
		anim.rightLimit-=1;
		anim.leftLimit+=1;
		
		ticks=SDL_GetTicks();
	}
	
	return false;
}

// animate a synchronized bounce animation
bool UI::Manager::animateSyncBounce(const ustring &id) {
	// get the animation
	if (!getAnimation(id)) {
		Utils::debugMessage("UIManager: animation '"+id+"' not registered.");
		return true;
	}
	
	Animation &anim=*getAnimation(id);
	
	// see if its time to progress the animation
	int now=SDL_GetTicks();
	if (now-anim.lastDraw>=anim.speed) {
		anim.lastDraw=now;
		
		// moving outward
		if (anim.velocity==1) {
			if (anim.delta.x()>=anim.rightLimit)
				anim.velocity=-1;
		}
		
		// move inward
		else {
			if (anim.delta.x()<=anim.leftLimit)
				anim.velocity=1;
		}
		
		anim.delta.setX(anim.delta.x()+anim.velocity);
	}
	
	// draw the first texture
	if (anim.texture1Active)
		Renderer::drawImage(anim.p1+anim.delta, Textures::queryTexture(anim.texture));
	
	// and then the second, with an inverted direction
	if (anim.texture2Active) {
		Point i=anim.delta;
		i.invert();
		Renderer::drawImage(anim.p2+i, Textures::queryTexture(anim.texture2));
	}
}

// animate the green bar for cross examination attempts and other misc things
bool UI::Manager::animateGreenBar(const ustring &id) {
	// get the animation
	if (!getAnimation(id)) {
		Utils::debugMessage("UIManager: animation '"+id+"' not registered.");
		return true;
	}
	
	Animation &anim=*getAnimation(id);
	
	// get the texture
	Textures::Texture texture=Textures::queryTexture(anim.texture);
	
	// see if we need to progress the animation
	int now=SDL_GetTicks();
	if (now-anim.lastDraw>=anim.speed) {
		anim.lastDraw=now;
		
		// decrease fill in percentage until leftLimit is 0
		if (anim.leftLimit!=0) {
			anim.rightLimit-=anim.velocity;
			anim.leftLimit-=anim.velocity;
			
			// clamp the value
			if (anim.leftLimit<0)
				anim.leftLimit=0;
		}
	}
	
	// draw the correct percentage of the bar
	int fill=texture.w*(anim.rightLimit/100);
	
	// FIXME
	/*
	// prepare source rectangle
	SDL_Rect src;
	src.x=0;
	src.y=0;
	src.w=fill;
	src.h=texture.h;
	
	// prepare destination rectangle
	SDL_Rect dest;
	dest.x=anim.current.x();
	dest.y=anim.current.y();
	
	// blit the image
	SDL_BlitSurface(texture, &src, SDL_GetVideoSurface(), &dest);
	*/
	
	return false;
}

// perform an exclamation animation
bool UI::Manager::exclamation(const ustring &id, const Audio::Sample *effect, const Character *source) {
	// get the animation
	if (!getAnimation(id)) {
		Utils::debugMessage("UIManager: animation '"+id+"' not registered.");
		return true;
	}
	
	Animation &anim=*getAnimation(id);
	
	// get the texture
	Textures::Texture texture=Textures::queryTexture(anim.texture);
	
	// play the sound effect, once only
	if (anim.ticks==0 && effect)
		Audio::playEffect(effect->id, Audio::CHANNEL_SCRIPT);
	
	// see if we should keep drawing
	if (anim.ticks<anim.speed) {
		Point shake(0, 0, Z_ANIM_SPRITE);
		
		// shake only for the first two thirds of the animation
		if (anim.speed-anim.ticks>=(anim.speed/3)) {
			shake=Utils::calculateShakePoint(5);
			shake.setZ(Z_ANIM_SPRITE);
		}
		
		// draw the texture
		Renderer::drawImage(anim.current+shake, texture);
		
		// increment tick counter
		anim.ticks++;
		
		return false;
	}
	
	// we're done
	else {
		anim.ticks=0;
		return true;
	}
}

// slide a background down
bool UI::Manager::slideBG(const ustring &id, const GLuint &bg) {
	// get the animation
	if (!getAnimation(id)) {
		Utils::debugMessage("UIManager: animation '"+id+"' not registered.");
		return true;
	}
	
	Animation &anim=*getAnimation(id);
	
	// get the background texture
	Textures::Texture tex=Textures::queryTexture(bg);
	
	// still sliding down
	int y=anim.current.y();
	if (y!=192) {
		// draw the texture at this current point
		Renderer::drawImage(Point(0, y+5, 1.5f), tex);
		y+=anim.speed;
		if (y>192)
			y=192;
		
		anim.current.setY(y);
		
		return false;
	}
	
	else {
		Renderer::drawImage(Point(0, 197, 1.5f), tex);
		return true;
	}
	
	return true;
}
