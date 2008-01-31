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
// game.cpp: implementation of Game class

#include <cmath>

#include "audio.h"
#include "game.h"
#include "font.h"
#include "iohandler.h"
#include "renderer.h"
#include "texture.h"
#include "utilities.h"

static Game *g_Game=NULL;

// constructor
Game::Game(const std::string &rootPath, Case::Case *pcase): m_RootPath(rootPath), m_Case(pcase) {
	// reset draw flags
	m_State.drawFlags=0;
	
	// reset counters
	m_State.evidencePage=0;
	m_State.profilesPage=0;
	
	// reset selections
	m_State.selectedEvidence=0;
	m_State.selectedProfile=0;
	m_State.selectedControl=0;
	m_State.selectedLocation=0;
	m_State.selectedTalkOption=0;
	
	// reset trial variables
	m_State.requestingEvidence=false;
	m_State.requestingAnswer=false;
	m_State.requestedEvidenceParams="null";
	m_State.requestedAnswerParams="null";
	
	// reset examination cursor position
	m_State.examinePt=Point(256/2, 192/2);
	
	// reset contradiction image variables
	m_State.contradictionImg="null";
	m_State.contradictionRegion=Rect(Point(0, 0), 0, 0);
	
	// reset temporary state variables
	m_State.hideTextBox=false;
	
	// reset testimony variables
	m_State.curTestimony="null";
	m_State.curTestimonyPiece=0;
	m_State.barPercent=100;
	m_State.curExamination=false;
	m_State.curExaminationPaused=false;
	
	// reset courtroom sprites
	m_State.crOverviewDefense="null";
	m_State.crOverviewProsecutor="null";
	m_State.crOverviewWitness="null";
	
	// reset previous page
	m_State.prevScreen=0;
	
	// reset music variables
	m_State.continueMusic=false;
	
	// reset temporary image
	m_State.tempImage="null";
	
	// reset queued events
	m_State.queuedFlags=0;
	m_State.queuedLocation="null";
	m_State.queuedBlock="null";
	
	// reset special effects
	m_State.shake=0;
	m_State.fadeOut="none";
	m_State.flash="none";
	m_State.blink="none";
	m_State.gavel="none";
	m_State.courtCamera="none";
	m_State.testimonySequence="none";
	m_State.crossExamineSequence="none";
	m_State.exclamation="none";
	
	// reset cross examination lawyer image ids
	m_State.crossExamineLawyers.first="null";
	m_State.crossExamineLawyers.second="null";
	
	// null out variables
	m_State.currentLocation="null";
	m_State.shownEvidence="null";
	
	// allocate text parser
	m_Parser=new TextParser(this);
	
	// allocate ui manager
	m_UI=new UI::Manager(m_Case);
	
	g_Game=this;
}

// destructor
Game::~Game() {
	// free textures used
	Textures::clearStack();
	
	// free fonts used
	Fonts::clearFontStack();
	
	// free used audio
	Audio::clearAudioStack();
	
	// delete case
	delete m_Case;
	
	// delete parser
	delete m_Parser;
	
	// delete ui manager
	delete m_UI;
}

// get a pointer to an instance of this object
Game* Game::instance() {
	return g_Game;
}

// load stock textures
bool Game::loadStockTextures() {
	// load stock assets
	if (!IO::loadStockFile(".temp/data/stock.cfg", m_Case)) {
		Utils::alert("Unable to load stock assets from file.");
		return false;
	}
	
	// text box alpha
	Case::Overrides ov=m_Case->getOverrides();
	int tbAlpha=(ov.textboxAlpha!=-1 ? ov.textboxAlpha : 165);
	
	// modify certain textures
	SDL_SetAlpha(Textures::queryTexture("scanlines_overlay"), SDL_SRCALPHA, 30);
	SDL_SetAlpha(Textures::queryTexture("tc_next_btn"), SDL_SRCALPHA, 225);
	SDL_SetAlpha(Textures::queryTexture("tc_text_box"), SDL_SRCALPHA, tbAlpha);
	SDL_SetAlpha(Textures::queryTexture("tc_text_box_border"), SDL_SRCALPHA, tbAlpha);
	
	SDL_SetAlpha(Textures::queryTexture("tc_select_tl"), SDL_SRCALPHA, 225);
	SDL_SetAlpha(Textures::queryTexture("tc_select_tr"), SDL_SRCALPHA, 225);
	SDL_SetAlpha(Textures::queryTexture("tc_select_br"), SDL_SRCALPHA, 225);
	SDL_SetAlpha(Textures::queryTexture("tc_select_bl"), SDL_SRCALPHA, 225);
	
	// map of opaque surfaces
	std::map<std::string, std::pair<int, int> > surfaces;
	surfaces["opaque_black"]=std::make_pair<int, int>(256, 192);
	surfaces["transparent"]=std::make_pair<int, int>(256, 192);
	
	// create these surfaces and add them as textures
	for (std::map<std::string, std::pair<int, int> >::iterator it=surfaces.begin(); it!=surfaces.end(); ++it) {
		SDL_Surface *opaque=SDL_CreateRGBSurface(SDL_SWSURFACE, (*it).second.first, (*it).second.second, 32, 0, 0, 0, 0);
		SDL_LockSurface(opaque);
		
		// set all pixels to black
		char *pixels=(char*) opaque->pixels;
		for (int i=0; i<(*it).second.first*(*it).second.second*4; i++)
			pixels[i]=(char) 0;
		opaque->pixels=(void*) pixels;
		
		SDL_UnlockSurface(opaque);
		
		// add this surface as a texture
		Textures::pushTexture((*it).first, opaque);
	}
	
	// modify surfaces
	SDL_SetColorKey(Textures::queryTexture("transparent"), SDL_SRCCOLORKEY, 0);
	
	// add other surfaces
	Textures::pushTexture("court_panorama_filled", Utils::createSurface(1296, 192));
	
	// register stock texture animations
	registerAnimations();
	
	// set our title screen to begin with
	toggle(STATE_INITIAL_SCREEN);
	m_State.fadeIn="both";
	
	return true;
}

// render the current scene
void Game::render() {
	// render top screen
	renderTopView();
	
	// render lower screen
	renderMenuView();
	
	// render special effects now
	if (!renderSpecialEffects())
		return;
	
	// that annoying black bar separating the top and bottom screens? yeah,
	// we need to redraw it as well
	Renderer::drawRect(SDL_GetVideoSurface(), Point(0, 192), 256, 5, Color(0, 0, 0));
	
	// once everything static is drawn, parse the block
	std::string status=m_Parser->parse(shouldDrawTextBox());
	
	// new block ready for parsing
	if (status!="null") {
		m_Parser->setBlock(m_Case->getBuffers()[status]);
		m_Parser->nextStep();
	}
	
	// pause here and wait for next step
	// while waiting, be sure to keep redrawing the already present screen
	if (m_Parser->paused()) {
		// set flags according to what's toggled right now
		updateFlags();
		
		// also, if we're in cross examination, update the speaker
		if (m_State.curExamination && !m_State.curExaminationPaused && m_Parser->getSpeaker()!="none")
			m_Parser->setSpeaker(m_Case->getTestimonies()[m_State.curTestimony].speaker);
	}
	
	// check input state at this point
	checkInputState();
	
}

// handle keyboard event
void Game::onKeyboardEvent(SDL_KeyboardEvent *e) {
	// if the evidence page is displayed, change selected evidence
	if (flagged(STATE_EVIDENCE_PAGE)) {
		// get the amount of evidence on this page
		int amount=m_State.visibleEvidence.size()-(m_State.evidencePage*8);
		if (amount>8)
			amount=8;
		
		// select the next evidence
		if (e->keysym.sym==SDLK_RIGHT)
			selectEvidence(true);
		
		// select the previous evidence
		else if (e->keysym.sym==SDLK_LEFT)
			selectEvidence(false);
		
		// ask for more information about selected evidence
		else if (e->keysym.sym==SDLK_RETURN)
			// show the evidence info page for this evidence
			toggle(STATE_EVIDENCE_INFO_PAGE | STATE_BACK_BTN | STATE_PROFILES_BTN);
	}
	
	// if controls are drawn, change the selection
	else if (flagged(STATE_CONTROLS)) {
		Case::Location *location=m_Case->getLocation(m_State.currentLocation);
		if (!location)
			return;
		
		// check the character set here, if any
		bool all=(location->character!="null");
		
		// select the right control
		if (e->keysym.sym==SDLK_RIGHT) {
			m_State.selectedControl+=1;
			
			// restrict selection based on controls drawn
			if (all) {
				if (m_State.selectedControl>3)
					m_State.selectedControl=0;
			}
			
			else {
				if (m_State.selectedControl>1)
					m_State.selectedControl=0;
			}
		}
		
		// select the left control
		else if (e->keysym.sym==SDLK_LEFT) {
			m_State.selectedControl-=1;
			
			// restrict selection based on controls drawn
			if (all) {
				if (m_State.selectedControl<0)
					m_State.selectedControl=3;
			}
			
			else {
				if (m_State.selectedControl<0)
					m_State.selectedControl=1;
			}
		}
		
		// select the upper control
		else if (e->keysym.sym==SDLK_UP && all) {
			if (m_State.selectedControl==2)
				m_State.selectedControl=0;
			
			else if (m_State.selectedControl==3)
				m_State.selectedControl=1;
		}
		
		// select the lower control
		else if (e->keysym.sym==SDLK_DOWN && all) {
			if (m_State.selectedControl==0)
				m_State.selectedControl=2;
			
			else if (m_State.selectedControl==1)
				m_State.selectedControl=3;
		}
		
		// activate a button
		else if (e->keysym.sym==SDLK_RETURN) {
			// based on selected control, ask to activate it via keyboard
			switch(m_State.selectedControl) {
				case 0: onExamineButtonActivated(); break;
				case 1: onMoveButtonActivated(); break;
				case 2: onTalkButtonActivated(); break;
				case 3: onPresentButtonActivated(); break;
			}
		}
	}
	
	// if the menu view is drawn, change selection
	else if (flagged(STATE_MOVE)) {
		// get the current location
		Case::Location *location=m_Case->getLocation(m_State.currentLocation);
		if (!location)
			return;
		
		// move up
		if (e->keysym.sym==SDLK_UP && m_State.selectedLocation>0)
			m_State.selectedLocation-=1;
		
		// move down
		else if (e->keysym.sym==SDLK_DOWN && m_State.selectedLocation<location->moveLocations.size()-1)
			m_State.selectedLocation+=1;
	}
	
	// if the talk menu view is drawn, change selection
	else if (flagged(STATE_TALK)) {
		// get the current location
		Case::Location *location=m_Case->getLocation(m_State.currentLocation);
		if (!location)
			return;
		
		// and get the character
		Character *character=m_Case->getCharacter(location->character);
		if (!character)
			return;
		
		// get a count of how many talk options he has
		int amount=character->getTalkOptions().size();
		
		// move up
		if (e->keysym.sym==SDLK_UP && m_State.selectedTalkOption>0)
			m_State.selectedTalkOption-=1;
		
		// move down
		else if (e->keysym.sym==SDLK_DOWN && m_State.selectedTalkOption<amount-1)
			m_State.selectedTalkOption+=1;
		
		// select option
		else if (e->keysym.sym==SDLK_RETURN) {// get the target block id
			std::string target=character->getTalkOptions()[m_State.selectedTalkOption].second;
			
			// set this block
			m_Parser->setBlock(m_Case->getBuffers()[target]);
			m_Parser->nextStep();
		}
	}
}

