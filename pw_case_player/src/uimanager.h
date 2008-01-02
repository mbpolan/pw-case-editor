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

class Case::Case;

// the UI namespace
namespace UI {

// define limits for special animations
enum Limit { LIMIT_NONE=0,
             LIMIT_DEFENSE_STAND,
             LIMIT_PROSECUTOR_STAND,
             LIMIT_WITNESS_STAND };

// animation types
enum AnimType { ANIM_SIDE_HBOUNCE=0,
		ANIM_SIDE_VBOUNCE,
		ANIM_FADE_OUT_TOP,
		ANIM_FADE_OUT_BOTTOM,
		ANIM_FADE_OUT_BOTTOM_GUI, // doesn't fade out the top/bottom gui bars
		ANIM_FADE_OUT_BOTH,
		ANIM_FLASH,
		ANIM_COURT_CAMERA,
		ANIM_TESTIMONY_SPR,
		ANIM_BLINK,
		ANIM_SYNC_BOUNCE };

// a struct containing animation data (not all variables pertinent)
struct _Animation {
	// texture images associated with this animation
	std::string texture;
	std::string texture2;
	
	// enabled textures
	bool texture1Active;
	bool texture2Active;
	
	// direct pointer to texture
	SDL_Surface *surface;
	
	// current position of element
	Point current;
	
	// points of origin for sync bounce animations
	Point p1;
	Point p2;
	
	// location differences
	Point delta;
	
	// type of animation
	AnimType type;
	
	// for sideways bounce limitations
	int leftLimit;
	int rightLimit;
	int topLimit;
	int bottomLimit;
	
	// the velocity and its speed multiplier of the animation
	int velocity;
	int multiplier;
	
	// current alpha value
	int alpha;
	
	// internal tick counter
	int ticks;
	
	// speed of the animation in milliseconds
	int speed;
	int lastDraw;
};
typedef struct _Animation Animation;

// class that manages ui state
class Manager {
	public:
		// constructor
		Manager(Case::Case *pcase);
		
		// reverse the velocity of a registered animation
		void reverseVelocity(const std::string &id);
		
		// disable one texture of a synchronized bounce animation
		// passing true disables left texture, false disables right
		void unsyncBounceTexture(const std::string &id, bool left);
		
		// enable one texture of a synchronized bounce animation
		// passing true enables left texture, false enables right
		void resyncBounceTexture(const std::string &id, bool left);
		
		// register a ui animation that bounces the image from side to side
		// limits are relative to origin; that is, if origin is (100, 100), and if the animation
		// should bounce sideways 10 pixels in each direction, limits should be -10 and 10, respectively
		void registerSideBounceAnimation(const std::string &id, const std::string &texture, bool horizontal, 
						 const Point &origin, int limitA, int limitB, int speed);
		
		// register a fade out effect
		void registerFadeOut(const std::string &id, int speed, const AnimType &type);
		
		// register a flash effect
		void registerFlash(const std::string &id, int speed);
		
		// register a court camera effect
		void registerCourtCameraMovement(const std::string &id);
		
		// register a testimony sprite sequence animation
		void registerTestimonySequence(const std::string &id);
		
		// register a cross examination sprite sequence animation
		void registerCrossExamineSequence(const std::string &id);
		
		// register a blinking animation
		void registerBlink(const std::string &id, const std::string &texture, const Point &p, int speed);
		
		// register a synchronized bounce animation
		// offset: see registerSideBounceAnimation
		void registerSyncBounce(const std::string &id, const std::string &tex1, const std::string &tex2,
					const Point &p1, const Point &p2, int limA, int limB, int speed);
		
		// register a green bar control animation
		void registerGreenBarControl(const std::string &id, const std::string &texture, const Point &p);
		
		// register an exclamation animation ("Objection!", "Hold It!", and "Take That!")
		void registerExclamation(const std::string &id, const std::string &texture, const Point &p);
		
		// draw an animation
		void drawAnimation(const std::string &id);
		
		/* 
		   Note: most of the following functions return a bool, which signifies whether
		   or not the animation in question has completed
		*/
		
		// fade out the current scene to black
		// returns -1 for beginning, 0 for midpoint, 1 when done
		int fadeOut(const std::string &id);
		
		// perform a flash effect
		bool flash(const std::string &id);
		
		// perform a blinking animation
		bool blink(const std::string &id);
		
		// perform an exclamation animation
		bool exclamation(const std::string &id, const Character *source);
		
		// perform a court camera movement
		bool moveCourtCamera(const std::string &id, SDL_Surface *panorama, Limit start, Limit end);
		
		// animate the testimony sprite sequence
		bool animateTestimonySequence(const std::string &id);
		
		// animate the cross examination sprite sequence
		bool animateCrossExamineSequence(const std::string &id, SDL_Surface *leftLawyer, SDL_Surface *rightLawyer);
		
		// animate a synchronized bounce animation
		bool animateSyncBounce(const std::string &id);
		
		// animate the green bar for cross examination attempts and other misc things
		bool animateGreenBar(const std::string &id);
		
	private:
		// pointer to current case
		Case::Case *m_Case;
		
		// map of registered animations
		std::map<std::string, Animation> m_Animations;
};

}; // namespace UI

#endif
