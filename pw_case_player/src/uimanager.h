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
#include <GL/gl.h>
#include <map>
#include "SDL.h"

#include "audio.h"
#include "callback.h"
#include "common.h"
#include "texture.h"

class Case::Case;
class Game;

/// Namespace for animations and other interface components
namespace UI {

/// Typedef'd callback for button clicks
typedef Callback<Game, void, const ustring &> ButtonSlot;

/// Limits for animations involving movement of court camera
enum Limit { LIMIT_NONE=0,
             LIMIT_DEFENSE_STAND,
             LIMIT_PROSECUTOR_STAND,
             LIMIT_WITNESS_STAND };

/// Animation stages
enum AnimStage { STAGE_ANIM_INITIAL=-1,		/// The animation is progressing from the start point to the mid point
		 STAGE_ANIM_MID,		/// The animation has reached the mid point
		 STAGE_ANIM_END 		/// The animation has completed
};

/// Types of animations
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
		ANIM_BG_SLIDE,
		ANIM_ADD_EVIDENCE,
		ANIM_ALPHA_DECAY,
		ANIM_WHITE_FLASH };

/** A struct containing animation data.
  * This is a general purpose struct that holds any pertinent data about 
  * an animation. Not all variables are used by some animations.
*/
struct _Animation {
	/// First texture image associated with this animation
	ustring texture;
	
	/// Second texture image associated with this animation
	ustring texture2;
	
	/// Flag that enables drawing the first texture
	bool texture1Active;
	
	/// Flag that enables drawing the second texture
	bool texture2Active;
	
	/// The texture ID
	GLuint textureID;
	
	/// Text for GUI elements
	ustring txt;
	
	/// ID of a sound effect
	ustring sfx;
	
	/// Callback for GUI clicks
	ButtonSlot *callback;
	
	/// Arbitrary width
	int w;
	
	/// Arbitrary height
	int h;
	
	/// Current position of an element
	Point current;
	
	//@{
	/** Points of origin for sync bounce animations */
	Point p1;
	Point p2;
	//@}
	
	/// Used to store difference of two other points
	Point delta;
	
	/// Type of animation this struct represents
	AnimType type;
	
	//@{
	/** Variables to store coordinates for side bounce animations */
	int leftLimit;
	int rightLimit;
	int topLimit;
	int bottomLimit;
	//@}
	
	/// The pixel velocity of the animation
	int velocity;
	
	/// Value included in calculations in animation speed
	int multiplier;
	
	/// The current alpha value
	int alpha;
	
	/// Internal tick counter
	int ticks;
	
	/// The duration of the animation in milliseconds
	int speed;
	
	/// The last time this animation was drawn
	int lastDraw;
};
typedef struct _Animation Animation;

/** Class that represents a GUI button.
  * For simplicity, each GUI button has to be allocated by providing this class
  * with the proper values. The UI::Manager then handles converting the values to 
  * correctly assign the Animation struct members.
*/
class Button {
	public:
		/// Default constructor
		Button() { }
		
		/** Constructor for a text button
		  * \param text The text of the button
		  * \param width The maximum width for this button
		  * \param p The top-left corner where this button should be drawn
		  * \param slot The function to call when clicked, or NULL to take no action
		  * \param sfx Optional sound effect to play on click
		*/
		Button(const ustring &text, int width, const Point &p, ButtonSlot *slot, const ustring &sfx=STR_NULL);
		
		/** Constructor for an image button
		  * \param idle The ID of texture to display when idle
		  * \param active The ID of texture to display when clicked on
		  * \param blinkTime Amount of milliseconds to display the active texture
		  * \param p The top-left corner where this button should be drawn
		  * \param slot The function to call when clicked, or NULL to take no action
		  * \param sfx Optional sound effect to play on click
		*/
		Button(const ustring &idle, const ustring &active, int blinkTime, const Point &p, ButtonSlot *slot, const ustring &sfx=STR_NULL);
		
		/// Free any used memory associated with this button
		void free();
		
		/** Finish the button's animation, regardless of its state
		  * \note This will not execute any callbacks
		*/
		void finishAnimation() { m_Anim.velocity=0; }
		