// handle mouse click event
void Game::onMouseEvent(SDL_MouseButtonEvent *e) {
	// button pressed down
	if (e->type==SDL_MOUSEBUTTONDOWN && m_State.fadeOut=="none" && !m_UI->isGUIBusy()) {
		Point mouse=Utils::getMouseLocation();
		
		// initial screen clicks
		if (flagged(STATE_INITIAL_SCREEN))
			m_UI->handleGUIClick(mouse, 1, "an_new_game_btn");
		
		// check for clicks on locations in move scene
		if (flagged(STATE_MOVE))
			m_UI->handleGUIClick(mouse, 4, "an_move_loc1_btn", "an_move_loc2_btn", "an_move_loc3_btn", "an_move_loc4_btn");
		
		// check for clicks on talk scene
		else if (flagged(STATE_TALK))
			m_UI->handleGUIClick(mouse, 4, "an_talk_op1_btn", "an_talk_op2_btn", "an_talk_op3_btn", "an_talk_op4_btn");
		
		// check for clicks on examine scene
		if (flagged(STATE_EXAMINE)) {
			// this will always be the court record button in this case
			if ((e->x>=176 && e->x<=176+79) && (e->y>=197 && e->y<=197+21))
				onTopRightButtonClicked();
			
			// likewise, this will always be the back button
			else if ((e->x>=0 && e->x<=79) && (e->y>=369 && e->y<=369+21))
				onBottomLeftButtonClicked();
			
			// examine button clicked
			else if ((e->x>=177 && e->x<=256) && (e->y>=369 && e->y<=369+21))
				onExamineThing(m_State.examinePt.x(), m_State.examinePt.y()+197);
		}
		
		// check for clicks on check image scene
		else if (flagged(STATE_CHECK_EVIDENCE_IMAGE)) {
			// back button
			if ((e->x>=0 && e->x<=79) && (e->y>=369 && e->y<=369+21))
				onBottomLeftButtonClicked();
			
			// confirm selection button
			else if ((e->x>=177 && e->x<=256) && (e->y>=359 && e->y<=389) && m_State.contradictionImg!="null")
				onBottomRightButtonClicked();
		}
		
		// buttons have different positions in the examine scene
		else if (!flagged(STATE_EXAMINE)) {
			// see if the click hit the top right button, if any
			if ((e->x>=176 && e->x<=176+80) && (e->y>=197 && e->y<=197+33))
				onTopRightButtonClicked();
			
			// see if the click hit the top left button
			else if ((e->x>=0 && e->x<=80) && (e->y>=197 && e->y<197+33))
				onTopLeftButtonClicked();
			
			// see if the bottom left button was clicked
			else if ((e->x>=0 && e->x<=79) && (e->y>=359 && e->y<=389))
				onBottomLeftButtonClicked();
		}
		
		// see if the center button was clicked
		if (flagged(STATE_NEXT_BTN) && ((e->x>=16 && e->x<=16+223) && (e->y>=242 && e->y<=242+111)) &&
		    !m_State.requestingAnswer && !m_State.requestingEvidence) {
			// if the parser is blocking the dialogue, don't skip
			if (!m_Parser->dialogueDone() && (m_Parser->isBlocking() || m_State.curTestimony!="null"))
				return;
			
			// play a sound effect if done
			else if (m_Parser->dialogueDone()) {
				// play a sound effect
				Audio::playEffect("sfx_next_part", Audio::CHANNEL_GUI);
			}
			
			// if we are in the process of a testimony, set the next piece
			// as the following block
			if ((m_State.curTestimony!="null" && !m_State.curExamination) || 
			   (m_State.curExamination && !m_State.curExaminationPaused)) {
				Case::Testimony *testimony=m_Case->getTestimony(m_State.curTestimony);
				
				// check if there's another piece
				if (m_State.curTestimonyPiece>=testimony->pieces.size()-1) {
					// schedule a fade out
					m_State.fadeOut="top";
					
					// save references to queued events
					m_State.queuedBlock=testimony->nextBlock;
					m_State.queuedLocation=testimony->followLocation;
					
					// reset everything related to a testimony
					m_State.curTestimonyPiece=0;
					m_State.blink="none";
					m_State.curTestimony="null";
				}
				
				// otherwise, move along
				else {
					// set the speaker
					m_Parser->setSpeaker(testimony->speaker);
					
					// we need to make sure to show the first block and not automatically
					// increment the counter
					// FIXME: this is a nasty little hack job, maybe find a more elegant solution later
					static bool once=true;
					if (once) {
						m_Parser->setBlock(testimony->pieces[m_State.curTestimonyPiece].text);
						once=false;
					}
					
					// set the next block and increment counter
					else {
						m_State.curTestimonyPiece++;	
						m_Parser->setBlock(testimony->pieces[m_State.curTestimonyPiece].text);
					}
				}
			}
			
			// proceed to the next block
			m_Parser->nextStep();
			
			return;
		}
		
		// check if one of the cross examination buttons was clicked
		else if (flagged(STATE_CROSS_EXAMINE_BTNS)) {
			// if the parser is blocking the dialogue, don't skip
			if (!m_Parser->dialogueDone() && (m_Parser->isBlocking() || m_State.curTestimony!="null"))
				return;
			
			Case::Testimony *testimony=m_Case->getTestimony(m_State.curTestimony);
			
			// left button clicked
			if ((e->x>=16 && e->x<=122) && (e->y>=261 && e->y<=341)) {
				m_State.curTestimonyPiece--;
				if (m_State.curTestimonyPiece<0)
					m_State.curTestimonyPiece=0;
				
				m_Parser->setBlock(testimony->pieces[m_State.curTestimonyPiece].text);
				
				m_Parser->nextStep();
			}
			
			// right button clicked
			else if ((e->x>=134 && e->x<=240) && (e->y>=261 && e->y<=341)) {
				// we've reached the end of the testimony
				if (m_State.curTestimonyPiece>=testimony->pieces.size()-1) {
					m_State.curExaminationPaused=true;
					m_State.curTestimonyPiece=0;
					
					// now set the ending block
					m_Parser->setBlock(m_Case->getBuffers()[testimony->xExamineEndBlock]);
					
					m_Parser->nextStep();
				}
				
				// move to the next block of testimony
				else {
					// set the next block and increment counter
					m_State.curTestimonyPiece++;
					m_Parser->setBlock(testimony->pieces[m_State.curTestimonyPiece].text);
					
					m_Parser->nextStep();
				}
			}
		}
		
		// check if the centered, present button was clicked
		if (flagged(STATE_PRESENT_TOP_BTN) && ((e->x>=89 && e->x<=167) && (e->y>=197 && e->y<=229)))
			onPresentCenterClicked();
		
		// if the controls are drawn, see if one was clicked
		else if (flagged(STATE_CONTROLS))
			onControlsClicked(e->x, e->y);
		
		// if the court record page is up, see if anything was clicked
		if (flagged(STATE_EVIDENCE_PAGE) || flagged(STATE_PROFILES_PAGE))
			onRecPageClickEvent(e->x, e->y);
		
		// same applies for evidence/profiles info page
		else if (flagged(STATE_EVIDENCE_INFO_PAGE) || flagged(STATE_PROFILE_INFO_PAGE))
			onRecInfoPageClickEvent(e->x, e->y);
	}
}

// register default animations for ui elements
void Game::registerAnimations() {
	// register some default animations
	m_UI->registerSideBounceAnimation("an_button_arrow_next", "tc_button_arrow_right", true, Point(110, 284), -2, 2, 25);
	m_UI->registerSideBounceAnimation("an_info_page_button_left", "tc_button_arrow_small_left", true, Point(3, 267), -2, 2, 25);
	m_UI->registerSideBounceAnimation("an_info_page_button_right", "tc_button_arrow_small_right", true, Point(247, 267), -2, 2, 25);
	m_UI->registerSyncBounce("an_x_examine_arrows", "tc_button_arrow_left", "tc_button_arrow_right",
				 Point(51, 285), Point(169, 285), -2, 2, 25);
	
	// register fade effects
	m_UI->registerFadeOut("an_next_location_fade_top", 1, UI::ANIM_FADE_OUT_TOP);
	m_UI->registerFadeOut("an_next_location_fade_bottom", 1, UI::ANIM_FADE_OUT_BOTTOM);
	m_UI->registerFadeOut("an_next_location_fade_both", 1, UI::ANIM_FADE_OUT_BOTH);
	m_UI->registerFadeOut("an_fade_top_half", 1, UI::ANIM_FADE_OUT_TOP_HALF);
	m_UI->registerFadeOut("an_fade_bottom_half", 1, UI::ANIM_FADE_OUT_BOTTOM_HALF);
	m_UI->registerFadeOut("an_fade_both_half", 1, UI::ANIM_FADE_OUT_BOTH_HALF);
	m_UI->registerFadeOut("an_gui_fade_bottom", 5, UI::ANIM_FADE_OUT_BOTTOM_GUI);
	
	// register court camera effect
	m_UI->registerCourtCameraMovement("an_court_camera");
	
	// register exclamations
	m_UI->registerExclamation("an_hold_it", "tc_hold_it", Point(0, 0));
	
	// register green bars
	m_UI->registerGreenBarControl("an_court_green_bar", "tc_court_green_bar", Point(172, 10));
	
	// register blink effects
	m_UI->registerBlink("an_testimony_blink", "testimony_logo", Point(2, 2), 1500);
	
	// register flash effects
	m_UI->registerFlash("an_flash", 5);
	
	// register gui animations
	m_UI->registerGUIButton("an_new_game_btn", 150, UI::Button("New Game", Point(53, 240), &Game::onInitialScreenClicked, "sfx_gavel"));
	m_UI->registerGUIButton("an_talk_op1_btn", 200, UI::Button("", Point(28, 236), &Game::onTalkSceneClicked));
	m_UI->registerGUIButton("an_talk_op2_btn", 200, UI::Button("", Point(28, 267), &Game::onTalkSceneClicked));
	m_UI->registerGUIButton("an_talk_op3_btn", 200, UI::Button("", Point(28, 298), &Game::onTalkSceneClicked));
	m_UI->registerGUIButton("an_talk_op4_btn", 200, UI::Button("", Point(28, 329), &Game::onTalkSceneClicked));
	
	m_UI->registerGUIButton("an_move_loc1_btn", 150, UI::Button("", Point(85, 236), &Game::onMoveSceneClicked));
	m_UI->registerGUIButton("an_move_loc2_btn", 150, UI::Button("", Point(85, 267), &Game::onMoveSceneClicked));
	m_UI->registerGUIButton("an_move_loc3_btn", 150, UI::Button("", Point(85, 298), &Game::onMoveSceneClicked));
	m_UI->registerGUIButton("an_move_loc4_btn", 150, UI::Button("", Point(85, 329), &Game::onMoveSceneClicked));
	
	// register sprite sequences
	m_UI->registerTestimonySequence("an_testimony_sequence");
	m_UI->registerCrossExamineSequence("an_cross_examine_sequence");
	
	
	// flip velocities on certain animations to reverse them
	m_UI->reverseVelocity("an_info_page_button_left");
}

