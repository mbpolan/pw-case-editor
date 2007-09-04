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
// textparser.cpp: implementation of TextParser class

#include "audio.h"
#include "font.h"
#include "game.h"
#include "textparser.h"

// constructor
TextParser::TextParser(Game *game): m_Game(game) {
	// reset and prepare the parser
	reset();
	clearFormatting();
	
	// reset variables
	m_Dialogue="";
	m_Direct=false;
	m_Speed=50;
}

// set the text block
void TextParser::setBlock(const std::string &block) {
	reset();
	
	// set this block
	m_Block=block;
	m_Pause=true;
	m_Done=false;
	
	// proceed directly to the next block if requested
	if (m_Direct) {
		m_Pause=false;
		m_Direct=false;
	}
}

// reset the parser
void TextParser::reset() {
	m_BreakPoint=0;
	m_Pause=false;
	m_Done=true;
	m_TagOpen=false;
	m_CurTag="";
	m_NextBlock="null";
	
	m_StrPos=1;
	m_LastChar=0;
}

// parse the given control block
std::string TextParser::parse() {
	// if we are done parsing, return here
	if (m_Done)
		return "end";
	
	// if the parser is not paused, start parsing the set block
	if (!m_Pause) {
		// erase up to this point
		m_Block.erase(0, m_BreakPoint);
		
		// reset draw string
		m_Dialogue="";
		
		// start going over block
		while(1) {
			char ch=m_Block[0];
			
			// tag delimiter
			if (ch=='<') {
				// if a tag is already open, this one must close it
				if (m_TagOpen) {
					int npos=m_Block.find(">");
					
					// erase up to this point
					m_Block.erase(0, npos+1);
					
					m_TagOpen=false;
				}
				
				else {
					// find the closing tag
					int npos=m_Block.find(">");
					m_TagOpen=true;
					
					// extract tag name and make it lowercase
					m_CurTag=m_Block.substr(1, npos-1);
					
					// parse this tag
					parseTag(m_CurTag);
					
					// erase up to this point
					m_Block.erase(0, npos+1);
				}
				
				continue;
			}
			
			// trigger
			else if (ch=='{') {
				// find trigger op end
				int npos=m_Block.find(':');
				
				// get our trigger operator
				std::string trigOp=m_Block.substr(2, npos-2);
				m_Block.erase(0, npos+1);
				
				// get trigger command
				npos=m_Block.find(';');
				std::string trigComm=m_Block.substr(0, npos);
				
				// execute this trigger
				std::string ret=doTrigger(trigOp, trigComm);
				
				// erase to the end of this trigger
				int end=m_Block.find('}');
				m_Block.erase(0, end);
				
				continue;
			}
			
			// normal character
			else {
				// make sure useless characters aren't dealt with
				if (ch!='\n') {
					// add this character to the draw string
					m_Dialogue+=(char) ch;
					
					// see if we are over the text limit and need to break
					if (Fonts::willBreak(4, 132, SDL_GetVideoSurface()->w, m_Dialogue, "white")) {
						m_Pause=true;
						break;
					}
				}
				
				// erase up to this point
				m_Block.erase(0, 1);
			}
			
			// if we are done parsing, end this loop
			if (m_Done) {
				m_Pause=true;
				return "end";
			}
			
			// we have completely parsed this block
			if (m_Block.empty()) {
				m_Pause=true;
				return m_NextBlock;
			}
		}
	}
	
	else {
		// see if we should draw the next character in the string
		int now=SDL_GetTicks();
		if (now-m_LastChar>m_Speed && m_StrPos<m_Dialogue.size()) {
			m_LastChar=now;
			m_StrPos++;
		}
		
		// draw the string
		Fonts::drawString(4, 132, m_StrPos, SDL_GetVideoSurface()->w, m_Dialogue, m_FontColor);
	}
	
	return "pause";
}