		/** Set the ID for this button
		  * \param id The ID
		*/
		void setID(const ustring &id) { m_ID=id; }
		
		/** Set the button's text
		  * \param text The text to set
		*/
		void setText(const ustring &text) { m_Anim.txt=text; }
		
		/** Get the button's width
		  * \return The allocated width
		*/
		int getWidth() const { return m_Anim.w; }
		
		/** Get the button's height
		  * \return The allocated height
		*/
		int getHeight() const;
		
		/** Get the button's origin in world coordinates
		  * \return The button's top-left corner coordinates
		*/
		Point getOrigin() const { return m_Anim.current; }
		
		/// Draw the button
		void draw();
		
		/// Activate the button
		void click();
		
		/** Check if a button is still animating
		  * \return <b>true</b> if animating, <b>false</b> otherwise
		*/
		bool isAnimating() const { return m_Anim.velocity; }
		
	private:
		/// Prepare the button's internal animation data
		void initAnim(const Point &p, const ustring &text, int width, const ustring &idle, 
			      const ustring &active, ButtonSlot *slot, const ustring &sfx);
		
		ustring m_ID;
		
		ustring m_IdleID;
		ustring m_ActiveID;
		
		Animation m_Anim;
};

/** The core class that manages all user interface animations and GUI effects.
  * This class is responsible for storing, allocating, and drawing various types of
  * animations that are found in the player. There should only be once instance of this
  * class, and that is in the actual Game class itself, although other functions can make
  * use of the UI::Manager's drawing utilities by calling the UI::Manager::instance() function,
  * which will return the pointer to the instance of the allocated class itself.
  *
  * Most of the rendering functions return a bool, which signifies whether or not the animation 
  * in question has completed. If an animation returns an AnimStage value, then the value returned will 
  * represent the current stage of the animation; that is, these types of animations are done in two parts:
	- STAGE_ANIM_INITIAL: animation is still progressing towards midpoint or towards the end
	- STAGE_ANIM_MID: animation has reached the midpoint
	- STAGE_ANIM_END: animation is done
*/
class Manager {
	public:
		/** Constructor
		  * \param pcase Pointer to a Case::Case object to use for this manager
		*/
		Manager(Case::Case *pcase);
		
		/// Destructor
		~Manager();
		
		/** Get the only instance of this class
		  * \return A pointer to the only instance of this class
		*/
		static Manager* instance();
		
		/// Frees any used memory from all animations
		void free();
		
		/** Get a pointer to an animation struct
		  * \return A pointer to the Animation struct requested, NULL if it doesn't exist
		*/
		Animation* getAnimation(const ustring &id);
		
		/** Handle any mouse events on a GUI element
		  * \param mouse Current position of cursor
		  * \param id ID of GUI element to check
		*/
		void handleGUIClick(const Point &mouse, const ustring &id);
		
		/** Iterate over all GUI elements, see which were clicked, and execute callbacks
		  * \param mouse Current position of cursor
		  * \param ids String vector of IDs of elements to check
		*/
		void handleGUIClick(const Point &mouse, const StringVector &ids);
		
		/** Reverse the velocity of a registered animation
		  * \param id The ID of the animation
		*/
		void reverseVelocity(const ustring &id);
		
		/** Disable one texture of a synchronized bounce animation
		  * \param id The ID of the animation
		  * \param left Passing <b>true</b> disables left texture, <b>false</b> disables right
		*/
		void unsyncBounceTexture(const ustring &id, bool left);
		
		/** Enable one texture of a synchronized bounce animation
		  * \param id The ID of the animation
		  * \param left Passing <b>true</b> enables left texture, <b>false</b> enables right
		*/
		void resyncBounceTexture(const ustring &id, bool left);
		
		/** See if any GUI animations are still occurring
		  * \return <b>true</b> if there are animations still active, <b>false</b> otherwise
		*/
		bool isGUIBusy();
		
		/** Get a button from the internal stack
		  * \param id The button's ID
		  * \return A pointer to the requested button
		*/
		Button* getButton(const ustring &id) { return &m_Buttons[id]; }
		