// check input device state
void Game::checkInputState() {
	// if the examination scene is shown, move the crosshairs
	if (flagged(STATE_EXAMINE) || (flagged(STATE_CHECK_EVIDENCE_IMAGE) && m_State.contradictionImg!="null")) {
		// get the current keyboard state
		Uint8 *keys=SDL_GetKeyState(NULL);
		
		int ex=m_State.examinePt.x();
		int ey=m_State.examinePt.y();
		
		// move cursor up
		if (keys[SDLK_UP] && ey!=0)
			m_State.examinePt.setY(ey-1);
		
		// move cursor down
		if (keys[SDLK_DOWN] && ey<256)
			m_State.examinePt.setY(ey+1);
		
		// move cursor left
		if (keys[SDLK_LEFT] && ex!=0)
			m_State.examinePt.setX(ex-1);
		
		// move cursor right
		if (keys[SDLK_RIGHT] && ex<256)
			m_State.examinePt.setX(ex+1);
		
		// get the mouse state
		Uint8 state;
		Point p=Utils::getMouseLocation(&state);
		
		// move the cursors if the first button is pressed
		if ((state & SDL_BUTTON(1)) && (p.y()>=197+22 && p.y()<=391-22)) {
			// update position
			m_State.examinePt.setX(p.x());
			m_State.examinePt.setY(192-(391-p.y()));
		}
	}
}

// get the id of the selected court record evidence
std::string Game::getSelectedEvidence() {
	int index=m_State.evidencePage*8+m_State.selectedEvidence;
	Case::Evidence *e=m_Case->getEvidence(m_State.visibleEvidence[index]);
	return e->id;
}

// get the id of the selected court record profile
std::string Game::getSelectedProfile() {
	int index=m_State.profilesPage*8+m_State.selectedProfile;
	Character *c=m_Case->getCharacter(m_State.visibleProfiles[index]);
	return c->getInternalName();
}

// toggle game state flags
void Game::toggle(int flags) {
	m_State.drawFlags=0;
	m_State.drawFlags |= flags;
}

// see if an element is flagged to be drawn
bool Game::flagged(int flag) {
	return (m_State.drawFlags & flag);
}

// see if the text box should be drawn
bool Game::shouldDrawTextBox() {
	// first of all, fade out effects (other than gui ones) need to be text-box free
	if (m_State.fadeOut!="gui" && m_State.fadeOut!="none")
		return false;
	
	// same applies to the court camera
	if (m_State.courtCamera!="none")
		return false;
	
	// explicitly hidden text boxes
	if (m_State.hideTextBox && m_Parser->dialogueDone())
		return false;
	
	// cross examination effect
	if (m_State.crossExamineSequence!="none")
		return false;
	
	// exclamations
	if (m_State.exclamation!="none")
		return false;
	
	return true;
}

// checks to see if area under examine cursors can be examined
bool Game::canExamineRegion() {
	Case::Location *location=m_Case->getLocation(m_State.currentLocation);
	
	// iterate over hotspots
	for (std::vector<Case::Hotspot>::iterator it=location->hotspots.begin(); it!=location->hotspots.end(); ++it) {
		if (Utils::pointInRect(m_State.examinePt, (*it).rect))
			return true;
	}
	
	return false;
}

// set the current backdrop location
void Game::setLocation(const std::string &locationId) {
	if (!m_Case->getLocation(locationId)) {
		Utils::debugMessage("Unable to set nonexistent location: "+locationId);
		return;
	}
	
	// get the location
	Case::Location *location=m_Case->getLocation(locationId);
	
	// don't play new music if requested
	if (!m_State.continueMusic || !Audio::isMusicPlaying()) {
		// fade out the current music, if any
		Audio::haltMusic();
		
		// if this location has set music, then play it
		if (location->music!="null")
			Audio::playMusic(location->music);
	}
	
	// if this location has a trigger block, execute it now
	if (location->triggerBlock!="null") {
		m_Parser->setBlock(m_Case->getBuffers()[location->triggerBlock]);
		m_Parser->nextStep();
		
		// clear the trigger
		location->triggerBlock="null";
	}
	
	// set the new location
	m_State.currentLocation=locationId;
}

// set the evidence to draw on top screen
void Game::setShownEvidence(const std::string &id, const Position &pos) {
	if (id=="null") {
		// play hide effect
		Audio::playEffect("sfx_hide_item", Audio::CHANNEL_GUI);
		
		m_State.shownEvidence="null";
	}
	
	else {
		// play show effect
		Audio::playEffect("sfx_show_item", Audio::CHANNEL_GUI);
		
		m_State.shownEvidence=id;
		m_State.shownEvidencePos=pos;
	}
}

// display a testimony
void Game::displayTestimony(const std::string &id, bool crossExamine) {
	// get the testimony, if it exists
	Case::Testimony *testimony=m_Case->getTestimony(id);
	if (!testimony) {
		Utils::debugMessage("Requested testimony '"+id+"' doesn't exist and can't be displayed.");
		return;
	}
	
	// now, check the speaker's character
	if (!m_Case->getCharacter(testimony->speaker)) {
		Utils::debugMessage("Speaker '"+testimony->speaker+"' for testimony '"+id+"' doesn't exist");
		return;
	}
	
	// set our current testimony flag
	m_State.curTestimony=id;
	m_State.curExamination=crossExamine;
	
	// set the speaker at this location
	m_Case->getLocation("witness_stand")->character=testimony->speaker;
	m_Parser->setSpeaker("none");
	
	// go to witness stand
	m_State.queuedLocation="witness_stand";
	
	// request the talk animation be locked
	m_Parser->lockTalk(true);
	
	// start testimony sequence
	if (!crossExamine)
		m_State.testimonySequence="top";
	else
		m_State.crossExamineSequence="top";
	
	// and schedule a fade out
	m_State.fadeOut="top";
}

// change the selected evidence
void Game::selectEvidence(bool evidence, bool increment) {
	if (evidence) {
		// calculate amount of evidence on this page
		int amount=m_State.visibleEvidence.size()-(m_State.evidencePage*8);
		
		if (increment) {
			m_State.selectedEvidence++;
			if (m_State.selectedEvidence>amount-1)
				m_State.selectedEvidence=0;
		}
		
		else {
			m_State.selectedEvidence--;
			if (m_State.selectedEvidence<0)
				m_State.selectedEvidence=amount-1;
		}
		
		// see if the check button should still be shown
		if (m_Case->getEvidence(getSelectedEvidence())->checkID!="null")
			m_State.drawFlags |= STATE_CHECK_BTN;
		else
			m_State.drawFlags &= ~STATE_CHECK_BTN;
	}
	
	else {
		// calculate amount of profiles on this page
		int amount=m_State.visibleProfiles.size()-(m_State.profilesPage*8);
		
		if (increment) {
			m_State.selectedProfile++;
			if (m_State.selectedProfile>amount-1)
				m_State.selectedProfile=0;
		}
		
		else {
			m_State.selectedProfile--;
			if (m_State.selectedProfile<0)
				m_State.selectedProfile=amount-1;
		}
	}
}

// see if a location is a court location
bool Game::isCourtLocation(const std::string &id) {
	// test the string
	if (id=="prosecutor_stand" || id=="defense_stand" || 
	    id=="defense_helper_stand" || id=="witness_stand" ||
	    id=="judge_stand" || id=="courtroom")
		return true;
	else
		return false;
}

// update current game state flags
void Game::updateFlags() {
	int flags=STATE_TEXT_BOX;
	
	// draw check image screen
	if (flagged(STATE_CHECK_EVIDENCE_IMAGE)) {
		if (m_State.contradictionImg=="null")
			flags |= STATE_BACK_BTN;
		else
			flags |= STATE_CONFIRM_BTN;
			
		flags |= STATE_CHECK_EVIDENCE_IMAGE;
	}
	
	// draw evidence page
	else if (flagged(STATE_EVIDENCE_PAGE)) {
		flags |= STATE_PROFILES_BTN;
		flags |= STATE_EVIDENCE_PAGE;
		flags |= STATE_BACK_BTN;
			
		if (m_State.curExamination && !m_State.curExaminationPaused)
			flags |= STATE_COURT_GREEN_BAR;
	}
	
	// draw evidence info page
	else if (flagged(STATE_EVIDENCE_INFO_PAGE)) {
		flags |= STATE_PROFILES_BTN;
		flags |= STATE_EVIDENCE_INFO_PAGE;
		flags |= STATE_BACK_BTN;
		
		if (m_State.curExamination) {
			flags |= STATE_PRESENT_TOP_BTN;
				
			if (!m_State.curExaminationPaused)
				flags |= STATE_COURT_GREEN_BAR;
		}
	}
	
	// draw profiles page
	else if (flagged(STATE_PROFILES_PAGE)) {
		flags |= STATE_EVIDENCE_BTN;
		flags |= STATE_PROFILES_PAGE;
		flags |= STATE_BACK_BTN;
			
		if (m_State.curExamination && !m_State.curExaminationPaused)
			flags |= STATE_COURT_GREEN_BAR;
	}
	
	// draw profile info page
	else if (flagged(STATE_PROFILE_INFO_PAGE)) {
		flags |= STATE_EVIDENCE_BTN;
		flags |= STATE_PROFILE_INFO_PAGE;
		flags |= STATE_BACK_BTN;
		
		if (m_State.curExamination) {
			flags |= STATE_PRESENT_TOP_BTN;
			
			if (!m_State.curExaminationPaused)
				flags |= STATE_COURT_GREEN_BAR;
		}
	}
	
	// otherwise draw the dialog next button
	else {
		// only draw the cross examination buttons if we're going through
		// the speaker's testimony, and if we didn't pause the examination
		if (m_State.curExamination && !m_State.curExaminationPaused) {
			if (m_Parser->getSpeaker()!="none") {
				flags |= STATE_CROSS_EXAMINE_BTNS;
				flags |= STATE_PRESENT_BTN;
				flags |= STATE_PRESS_BTN;
				flags |= STATE_COURT_GREEN_BAR;
			}
			
			else
				flags |= STATE_NEXT_BTN;
		}
		
		else {
			flags |= STATE_NEXT_BTN;
			flags |= STATE_COURT_REC_BTN;
		}
	}
	
	// remove certain flags in specific situations
	if (m_State.requestingEvidence) {
		// back button should not be shown when record pages are shown
		if ((flags & STATE_EVIDENCE_PAGE) || (flags & STATE_PROFILES_PAGE))
			flags &= ~STATE_BACK_BTN;
		
		// add the present top button
		if ((flags & STATE_EVIDENCE_INFO_PAGE) || (flags & STATE_PROFILE_INFO_PAGE))
			flags |= STATE_PRESENT_TOP_BTN;
	}
	
	// toggle certain flags if there is an answer requested
	if (m_State.requestingAnswer)
		flags |= STATE_TALK;
	
	toggle(flags);
}

