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
// game.h: the Game class

#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <vector>

#include "case.h"
#include "common.h"
#include "textparser.h"
#include "uimanager.h"

class Game;

/// Flags that define what should be drawn every cycle
enum GameFlags {
	STATE_QUEUED=			1 << 0,  ///< Specifies that the flags are queued to be drawn
	STATE_COURT_REC_BTN=		1 << 1,  ///< Draw the Court Record button
	STATE_EVIDENCE_BTN=		1 << 2,  ///< Draw the Evidence button
	STATE_PROFILES_BTN=		1 << 3,  ///< Draw the Profiles button
	STATE_BACK_BTN=			1 << 4,  ///< Draw the Back button
	STATE_PRESENT_BTN=		1 << 5,  ///< Draw the Present button
	STATE_EVIDENCE_PAGE=		1 << 6,  ///< Draw the evidence page in the Court Record
	STATE_PROFILES_PAGE=		1 << 7,  ///< Draw the profiles page in the Court Record
	STATE_EVIDENCE_INFO_PAGE=	1 << 8,  ///< Draw the evidence information page
	STATE_PROFILE_INFO_PAGE=	1 << 9,  ///< Draw the profile information page
	STATE_NEXT_BTN=			1 << 10, ///< Draw the large button on the lower screen
	STATE_TEXT_BOX=			1 << 11, ///< Draw the textbox
	STATE_CONTROLS=			1 << 12, ///< Draw the Examine, Move, Talk, and Present buttons
	STATE_EXAMINE=			1 << 13, ///< Draw the examine cursors over a background
	STATE_MOVE=			1 << 14, ///< Draw the locations and previews to move to
	STATE_TALK=			1 << 15, ///< Draw the talk options for a character
	STATE_PRESENT_TOP_BTN=		1 << 16, ///< Draw the Present button centered on the lower screen
	STATE_CROSS_EXAMINE_BTNS=	1 << 17, ///< Draw the two buttons during a cross examination
	STATE_PRESS_BTN=		1 << 18, ///< Draw the Press button
	STATE_COURT_GREEN_BAR=		1 << 19, ///< Draw the green bar for penalties
	STATE_CHECK_BTN=		1 << 20, ///< Draw the Check button to view evidence
	STATE_CHECK_EVIDENCE_IMAGE=	1 << 21, ///< Draw the evidence's check image
	STATE_CONFIRM_BTN=		1 << 22, ///< Draw the Confirm button
	STATE_INITIAL_SCREEN=		1 << 23, ///< Draw the title screen
	STATE_EPISODE_SELECTION=	1 << 24, ///< Draw the episode selection screen
	STATE_CONTINUE_SCREEN=		1 << 25  ///< Draw the continue game screen
						};

/// Menu controls to draw
enum Controls {
	CONTROLS_ALL=		1 << 0,
	CONTROLS_EXAMINE=	1 << 1,
	CONTROLS_MOVE=		1 << 2,
	CONTROLS_TALK=		1 << 3,
	CONTROLS_PRESENT=	1 << 4 };

/// Positions on screen
enum Position {
	POSITION_LEFT=	1,
	POSITION_RIGHT=	2 };

/// Basic screens to draw
enum Screen {
	SCREEN_MAIN=	0,
	SCREEN_EXAMINE=	1,
	SCREEN_MOVE=	2,
	SCREEN_TALK=	3,
	SCREEN_PRESENT=	4 };

/// Court locations
enum Stand {
	COURT_PROSECUTOR_STAND=0,
	COURT_DEFENSE_STAND,
	COURT_WITNESS_STAND };

/** Struct that stores the current game state.
  * Everything that goes on in the game is triggered by the variables
  * in this struct. The drawFlags variable keeps track of what elements
  * of the user interface are drawn, and all other animations are scheduled
  * through the use other variables.
  * 
  * Animation variables are initially set to "none", but through script triggers,
  * these variables can be modified. Additionally, only one animation can be active
  * at a time. If two or more animation variables are active, the more important one,
  * based on order in the struct, is handled first.
*/
struct _GameState {
	/// Flags that toggle what to draw
	int drawFlags;
	
	/// Current evidence page
	int evidencePage;
	
	/// Current profiles page
	int profilesPage;
	
	// selections
	int selectedEvidence;
	int selectedProfile;
	int selectedControl;
	int selectedLocation;
	int selectedTalkOption;
	
	// trial variables
	bool requestingEvidence;
	bool requestingAnswer;
	ustring requestedEvidenceParams;
	ustring requestedAnswerParams;
	ustring requestedContrParams;
	
