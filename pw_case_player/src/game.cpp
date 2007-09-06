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
	
	// reset examination cursor position
	m_State.examineX=256/2;
	m_State.examineY=192/2;
	
	// reset previous page
	m_State.prevScreen=0;
	
	// reset displayed character
	m_State.displayChar="null";
	
	// null out variables
	m_State.currentLocation="null";
	m_State.shownEvidence="null";
	
	// allocate text parser
	m_Parser=new TextParser(this);
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
}

// load stock textures
bool Game::loadStockTextures() {
	// vector of fonts to load
	std::vector<std::pair<std::string, std::string> > fonts;
	fonts.push_back(std::make_pair<std::string, std::string>("white", "data/fonts/white.pwf"));
	fonts.push_back(std::make_pair<std::string, std::string>("black", "data/fonts/black.pwf"));
	fonts.push_back(std::make_pair<std::string, std::string>("blue", "data/fonts/blue.pwf"));
	fonts.push_back(std::make_pair<std::string, std::string>("orange", "data/fonts/orange.pwf"));
	fonts.push_back(std::make_pair<std::string, std::string>("green", "data/fonts/green.pwf"));
	
	// iterate over fonts and load them
	for (int i=0; i<fonts.size(); i++) {
		Fonts::Font font;
		if (!Fonts::loadFont(fonts[i].second, font)) {
			std::cout << "Unable to load font '" << fonts[i].first << "': " << fonts[i].second << std::endl;
			return false;
		}
		
		// add this font
		Fonts::pushFont(fonts[i].first, font);
	}
	
	// load stock assets
	if (!IO::loadStockFile("stock.cfg")) {
		std::cout << "Unable to load stock assets from file\n";
		return false;
	}
	
	// modify certain textures
	SDL_SetAlpha(Textures::queryTexture("scanlines_overlay"), SDL_SRCALPHA, 30);
	SDL_SetAlpha(Textures::queryTexture("tc_next_btn"), SDL_SRCALPHA, 225);
	SDL_SetAlpha(Textures::queryTexture("tc_text_box"), SDL_SRCALPHA, 165);
	SDL_SetAlpha(Textures::queryTexture("tc_text_box_border"), SDL_SRCALPHA, 165);
	
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
	
	// begin parsing the game script
	BufferMap bmap=m_Case->getBuffers();
	m_Parser->setBlock(bmap[m_Case->getInitialBlockId()]);
	m_Parser->nextStep();
	
	return true;
}

// render the current scene
void Game::render() {
	// render top screen
	renderTopView();
	
	// render lower screen
	renderMenuView();
	
	// once everything static is drawn, parse the block
	std::string status=m_Parser->parse();
	
	// new block ready for parsing
	if (status!="null" && status!="pause" && status!="end") {
		m_Parser->setBlock(m_Case->getBuffers()[status]);
	}
		
	// pause here and wait for next step
	// while waiting, be sure to keep redrawing the already present screen
	if (m_Parser->paused()) {
		int flags=STATE_TEXT_BOX;
		
		// draw evidence page
		if (flagged(STATE_EVIDENCE_PAGE)) {
			flags |= STATE_LOWER_BAR;
			flags |= STATE_PROFILES_BTN;
			flags |= STATE_EVIDENCE_PAGE;
			flags |= STATE_BACK_BTN;
		}
		
		// draw evidence info page
		else if (flagged(STATE_EVIDENCE_INFO_PAGE)) {
			flags |= STATE_LOWER_BAR;
			flags |= STATE_PROFILES_BTN;
			flags |= STATE_EVIDENCE_INFO_PAGE;
			flags |= STATE_BACK_BTN;
		}
		
		// draw profiles page
		else if (flagged(STATE_PROFILES_PAGE)) {
			flags |= STATE_LOWER_BAR;
			flags |= STATE_EVIDENCE_BTN;
			flags |= STATE_PROFILES_PAGE;
			flags |= STATE_BACK_BTN;
		}
		
		// draw profile info page
		else if (flagged(STATE_PROFILE_INFO_PAGE)) {
			flags |= STATE_LOWER_BAR;
			flags |= STATE_EVIDENCE_BTN;
			flags |= STATE_PROFILE_INFO_PAGE;
			flags |= STATE_BACK_BTN;
		}
		
		// otherwise draw the dialog next button
		else {
			flags |= STATE_COURT_REC_BTN;
			flags |= STATE_NEXT_BTN;
		}
		
		toggle(flags);
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
			toggle(STATE_EVIDENCE_INFO_PAGE | STATE_LOWER_BAR | STATE_BACK_BTN | STATE_PROFILES_BTN);
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
	}
}

