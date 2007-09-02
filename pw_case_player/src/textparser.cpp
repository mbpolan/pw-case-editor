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

#include "font.h"
#include "game.h"
#include "textparser.h"

// constructor
TextParser::TextParser(Game *game): m_Game(game) {
	reset();
	m_Direct=false;
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
}

// parse the given control block
std::string TextParser::parse() {
	static std::string str="";
	
	if (m_Done)
		return "end";
	
	if (!m_Pause) {
		// erase up to this point
		m_Block.erase(0, m_BreakPoint);
		
		// reset draw string
		str="";
		
		// start going over block
		while(1) {
			char ch=m_Block[0];
			
			// tag delimiter
			if (ch=='<') {
				// if a tag is already open, this one must close it
				if (m_TagOpen) {
					int npos=m_Block.find(">");
					m_Block.erase(0, npos+1);
					
					m_TagOpen=false;
				}
				
				else {
					int npos=m_Block.find(">");
					m_CurTag=m_Block.substr(0, npos);
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
					str+=(char) ch;
					
					// see if we are over the text limit and need to break
					if (Fonts::willBreak(4, 132, SDL_GetVideoSurface()->w, str, "white")) {
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
		// draw the string
		Fonts::drawString(4, 132, 100, SDL_GetVideoSurface()->w, str, "white");
	}
	
	return "pause";
}

// move on to the next break point
void TextParser::nextStep() {
	// if this block is empty and we didn't find the next one,
	// then we flag that we're done
	if (m_Block.empty() && m_NextBlock=="null") {
		// draw the previous screen
		if (m_Game->m_State.prevScreen==SCREEN_EXAMINE)
			m_Game->toggle(STATE_EXAMINE | STATE_COURT_REC_BTN | STATE_LOWER_BAR | STATE_BACK_BTN);
		
		else
			m_Game->toggle(STATE_COURT_REC_BTN | STATE_CONTROLS);
		
		// flag that we are done
		m_Done=true;
	}
	
	m_Pause=false;
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
	else if (trigger=="location")
		m_Game->setLocation(command);
	
	// make a location accessible
	else if (trigger=="add_location") {
		// split this command string
		std::vector<std::string> params=splitCommand(command);
		std::string target=params[0];
		std::string location=params[1];
		
		// add this location to the target, if the target exists
		if (pcase->getLocation(target)) {
			Case::Location *tloc=pcase->getLocation(target);
			
			// add the target as an id
			tloc->moveLocations.push_back(location);
		}
		
		else
			std::cout << "Warning: trying to add location " << location << " to non existant " << target << " target\n";
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
	
	// set the current speaker
	else if (trigger=="speaker")
		m_Speaker=command;
	
	// end the current dialog
	else if (trigger=="end_dialogue")
		m_Done=true;
	
	return "null";
}