	/// Temporary image to be displayed over location background
	ustring tempImage;
	
	/// Location of the examination cursor
	Point examinePt;
	
	// region of contradiction in provided image of evidence
	ustring contradictionImg;
	Rect contradictionRegion;
	
	/// The previous screen (Move, Examine, etc) displayed
	int prevScreen;
	
	/// Whether or not to hide the textbox
	bool hideTextBox;
	
	/// Whether or not music should continue playing
	bool continueMusic;
	
	/// The alpha value of the current background
	int bgFade;
	
	// testimony variables
	ustring curTestimony;
	int curTestimonyPiece;
	int barPercent; // amount of green bar filled in
	bool curExamination;
	bool curExaminationPaused; // for temporarily breaking away from cross examinations
	
	// special effects
	int shake;
	ustring alphaDecay;
	ustring fadeOut;
	ustring fadeIn;
	ustring flash;
	ustring blink;
	ustring gavel;
	ustring courtCamera;
	ustring testimonySequence;
	ustring crossExamineSequence;
	ustring exclamation;
	ustring addEvidence;
	
	/// Character images for cross examination sequences
	StringPair crossExamineLawyers;
	
	// the current evidence being shown
	ustring shownEvidence;
	Position shownEvidencePos;
	
	// images to display for various parts of the courtroom
	ustring crOverviewDefense;
	ustring crOverviewProsecutor;
	ustring crOverviewWitness;
	
	/// The current location
	ustring currentLocation;
	
	// scheduled events
	int queuedFlags;
	ustring queuedLocation;
	ustring queuedBlock;
	ustring resetAnimations;
	
	/// User defined flags
	std::vector<StringPair> custom;
	
	/// Vector of current talk options
	std::vector<StringPair> talkOptions;
	
	/// List of visible evidence (evidence that can be seen in Court Record)
	std::vector<ustring> visibleEvidence;
	
	/// List of visible profiles in the Court Record
	std::vector<ustring> visibleProfiles;
};
typedef struct _GameState GameState;

/** The player's game engine.
  * The Game class is the central engine that runs the entire player. Everything 
  * from input handling, to drawing is done from this class's render() function,
  * which also makes sure to manage the flags in the GameState struct so everything
  * is drawn correctly.
  * 
  * This class also relies on the TextParser class to run the script, and responds 
  * to triggers and other user related commands. Every animation is registered here,
  * and use of the UI::Manager and Renderer namespaces is handled mainly through
  * the functions in this class.
  * 
  * Furthermore, all memory related management, that is, deletion of allocated memory,
  * is done in the Game class's destructor. Specifically, the Case::Case object, along
  * with all of the images used in the player, are freed internally through the Game
  * class.
  * 
  * Any functions that need to make use of this class, can call the Game::instance()
  * function, which will return a pointer to the only allocated Game object.
*/
class Game {
	public:
		/** Constructor
		  * \param rootPath The root path to the case file
		  * \param pcase Pointer to a loaded Case::Case object
		*/
		Game(const ustring &rootPath, Case::Case *pcase);
		
		/// Destructor
		~Game();
		
		/** Get a pointer to an instance of this object
		  * \return Pointer to an instance of this class
		*/
		static Game* instance();
		
		/** Load stock textures
		  * \return <b>true</b> if no errors occurred, <b>false</b> otherwise
		*/
		bool loadStockTextures();
		
		/// Render the current scene
		void render();
		
		/** Get a pointer the current case
		  * \return Pointer to the case bound to this engine
		*/
		Case::Case* getCase() { return m_Case; }
		
		/** Handle keyboard events
		  * \param e An SDL_KeyboardEvent struct
		*/
		void onKeyboardEvent(SDL_KeyboardEvent *e);
		
		/** Handle mouse click events
		  * \param e An SDL_MouseButtonEvent struct
		*/
		void onMouseEvent(SDL_MouseButtonEvent *e);
		
	private:
		/// register default animations for UI elements
		void registerAnimations();
		
		/// Check the current input device state
		void checkInputState();
		
		/** Get the ID of the selected Court Record evidence
		  * \return The ID of the selected evidence
		*/
		ustring getSelectedEvidence();
		
		/** Get the ID of the selected Court Record profile
		  * \return The ID of the selected profile
		*/
		ustring getSelectedProfile();
		
		/** Toggle a game state flag
		  * \param flags The flags to toggle
		*/
		void toggle(int flags);
		