// render the game view (top screen)
void Game::renderTopView() {
	// if we are toggled in an initial screen, draw only the title on the top view
	if (flagged(STATE_INITIAL_SCREEN)) {
		// see if there is a user defined override
		if (m_Case->getOverrides().titleScreen!="null") {
			Renderer::drawImage(Point(0, 0), m_Case->getImage(m_Case->getOverrides().titleScreen)->texture);
		}
		
		// otherwise, fall back on the stock title screen set
		else {
			Renderer::drawImage(Point(0, 0), "stock_title");
			
			// get the case overview
			Case::Overview overview=m_Case->getOverview();
			
			// also draw case name, and author
			Fonts::drawStringBlended(Point(128-(Fonts::getWidth(overview.name, Fonts::FONT_BUTTON_TEXT)/2), 130), 
					     overview.name, Fonts::FONT_BUTTON_TEXT, Color(255, 255, 255));
			
			Fonts::drawStringBlended(Point(128-(Fonts::getWidth(overview.author, Fonts::FONT_BUTTON_TEXT)/2), 	
				             135+Fonts::getHeight(Fonts::FONT_BUTTON_TEXT)),
				             overview.author, Fonts::FONT_BUTTON_TEXT, Color(255, 255, 255));
		}
		
		return;
	}
	
	// temporary image has priority over background
	if (m_State.tempImage!="null") {
		// see if this image exists
		Case::Image *temp=m_Case->getImage(m_State.tempImage);
		if (!temp)
			Utils::debugMessage("Unknown temporary image requested: "+m_State.tempImage);
		
		else
			Renderer::drawImage(Point(0, 0), temp->texture);
	}
	
	// draw the background for this location
	else if (m_State.currentLocation!="null" && m_Case->getLocation(m_State.currentLocation)) {
		// get the background surface
		Case::Location *location=m_Case->getLocation(m_State.currentLocation);
		
		// just a static background
		if (location->bg[0]!='&') {
			Case::Background *bg=m_Case->getBackground(location->bg);
			
			// and draw it
			if (bg)
				Renderer::drawImage(Point(0, 0), bg->texture);
			else
				Utils::debugMessage("Background for location '"+m_State.currentLocation+"' not found");
		}
		
		// sprite background otherwise
		else {
			Sprite *bg=m_Case->getCharacter(location->bg.substr(1, location->bg.size()))->getSprite();
			bg->animate(0, 0);
		}
		
		// if there is a character set here, draw him now
		if (m_Case->getCharacter(location->character)) {
			Character *character=m_Case->getCharacter(location->character);
			Sprite *sprite=character->getSprite();
			
			// get character's current animation
			std::string root=character->getRootAnimation();
			
			// if we are at either the defense stand, co-counsel stand, or prosecutor stand, automatically
			// use the trial_* animations for characters
			std::string prepend="";
			if (m_State.currentLocation=="defense_stand" || m_State.currentLocation=="defense_helper_stand" ||
			    m_State.currentLocation=="prosecutor_stand")
				prepend="trial_";
			
			// if we are in a zoom special effect, use the zoom_ root animation instead
			else if (m_State.currentLocation.find("zoom")!=-1)
				root="zoom";
			
			// set talk animation if the dialogue is still being drawn and if we're not in any fade outs
			if (!m_Parser->talkLocked() && !m_Parser->dialogueDone() && 
			    m_Parser->getSpeaker()==character->getInternalName() && m_State.fadeOut=="none")
				sprite->setAnimation(prepend+root+"_talk");
			else
				sprite->setAnimation(prepend+root+"_idle");
			
			// draw the sprite
			// if the sprite's general size is not 256x192, try to center it
			Frame *fr=sprite->getCurrentFrame();
			if (fr && (fr->image->w!=256 || fr->image->h!=192))
				sprite->animate(256-(fr->image->w/2), 192-(fr->image->h));
			
			else
				sprite->animate(0, 0);
		}
		
		// handle special locations
		if (m_State.currentLocation=="courtroom")
			renderCourtroomOverview();
		
		else if (m_State.currentLocation=="prosecutor_stand")
			renderStand(COURT_PROSECUTOR_STAND);
		
		else if (m_State.currentLocation=="defense_stand")
			renderStand(COURT_DEFENSE_STAND);
		
		else if (m_State.currentLocation=="witness_stand")
			renderStand(COURT_WITNESS_STAND);
		
		// if the examination scene is up, dim the upper screen
		if (flagged(STATE_EXAMINE) || (m_State.prevScreen==SCREEN_EXAMINE && !flagged(STATE_TEXT_BOX)) ||
		    m_State.requestingEvidence || m_State.requestingAnswer) {
			// get an opaque black surface
			SDL_Surface *opaque=Textures::queryTexture("opaque_black");
			SDL_SetAlpha(opaque, SDL_SRCALPHA, 128);
			
			// draw it over the location
			Renderer::drawImage(Point(0, 0), opaque);
		}
	}
	
	else {
		// get an opaque black surface
		SDL_Surface *opaque=Textures::queryTexture("opaque_black");
		SDL_SetAlpha(opaque, SDL_SRCALPHA, 255);
		
		// draw it as the background
		Renderer::drawImage(Point(0, 0), opaque);
	}
	
	// if we are to shake the screen, do so now, since the following elements can't
	// be shaken, otherwise it will be a detriment to the interface...
	// lol -- "detriment to the interface" :P
	if (m_State.shake>0) {
		Point p=Utils::calculateShakePoint(3);
		
		// first, define the top of the screen
		SDL_Rect drect={ p.x(), p.y() };
		SDL_Rect srect={ 0, 0, 256, 192 };
		
		// create a new surface
		SDL_Surface *top=SDL_CreateRGBSurface(SDL_SWSURFACE, 256, 192, 32, 255U << 16, 255 << 8, 255 << 0, 0);
		
		// and copy the top portion of the screen into temporary surface, offset
		SDL_BlitSurface(SDL_GetVideoSurface(), &srect, top, &drect);
		
		// now just blit the modified top surface back onto the screen
		SDL_Rect rsrect={ 0, 0 };
		SDL_BlitSurface(top, NULL, SDL_GetVideoSurface(), &rsrect);
		
		// we no longer need this surface
		SDL_FreeSurface(top);
		
		// decrement counter
		m_State.shake--;
		if (m_State.shake<=0)
			m_State.shake=0;
	}
	
	// draw text box, if needed
	if (flagged(STATE_TEXT_BOX) && shouldDrawTextBox()) {
		// draw the text box over everything
		renderTextBox();
	}
	
	// draw the green court record bar, if needed
	if (flagged(STATE_COURT_GREEN_BAR) && m_State.exclamation=="none") {
		// first, draw the border
		Renderer::drawImage(Point(170, 8), "tc_court_bar_border");
		
		// now animate the green bar
		m_UI->animateGreenBar("an_court_green_bar");
	}
	
	// if we are required to present evidence, draw elements now
	if (m_State.requestingEvidence || m_State.requestingAnswer)
		Renderer::drawImage(Point(0, 168), "tc_answer_bar");
	
	// if there is shown evidence, draw it as well
	if (m_State.shownEvidence!="null") {
		Case::Evidence *ev=m_Case->getEvidence(m_State.shownEvidence);
		if (!ev)
			return;
		
		// draw the evidence depending on position
		if (m_State.shownEvidencePos==POSITION_LEFT)
			Renderer::drawImage(Point(20, 20), ev->texture);
		else
			Renderer::drawImage(Point(166, 20), ev->texture);
	}
}