// move on to the next break point
void TextParser::nextStep() {
	// if this block is empty and we didn't find the next one,
	// then we flag that we're done
	if (m_Block.empty() && m_NextBlock=="null" && m_StrPos==m_Dialogue.size()) {
		// draw the previous screen
		if (m_Game->m_State.prevScreen==SCREEN_EXAMINE)
			m_Game->toggle(STATE_EXAMINE | STATE_COURT_REC_BTN | STATE_LOWER_BAR | STATE_BACK_BTN);
		
		else
			m_Game->toggle(STATE_COURT_REC_BTN | STATE_CONTROLS);
		
		// also, if there was a shown character talking, reset him
		m_Game->m_State.displayChar="null";
		
		// flag that we are done
		m_Done=true;
		m_Pause=false;
		
		// also, make sure to end any talk animations
		m_StrPos=m_Dialogue.size();
		m_Dialogue="";
		
		return;
	}
	
	// if the dialogue string is still being drawn, display it all
	if (m_StrPos!=m_Dialogue.size() && !m_Dialogue.empty())
		m_StrPos=m_Dialogue.size();
	
	else {
		// reset string position and clear previous formatting
		m_StrPos=0;
		clearFormatting();
		
		m_Pause=false;
	}
}

// parse a tag and apply styling
void TextParser::parseTag(const std::string &tag) {
	// blue color tag -- set blue font
	if (tag=="blue")
		m_FontColor="blue";
}

// clear current font formatting
void TextParser::clearFormatting() {
	m_FontColor="white";
}

// split a command string into pieces based on commas
std::vector<std::string> TextParser::splitCommand(const std::string &command) {
	std::vector<std::string> params;
	
	// for now, there can only be two parameters
	// find the separator comma
	int npos=command.find(",");
	
	// break the string apart
	params.push_back(command.substr(0, npos));
	params.push_back(command.substr(npos+1, command.size()-1));
	
	return params;
}

// execute a trigger
std::string TextParser::doTrigger(const std::string &trigger, const std::string &command) {
	Case::Case *pcase=m_Game->m_Case;
	
	// go to another text block
	if (trigger=="goto")
		m_NextBlock=command;
	
	// go to another text block without pausing
	else if (trigger=="direct_goto") {
		m_NextBlock=command;
		m_Direct=true;
	}
	
	// add evidence to court record
	else if (trigger=="add_evidence") {
		// make sure this evidence exists at all
		if (pcase->getEvidence(command))
			m_Game->m_State.visibleEvidence.push_back(*pcase->getEvidence(command));
	}
	
	// add a profile to court record
	else if (trigger=="add_profile") {
		// make sure this character exists
		if (pcase->getCharacter(command))
			m_Game->m_State.visibleProfiles.push_back(*pcase->getCharacter(command));
	}
	
	// show evidence on screen
	else if (trigger=="show_evidence_left") {
		// given the id, get the actual evidence struct
		if (pcase->getEvidence(command))
			m_Game->setShownEvidence(command, POSITION_LEFT);
	}
	
	// hide shown evidence
	else if (trigger=="hide_evidence")
		m_Game->setShownEvidence("null", POSITION_LEFT);
	
	// set a location
	else if (trigger=="set_location")
		m_Game->setLocation(command);
	
	// make a location accessible
	else if (trigger=="add_location") {
		// split this command string
		std::vector<std::string> params=splitCommand(command);
		std::string target=params[0];
		std::string location=params[1];
		
		// add the target to the location, if the location exists
		if (pcase->getLocation(location)) {
			Case::Location *tloc=pcase->getLocation(location);
			
			// add the target as an id
			tloc->moveLocations.push_back(target);
		}
		
		else
			std::cout << "Warning: trying to add location " << target << " to non existant " << location << " target\n";
	}
	
	// set a trigger block at a location
	else if (trigger=="set_location_trigger") {
		// split this command string
		std::vector<std::string> params=splitCommand(command);
		std::string target=params[0];
		std::string block=params[1];
		
		// get the target location
		if (pcase->getLocation(target)) {
			Case::Location *location=pcase->getLocation(target);
			
			// set the trigger here
			location->triggerBlock=block;
		}
	}
	
	// show a character in this dialog
	else if (trigger=="show_character") {
		// set the character to display for this dialog
		m_Game->m_State.displayChar=command;
	}
	
	// put a character at a location
	else if (trigger=="put_character") {
		// split this command string
		std::vector<std::string> params=splitCommand(command);
		std::string character=params[0];
		std::string target=params[1];
		
		// get the target location
		if (pcase->getLocation(target))
			pcase->getLocation(target)->character=character;
	}
	
	// play a sample of music
	else if (trigger=="play_music")
		Audio::playMusic(command);
	
	// set the current speaker
	else if (trigger=="speaker")
		m_Speaker=command;
	
	// end the current dialog
	else if (trigger=="end_dialogue")
		m_Done=true;
	
	return "null";
}
