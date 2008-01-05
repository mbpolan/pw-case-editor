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

// game state flags
enum GameFlags {
	STATE_QUEUED=			1 << 0,
	STATE_COURT_REC_BTN=		1 << 1,
	STATE_EVIDENCE_BTN=		1 << 2,
	STATE_PROFILES_BTN=		1 << 3,
	STATE_BACK_BTN=			1 << 4,
	STATE_PRESENT_BTN=		1 << 5,
	STATE_EVIDENCE_PAGE=		1 << 6,
	STATE_PROFILES_PAGE=		1 << 7,
	STATE_EVIDENCE_INFO_PAGE=	1 << 8,
	STATE_PROFILE_INFO_PAGE=	1 << 9,
	STATE_NEXT_BTN=			1 << 10,
	STATE_TEXT_BOX=			1 << 11,
	STATE_CONTROLS=			1 << 12,
	STATE_EXAMINE=			1 << 13,
	STATE_MOVE=			1 << 14,
	STATE_TALK=			1 << 15,
	STATE_PRESENT_TOP_BTN=		1 << 16,
	STATE_CROSS_EXAMINE_BTNS=	1 << 17,
	STATE_PRESS_BTN=		1 << 18,
	STATE_COURT_GREEN_BAR=		1 << 19,
	STATE_CHECK_BTN=		1 << 20,
	STATE_CHECK_EVIDENCE_IMAGE=	1 << 21,
	STATE_CONFIRM_BTN=		1 << 22 };

// menu controls to draw
enum Controls {
	CONTROLS_ALL=		1 << 0,
	CONTROLS_EXAMINE=	1 << 1,
	CONTROLS_MOVE=		1 << 2,
	CONTROLS_TALK=		1 << 3,
	CONTROLS_PRESENT=	1 << 4 };

// position on screen
enum Position {
	POSITION_LEFT=	1,
	POSITION_RIGHT=	2 };

// screens to draw
enum Screen {
	SCREEN_MAIN=	0,
	SCREEN_EXAMINE=	1,
	SCREEN_MOVE=	2,
	SCREEN_TALK=	3,
	SCREEN_PRESENT=	4 };

// court location stand
enum Stand {
	COURT_PROSECUTOR_STAND=0,
	COURT_DEFENSE_STAND,
	COURT_WITNESS_STAND };

// struct that stores the current game state
struct _GameState {
	// flags that toggle what to draw
	int drawFlags;
	
	// current evidence/profile page
	int evidencePage;
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
	std::string requestedEvidenceParams;
	std::string requestedAnswerParams;
	std::string requestedContrParams;
	
	// temporary image to be displayed over location background
	std::string tempImage;
	
	// x,y coordinates of examination cursor
	Point examinePt;
	
	// region of contradiction in provided image of evidence
	std::string contradictionImg;
	Rect contradictionRegion;
	
	// the previous screen displayed
	int prevScreen;
	
	// temporary state changes
	bool hideTextBox;
	
	// music variables
	bool continueMusic;
	
	// testimony variables
	std::string curTestimony;
	int curTestimonyPiece;
	int barPercent; // amount of green bar filled in
	bool curExamination;
	bool curExaminationPaused; // for temporarily breaking away from cross examinations
	
	// special effects
	std::string fadeOut;
	std::string flash;
	std::string blink;
	std::string gavel;
	std::string courtCamera;
	std::string testimonySequence;
	std::string crossExamineSequence;
	std::string exclamation;
	
	// lawyer images for cross examination sequences
	StringPair crossExamineLawyers;
	
	// the current evidence being shown
	std::string shownEvidence;
	Position shownEvidencePos;
	
	// images to display for various parts of the courtroom
	std::string crOverviewDefense;
	std::string crOverviewProsecutor;
	std::string crOverviewWitness;
	
	// our current location
	std::string currentLocation;
	
	// scheduled events
	int queuedFlags;
	std::string queuedLocation;
	std::string queuedBlock;
	
