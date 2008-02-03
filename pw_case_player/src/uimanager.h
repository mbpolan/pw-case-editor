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
class Game;

// the UI namespace
namespace UI {

typedef void (Game::*Callback) (const ustring &);

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
		ANIM_FADE_OUT_TOP_HALF,
		ANIM_FADE_OUT_BOTTOM_HALF,
		ANIM_FADE_OUT_BOTH_HALF,
		ANIM_FLASH,
		ANIM_COURT_CAMERA,
		ANIM_TESTIMONY_SPR,
		ANIM_CROSS_EXAMINE_SPR,
		ANIM_BLINK,
		ANIM_SYNC_BOUNCE,
		ANIM_GREEN_BAR,
		ANIM_EXCLAMATION,
		ANIM_GUI_BUTTON,
		ANIM_BG_SLIDE };

// a struct containing animation data (not all variables pertinent)
struct _Animation {
	// texture images associated with this animation
	ustring texture;
	ustring texture2;
	
	// enabled textures
	bool texture1Active;
	bool texture2Active;
	
	// direct pointer to texture
	SDL_Surface *surface;
	
	// text for gui
	ustring txt;
	
	// sound effect
	ustring sfx;
	
	// callback for gui clicks
	Callback callback;
	
	// dimensions
	int w;
	int h;
	
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

// a gui button
class Button {
	public:
		// constructor
		Button(const ustring &text, const Point &p, Callback slot, const ustring &sfx="null") {
			m_Text=text;
			m_Point=p;
			m_Slot=slot;
			m_SFX=sfx;
		}
		
		// get attributes of button
		ustring getText() const { return m_Text; }
		ustring getSFX() const { return m_SFX; }
		Point getPoint() const { return m_Point; }
		Callback getSlot() const { return m_Slot; }
		
	private:
		ustring m_Text;
		ustring m_SFX;
		Point m_Point;
		Callback m_Slot;
};

// class that manages ui state
class Manager {
	public:
		// constructor
		Manager(Case::Case *pcase);
		
		// get the only instance of the ui manager
		static Manager* instance();
		
		// handle any mouse events on gui elements
		// mouse: mouse pointer location; num: number of buttons to test
		void handleGUIClick(const Point &mouse, int num, ...);
		
		// reverse the velocity of a registered animation
		void reverseVelocity(const ustring &id);
		
		// disable one texture of a synchronized bounce animation
		// passing true disables left texture, false disables right
		void unsyncBounceTexture(const ustring &id, bool left);
		
		// enable one texture of a synchronized bounce animation
		// passing true enables left texture, false enables right
		void resyncBounceTexture(const ustring &id, bool left);
		
		// see if any gui animations are still occurring
		bool isGUIBusy();
		
		// set button text for a gui button
		void setGUIButtonText(const ustring &id, const ustring &text);
		
		// check to see if the mouse is over a button
		bool mouseOverButton(const ustring &id, const Point &p);
		
		// set a gui button's state to clicked
		void clickGUIButton(const ustring &id);
		
		// register a gui button
		void registerGUIButton(const ustring &id, int w, const Button &button);
		
		// register a ui animation that bounces the image from side to side
		// limits are relative to origin; that is, if origin is (100, 100), and if the animation
		// should bounce sideways 10 pixels in each direction, limits should be -10 and 10, respectively
		void registerSideBounceAnimation(const ustring &id, const ustring &texture, bool horizontal, 
						 const Point &origin, int limitA, int limitB, int speed);
		
		// register a fade out effect
		void registerFadeOut(const ustring &id, int speed, const AnimType &type);
		
		// register a flash effect
		void registerFlash(const ustring &id, int speed);
		
		// register a court camera effect
		void registerCourtCameraMovement(const ustring &id);
		
		// register a testimony sprite sequence animation
		void registerTestimonySequence(const ustring &id);
		
		// register a cross examination sprite sequence animation
		void registerCrossExamineSequence(const ustring &id);
		
		// register a blinking animation
		void registerBlink(const ustring &id, const ustring &texture, const Point &p, int speed);
		
		// register a synchronized bounce animation
		// offset: see registerSideBounceAnimation
		void registerSyncBounce(const ustring &id, const ustring &tex1, const ustring &tex2,
					const Point &p1, const Point &p2, int limA, int limB, int speed);
		
		// register a green bar control animation
		void registerGreenBarControl(const ustring &id, const ustring &texture, const Point &p);
		
		// register an exclamation animation ("Objection!", "Hold It!", and "Take That!")
		void registerExclamation(const ustring &id, const ustring &texture, const Point &p);
		
		// draw an animation
		void drawAnimation(const ustring &id);
		
		/* 
		   Note: most of the following functions return a bool, which signifies whether
		   or not the animation in question has completed
		*/
		
		// fade out the current scene to black
		// returns -1 for beginning, 0 for midpoint, 1 when done
		int fadeOut(const ustring &id);
		
		// perform a flash effect
		bool flash(const ustring &id);
		
		// perform a blinking animation
		bool blink(const ustring &id);
		
		// perform an exclamation animation
		bool exclamation(const ustring &id, const Character *source);
		
		// perform a court camera movement
		bool moveCourtCamera(const ustring &id, SDL_Surface *panorama, Limit start, Limit end);
		
		// animate the testimony sprite sequence
		bool animateTestimonySequence(const ustring &id);
		
		// animate the cross examination sprite sequence
		bool animateCrossExamineSequence(const ustring &id, SDL_Surface *leftLawyer, SDL_Surface *rightLawyer);
		
		// animate a synchronized bounce animation
		bool animateSyncBounce(const ustring &id);
		
		// animate the green bar for cross examination attempts and other misc things
		bool animateGreenBar(const ustring &id);
		
		// animate a gui button
		bool animateGUIButton(const ustring &id);
		
	private:
		// pointer to current case
		Case::Case *m_Case;
		
		// map of registered animations
		std::map<ustring, Animation> m_Animations;
};

}; // namespace UI

#endif