		/** See if an element is flagged to be drawn
		  * \param flag The flag to test
		  * \return <b>true</b> if flagged, <b>false</b> otherwise
		*/
		bool flagged(int flag);
		
		/** See if the textbox should be drawn
		  * \return <b>true</b> if yes, <b>false</b> otherwise
		*/
		bool shouldDrawTextBox();
		
		/** Check to see if the area under examine cursors can be examined
		  * \return <b>true</b> if the area can be examined, <b>false</b> otherwise
		*/
		bool canExamineRegion();
		
		/** Set the current location
		  * \param location The ID of the location to set
		*/
		void setLocation(const ustring &location);
		
		/** Set the evidence to draw on top screen
		  * \param id The ID of the evidence
		  * \param pos Which place on the screen should the evidence be drawn at
		*/
		void setShownEvidence(const ustring &id, const Position &pos);
		
		/** Begin displaying a witness testimony
		  * \param id The ID of the testimony
		  * \param crossExamine <b>true</b> if the testimony is to be cross examined, <b>false</b> otherwise
		*/
		void displayTestimony(const ustring &id, bool crossExamine);
		
		/** Change the selected evidence or profile
		  * \param evidence <b>true</b> to change evidence, <b>false</b> to change profile
		  * \param increment <b>true</b> to increment position, <b>false</b> to decrement
		*/
		void selectEvidence(bool evidence=true, bool increment=true);
		
		/** See if a location is a court location
		  * \return <b>true</b> if the location is a court location, <b>false</b> otherwise
		*/
		bool isCourtLocation(const ustring &id);
		
		/// Update the current flags for game state
		void updateFlags();
		
		/// Render the game view (top screen)
		void renderTopView();
		
		/// Render the menu view (lower screen)
		void renderMenuView();
		
		/** Render special effects
		  * \return <b>true</b> if all animations are done, <b>false</b> otherwise
		*/
		bool renderSpecialEffects();
		
		/// Render the textbox
		void renderTextBox();
		
		/** Render the menu controls (move, talk, etc)
		  * \param flags Additional flags to take notice of
		*/
		void renderControls(int flags);
		
		/// Render the courtroom overview
		void renderCourtroomOverview();
		
		/** Render the defense or prosecutor stand
		  * \param stand The court stand to draw
		*/
		void renderStand(const Stand stand);
		
		/** Handler for clicks on the initial screen
		  * \param id The ID of the element clicked
		*/
		void onInitialScreenClicked(const ustring &id);
		
		/// Handler for clicks on the top right button
		void onTopRightButtonClicked();
		
		/// Handler for clicks on the top left button
		void onTopLeftButtonClicked();
		
		/// Handler for clicks on the bottom left button
		void onBottomLeftButtonClicked();
		
		/// Handler for clicks on the bottom right button
		void onBottomRightButtonClicked();
		
		/// Handler for Present button clicks
		void onPresentCenterClicked();
		
		/// Handler for clicks on the Check button for evidence with check images
		void onCheckButtonClicked();
		
		/** Handler for clicks on menu controls
		  * \param p The position of the cursor
		*/
		void onControlsClicked(const Point &p);
		
		/** Handler for clicks on the Move screen
		  * \param button The button ID
		*/
		void onMoveSceneClicked(const ustring &button);
		
		/** Handler for clicks on the Talk screen
		  * \param button The button ID
		*/
		void onTalkSceneClicked(const ustring &button);
		
		/** Handler for clicks on the Court Record
		  * \param p The position of the cursor
		*/
		void onRecPageClickEvent(const Point &p);
		
		/** Handler for clicks on the Court Record information page
		  * \param p The position of the cursor
		*/
		void onRecInfoPageClickEvent(const Point &p);
		
		/// Handler for Examine control button clicks
		void onExamineButtonActivated();
		
		/// Handler for Move control button clicks
		void onMoveButtonActivated();
		
		/// Handler for Talk control button clicks
		void onTalkButtonActivated();
		
		/// Handler for Present control button clicks
		void onPresentButtonActivated();
		
		/** Handler for examining the hotspot in the provided coordinate range
		  * \param p The point under the examination cursor
		*/
		void onExamineThing(const Point &p);
		
		/// Pointer to current case
		Case::Case *m_Case;
		
		/// TextParser instance
		TextParser *m_Parser;
		
		/// UI::Manager instance
		UI::Manager *m_UI;
		
		/// Current text block being executed
		ustring m_CurBlock;
		
		/// Path where case file resides
		ustring m_RootPath;
		
		/// The current state of the game
		GameState m_State;
		
		// friend classes
		friend class TextParser;
};

#endif