// render the menu view (lower screen)
void Game::renderMenuView() {
	// for initial screen, draw the New Game and Continue buttons
	if (flagged(STATE_INITIAL_SCREEN)) {
		Renderer::drawInitialScreen(m_UI);
		return;
	}
	
	// if the cross examination sequence is being animated, don't draw anything;
	// graphics are handled by the UIManager function
	if (m_State.crossExamineSequence!="none")
		return;
	
	// when dealing with drawing elements, some need to be drawn "thin"
	// this is only true when the examine scene is being drawn
	std::string append="";
	if (flagged(STATE_EXAMINE) || flagged(STATE_CHECK_EVIDENCE_IMAGE))
		append="_thin";
	
	// if requesting an image contradiction, or checking evidence, handle that now
	if (flagged(STATE_CHECK_EVIDENCE_IMAGE)) {
		// we're not expecting the user to point out a contradiction
		if (m_State.contradictionImg=="null") {
			// get the image for the evidence
			std::string checkID=m_Case->getEvidence(m_State.visibleEvidence[m_State.selectedEvidence])->checkID;
			Renderer::drawImage(Point(0, 197), m_Case->getImage(checkID)->texture);
		}
		
		// otherwise, we are, so use the stored id
		else {
			// get our image
			Case::Image *img=m_Case->getImage(m_State.contradictionImg);
			if (img) {
				// we can reuse the renderer for this purpose
				Renderer::drawExamineScene(img->texture, m_State.examinePt, false);
			}
		}
	}
	
	// default to background
	else
		Renderer::drawImage(Point(0, 197), "court_overview_g");
	
	// draw the evidence page
	if (flagged(STATE_EVIDENCE_PAGE))
		Renderer::drawEvidencePage(m_Case->getEvidenceFromIds(m_State.visibleEvidence), 
					   m_State.evidencePage, m_State.selectedEvidence);
	
	// draw the profiles page
	else if (flagged(STATE_PROFILES_PAGE))
		Renderer::drawProfilesPage(m_Case->getCharactersFromIds(m_State.visibleProfiles), 
					   m_State.profilesPage, m_State.selectedProfile);
	
	// draw the evidence info page
	else if (flagged(STATE_EVIDENCE_INFO_PAGE))
		Renderer::drawEvidenceInfoPage(m_Case->getEvidence(m_State.visibleEvidence[m_State.selectedEvidence]));
	
	// draw the profile info page
	else if (flagged(STATE_PROFILE_INFO_PAGE))
		Renderer::drawProfileInfoPage(m_Case->getCharacter(m_State.visibleProfiles[m_State.selectedProfile]));
	
	// draw the examination scene
	else if (flagged(STATE_EXAMINE)) {
		// get the current location and its mapped background
		Case::Location *location=m_Case->getLocation(m_State.currentLocation);
		Case::Background *bg=m_Case->getBackground(location->bg);
		
		Renderer::drawExamineScene(bg->texture, m_State.examinePt);
	}
	
	// draw the move scene
	else if (flagged(STATE_MOVE)) {
		// get current location
		Case::Location *location=m_Case->getLocation(m_State.currentLocation);
		
		Renderer::drawMoveScene(location->moveLocations, m_Case->getLocations(), m_State.selectedLocation);
	}
	
	// draw talk scene
	else if (flagged(STATE_TALK)) {
		// check if we are requiring the player to select an answer
		if (m_State.requestingAnswer)
			Renderer::drawTalkScene(m_State.talkOptions, 0, true);
		
		else {
			// get current character at location
			Case::Location *location=m_Case->getLocation(m_State.currentLocation);
			Character *character=m_Case->getCharacter(location->character);
			
			// draw the talk scene
			if (character)
				Renderer::drawTalkScene(character->getTalkOptions(), m_State.selectedTalkOption);
		}
	}
	
	// draw next button in case of dialog
	else if (flagged(STATE_NEXT_BTN)) {
		Renderer::drawImage(Point(16, 242), "tc_next_btn");
		
		// only animate the arrow if the dialogue is done, or if the dialogue
		// can be skipped
		bool draw=true;
		if (m_Parser->isBlocking()) {
			if (!m_Parser->dialogueDone())
				draw=false;
		}
		
		// don't draw during effects
		if (m_State.testimonySequence!="none" || m_State.fadeOut!="none")
			draw=false;
		
		if (draw)
			m_UI->drawAnimation("an_button_arrow_next");
	}
	
	// draw controls, if needed
	else if (flagged(STATE_CONTROLS)) {
		// get the character at this location, if any
		if (m_Case->getLocation(m_State.currentLocation)) {
			// if there is a character set, enable the talk and present controls
			Case::Location *location=m_Case->getLocation(m_State.currentLocation);
			if (location->character!="null")
				renderControls(CONTROLS_ALL);
			else
				renderControls(CONTROLS_EXAMINE | CONTROLS_MOVE);
		}
	}
	
	// draw testimony movement buttons
	else if (flagged(STATE_CROSS_EXAMINE_BTNS)) {
		// first, draw the two testimony movement buttons
		Renderer::drawImage(Point(16, 261), "tc_x_examine_btn");
		Renderer::drawImage(Point(134, 261), "tc_x_examine_btn");
		
		// don't draw the left arrow if there is not testimony piece
		// that precedes this one
		if (m_State.curTestimonyPiece==0)
			m_UI->unsyncBounceTexture("an_x_examine_arrows", true);
		else
			m_UI->resyncBounceTexture("an_x_examine_arrows", true);
		
		// draw the arrows on the buttons
		m_UI->animateSyncBounce("an_x_examine_arrows");
	}
	
	// top everything off with scanlines, except when presenting an image contradiction
	if (!flagged(STATE_CHECK_EVIDENCE_IMAGE) || (flagged(STATE_CHECK_EVIDENCE_IMAGE) && m_State.contradictionImg=="null"))
		Renderer::drawImage(Point(0, 197), "scanlines_overlay");
	
	// draw the top and lower border bar
	Renderer::drawImage(Point(0, 197), "tc_top_bar"+append);
	Renderer::drawImage(Point(0, 389-(Textures::queryTexture("tc_lower_bar"+append)->h)), "tc_lower_bar"+append);
	
	// any elements other than the top/bottom bars are only drawn if there is no gui fade out in effect
	if (m_State.fadeOut!="gui") {
		// draw titles bars, if needed
		if (flagged(STATE_EVIDENCE_PAGE) || flagged(STATE_EVIDENCE_INFO_PAGE))
			Renderer::drawImage(Point(0, 206), "tc_evidence_bar");
		if (flagged(STATE_PROFILES_PAGE) || flagged(STATE_PROFILE_INFO_PAGE))
			Renderer::drawImage(Point(0, 206), "tc_profiles_bar");
		
		// draw activated buttons
		if (flagged(STATE_COURT_REC_BTN))
			Renderer::drawImage(Point(176, 197), "tc_court_rec_btn"+append);
		else if (flagged(STATE_PRESENT_BTN))
			Renderer::drawImage(Point(176, 197), "tc_present_item_btn");
		else if (flagged(STATE_EVIDENCE_BTN))
			Renderer::drawImage(Point(177, 197), "tc_evidence_btn");
		else if (flagged(STATE_PROFILES_BTN))
			Renderer::drawImage(Point(177, 197), "tc_profiles_btn");
		
		if (flagged(STATE_PRESS_BTN))
			Renderer::drawImage(Point(0, 197), "tc_press_btn");
		
		// check evidence or confirm button, if needed
		if (flagged(STATE_CHECK_BTN))
			Renderer::drawImage(Point(177, 359), "tc_check_btn");
		else if (flagged(STATE_CONFIRM_BTN))
			Renderer::drawImage(Point(177, 359), "tc_confirm_btn");
	
		// draw the present evidence button, centered on the upper portion of the lower screen
		if (flagged(STATE_PRESENT_TOP_BTN))
			Renderer::drawImage(Point(89, 197), "tc_present_top_btn");
		
		// draw examine button if needed
		if (flagged(STATE_EXAMINE) && canExamineRegion())
			Renderer::drawImage(Point(256-79, 369), "tc_examine_btn_thin");
		
		// draw the back button
		if (flagged(STATE_BACK_BTN)) {
			// same applies here, draw the back button lower if it is the thin variety
			int y=359;
			if (append=="_thin")
				y+=10;
			
			// draw the button
			Renderer::drawImage(Point(0, y), "tc_back_btn"+append);
		}
	}
}

// render special effects
bool Game::renderSpecialEffects() {
	// if we are still fading out, don't parse the text
	if (m_State.fadeOut!="none") {
		// see if this fade out should end half way
		bool half=(m_State.fadeOut.rfind("_half")!=-1);
		
		// see if we are done fading
		int ret;
		
		// half fade out
		if (half)
			ret=m_UI->fadeOut("an_fade_"+m_State.fadeOut);
		
		// gui fade out
		else if (m_State.fadeOut!="gui")
			ret=m_UI->fadeOut("an_next_location_fade_"+m_State.fadeOut);
		
		// location fade out
		else
			ret=m_UI->fadeOut("an_gui_fade_bottom");
		
		// midpoint of fade out reached, or done if half fade
		if (ret==0 || (ret==1 && half)) {
			// if there are any queued flags, toggle them now
			if ((m_State.queuedFlags & STATE_QUEUED)) {
				toggle(m_State.queuedFlags);
				m_State.queuedFlags=0;
			}
			
			// set a location, if requested
			if (m_State.queuedLocation!="null") {
				setLocation(m_State.queuedLocation);
				m_State.queuedLocation="null";
			}
			
			// set the next block, if requested
			if (m_State.queuedBlock!="null") {
				// return to the testimony
				if (m_State.queuedBlock=="INTERNAL_testimony") {
					m_Parser->setBlock(m_Case->getTestimony(m_State.curTestimony)->pieces[m_State.curTestimonyPiece].text);
					m_Parser->nextStep();
				}
				
				else
					m_Parser->setBlock(m_Case->getBuffers()[m_State.queuedBlock]);
				
				m_Parser->nextStep();
				m_State.queuedBlock="null";
			}
			
			// end half animations here
			if (half) {
				// disable certain flags as this point
				m_State.drawFlags &= ~STATE_INITIAL_SCREEN;
				
				m_State.fadeOut="none";
			}
		}
		
		// effect was completed
		else if (ret==1)
			m_State.fadeOut="none";
		
		return false;
	}
	
	// draw the gavel animation
	else if (m_State.gavel!="none") {
		// animate the gavel
		Sprite *spr=m_Case->getCharacter("gavel")->getSprite();
		spr->animate(0, 0);
		
		// if we're done, reset the special effect
		if (spr->done())
			m_State.gavel="none";
		
		return false;
	}
	
	// draw court room camera movement
	else if (m_State.courtCamera!="none") {
		// convert the string to limits
		UI::Limit start, end;
		Utils::scriptToLimits(m_State.courtCamera, start, end);
		
		// get locations
		Case::Location *pStand=m_Case->getLocation("prosecutor_stand");
		Case::Location *dStand=m_Case->getLocation("defense_stand");
		Case::Location *wStand=m_Case->getLocation("witness_stand");
		
		// verify locations
		if (!pStand || !dStand || !wStand) {
			Utils::debugMessage("Unable to get needed locations for court camera movement");
			return true;
		}
		
		// get a new panorama
		SDL_Surface *panorama=Renderer::generateCourtPanorama(m_Case, pStand->character, dStand->character, wStand->character);
		
		// animate the camera
		if (m_UI->moveCourtCamera("an_court_camera", Textures::queryTexture("court_panorama_filled"), start, end)) {
			// flag that we're done
			m_State.courtCamera="none";
			
			// set our ending location
			switch(end) {
				case UI::LIMIT_DEFENSE_STAND: setLocation("defense_stand"); break;
				case UI::LIMIT_WITNESS_STAND: setLocation("witness_stand"); break;
				case UI::LIMIT_PROSECUTOR_STAND: setLocation("prosecutor_stand"); break;
			}
			
			// also, reset the animation for future use
			m_UI->registerCourtCameraMovement("an_court_camera");
			
			// execute any queued cross examination blocks
			if (m_State.curExamination && m_State.queuedBlock!="null") {
				m_State.curExaminationPaused=true;
				m_Parser->setBlock(m_Case->getBuffers()[m_State.queuedBlock]);
				m_Parser->nextStep();
				
				m_State.queuedBlock="null";
			}
			
			return true;
		}
		
		else
			return false;
	}
	
	// render flash now, if requested
	else if (m_State.flash!="none") {
		bool ret=m_UI->flash("an_flash");
		if (!ret)
			m_State.flash="none";
	}
	
	// render a blinking animation
	else if (m_State.blink!="none")
		m_UI->blink(m_State.blink);
	
	// render testimony sprite sequence, if requested
	else if (m_State.testimonySequence!="none") {
		// and halt any music playback
		Audio::haltMusic();
		
		bool ret=m_UI->animateTestimonySequence("an_testimony_sequence");
		if (ret) {
			// reset the animation
			m_UI->registerTestimonySequence("an_testimony_sequence");
			
			Case::Testimony *testimony=m_Case->getTestimony(m_State.curTestimony);
			
			// now start the testimony blink animation
			m_State.blink="an_testimony_blink";
			
			// set the first block of testimony: the title
			m_Parser->setBlock("<testimony-title>"+testimony->title+"</testimony-title>");
			
			m_State.testimonySequence="none";
			
			return true;
		}
		
		return false;
	}
	
	// render cross examination sprite sequence
	else if (m_State.crossExamineSequence!="none") {
		Audio::haltMusic();
		
		// make sure lawyer images exist
		Case::Image *leftImg=m_Case->getImage(m_State.crossExamineLawyers.first);
		Case::Image *rightImg=m_Case->getImage(m_State.crossExamineLawyers.second);
		if (!leftImg || !rightImg) {
			Utils::debugMessage("Needed sprites 'cross_examine_top' and 'cross_examine_bottom' not found");
			m_State.crossExamineSequence="none";
			return true;
		}
		
		// animate the sequence
		bool ret=m_UI->animateCrossExamineSequence("an_cross_examine_sequence", leftImg->texture, rightImg->texture);
		if (ret) {
			// reset the animation
			m_UI->registerCrossExamineSequence("an_cross_examine_sequence");
			
			// get the testimony in question
			Case::Testimony *testimony=m_Case->getTestimony(m_State.curTestimony);
			
			m_State.crossExamineSequence="none";
			
			// once again, we set the testimony title as our cross examination title
			m_Parser->setBlock("<testimony-title>"+testimony->title+"</testimony-title>");
			
			return true;
		}
		
		return false;
	}
	
	// render exclamations
	else if (m_State.exclamation!="none") {
		bool ret=m_UI->exclamation(m_State.exclamation, NULL);
		if (ret) {
			m_State.courtCamera="witness_stand,defense_stand";
			m_State.exclamation="none";
		}
	}
	
	return true;
}

