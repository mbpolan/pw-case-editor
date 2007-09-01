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

#include "game.h"
#include "font.h"
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
	m_State.selectedControl=0;
	
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
	
	// vector of textures to load
	std::vector<std::pair<std::string, std::string> > textures;
	textures.push_back(std::make_pair<std::string, std::string>("court_overview", "data/stock/courtoverview.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("court_overview_g", "data/stock/courtoverview_g.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("scanlines_overlay", "data/stock/scanlines.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_top_bar", "data/stock/touchscreen_topbar.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_lower_bar", "data/stock/touchscreen_lowerbar.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_text_box", "data/gui/textbox.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_text_box_border", "data/gui/textbox_border.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_court_rec_btn", "data/gui/court_record.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_next_btn", "data/gui/button.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_button_arrow_next", "data/gui/button_arrow_large.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_profiles_btn", "data/gui/profiles.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_evidence_btn", "data/gui/evidence.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_profiles_bar", "data/gui/profiles_bar.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_evidence_bar", "data/gui/evidence_bar.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_back_btn", "data/gui/back.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_large_btn_right", "data/gui/large_button_right.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_large_btn_left", "data/gui/large_button_left.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_small_btn_right", "data/gui/small_button_right.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_small_btn_left", "data/gui/small_button_left.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_button_arrow_right", "data/gui/button_arrow_right.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_button_arrow_left", "data/gui/button_arrow_left.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_examine_btn", "data/gui/examine.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_move_btn", "data/gui/move.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_talk_btn", "data/gui/talk.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_present_btn", "data/gui/present.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_select_tl", "data/gui/select_tl.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_select_tr", "data/gui/select_tr.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_select_br", "data/gui/select_br.tga"));
	textures.push_back(std::make_pair<std::string, std::string>("tc_select_bl", "data/gui/select_bl.tga"));
	
	// iterate over textures and load them
	for (int i=0; i<textures.size(); i++) {
		SDL_Surface *surface=Textures::createTexture(textures[i].first, textures[i].second);
		if (!surface)
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
	
	// if the parser has finished this block, default to the main screen
	// we flag this only once per complete parse
	static bool once=false;
	if (m_Parser->done() && !once) {
		toggle(STATE_COURT_REC_BTN | STATE_CONTROLS);
		once=true;
	}
	
	else {
		// new block ready for parsing
		if (status!="null" && status!="pause" && status!="end") {
			m_Parser->setBlock(m_Case->getBuffers()[status]);
		}
		
		// pause here and wait for next step
		if (m_Parser->paused()) {
			int flags=STATE_TEXT_BOX;
			
			if (m_State.drawFlags & STATE_EVIDENCE_PAGE) {
				flags |= STATE_LOWER_BAR;
				flags |= STATE_PROFILES_BTN;
				flags |= STATE_EVIDENCE_PAGE;
				flags |= STATE_BACK_BTN;
			}
			else if (m_State.drawFlags & STATE_EVIDENCE_INFO_PAGE) {
				flags |= STATE_LOWER_BAR;
				flags |= STATE_PROFILES_BTN;
				flags |= STATE_EVIDENCE_INFO_PAGE;
				flags |= STATE_BACK_BTN;
			}
			
			else if (m_State.drawFlags & STATE_PROFILES_PAGE) {
				flags |= STATE_LOWER_BAR;
				flags |= STATE_EVIDENCE_BTN;
				flags |= STATE_PROFILES_PAGE;
				flags |= STATE_BACK_BTN;
			}
			
			else if (m_State.drawFlags & STATE_PROFILE_INFO_PAGE) {
				flags |= STATE_LOWER_BAR;
				flags |= STATE_EVIDENCE_BTN;
				flags |= STATE_PROFILE_INFO_PAGE;
				flags |= STATE_BACK_BTN;
			}
			
			else {
				flags |= STATE_COURT_REC_BTN;
				flags |= STATE_NEXT_BTN;
			}
			
			toggle(flags);
		}
	}
}

// handle keyboard event
void Game::onKeyboardEvent(SDL_KeyboardEvent *e) {
	// if the evidence page is displayed, change selected evidence
	if (m_State.drawFlags & STATE_EVIDENCE_PAGE) {
		// get the amount of evidence on this page
		int amount=m_State.visibleEvidence.size()-(m_State.evidencePage*8);
		if (amount>8)
			amount=8;
		
		if (e->keysym.sym==SDLK_RIGHT)
			selectEvidence(true);
		
		else if (e->keysym.sym==SDLK_LEFT)
			selectEvidence(false);
		
		else if (e->keysym.sym==SDLK_RETURN)
			// show the evidence info page for this evidence
			toggle(STATE_EVIDENCE_INFO_PAGE | STATE_LOWER_BAR | STATE_BACK_BTN | STATE_PROFILES_BTN);
	}
	
	// if controls are drawn, change the selection
	else if (m_State.drawFlags & STATE_CONTROLS) {
		if (e->keysym.sym==SDLK_RIGHT) {
			m_State.selectedControl+=1;
			if (m_State.selectedControl>3)
				m_State.selectedControl=0;
		}
		
		else if (e->keysym.sym==SDLK_LEFT) {
			m_State.selectedControl-=1;
			if (m_State.selectedControl<0)
				m_State.selectedControl=3;
		}
		
		else if (e->keysym.sym==SDLK_UP) {
			if (m_State.selectedControl==2)
				m_State.selectedControl=0;
			
			else if (m_State.selectedControl==3)
				m_State.selectedControl=1;
		}
		
		else if (e->keysym.sym==SDLK_DOWN) {
			if (m_State.selectedControl==0)
				m_State.selectedControl=2;
			
			else if (m_State.selectedControl==1)
				m_State.selectedControl=3;
		}
	}
}

// handle mouse click event
void Game::onMouseEvent(SDL_MouseButtonEvent *e) {
	if (e->type==SDL_MOUSEBUTTONDOWN) {
		// see if the click hit the top right button, if any
		if ((e->x>=176 && e->x<=176+80) && (e->y>=197 && e->y<=197+33))
			onTopRightButtonClicked();
		
		// see if the bottom left button was clicked
		else if ((e->x>=0 && e->x<=79) && (e->y>=359 && e->y<=359+30))
			onBottomLeftButtonClicked();
		
		// see if the center button was clicked
		if ((m_State.drawFlags & STATE_NEXT_BTN) && ((e->x>=16 && e->x<=16+223) && (e->y>=242 && e->y<=242+111)))
			m_Parser->nextStep();
		
		// if the evidence page is up, see if anything was clicked
		if (m_State.drawFlags & STATE_EVIDENCE_PAGE)
			onEvidencePageClickEvent(e->x, e->y);
		
		// same applies for evidence info page
		else if (m_State.drawFlags & STATE_EVIDENCE_INFO_PAGE)
			onEvidenceInfoPageClickEvent(e->x, e->y);
	}
}

// toggle game state flags
void Game::toggle(int flags) {
	m_State.drawFlags=0;
	m_State.drawFlags |= flags;
}

// set the current backdrop location
void Game::setLocation(const std::string &location) {
	m_State.currentLocation=location;
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
	if (m_State.currentLocation!="null" &&
	    m_Case->getLocations().find(m_State.currentLocation)!=m_Case->getLocations().end()) {
		// get the background surface
		Case::Location location=m_Case->getLocations()[m_State.currentLocation];
		SDL_Surface *bg=m_Case->getBackgrounds()[location.bg].texture;
		
		// and draw it
		if (bg)
			Renderer::drawImage(0, 0, bg);
	}
	
	else {
		// get an opaque black surface
		SDL_Surface *opaque=Textures::queryTexture("opaque_black");
		SDL_SetAlpha(opaque, SDL_SRCALPHA, 255);
		
		// draw it as the background
		Renderer::drawImage(0, 0, opaque);
	}
	
	// draw text box, if needed
	if (m_State.drawFlags & STATE_TEXT_BOX)
		renderTextBox();
	
	// if there is shown evidence, draw it as well
	if (m_State.shownEvidence!="null") {
		// draw the evidence depending on position
		if (m_State.shownEvidencePos==POSITION_LEFT)
			Renderer::drawImage(20, 20, m_Case->getEvidence()[m_State.shownEvidence].texture);
		else
			Renderer::drawImage(166, 20, m_Case->getEvidence()[m_State.shownEvidence].texture);
	}
}

// render the menu view (lower screen)
void Game::renderMenuView() {
	// render the background texture
	Renderer::drawImage(0, 197, "court_overview_g");
	
	// render court record pages, if needed
	if (m_State.drawFlags & STATE_EVIDENCE_PAGE)
		Renderer::drawEvidencePage(m_State.visibleEvidence, m_State.evidencePage, m_State.selectedEvidence);
	
	else if (m_State.drawFlags & STATE_PROFILES_PAGE)
		Renderer::drawProfilesPage(m_State.profilesPage);
	
	else if (m_State.drawFlags & STATE_EVIDENCE_INFO_PAGE)
		Renderer::drawEvidenceInfoPage(m_State.visibleEvidence, m_State.evidencePage*8+m_State.selectedEvidence);
	
	// draw next button in case of dialog
	else if (m_State.drawFlags & STATE_NEXT_BTN) {
		Renderer::drawImage(16, 242, "tc_next_btn");
		Renderer::drawImage(16+94, 242+42, "tc_button_arrow_next");
	}
	
	// draw controls, if needed
	else if (m_State.drawFlags & STATE_CONTROLS)
		renderControls(CONTROLS_ALL);
	
	// top everything off with scanlines
	Renderer::drawImage(0, 197, "scanlines_overlay");
	
	// draw the top border bar
	Renderer::drawImage(0, 197, "tc_top_bar");
	
	// draw border bars
	if (m_State.drawFlags & STATE_LOWER_BAR)
		Renderer::drawImage(0, 357, "tc_lower_bar");
	
	// draw activated buttons
	if (m_State.drawFlags & STATE_COURT_REC_BTN)
		Renderer::drawImage(176, 197, "tc_court_rec_btn");
	else if (m_State.drawFlags & STATE_EVIDENCE_BTN)
		Renderer::drawImage(177, 197, "tc_evidence_btn");
	else if (m_State.drawFlags & STATE_PROFILES_BTN)
		Renderer::drawImage(177, 197, "tc_profiles_btn");
	
	// draw the back button
	if (m_State.drawFlags & STATE_BACK_BTN)
		Renderer::drawImage(0, 359, "tc_back_btn");
}

// render the text box
void Game::renderTextBox() {
	static bool speakerChecked=false;
	static std::string speaker="";
	
	// draw the actual text box body
	Renderer::drawImage(0, 128, "tc_text_box");
	
	// character speaking
	if (speaker!="none" && speaker!="") {
		// get the tag
		SDL_Surface *tag=m_Case->getCharacters()[speaker].getTextBoxTag();
		
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
		CharacterMap cmap=m_Case->getCharacters();
		if (cmap.find(cspeaker)!=cmap.end()) {
			// get this character's text box tag, if he has one
			Character character=cmap[cspeaker];
			if (character.hasTextBoxTag())
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

// render the examine scene view
void Game::renderExamineScene() {
}

// top right button was clicked
void Game::onTopRightButtonClicked() {
	// if the court record button is shown, activate evidence page
	if ((m_State.drawFlags & STATE_COURT_REC_BTN) || (m_State.drawFlags & STATE_EVIDENCE_BTN)) {
		int flags=STATE_BACK_BTN | STATE_PROFILES_BTN | STATE_EVIDENCE_PAGE | STATE_LOWER_BAR;
		
		// if the text box is also present, draw it as well
		if (m_State.drawFlags & STATE_TEXT_BOX)
			flags |= STATE_TEXT_BOX;
		
		toggle(flags);
	}
	
	// if the profiles button is shown, activate profiles page
	else if (m_State.drawFlags & STATE_PROFILES_BTN) {
		int flags=STATE_BACK_BTN | STATE_EVIDENCE_BTN | STATE_PROFILES_PAGE | STATE_LOWER_BAR;
		
		// if the text box is also present, draw it as well
		if (m_State.drawFlags & STATE_TEXT_BOX)
			flags |= STATE_TEXT_BOX;
		
		toggle(flags);
	}
}

// bottom left button was clicked
void Game::onBottomLeftButtonClicked() {
	// if the back button is shown, return to main screen
	if (m_State.drawFlags & STATE_BACK_BTN) {
		// if either evidence or profiles pages are shown, revert to main screen
		if ((m_State.drawFlags & STATE_EVIDENCE_PAGE) || (m_State.drawFlags & STATE_PROFILES_PAGE)) {
			int flags=STATE_COURT_REC_BTN | STATE_CONTROLS;
			
			// if the text box is also present, draw it as well
			if (m_State.drawFlags & STATE_TEXT_BOX)
				flags |= STATE_TEXT_BOX;
			
			toggle(flags);
		}
		
		// if evidence info page is shown, revert back to evidence page
		else if (m_State.drawFlags & STATE_EVIDENCE_INFO_PAGE) {
			int flags=STATE_PROFILES_BTN | STATE_BACK_BTN | STATE_EVIDENCE_PAGE | STATE_LOWER_BAR;
			
			// if the text box is also present, draw it as well
			if (m_State.drawFlags & STATE_TEXT_BOX)
				flags |= STATE_TEXT_BOX;
			
			toggle(flags);
		}
	}
}

// evidence page click handler
void Game::onEvidencePageClickEvent(int x, int y) {
	// see where the click occurred
	int ex=24+12;
	int ey=259;
	
	// iterate over slots and see if something was clicked on
	for (int i=0; i<8; i++) {
		// check click bounds
		if ((x>=ex && x<=ex+40) && (y>=ey && y<=ey+40) && m_State.evidencePage*8+i<m_State.visibleEvidence.size()) {
			// set the selected evidence
			m_State.selectedEvidence=i;
			
			// and view the evidence info page
			int flags=STATE_EVIDENCE_INFO_PAGE | STATE_LOWER_BAR | STATE_BACK_BTN | STATE_PROFILES_BTN;
			if (m_State.drawFlags & STATE_TEXT_BOX)
				flags |= STATE_TEXT_BOX;
			toggle(flags);
			
			return;
		}
		
		// advance to next slot
		ex+=40+8;
		
		// reset for next row
		if (i==3) {
			ey=305;
			ex=24+12;
		}
	}
	
	// no evidence was clicked, check the page buttons on either side
	// left button
	if ((x>=1 && x<=1+16) && (y>=253 && y<=253+95)) {
		m_State.evidencePage-=1;
		if (m_State.evidencePage<0)
			m_State.evidencePage=(int) ceil(m_State.visibleEvidence.size()/8);
	}
	
	// right button
	else if ((x>=256-17 && x<=256-1) && (y>=253 && y<=253+95)) {
		m_State.evidencePage+=1;
		if (m_State.evidencePage>ceil(m_State.visibleEvidence.size()/8)-1)
			m_State.evidencePage=0;
	}
}

// evidence info page click handler
void Game::onEvidenceInfoPageClickEvent(int x, int y) {
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
