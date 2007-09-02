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

// game state flags
enum GameFlags { 
		 STATE_LOWER_BAR=1,
		 STATE_COURT_REC_BTN=2,
		 STATE_EVIDENCE_BTN=4,
		 STATE_PROFILES_BTN=8,
		 STATE_BACK_BTN=16,
		 STATE_EVIDENCE_PAGE=32,
		 STATE_PROFILES_PAGE=64,
		 STATE_EVIDENCE_INFO_PAGE=128,
		 STATE_PROFILE_INFO_PAGE=512,
		 STATE_NEXT_BTN=1024,
		 STATE_TEXT_BOX=2048,
		 STATE_CONTROLS=4096,
		 STATE_EXAMINE=8192,
		 STATE_MOVE=16384 };

// menu controls to draw
enum Controls {
		CONTROLS_ALL=1,
		CONTROLS_EXAMINE=2,
		CONTROLS_MOVE=4,
		CONTROLS_TALK=8,
		CONTROLS_PRESENT=16 };

// position on screen
enum Position {
		POSITION_LEFT=1,
		POSITION_RIGHT=2 };

// screens to draw
enum Screen {
		SCREEN_MAIN=0,
		SCREEN_EXAMINE=1,
		SCREEN_MOVE=2 };

// struct that stores the current game state
struct _GameState {
	// flags that toggle what to draw
	int drawFlags;
	
	// current evidence/profile page
	int evidencePage;
	int profilesPage;
	
	// selections
	int selectedEvidence;
	int selectedControl;
	int selectedLocation;
	
	// x,y coordinates or examination cursor
	int examineX, examineY;
	
	// the previous screen displayed
	int prevScreen;
	
	// the current evidence being shown
	std::string shownEvidence;
	Position shownEvidencePos;
	
	// our current location
	std::string currentLocation;
	
	std::vector<StringPair> custom; // user defined flags
	
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
		// check the current input device state
		void checkInputState();
		
		// toggle game state flags
		void toggle(int flags);
		
		// set the current backdrop location
		void setLocation(const std::string &location);
		
		// set the evidence to draw on top screen
		void setShownEvidence(const std::string &id, const Position &pos);
		
		// change the selected evidence
		void selectEvidence(bool increment=true);
		
		// render the game view (top screen)
		void renderTopView();
		
		// render the menu view (lower screen)
		void renderMenuView();
		
		// render the text box
		void renderTextBox();
		
		// render the controls (move, talk, etc)
		void renderControls(int flags);
		
		// top right button was clicked
		void onTopRightButtonClicked();
		
		// bottom left button was clicked
		void onBottomLeftButtonClicked();
		
		// click handler for move scene
		void onMoveSceneClicked(int x, int y);
		
		// evidence page click handler
		void onEvidencePageClickEvent(int x, int y);
		
		// evidence info page click handler
		void onEvidenceInfoPageClickEvent(int x, int y);
		
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