		/** Check to see if the mouse is over a button
		  * \param id The ID of the button to check
		  * \param p The current position of the cursor
		  * \return <b>true</b> if there is a button under the cursor, <b>false</b> otherwise
		*/
		bool mouseOverButton(const ustring &id, const Point &p);
		
		/** Executes the callback for the clicked button and prepares it for animation
		  * \param id The ID of the button
		*/
		void clickGUIButton(const ustring &id);
		
		/** Register a GUI button
		  * \param id The ID of the button
		  * \param button The Button object to register
		*/
		void registerGUIButton(const ustring &id, const Button &button);
		
		/** Register an animation that bounces an image from side to side.
		  * Limits are relative to origin; that is, if origin is (100, 100), and if the animation
		  * should bounce sideways 10 pixels in each direction, limits should be -10 and 10, respectively
		  * \param id The ID of the animation
		  * \param texture The image to associate
		  * \param horizontal Flag whether or not this animation bounces the image horizontally, or vertically
		  * \param origin The initial starting point
		  * \param limitA The left limit
		  * \param limitB The right limit
		  * \param speed The speed of the animation
		*/
		void registerSideBounceAnimation(const ustring &id, const ustring &texture, bool horizontal, 
						 const Point &origin, int limitA, int limitB, int speed);
		
		/** Register a fade out effect
		  * \param id The ID of the animation
		  * \param speed The speed of the fade out
		  * \param type The type of fade out
		*/
		void registerFadeOut(const ustring &id, int speed, const AnimType &type);
		
		/** Register a flash effect
		  * \param id The ID of the animation
		  * \param speed The speed of the flash
		*/
		void registerFlash(const ustring &id, int speed);
		
		/** Register a court camera movement effect
		  * \param id The ID of the animation
		*/
		void registerCourtCameraMovement(const ustring &id);
		
		/** Register a testimony sprite sequence animation
		  * \param id The ID of the animation
		*/
		void registerTestimonySequence(const ustring &id);
		
		/** Register a cross examination sprite sequence animation
		  * \param id The ID of the animation
		*/
		void registerCrossExamineSequence(const ustring &id);
		
		/** Register an animation that blinks an image
		  * \param id The ID of the animation
		  * \param texture The image to associate
		  * \param p The point at which to draw the image
		  * \param speed The speed of the blinking effect
		*/
		void registerBlink(const ustring &id, const ustring &texture, const Point &p, int speed);
		
		/** Register a synchronized image bounce animation.
		  * For an explanation of the limits in this function, see the documentation for
		  * UI::Manager::registerSideBounceAnimation()
		  * \param id The ID of the animation
		  * \param tex1 The first image
		  * \param tex2 The second image
		  * \param p1 The point at which to draw the first image
		  * \param p2 The point at which to draw the second image
		  * \param limA The left limit
		  * \param limB The right limit
		  * \param speed The speed of the bounce animation
		*/
		void registerSyncBounce(const ustring &id, const ustring &tex1, const ustring &tex2,
					const Point &p1, const Point &p2, int limA, int limB, int speed);
		
		/** Register a green bar control animation
		  * \param id The ID of the animation
		  * \param texture The image to use for the green bar
		  * \param p The point at which to draw this animation
		*/
		void registerGreenBarControl(const ustring &id, const ustring &texture, const Point &p);
		
		/** Register an exclamation animation ("Objection!", "Hold It!", and "Take That!")
		  * \param id The ID of the animation
		  * \param texture The image to use for the exclamation
		  * \param p The point at which to draw the animation
		*/
		void registerExclamation(const ustring &id, const ustring &texture, const Point &p);
		
		/** Register a sliding background animation
		  * \param id The ID of the animation
		*/
		void registerBGSlide(const ustring &id);
		
		/** Register an animation to handle adding evidence
		  * \param id The ID of the animation
		*/
		void registerAddEvidenceSequence(const ustring &id);
		
		/** Register an animation to decrease an alpha value over time
		  * \param id The ID of the animation
		  * \param alpha The variable to associate with this animation, set to initial value
		*/
		void registerAlphaDecay(const ustring &id, int alpha);
		