// handle mouse click event
void Game::onMouseEvent(SDL_MouseButtonEvent *e) {
	// button pressed down
	if (e->type==SDL_MOUSEBUTTONDOWN) {
		// check for clicks on locations in move scene
		if (flagged(STATE_MOVE))
			onMoveSceneClicked(e->x, e->y);
		
		// check for clicks on examine scene
		if (flagged(STATE_EXAMINE)) {
			// this will always be the court record button in this case
			if ((e->x>=176 && e->x<=176+79) && (e->y>=197 && e->y<=197+21))
				onTopRightButtonClicked();
			
			// likewise, this will always be the back button
			else if ((e->x>=0 && e->x<=79) && (e->y>=369 && e->y<=369+21))
				onBottomLeftButtonClicked();
			
			// examine button clicked
			else if ((e->x>=256-79 && e->x<=256) && (e->y>=369 && e->y<=369+21))
				onExamineThing(m_State.examineX, m_State.examineY+197);
		}
		
		// buttons have different positions in the examine scene
		else if (!flagged(STATE_EXAMINE)) {
			// see if the click hit the top right button, if any
			if ((e->x>=176 && e->x<=176+80) && (e->y>=197 && e->y<=197+33))
				onTopRightButtonClicked();
			
			// see if the bottom left button was clicked
			else if ((e->x>=0 && e->x<=79) && (e->y>=359 && e->y<=359+30))
				onBottomLeftButtonClicked();
		}
		
		// see if the center button was clicked
		if (flagged(STATE_NEXT_BTN) && ((e->x>=16 && e->x<=16+223) && (e->y>=242 && e->y<=242+111))) {
			if (m_Parser->dialogueDone()) {
				// play a sound effect
				Audio::playEffect("sfx_next_part", GUI_SFX_CHANNEL);
			}
			
			// proceed to the next block
			m_Parser->nextStep();
		}
		
		// if the controls are drawn, see if one was clicked
		else if (flagged(STATE_CONTROLS))
			onControlsClicked(e->x, e->y);
		
		// if talk scene is drawn, see if one option was clicked
		else if (flagged(STATE_TALK))
			onTalkSceneClicked(e->x, e->y);
		
		// if the court record page is up, see if anything was clicked
		if (flagged(STATE_EVIDENCE_PAGE) || flagged(STATE_PROFILES_PAGE))
			onRecPageClickEvent(e->x, e->y);
		
		// same applies for evidence/profiles info page
		else if (flagged(STATE_EVIDENCE_INFO_PAGE) || flagged(STATE_PROFILE_INFO_PAGE))
			onRecInfoPageClickEvent(e->x, e->y);
	}
}