// render the text box
void Game::renderTextBox() {
	static bool speakerChecked=false;
	static std::string speaker="";
	
	// if there is evidence to present, shift the text box up
	int shift=0;
	if (m_State.requestingEvidence || m_State.requestingAnswer)
		shift=-24;
	
	// draw the actual text box body
	Renderer::drawImage(Point(0, 128+shift), "tc_text_box");
	
	// character speaking
	if (speaker!="none" && speaker!="" && m_Case->getCharacter(speaker)) {
		// get the tag
		SDL_Surface *tag=m_Case->getCharacter(speaker)->getTextBoxTag();
		
		// and draw it
		Renderer::drawImage(Point(0, 118+shift), tag);
	}
	
	// narration or no one speaking
	else
		Renderer::drawImage(Point(0, 128+shift), Textures::queryTexture("tc_text_box_border"));
	
	// see if the two speakers vary
	if (m_Parser->getSpeaker()!=speaker)
		speakerChecked=false;
	
	// find our current speaker
	if (!speakerChecked) {
		// get the speaker of the current block
		std::string cspeaker=m_Parser->getSpeaker();
		
		// see if this character exists
		if (m_Case->getCharacter(cspeaker)) {
			// get this character's text box tag, if he has one
			Character *character=m_Case->getCharacter(cspeaker);
			if (character->hasTextBoxTag())
				speaker=cspeaker;
		}
		
		else
			speaker="none";
		
		// flag that we've check this speaker already
		speakerChecked=true;
	}
}

// render the controls (move, talk, etc)
void Game::renderControls(int flags) {
	// y coordinate of first row of controls
	int y=197+20+34;
	
	// coordinates for selection box
	int sx1=5, sx2=107;
	int sy1=y, sy2=y+14;
	
	if ((flags & CONTROLS_EXAMINE) || (flags & CONTROLS_ALL)) {
		Renderer::drawImage(Point(8, y), "tc_examine_btn");
		
		// set high light coordinates if this is selected
		if (m_State.selectedControl==0) {
			sx1=5;
			sx2=107;
			sy1=y;
			sy2=y+14;
		}
	}
	
	if ((flags & CONTROLS_MOVE) || (flags & CONTROLS_ALL)) {
		Renderer::drawImage(Point(134, y), "tc_move_btn");
		
		// set high light coordinates if this is selected
		if (m_State.selectedControl==1) {
			sx1=131;
			sx2=234;
			sy1=y;
			sy2=y+14;
		}
	}
	
	if ((flags & CONTROLS_TALK) || (flags & CONTROLS_ALL)) {
		Renderer::drawImage(Point(8, y+26+36), "tc_talk_btn");
		
		// set high light coordinates if this is selected
		if (m_State.selectedControl==2) {
			sx1=5;
			sx2=107;
			sy1=y+26+36;
			sy2=sy1+14;
		}
	}
	
	if ((flags & CONTROLS_PRESENT) || (flags & CONTROLS_ALL)) {
		Renderer::drawImage(Point(134, y+26+36), "tc_present_btn");
		
		// set high light coordinates if this is selected
		if (m_State.selectedControl==3) {
			sx1=131;
			sx2=234;
			sy1=y+26+36;
			sy2=sy1+14;
		}
	}
	
	// draw selection box
	Renderer::drawImage(Point(sx1, sy1), "tc_select_tl");
	Renderer::drawImage(Point(sx1, sy2), "tc_select_bl");
	Renderer::drawImage(Point(sx2, sy2), "tc_select_br");
	Renderer::drawImage(Point(sx2, sy1), "tc_select_tr");
}

// render the courtroom overview
void Game::renderCourtroomOverview() {
	// here's the situation:
	// the courtroom overview can contain any of three images, if not all at once
	// there's the defense and prosecutor images, in addition to the witness
	// we need to draw any specified images from the GameState at this point
	
	// first, we check to see if there are any defense stand images
	if (m_State.crOverviewDefense!="null") {
		// get the image
		Case::Image *image=m_Case->getImage(m_State.crOverviewDefense);
		if (image)
			Renderer::drawImage(Point(179, 90), image->texture);
		else
			Utils::debugMessage("Courtroom overview image '"+m_State.crOverviewDefense+"' not found");
	}
	
	// draw prosecutor image
	if (m_State.crOverviewProsecutor!="null") {
		// get the image
		Case::Image *image=m_Case->getImage(m_State.crOverviewProsecutor);
		if (image)
			Renderer::drawImage(Point(48, 91), image->texture);
		else
			Utils::debugMessage("Courtroom overview image '"+m_State.crOverviewDefense+"' not found");
	}
	
	// draw the judge
	Renderer::drawImage(Point(121, 68), "overview_judge");
	
	// draw jury
	m_Case->getCharacter("jury_left")->getSprite()->animate(0, 59);
	m_Case->getCharacter("jury_right")->getSprite()->animate(215, 59);
}

// render the attorney's stand
void Game::renderStand(const Stand stand) {
	// the background and sprite should be drawn by this point
	// we just need to superimpose the bench seen ingame over the sprite
	std::string sId="null";
	switch(stand) {
		case COURT_PROSECUTOR_STAND: sId="prosecutor_bench"; break;
		case COURT_DEFENSE_STAND: sId="defense_bench"; break;
		case COURT_WITNESS_STAND: sId="witness_bench"; break;
		
		default: break;
	}
	
	// draw the image
	Renderer::drawImage(Point(0, 0), sId);
}

// initial screen button activated handler
void Game::onInitialScreenClicked(const std::string &id) {
	if (!flagged(STATE_INITIAL_SCREEN))
		return;
	
	// new game button clicked
	if (id=="an_new_game_btn") {
		// begin parsing the game script
		m_State.fadeOut="both_half";
		m_State.queuedBlock=m_Case->getInitialBlockId();
	}
}