		/** Register an animation that fades to white, then slowly fades in to a new image
		  * \param id The ID of the animation
		*/
		void registerWhiteFlash(const ustring &id);
		
		/** Draw an arbitrary animation
		  * \param id The ID of the animation
		*/
		void drawAnimation(const ustring &id);
		
		/** Draw a button
		  * \param id The button's ID
		*/
		void drawButton(const ustring &id);
		
		/** Fade out the current scene to black
		  * \param id The ID of the animation
		  * \return An animation return code (see the description of these functions)
		*/
		AnimStage fadeOut(const ustring &id);
		
		/** animate the add evidence animation
		  * \param id The ID of the animation
		  * \param evidence Pointer to evidence to include in the animation
		  * \return An animation return code (see the description of these functions)
		*/
		AnimStage animateAddEvidence(const ustring &id, const Case::Evidence *evidence);
		
		/** Quickly fade out to white, then slowly fade back in
		  * \param id The ID of the animation
		  * \return An animation return code (see the description of these functions)
		*/
		AnimStage whiteFlash(const ustring &id);
		
		/** Perform a flash effect
		  * \param id The ID of the animation
		  * \return <b>true</b> if the animation is done, <b>false</b> otherwise
		*/
		bool flash(const ustring &id);
		
		/** Perform an image blink animation
		  * \param id The ID of the animation
		  * \return <b>true</b> if the animation is done, <b>false</b> otherwise
		*/
		bool blink(const ustring &id);
		
		/** Decay an alph value
		  * \param id The ID of the animation
		  * \param alpha Variable to update with current alpha
		  * \return <b>true</b> if the animation is done, <b>false</b> otherwise
		*/
		bool decayAlpha(const ustring &id, int &alpha);
		
		/** Perform an exclamation animation
		  * \param id The ID of the animation
		  * \param effect The sound effect to play in accordance, or NULL for no sound effect
		  * \param source The character who is the source of this exclamation
		  * \return <b>true</b> if the animation is done, <b>false</b> otherwise
		*/
		bool exclamation(const ustring &id, const Audio::Sample *effect, const Character *source);
		
		/** Slide a background image
		  * \param id The ID of the animation
		  * \param bg The background texture ID
		  * \return <b>true</b> if the animation is done, <b>false</b> otherwise
		*/
		bool slideBG(const ustring &id, const GLuint &bg);
		
		/** Perform a court camera movement
		  * \param id The ID of the animation
		  * \param start The starting location
		  * \param end The ending location
		  * \return <b>true</b> if the animation is done, <b>false</b> otherwise
		*/
		bool moveCourtCamera(const ustring &id, Limit start, Limit end);
		
		/** Animate the testimony sprite sequence
		  * \param id The ID of the animation
		  * \return <b>true</b> if the animation is done, <b>false</b> otherwise
		*/
		bool animateTestimonySequence(const ustring &id);
		
		/** Animate the cross examination sprite sequence
		  * \param id The ID of the animation
		  * \param leftLawyer Image of the character to appear in the upper image strip
		  * \param rightLawyer Image of the character to appear in the lower image strip
		  * \return <b>true</b> if the animation is done, <b>false</b> otherwise
		*/
		bool animateCrossExamineSequence(const ustring &id, const Textures::Texture &leftImg, const Textures::Texture &rightImg);
		
		/** Animate a synchronized bounce animation
		  * \param id The ID of the animation
		  * \return <b>true</b> if the animation is done, <b>false</b> otherwise
		*/
		bool animateSyncBounce(const ustring &id);
		
		/** Animate the green bar for cross examination attempts and other misc things
		  * \param id The ID of the animation
		  * \return <b>true</b> if the animation is done, <b>false</b> otherwise
		*/
		bool animateGreenBar(const ustring &id);
		
	private:
		/// Pointer to current case
		Case::Case *m_Case;
		
		/// Map of registered animations
		std::map<ustring, Animation> m_Animations;
		
		/// Map of registered GUI buttons
		std::map<ustring, Button> m_Buttons;
};

}; // namespace UI

#endif