// check input device state
void Game::checkInputState() {
	// if the examination scene is shown, move the crosshairs
	if (flagged(STATE_EXAMINE)) {
		// get the current keyboard state
		Uint8 *keys=SDL_GetKeyState(NULL);
		
		// move cursor up
		if (keys[SDLK_UP] && m_State.examineY!=0) {
			m_State.examineY-=1;
		}
		
		// move cursor down
		if (keys[SDLK_DOWN] && m_State.examineY<256)
			m_State.examineY+=1;
		
		// move cursor left
		if (keys[SDLK_LEFT] && m_State.examineX!=0)
			m_State.examineX-=1;
		
		// move cursor right
		if (keys[SDLK_RIGHT] && m_State.examineX<256)
			m_State.examineX+=1;
		
		// get the mouse state
		int mx, my;
		Uint8 state=SDL_GetMouseState(&mx, &my);
		
		// move the cursors if the first button is pressed
		if ((state & SDL_BUTTON(1)) && (my>=197+22 && my<=391-22)) {
			// update position
			m_State.examineX=mx;
			m_State.examineY=192-(391-my);
		}
	}
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

// set the current backdrop location
void Game::setLocation(const std::string &locationId) {
	if (!m_Case->getLocation(locationId))
		return;
	
	// get the location
	Case::Location *location=m_Case->getLocation(locationId);
	
	// set the new location
	m_State.currentLocation=locationId;
	
	// fade out the current music, if any
	Audio::haltMusic();
	
	// if this location has set music, then play it
	if (location->music!="null")
		Audio::playMusic(location->music);
	
	// if this location has a trigger block, execute it now
	if (location->triggerBlock!="null") {
		m_Parser->setBlock(m_Case->getBuffers()[location->triggerBlock]);
		m_Parser->nextStep();
		
		// clear the trigger
		location->triggerBlock="null";
	}
}

// set the evidence to draw on top screen
void Game::setShownEvidence(const std::string &id, const Position &pos) {
	if (id=="null")
		m_State.shownEvidence="null";
	
	else {
		m_State.shownEvidence=id;
		m_State.shownEvidencePos=pos;
	}
}

// change the selected evidence
void Game::selectEvidence(bool increment) {
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
}

// render the game view (top screen)
void Game::renderTopView() {
	// draw the background for this location
	if (m_State.currentLocation!="null" && m_Case->getLocation(m_State.currentLocation)) {
		// get the background surface
		Case::Location *location=m_Case->getLocation(m_State.currentLocation);
		SDL_Surface *bg=m_Case->getBackground(location->bg)->texture;
		
		// and draw it
		if (bg)
			Renderer::drawImage(0, 0, bg);
		
		// if there is a character set here, draw him now
		if (m_Case->getCharacter(location->character)) {
			Sprite *sprite=m_Case->getCharacter(location->character)->getSprite();
			
			// reset animation
			sprite->setAnimation("idle");
			
			// calculate the x,y draw position
			int x=(256/2)-(sprite->getCurrentFrame()->image->w/2);
			int y=192-(sprite->getCurrentFrame()->image->h);
			
			// draw the sprite
			sprite->animate(x, y);
		}
		
		// if the examination scene is up, dim the upper screen
		if (flagged(STATE_EXAMINE)) {
			// get an opaque black surface
			SDL_Surface *opaque=Textures::queryTexture("opaque_black");
			SDL_SetAlpha(opaque, SDL_SRCALPHA, 128);
			
			// draw it over the location
			Renderer::drawImage(0, 0, opaque);
		}
	}
	
	else {
		// get an opaque black surface
		SDL_Surface *opaque=Textures::queryTexture("opaque_black");
		SDL_SetAlpha(opaque, SDL_SRCALPHA, 255);
		
		// draw it as the background
		Renderer::drawImage(0, 0, opaque);
	}
	
	// draw text box, if needed
	if (flagged(STATE_TEXT_BOX)) {
		// if there is a character to display, handle the request
		if (m_State.displayChar!="null" && m_Case->getCharacter(m_State.displayChar)) {
			// get the sprite
			Sprite *sprite=m_Case->getCharacter(m_State.displayChar)->getSprite();
			
			// set talk animation if the dialogue is still being drawn
			if (!m_Parser->dialogueDone() && m_Parser->getSpeaker()==m_State.displayChar)
				sprite->setAnimation("talk");
			else
				sprite->setAnimation("idle");
			
			// calculate the x,y draw position
			int x=(256/2)-(sprite->getCurrentFrame()->image->w/2);
			int y=192-(sprite->getCurrentFrame()->image->h);
			
			// animate the sprite
			sprite->animate(x, y);
		}
		
		// draw the text box over everything
		renderTextBox();
	}
	
	// if there is shown evidence, draw it as well
	if (m_State.shownEvidence!="null") {
		Case::Evidence *ev=m_Case->getEvidence(m_State.shownEvidence);
		if (!ev)
			return;
		
		// draw the evidence depending on position
		if (m_State.shownEvidencePos==POSITION_LEFT)
			Renderer::drawImage(20, 20, ev->texture);
		else
			Renderer::drawImage(166, 20, ev->texture);
	}
}

// render the menu view (lower screen)
void Game::renderMenuView() {
	// when dealing with drawing elements, some need to be drawn "thin"
	// this is only true when the examine scene is being drawn
	std::string append="";
	if (flagged(STATE_EXAMINE))
		append="_thin";
	
	// render the background texture (or location background if examining)
	if (flagged(STATE_EXAMINE)) {
		// get the current background
		Case::Location *location=m_Case->getLocation(m_State.currentLocation);
		if (!location)
			return;
		
		// get the background
		Case::Background *bg=m_Case->getBackground(location->bg);
		
		Renderer::drawImage(0, 197, bg->texture);
	}
	else
		Renderer::drawImage(0, 197, "court_overview_g");
	
	// draw the evidence page
	if (flagged(STATE_EVIDENCE_PAGE))
		Renderer::drawEvidencePage(m_State.visibleEvidence, m_State.evidencePage, m_State.selectedEvidence);
	
	// draw the profiles page
	else if (flagged(STATE_PROFILES_PAGE))
		Renderer::drawProfilesPage(m_State.visibleProfiles, m_State.profilesPage, m_State.selectedProfile);
	
	// draw the evidence info page
	else if (flagged(STATE_EVIDENCE_INFO_PAGE))
		Renderer::drawEvidenceInfoPage(m_State.visibleEvidence, m_State.evidencePage*8+m_State.selectedEvidence);
	
	// draw the profile info page
	else if (flagged(STATE_PROFILE_INFO_PAGE))
		Renderer::drawProfileInfoPage(m_State.visibleProfiles, m_State.profilesPage*8+m_State.selectedProfile);
	
	// draw the examination scene
	else if (flagged(STATE_EXAMINE)) {
		// get the current location and its mapped background
		Case::Location *location=m_Case->getLocation(m_State.currentLocation);
		Case::Background *bg=m_Case->getBackground(location->bg);
		
		Renderer::drawExamineScene(bg->texture, m_State.examineX, m_State.examineY);
	}
	
	// draw the move scene
	else if (flagged(STATE_MOVE)) {
		// get current location
		Case::Location *location=m_Case->getLocation(m_State.currentLocation);
		
		Renderer::drawMoveScene(location->moveLocations, m_Case->getLocations(), m_State.selectedLocation);
	}
	
	// draw talk scene
	else if (flagged(STATE_TALK)) {
		// get current character at location
		Case::Location *location=m_Case->getLocation(m_State.currentLocation);
		Character *character=m_Case->getCharacter(location->character);
		if (character)
			Renderer::drawTalkScene(character->getTalkOptions(), m_State.selectedTalkOption);
	}
	
	// draw next button in case of dialog
	else if (flagged(STATE_NEXT_BTN)) {
		Renderer::drawImage(16, 242, "tc_next_btn");
		Renderer::drawImage(16+94, 242+42, "tc_button_arrow_next");
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
	
	// top everything off with scanlines
	Renderer::drawImage(0, 197, "scanlines_overlay");
	
	// draw the top border bar
	Renderer::drawImage(0, 197, "tc_top_bar"+append);
	
	// draw border bars
	if (flagged(STATE_LOWER_BAR)) {
		// thin bar needs to be drawn lower
		int y=357;
		if (append=="_thin")
			y+=10;
		
		// draw this lower bar border
		Renderer::drawImage(0, y, "tc_lower_bar"+append);
	}
	
	// draw activated buttons
	if (flagged(STATE_COURT_REC_BTN))
		Renderer::drawImage(176, 197, "tc_court_rec_btn"+append);
	else if (flagged(STATE_EVIDENCE_BTN))
		Renderer::drawImage(177, 197, "tc_evidence_btn");
	else if (flagged(STATE_PROFILES_BTN))
		Renderer::drawImage(177, 197, "tc_profiles_btn");
	
	// draw examine button if needed
	if (flagged(STATE_EXAMINE))
		Renderer::drawImage(256-79, 369, "tc_examine_btn_thin");
	
	// draw the back button
	if (flagged(STATE_BACK_BTN)) {
		// same applies here, draw the back button lower if it is the thin variety
		int y=359;
		if (append=="_thin")
			y+=10;
		
		// draw the button
		Renderer::drawImage(0, y, "tc_back_btn"+append);
	}
}

// render the text box
void Game::renderTextBox() {
	static bool speakerChecked=false;
	static std::string speaker="";
	
	// draw the actual text box body
	Renderer::drawImage(0, 128, "tc_text_box");
	
	// character speaking
	if (speaker!="none" && speaker!="" && m_Case->getCharacter(speaker)) {
		// get the tag
		SDL_Surface *tag=m_Case->getCharacter(speaker)->getTextBoxTag();
		
		// and draw it
		Renderer::drawImage(0, 118, tag);
	}
	
	// narration or no one speaking
	else
		Renderer::drawImage(0, 128, Textures::queryTexture("tc_text_box_border"));
	
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
		Renderer::drawImage(8, y, "tc_examine_btn");
		
		// set high light coordinates if this is selected
		if (m_State.selectedControl==0) {
			sx1=5;
			sx2=107;
			sy1=y;
			sy2=y+14;
		}
	}
	
	if ((flags & CONTROLS_MOVE) || (flags & CONTROLS_ALL)) {
		Renderer::drawImage(134, y, "tc_move_btn");
		
		// set high light coordinates if this is selected
		if (m_State.selectedControl==1) {
			sx1=131;
			sx2=234;
			sy1=y;
			sy2=y+14;
		}
	}
	
	if ((flags & CONTROLS_TALK) || (flags & CONTROLS_ALL)) {
		Renderer::drawImage(8, y+26+36, "tc_talk_btn");
		
		// set high light coordinates if this is selected
		if (m_State.selectedControl==2) {
			sx1=5;
			sx2=107;
			sy1=y+26+36;
			sy2=sy1+14;
		}
	}
	
	if ((flags & CONTROLS_PRESENT) || (flags & CONTROLS_ALL)) {
		Renderer::drawImage(134, y+26+36, "tc_present_btn");
		
		// set high light coordinates if this is selected
		if (m_State.selectedControl==3) {
			sx1=131;
			sx2=234;
			sy1=y+26+36;
			sy2=sy1+14;
		}
	}
	
	// draw selection box
	Renderer::drawImage(sx1, sy1, "tc_select_tl");
	Renderer::drawImage(sx1, sy2, "tc_select_bl");
	Renderer::drawImage(sx2, sy2, "tc_select_br");
	Renderer::drawImage(sx2, sy1, "tc_select_tr");
}

// top right button was clicked
void Game::onTopRightButtonClicked() {
	// if the court record button is shown, activate evidence page
	if (flagged(STATE_COURT_REC_BTN)) {
		int flags=STATE_BACK_BTN | STATE_PROFILES_BTN | STATE_EVIDENCE_PAGE | STATE_LOWER_BAR;
		
		// if the text box is also present, draw it as well
		if (flagged(STATE_TEXT_BOX))
			flags |= STATE_TEXT_BOX;
		
		toggle(flags);
	}
	
	// if the profiles button is shown, activate profiles page
	else if (flagged(STATE_PROFILES_BTN) && !flagged(STATE_EVIDENCE_INFO_PAGE)) {
		int flags=STATE_BACK_BTN | STATE_EVIDENCE_BTN | STATE_PROFILES_PAGE | STATE_LOWER_BAR;
		
		// if the text box is also present, draw it as well
		if (flagged(STATE_TEXT_BOX))
			flags |= STATE_TEXT_BOX;
		
		toggle(flags);
	}
	
	// if the evidence button is shown, activate evidence page
	else if (flagged(STATE_EVIDENCE_BTN) && !flagged(STATE_PROFILE_INFO_PAGE)) {
		int flags=STATE_BACK_BTN | STATE_PROFILES_BTN | STATE_EVIDENCE_PAGE | STATE_LOWER_BAR;
		
		// if the text box is also present, draw it as well
		if (flagged(STATE_TEXT_BOX))
			flags |= STATE_TEXT_BOX;
		
		toggle(flags);
	}
	
	// if the profiles button is shown during evidence info screen, switch to profile info screen
	else if (flagged(STATE_PROFILES_BTN) && flagged(STATE_EVIDENCE_INFO_PAGE)) {
		int flags=STATE_BACK_BTN | STATE_EVIDENCE_BTN | STATE_PROFILE_INFO_PAGE | STATE_LOWER_BAR;
		
		// if the text box is also present, draw it as well
		if (flagged(STATE_TEXT_BOX))
			flags |= STATE_TEXT_BOX;
		
		toggle(flags);
	}
	
	// if the evidence button is shown during profile info screen, switch to evidence info screen
	else if (flagged(STATE_EVIDENCE_BTN) && flagged(STATE_PROFILE_INFO_PAGE)) {
		int flags=STATE_BACK_BTN | STATE_PROFILES_BTN | STATE_EVIDENCE_INFO_PAGE | STATE_LOWER_BAR;
		
		// if the text box is also present, draw it as well
		if (flagged(STATE_TEXT_BOX))
			flags |= STATE_TEXT_BOX;
		
		toggle(flags);
	}
}

// bottom left button was clicked
void Game::onBottomLeftButtonClicked() {
	// if the back button is shown, return to previous screen
	if (flagged(STATE_BACK_BTN)) {
		// if either evidence or profiles pages, or examination scene are shown, revert to main screen
		if (flagged(STATE_EVIDENCE_PAGE) || flagged(STATE_PROFILES_PAGE)) {
			// if the previous screen is the examine screen, then draw it instead
			if (m_State.prevScreen==SCREEN_EXAMINE) {
				int flags=STATE_EXAMINE | STATE_COURT_REC_BTN | STATE_LOWER_BAR | STATE_BACK_BTN;
				toggle(flags);
			}
			
			// if the previous screen is the move screen, then draw it
			else if (m_State.prevScreen==SCREEN_MOVE) {
				int flags=STATE_MOVE | STATE_COURT_REC_BTN | STATE_LOWER_BAR | STATE_BACK_BTN;
				toggle(flags);
			}
			
			// if the previous screen is the talk screen, then draw it
			else if (m_State.prevScreen==SCREEN_TALK) {
				int flags=STATE_TALK | STATE_COURT_REC_BTN | STATE_LOWER_BAR | STATE_BACK_BTN;
				toggle(flags);
			}
			
			// must be the main screen
			else {
				int flags=STATE_COURT_REC_BTN | STATE_CONTROLS;
				
				// if the text box is also present, draw it as well
				if (flagged(STATE_TEXT_BOX))
					flags |= STATE_TEXT_BOX;
				
				// flag that were are now at the main screen
				m_State.prevScreen=SCREEN_MAIN;
				
				toggle(flags);
			}
		}
		
		// if examine, talk, or move screen is show, revert back to main screen
		else if (flagged(STATE_EXAMINE) || flagged(STATE_MOVE) || flagged(STATE_TALK)) {
			m_State.prevScreen=SCREEN_MAIN;
			
			toggle(STATE_COURT_REC_BTN | STATE_CONTROLS);
		}
		
		// if evidence info page is shown, revert back to evidence page
		else if (flagged(STATE_EVIDENCE_INFO_PAGE)) {
			int flags=STATE_PROFILES_BTN | STATE_BACK_BTN | STATE_EVIDENCE_PAGE | STATE_LOWER_BAR;
			
			// if the text box is also present, draw it as well
			if (flagged(STATE_TEXT_BOX))
				flags |= STATE_TEXT_BOX;
			
			toggle(flags);
		}
		
		// if profile info page is shown, revert back to profiles page
		else if (flagged(STATE_PROFILE_INFO_PAGE)) {
			int flags=STATE_EVIDENCE_BTN | STATE_BACK_BTN | STATE_PROFILES_PAGE | STATE_LOWER_BAR;
			
			// if the text box is also present, draw it as well
			if (flagged(STATE_TEXT_BOX))
				flags |= STATE_TEXT_BOX;
			
			toggle(flags);
		}
	}
}

// click handler for controls
void Game::onControlsClicked(int x, int y) {
	// 8, 134
	int dy=197+20+34;
	
	// examine control
	if ((x>=8 && x<=118) && (y>=dy && y<=dy+26)) {
		onExamineButtonActivated();
		m_State.selectedControl=0;
	}
	
	// move control
	else if ((x>=134 && x<=244) && (y>=dy && y<=dy+26)) {
		onMoveButtonActivated();
		m_State.selectedControl=1;
	}
	
	// talk control
	else if ((x>=8 && x<=118) && (y>=dy+62 && y<=dy+88)) {
		onTalkButtonActivated();
		m_State.selectedControl=2;
	}
	
	// present control
	else if ((x>=134 && x<=244) && (y>=dy+62 && y<=dy+88)) {
		onPresentButtonActivated();
		m_State.selectedControl=3;
	}
}

// click handler for move scene
void Game::onMoveSceneClicked(int x, int y) {
	if (!m_Case->getLocation(m_State.currentLocation))
		return;
	
	// get our current location
	Case::Location *location=m_Case->getLocation(m_State.currentLocation);
	
	// starting coordinates
	int dx=256/3;
	int dy=197+34+5;
	
	// iterate over drawn locations and see if one of them was clicked
	for (int i=0; i<location->moveLocations.size(); i++) {
		// see if this button was clicked
		if ((x>=dx+1 && x<=dx+148) && (y>=dy+1 && y<=dy+18)) {
			// set this as our selected location
			m_State.selectedLocation=i;
			
			// set our new location
			setLocation(location->moveLocations[i]);
			
			// once this is done, we need to revert back to main screen
			m_State.prevScreen=SCREEN_MAIN;
			toggle(STATE_CONTROLS | STATE_COURT_REC_BTN);
			
			break;
		}
		
		// move on to next button
		dy+=25;
	}
}

// click handler for talk scene
void Game::onTalkSceneClicked(int x, int y) {
	// get current location
	Case::Location *location=m_Case->getLocation(m_State.currentLocation);
	if (!location)
		return;
	
	// get character
	Character *character=m_Case->getCharacter(location->character);
	if (!character)
		return;
	
	// keep track of x,y changes
	int dx=5;
	int dy=197+34+5;
	
	// iterate over options
	for (int i=0; i<character->getTalkOptions().size(); i++) {
		// see if this one was clicked
		if ((x>=dx && x<=dx+200) && (y>=dy && y<=dy+20)) {
			m_State.selectedTalkOption=i;
			
			// get the target block id
			std::string target=character->getTalkOptions()[i].second;
			
			// set this block
			m_Parser->setBlock(m_Case->getBuffers()[target]);
			m_Parser->nextStep();
		}
		
		// move down to next slot
		dy+=25;
	}
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
				int flags=STATE_EVIDENCE_INFO_PAGE | STATE_LOWER_BAR | STATE_BACK_BTN | STATE_PROFILES_BTN;
				if (flagged(STATE_TEXT_BOX))
					flags |= STATE_TEXT_BOX;
				toggle(flags);
				
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
				int flags=STATE_PROFILE_INFO_PAGE | STATE_LOWER_BAR | STATE_BACK_BTN | STATE_EVIDENCE_BTN;
				if (flagged(STATE_TEXT_BOX))
					flags |= STATE_TEXT_BOX;
				toggle(flags);
				
				return;
			}
		}
		
		// advance to next slot
		ex+=40+8;
		
		// reset for next row
		if (i==3) {
			ey=305;
			ex=24+12;
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
	}
}