// top right button was clicked
void Game::onTopRightButtonClicked() {
	// if the court record button is shown, activate evidence page
	if (flagged(STATE_COURT_REC_BTN)) {
		int flags=STATE_BACK_BTN | STATE_PROFILES_BTN | STATE_EVIDENCE_PAGE;
		
		// if the text box is also present, draw it as well
		if (flagged(STATE_TEXT_BOX))
			flags |= STATE_TEXT_BOX;
		
		// toggle the flags
		toggle(flags);
	}
	
	// if the present button is shown, present shown evidence
	else if (flagged(STATE_PRESENT_BTN)) {
		int flags=0;
		
		// not in cross examination
		if (!m_State.curExamination) {
			flags=STATE_COURT_REC_BTN | STATE_CONTROLS;
			
			// get the current location
			Case::Location *location=m_Case->getLocation(m_State.currentLocation);
			if (!location)
				return;
			
			// and get the character here
			Character *character=m_Case->getCharacter(location->character);
			if (!character)
				return;
			
			// get the current evidence/profile
			std::string id;
			if (flagged(STATE_EVIDENCE_INFO_PAGE) && !m_State.visibleEvidence.empty())
				id=getSelectedEvidence();
			
			else if (!m_State.visibleProfiles.empty())
				id=getSelectedProfile();
			
			// see if this evidence/profile can be presented
			bool found=false;
			std::vector<StringPair> presentables=character->getPresentableItems();
			for (int i=0; i<presentables.size(); i++) {
				if (presentables[i].first==id) {
					// set the block to use
					m_Parser->setBlock(m_Case->getBuffers()[presentables[i].second]);
					m_Parser->nextStep();
					
					found=true;
				}
			}
			
			// if the presentable evidence was not found, then fall back on appropriate block
			if (!found && character->getBadPresentableBlock()!="null") {
				m_Parser->setBlock(m_Case->getBuffers()[character->getBadPresentableBlock()]);
				m_Parser->nextStep();
			}
			
			// reset screen
			m_State.prevScreen=SCREEN_MAIN;
			
			// if the text box is also present, draw it as well
			if (flagged(STATE_TEXT_BOX))
				flags |= STATE_TEXT_BOX;
		}
		
		// while in cross examination, only the court record can be viewed
		else
			flags |= STATE_BACK_BTN | STATE_PROFILES_BTN | STATE_COURT_GREEN_BAR |
				 STATE_EVIDENCE_PAGE | STATE_TEXT_BOX;
		
		toggle(flags);
	}
	
	// if the profiles button is shown, activate profiles page
	else if (flagged(STATE_PROFILES_BTN) && !flagged(STATE_EVIDENCE_INFO_PAGE)) {
		int flags=STATE_BACK_BTN | STATE_EVIDENCE_BTN | STATE_PROFILES_PAGE;
		
		// if the text box is also present, draw it as well
		if (flagged(STATE_TEXT_BOX))
			flags |= STATE_TEXT_BOX;
		
		// remove back button on certain ocassions
		if (m_State.requestingEvidence)
			flags &= ~STATE_BACK_BTN;
		
		// draw cross examination elements
		if (m_State.curExamination && !m_State.curExaminationPaused)
			flags |= STATE_COURT_GREEN_BAR;
		
		toggle(flags);
	}
	
	// if the evidence button is shown, activate evidence page
	else if (flagged(STATE_EVIDENCE_BTN) && !flagged(STATE_PROFILE_INFO_PAGE)) {
		int flags=STATE_BACK_BTN | STATE_PROFILES_BTN | STATE_EVIDENCE_PAGE;
		
		// if the text box is also present, draw it as well
		if (flagged(STATE_TEXT_BOX))
			flags |= STATE_TEXT_BOX;
		
		// remove back button on certain ocassions
		if (m_State.requestingEvidence)
			flags &= ~STATE_BACK_BTN;
		
		// draw cross examination elements
		if (m_State.curExamination && !m_State.curExaminationPaused)
			flags |= STATE_COURT_GREEN_BAR;
		
		toggle(flags);
	}
	
	// if the profiles button is shown during evidence info screen, switch to profile info screen
	else if (flagged(STATE_PROFILES_BTN) && flagged(STATE_EVIDENCE_INFO_PAGE)) {
		int flags=STATE_BACK_BTN | STATE_PROFILE_INFO_PAGE;
		
		// if the previous screen was the present screen, then draw present button instead
		if (m_State.prevScreen==SCREEN_PRESENT)
			flags |= STATE_PRESENT_BTN;
		else
			flags |= STATE_EVIDENCE_BTN;
		
		// for cross examinations, draw center present button
		if (m_State.curExamination) {
			flags |= STATE_PRESENT_TOP_BTN;
			
			// also see if we should draw the green bar
			if (!m_State.curExaminationPaused)
				flags |= STATE_COURT_GREEN_BAR;
		}
		
		// if the text box is also present, draw it as well
		if (flagged(STATE_TEXT_BOX))
			flags |= STATE_TEXT_BOX;
		
		toggle(flags);
	}
	
	// if the evidence button is shown during profile info screen, switch to evidence info screen
	else if (flagged(STATE_EVIDENCE_BTN) && flagged(STATE_PROFILE_INFO_PAGE)) {
		int flags=STATE_BACK_BTN | STATE_EVIDENCE_INFO_PAGE;
		
		// if the previous screen was the present screen, then draw present button instead
		if (m_State.prevScreen==SCREEN_PRESENT)
			flags |= STATE_PRESENT_BTN;
		else
			flags |= STATE_PROFILES_BTN;
		
		// for cross examinations, draw center present button
		if (m_State.curExamination) {
			flags |= STATE_PRESENT_TOP_BTN;
			
			// likewise
			if (!m_State.curExaminationPaused)
				flags |= STATE_COURT_GREEN_BAR;
		}
		
		// include a check button for evidence with images
		if (m_Case->getEvidence(getSelectedEvidence()) && 
		    m_Case->getEvidence(getSelectedEvidence())->checkID!="null")
			flags |= STATE_CHECK_BTN;
		
		// if the text box is also present, draw it as well
		if (flagged(STATE_TEXT_BOX))
			flags |= STATE_TEXT_BOX;
		
		toggle(flags);
	}
	
	// play a sound effect
	Audio::playEffect("sfx_click", Audio::CHANNEL_GUI);
}

// top left button was clicked
void Game::onTopLeftButtonClicked() {
	// the only button shown on the top left is the press button
	if (flagged(STATE_PRESS_BTN)) {
		// get the current testimony
		Case::Testimony *testimony=m_Case->getTestimony(m_State.curTestimony);
		
		// make sure we are in active cross examination
		if (testimony && m_State.curExamination && !m_State.curExaminationPaused) {
			// get the press block for this piece of the testimony
			std::string pBlock=testimony->pieces[m_State.curTestimonyPiece].pressBlock;
			
			// set that block to use after the "hold it!" animation
			m_State.queuedBlock=pBlock;
			
			// and schedule said animation
			m_State.exclamation="an_hold_it";
			m_State.curExaminationPaused=true;
		}
	}
}

// bottom left button was clicked
void Game::onBottomLeftButtonClicked() {
	// if the back button is shown, return to previous screen
	if (flagged(STATE_BACK_BTN)) {
		int flags=0;
		
		// if either evidence or profiles pages, or examination scene are shown, revert to main screen
		if (flagged(STATE_EVIDENCE_PAGE) || flagged(STATE_PROFILES_PAGE)) {
			// if the previous screen is the examine screen, then draw it instead
			if (m_State.prevScreen==SCREEN_EXAMINE)
				flags=STATE_EXAMINE | STATE_COURT_REC_BTN | STATE_BACK_BTN;
			
			// if the previous screen is the move screen, then draw it
			else if (m_State.prevScreen==SCREEN_MOVE)
				flags=STATE_MOVE | STATE_COURT_REC_BTN | STATE_BACK_BTN;
			
			// if the previous screen is the talk screen, then draw it
			else if (m_State.prevScreen==SCREEN_TALK)
				flags=STATE_TALK | STATE_COURT_REC_BTN | STATE_BACK_BTN;
			
			// must be the main screen
			else {
				flags=STATE_COURT_REC_BTN | STATE_CONTROLS;
				
				// if the text box is also present, draw it as well
				if (flagged(STATE_TEXT_BOX))
					flags |= STATE_TEXT_BOX;
				
				// flag that were are now at the main screen
				m_State.prevScreen=SCREEN_MAIN;
			}
		}
		
		// if examine, talk, or move screen is show, revert back to main screen
		else if (flagged(STATE_EXAMINE) || flagged(STATE_MOVE) || flagged(STATE_TALK)) {
			m_State.prevScreen=SCREEN_MAIN;
			
			m_State.fadeOut="gui";
			m_State.queuedFlags=STATE_COURT_REC_BTN | STATE_CONTROLS | STATE_QUEUED;
			
		}
		
		// if check evidence image screen is shown, revert to evidence info page
		else if (flagged(STATE_CHECK_EVIDENCE_IMAGE)) {
			flags=STATE_EVIDENCE_INFO_PAGE | STATE_BACK_BTN | STATE_PROFILES_BTN |
			      STATE_CHECK_BTN;
			
			// if the text box is also present, draw it as well
			if (flagged(STATE_TEXT_BOX))
				flags |= STATE_TEXT_BOX;
			
			m_State.fadeOut="gui";
			m_State.queuedFlags=0;
			m_State.queuedFlags |= flags | STATE_QUEUED;
		}
		
		// if evidence info page is shown, revert back to evidence page
		else if (flagged(STATE_EVIDENCE_INFO_PAGE)) {
			flags=STATE_PROFILES_BTN | STATE_BACK_BTN | STATE_EVIDENCE_PAGE;
			
			// if the text box is also present, draw it as well
			if (flagged(STATE_TEXT_BOX))
				flags |= STATE_TEXT_BOX;
		}
		
		// if profile info page is shown, revert back to profiles page
		else if (flagged(STATE_PROFILE_INFO_PAGE)) {
			flags=STATE_EVIDENCE_BTN | STATE_BACK_BTN | STATE_PROFILES_PAGE;
			
			// if the text box is also present, draw it as well
			if (flagged(STATE_TEXT_BOX))
				flags |= STATE_TEXT_BOX;
		}
		
		// also, if we are in cross examination, toggle the green bar again
		if (m_State.curExamination && !m_State.curExaminationPaused)
			flags |= STATE_COURT_GREEN_BAR;
		
		// toggle the set flags now
		if (m_State.fadeOut=="none")
			toggle(flags);
		
		// play a sound effect
		Audio::playEffect("sfx_return", Audio::CHANNEL_GUI);
	}
}

// bottom right button was clicked
void Game::onBottomRightButtonClicked() {
	// confirm button was clicked
	if (flagged(STATE_CONFIRM_BTN)) {
		// during examining an image for contradiction
		if (flagged(STATE_CHECK_EVIDENCE_IMAGE)) {
			StringVector vec=Utils::explodeString(',', m_State.requestedContrParams);
			
			// see if the click was in the contradiction region
			if (Utils::pointInRect(m_State.examinePt, m_State.contradictionRegion)) {
				// the user got it right, so follow up on the correct block
				m_Parser->setBlock(m_Case->getBuffers()[vec[0]]);
			}
			
			// the user is wrong, nice try though
			else
				m_Parser->setBlock(m_Case->getBuffers()[vec[1]]);
			
			// reset our variables relating to this image contradiction
			m_State.contradictionImg="null";
			m_State.contradictionRegion=Rect(Point(0, 0), 0, 0);
			m_State.requestedContrParams="null";
			
			m_State.drawFlags &= ~STATE_CHECK_EVIDENCE_IMAGE;
			
			m_Parser->nextStep();
		}
	}
}

// centered present button clicked
void Game::onPresentCenterClicked() {
	// we are presenting evidence
	if (m_State.requestingEvidence) {
		// disable the requesting evidence mode
		m_State.requestingEvidence=false;
		
		// get the current evidence/profile
		std::string id="null";
		if (flagged(STATE_EVIDENCE_INFO_PAGE) && !m_State.visibleEvidence.empty())
			id=m_State.visibleEvidence[m_State.evidencePage*8+m_State.selectedEvidence];
		
		else if (!m_State.visibleProfiles.empty())
			id=m_State.visibleProfiles[m_State.profilesPage*8+m_State.selectedProfile];
		
		// split the parameter string up
		StringVector vec=Utils::explodeString(',', m_State.requestedEvidenceParams);
		
		// first, end the current block
		m_Parser->nextStep();
		
		// now, we compare the correct evidence with what is presented
		if (id==vec[0]) {
			// set the next block to follow the correct choice
			m_Parser->setBlock(m_Case->getBuffers()[vec[1]]);
		}
		
		// otherwise, the player screwed up
		else
			m_Parser->setBlock(m_Case->getBuffers()[vec[2]]);
		
		// move along to set block
		m_Parser->nextStep();
		
		// clear variables
		m_State.requestedEvidenceParams="null";
		
		// and toggle flags
		toggle(STATE_COURT_REC_BTN | STATE_NEXT_BTN);
		
		// play sound effect
		Audio::playEffect("sfx_click", Audio::CHANNEL_GUI);
	}
}

// check evidence button clicked
void Game::onCheckButtonClicked() {
	// error checking
	if (m_State.visibleEvidence.empty() || m_State.selectedEvidence>m_State.visibleEvidence.size()-1)
		return;
	
	// schedule a fade out on the gui
	m_State.fadeOut="gui";
	
	// and queue some flags for this image
	m_State.queuedFlags=STATE_CHECK_EVIDENCE_IMAGE | STATE_BACK_BTN | STATE_QUEUED;
	
	// if the text box is also present, draw it as well
	if (flagged(STATE_TEXT_BOX))
		m_State.queuedFlags |= STATE_TEXT_BOX;
}