	std::vector<StringPair> custom; // user defined flags
	
	// vector of current talk options
	std::vector<StringPair> talkOptions;
	
	// list of visible evidence (evidence that can be seen in court record)
	std::vector<Case::Evidence> visibleEvidence;
	
	// list of visible profiles
	std::vector<Character> visibleProfiles;
};
typedef struct _GameState GameState;

// class that serves as the game engine
class Game {
	public:
		// constructor
		Game(const std::string &rootPath, Case::Case *pcase);
		
		// destructor
		~Game();
		
		// load stock textures
		bool loadStockTextures();
		
		// render the current scene
		void render();
		
		// return the current case
		Case::Case* getCase() { return m_Case; }
		
		// handle keyboard event
		void onKeyboardEvent(SDL_KeyboardEvent *e);
		
		// handle mouse click event
		void onMouseEvent(SDL_MouseButtonEvent *e);
		
	private:
		// register default animations for ui elements
		void registerAnimations();
		
		// check the current input device state
		void checkInputState();
		
		// get the id of the selected court record evidence
		std::string getSelectedEvidence();
		
		// get the id of the selected court record profile
		std::string getSelectedProfile();
		
		// toggle game state flags
		void toggle(int flags);
		
		// see if an element is flagged to be drawn
		bool flagged(int flag);
		
		// see if the text box should be drawn
		bool shouldDrawTextBox();
		
		// checks to see if area under examine cursors can be examined
		bool canExamineRegion();
		
		// set the current backdrop location
		void setLocation(const std::string &location);
		
		// set the evidence to draw on top screen
		void setShownEvidence(const std::string &id, const Position &pos);
		
		// begin displaying a testimony
		void displayTestimony(const std::string &id, bool crossExamine);
		
		// change the selected evidence/profile
		void selectEvidence(bool evidence=true, bool increment=true);
		
		// see if a location is a court location
		bool isCourtLocation(const std::string &id);
		
		// render the game view (top screen)
		void renderTopView();
		
		// render the menu view (lower screen)
		void renderMenuView();
		
		// render special effects
		bool renderSpecialEffects();
		
		// render the text box
		void renderTextBox();
		
		// render the controls (move, talk, etc)
		void renderControls(int flags);
		
		// render the courtroom overview
		void renderCourtroomOverview();
		
		// render the attorney's stand
		void renderStand(const Stand stand);
		
		// top right button was clicked
		void onTopRightButtonClicked();
		
		// top left button was clicked
		void onTopLeftButtonClicked();
		
		// bottom left button was clicked
		void onBottomLeftButtonClicked();
		
		// bottom right button was clicked
		void onBottomRightButtonClicked();
		
		// centered present button clicked
		void onPresentCenterClicked();
		
		// check evidence button clicked
		void onCheckButtonClicked();
		
		// click handler for controls
		void onControlsClicked(int x, int y);
		
		// click handler for move scene
		void onMoveSceneClicked(int x, int y);
		
		// click handler for talk scene
		void onTalkSceneClicked(int x, int y);
		
		// court record page click handler
		void onRecPageClickEvent(int x, int y);
		
		// court record info page click handler
		void onRecInfoPageClickEvent(int x, int y);
		
		// examine button activated handler
		void onExamineButtonActivated();
		
		// move button activated handler
		void onMoveButtonActivated();
		
		// talk button activated handler
		void onTalkButtonActivated();
		
		// present button activated handler
		void onPresentButtonActivated();
		
		// examine the hotspot in provided coordinate range
		void onExamineThing(int x, int y);
		
		// pointer to current case
		Case::Case *m_Case;
		
		// text parser instance
		TextParser *m_Parser;
		
		// uimanager instance
		UI::Manager *m_UI;
		
		// current text block being executed
		std::string m_CurBlock;
		
		// path where case file resides
		std::string m_RootPath;
		
		// the current state of the game
		GameState m_State;
		
		// friend classes
		friend class TextParser;
};

#endif