// court record info page click handler
void Game::onRecInfoPageClickEvent(int x, int y) {
	int ex=0;
	int ey=237;
	
	// see if one of the buttons was clicked
	// left button
	if ((x>=ex && x<=ex+16) && (y>=ey && y<=ey+63))
		selectEvidence(false);
	
	// right button
	else if ((x>=240 && x<=240+16) && (y>=ey && y<=ey+63))
		selectEvidence(true);
}

// examine button activated handler
void Game::onExamineButtonActivated() {
	// toggle the examination scene
	toggle(STATE_EXAMINE | STATE_COURT_REC_BTN | STATE_LOWER_BAR | STATE_BACK_BTN);
	
	// also set this as the previous page
	m_State.prevScreen=SCREEN_EXAMINE;
}

// move button activated handler
void Game::onMoveButtonActivated() {
	// toggle move scene
	toggle(STATE_MOVE | STATE_COURT_REC_BTN | STATE_LOWER_BAR | STATE_BACK_BTN);
	
	// also set this as the previous page
	m_State.prevScreen=SCREEN_MOVE;
}

// talk button activated handler
void Game::onTalkButtonActivated() {
	// toggle talk scene
	toggle(STATE_TALK | STATE_COURT_REC_BTN | STATE_LOWER_BAR | STATE_BACK_BTN);
	
	// also, set this as the previous scene
	m_State.prevScreen=SCREEN_TALK;
}

// present button activated handler
void Game::onPresentButtonActivated() {
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
		if ((x>=hspot.x && x<=hspot.x+hspot.w) && (y>=197+hspot.y && y<=197+hspot.y+hspot.h)) {
			if (m_Case->getBuffers().find(hspot.block)!=m_Case->getBuffers().end()) {
				m_Parser->setBlock(m_Case->getBuffers()[hspot.block]);
				m_Parser->nextStep();
			}
		}
	}
}