// click handler for controls
void Game::onControlsClicked(int x, int y) {
	// 8, 134
	bool clicked=false;
	int dy=251;
	
	// examine control
	if ((x>=8 && x<=118) && (y>=dy && y<=dy+26)) {
		onExamineButtonActivated();
		m_State.selectedControl=0;
		clicked=true;
	}
	
	// move control
	else if ((x>=134 && x<=244) && (y>=dy && y<=dy+26)) {
		onMoveButtonActivated();
		m_State.selectedControl=1;
		clicked=true;
	}
	
	// talk control
	else if ((x>=8 && x<=118) && (y>=dy+62 && y<=dy+88)) {
		onTalkButtonActivated();
		m_State.selectedControl=2;
		clicked=true;
	}
	
	// present control
	else if ((x>=134 && x<=244) && (y>=dy+62 && y<=dy+88)) {
		onPresentButtonActivated();
		m_State.selectedControl=3;
		clicked=true;
	}
	
	// play a sound effect if something was clicked
	if (clicked) {
		Audio::playEffect("sfx_click", Audio::CHANNEL_GUI);
		
		// also schedule a fade out, if needed
		if (m_State.selectedControl==1 || m_State.selectedControl==2)
			m_State.fadeOut="gui";
	}
}

// click handler for move scene
void Game::onMoveSceneClicked(const std::string &button) {
	if (!m_Case->getLocation(m_State.currentLocation))
		return;
	
	// get our current location
	Case::Location *location=m_Case->getLocation(m_State.currentLocation);
	
	// calculate the index (an_move_locX_btn)
	std::string tmp=button;
	tmp.erase(0, 11);
	tmp.erase(1, 4);
	int index=atoi(tmp.c_str());
	
	// set this as our selected location
	m_State.selectedLocation=index-1;
	
	// play a sound effect
	Audio::playEffect("sfx_click", Audio::CHANNEL_GUI);
	
	// schedule a fade effect, only if moving outside of court locations
	if (!isCourtLocation(m_State.currentLocation) && !isCourtLocation(location->moveLocations[index-1]))
		m_State.fadeOut="both";
	
	// set our new location
	m_State.queuedLocation=location->moveLocations[index-1];
	
	// once this is done, we need to revert back to main screen
	m_State.prevScreen=SCREEN_MAIN;
	toggle(STATE_CONTROLS | STATE_COURT_REC_BTN);
}

// click handler for talk scene
void Game::onTalkSceneClicked(const std::string &button) {
	if (!flagged(STATE_TALK) || (flagged(STATE_TALK) && m_State.fadeOut!="none"))
		return;
	
	// get current location
	Case::Location *location=m_Case->getLocation(m_State.currentLocation);
	if (!location)
		return;
	
	// get character
	Character *character=m_Case->getCharacter(location->character);
	if (!character && !m_State.requestingAnswer)
		return;
	
	// calculate the index (an_talk_opX_btn)
	std::string tmp=button;
	tmp.erase(0, 10);
	tmp.erase(1, 4);
	int index=atoi(tmp.c_str());
	
	std::string target;
	
	// if we are requesting an answer, select the appropriate option
	if (m_State.requestingAnswer) {
		m_State.requestingAnswer=false; // flag that we're done
		target=m_State.talkOptions[index-1].second;
	}
	
	// otherwise, select the target block
	else
		target=character->getTalkOptions()[index-1].second;
	
	// move along
	m_Parser->setBlock(m_Case->getBuffers()[target]);
	m_Parser->nextStep();
}

// court record page click handler
void Game::onRecPageClickEvent(int x, int y) {
	// see where the click occurred
	int ex=24+12;
	int ey=259;
	
	// cache the page
	bool evidence=flagged(STATE_EVIDENCE_PAGE);
	
	// iterate over slots and see if something was clicked on
	for (int i=0; i<8; i++) {
		// for evidence
		if (evidence) {
			// check click bounds
			if ((x>=ex && x<=ex+40) && (y>=ey && y<=ey+40) && m_State.evidencePage*8+i<m_State.visibleEvidence.size()) {
				// set the selected evidence
				m_State.selectedEvidence=i;
				
				// and view the evidence info page
				int flags=STATE_EVIDENCE_INFO_PAGE | STATE_BACK_BTN;
				
				// show check button where applicable
				if (m_Case->getEvidence(getSelectedEvidence())->checkID!="null")
					flags |= STATE_CHECK_BTN;
				
				// if the previous screen was the present screen, then draw present button instead
				if (m_State.prevScreen==SCREEN_PRESENT)
					flags |= STATE_PRESENT_BTN;
				else
					flags |= STATE_PROFILES_BTN;
				
				// also draw cross examination elements if needed
				if (m_State.curExamination && !m_State.curExaminationPaused)
					flags |= STATE_COURT_GREEN_BAR;
				
				// account for text box
				if (flagged(STATE_TEXT_BOX))
					flags |= STATE_TEXT_BOX;
				toggle(flags);
				
				// play sound effect
				Audio::playEffect("sfx_click", Audio::CHANNEL_GUI);
				
				return;
			}
		}
		
		// for profiles
		else {
			// check click bounds
			if ((x>=ex && x<=ex+40) && (y>=ey && y<=ey+40) && m_State.profilesPage*8+i<m_State.visibleProfiles.size()) {
				// set the selected profile
				m_State.selectedProfile=i;
				
				// and view the profile info page
				int flags=STATE_PROFILE_INFO_PAGE | STATE_BACK_BTN | STATE_EVIDENCE_BTN;
				
				// if the previous screen was the present screen, then draw present button instead
				if (m_State.prevScreen==SCREEN_PRESENT)
					flags |= STATE_PRESENT_BTN;
				else
					flags |= STATE_EVIDENCE_BTN;
				
				// also draw cross examination elements if needed
				if (m_State.curExamination && !m_State.curExaminationPaused)
					flags |= STATE_COURT_GREEN_BAR;
				
				// also account for text box
				if (flagged(STATE_TEXT_BOX))
					flags |= STATE_TEXT_BOX;
				toggle(flags);
				
				// play sound effect
				Audio::playEffect("sfx_click", Audio::CHANNEL_GUI);
				
				return;
			}
		}
		
		// advance to next slot
		ex+=48;
		
		// reset for next row
		if (i==3) {
			ey=305;
			ex=36;
		}
	}
	
	// no evidence/profile was clicked, check the page buttons on either side
	// left button
	if ((x>=1 && x<=1+16) && (y>=253 && y<=253+95)) {
		// for evidence
		if (evidence) {
			m_State.evidencePage-=1;
			if (m_State.evidencePage<0)
				m_State.evidencePage=(int) ceil(m_State.visibleEvidence.size()/8);
		}
		
		// for profiles
		else {
			m_State.profilesPage-=1;
			if (m_State.profilesPage<0)
				m_State.profilesPage=(int) ceil(m_State.visibleProfiles.size()/8);
		}
		
		// play sound effect
		Audio::playEffect("sfx_return", Audio::CHANNEL_GUI);
	}
	
	// right button
	else if ((x>=256-17 && x<=256-1) && (y>=253 && y<=253+95)) {
		// for evidence
		if (evidence) {
			m_State.evidencePage+=1;
			if (m_State.evidencePage>ceil(m_State.visibleEvidence.size()/8))
				m_State.evidencePage=0;
		}
		
		// for profiles
		else {
			m_State.profilesPage+=1;
			if (m_State.profilesPage>ceil(m_State.visibleProfiles.size()/8))
				m_State.profilesPage=0;
		}
		
		// play sound effect
		Audio::playEffect("sfx_return", Audio::CHANNEL_GUI);
	}
}

// court record info page click handler
void Game::onRecInfoPageClickEvent(int x, int y) {
	int ex=0;
	int ey=237;
	
	// see if one of the buttons was clicked
	// left button
	if ((x>=ex && x<=ex+16) && (y>=ey && y<=ey+63))
		selectEvidence(!flagged(STATE_PROFILE_INFO_PAGE), false);
	
	// right button
	else if ((x>=240 && x<=240+16) && (y>=ey && y<=ey+63))
		selectEvidence(!flagged(STATE_PROFILE_INFO_PAGE), true);
	
	// check button clicked, if it's displayed
	else if ((x>=177 && x<=256) && (y>=359 && y<=389) && flagged(STATE_CHECK_BTN))
		onCheckButtonClicked();
}

// examine button activated handler
void Game::onExamineButtonActivated() {
	// toggle the examination scene
	toggle(STATE_EXAMINE | STATE_COURT_REC_BTN | STATE_BACK_BTN);
	
	// also set this as the previous page
	m_State.prevScreen=SCREEN_EXAMINE;
}

// move button activated handler
void Game::onMoveButtonActivated() {
	// toggle move scene
	m_State.queuedFlags=STATE_MOVE | STATE_COURT_REC_BTN | STATE_BACK_BTN | STATE_QUEUED;
	
	// also set this as the previous page
	m_State.prevScreen=SCREEN_MOVE;
}

// talk button activated handler
void Game::onTalkButtonActivated() {
	// toggle talk scene
	m_State.queuedFlags=STATE_TALK | STATE_COURT_REC_BTN | STATE_BACK_BTN | STATE_QUEUED;
	
	// also, set this as the previous scene
	m_State.prevScreen=SCREEN_TALK;
}

// present button activated handler
void Game::onPresentButtonActivated() {
	// toggle present scene
	toggle(STATE_EVIDENCE_PAGE | STATE_PROFILES_BTN | STATE_BACK_BTN);
	
	// also, set this as the previous scene
	m_State.prevScreen=SCREEN_PRESENT;
}

// examine the hotspot in provided coordinate range
void Game::onExamineThing(int x, int y) {
	// get our current location
	Case::Location *location=m_Case->getLocation(m_State.currentLocation);
	if (!location)
		return;
	
	// iterate over hotspots and see if one of them was clicked
	for (int i=0; i<location->hotspots.size(); i++) {
		Case::Hotspot hspot=location->hotspots[i];
		
		// see if the click occured in this area
		//if ((x>=hspot.x && x<=hspot.x+hspot.w) && (y>=197+hspot.y && y<=197+hspot.y+hspot.h)) {
		if (Utils::pointInRect(Point(x, y), hspot.rect)) {
			if (m_Case->getBuffers().find(hspot.block)!=m_Case->getBuffers().end()) {
				m_Parser->setBlock(m_Case->getBuffers()[hspot.block]);
				m_Parser->nextStep();
			}
		}
	}
}
