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
#include "utilities.h"

// constructor
TextParser::TextParser(Game *game): m_Game(game) {
	// reset and prepare the parser
	reset();
	clearFormatting();
	
	// reset variables
	m_SpeakerGender=Character::GENDER_MALE;
	m_Dialogue="";
	m_QueuedFade="null";
	m_QueuedTestimony="null";
	m_QueuedExamination="null";
	m_QueuedResume="null";
	m_QueuedEvent="null";
	m_Direct=false;
	m_TalkLocked=false;
	m_BlockDiag=false;
	m_Speed=50;
	m_TimedGoto=0;
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
	m_TalkLocked=false;
	m_CurTag="";
	m_NextBlock="null";
	
	m_LastChar=0;
}

// parse the given control block
std::string TextParser::parse(bool drawDialogue) {
	// if we are done parsing, return here
	if (m_Done)
		return "null";
	
	// if there is a timed paused, wait it out
	if (m_TimedGoto!=0) {
		m_TimedGoto-=1;
		
		// once we have timed out, proceed to next block
		if (m_TimedGoto==0)
			nextStep();
		
		return "null";
	}
	
	// if the parser is not paused, start parsing the set block
	if (!m_Pause) {
		// erase up to this point
		m_Block.erase(0, m_BreakPoint);
		
		// reset variables
		m_Dialogue="";
		m_BlockDiag=false;
		m_StrPos=1;
		
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
				
				// erase to the end of this trigger
				int end=m_Block.find('}');
				m_Block.erase(0, end);
				
				// if this trigger is to be preparsed, then do so now
				if (!preparseTrigger(trigOp)) {
					// add a hook into the dialogue string
					m_Dialogue+='^';
					
					// and append this trigger
					m_QueuedTriggers.push(std::make_pair<std::string, std::string> (trigOp, trigComm));
				}
				
				else {
					// execute the trigger right away
					doTrigger(trigOp, trigComm);
				}
				
				continue;
			}
			
			// dialogue control
			else if (ch=='\\' && m_Block[1]!='n') {
				bool erase=true;
				
				// check the next ucharacter
				switch(m_Block[1]) {
					// dialogue break
					case 'b': {
						m_Pause=true;
						m_Block.erase(0, 2);
						return "null";
					}; break;
					
					// set blocking flag
					case 'd': {
						m_BlockDiag=true;
					}; break;
					
					// increase speed
					case '+': {
						int multiplier=atoi(Utils::ucharToStr(m_Block[2]).c_str());
						
						for (int i=0; i<multiplier; i++)
							m_Dialogue+=TEXT_SPEED_INCR_CHAR;
						
						m_Block.erase(0, 3);
						erase=false;
					}; break;
					
					// decrease speed
					case '-': {
						int multiplier=atoi(Utils::ucharToStr(m_Block[2]).c_str());
						
						for (int i=0; i<multiplier; i++)
							m_Dialogue+=TEXT_SPEED_DECR_CHAR;
						
						m_Block.erase(0, 3);
						erase=false;
					}; break;
					
					// normalized speed
					case '=': {
						m_Dialogue+=TEXT_SPEED_NORM_CHAR;
					}; break;
				}
				
				// erase this sequence of ucharacters
				if (erase)
					m_Block.erase(0, 2);
			}
			
			// normal ucharacter
			else {
				// make sure useless ucharacters aren't dealt with
				if (ch!='\n') {
					// add this ucharacter to the draw string
					m_Dialogue+=(char) ch;
					
					// see if we are over the text limit and need to break
					if (Fonts::lineWillBreak(8, 134, SDL_GetVideoSurface()->w-8, m_Dialogue, m_FontStyle.color)) {
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
				
				return "null";
			}
			
			// we have completely parsed this block
			if (m_Block.empty()) {
				m_Pause=true;
				return m_NextBlock;
			}
		}
	}
	
	else {
		// shift the drawing of the string based on state
		int shift=0;
		if (m_Game->m_State.requestingEvidence || m_Game->m_State.requestingAnswer)
			shift=-24;
		
		// see if we should draw the next ucharacter in the string
		int now=SDL_GetTicks();
		if (now-m_LastChar>m_FontStyle.speed && m_StrPos<m_Dialogue.size()) {
			// set the last draw time, and increment string position
			m_LastChar=now;
			
			// our current ucharacter
			char curChar=m_Dialogue[m_StrPos-1];
			
			// see if we need to execute a trigger
			if(curChar=='^')
				executeNextTrigger();
			
			// cache the previous and next ucharacters
			char prevChar=(m_StrPos>1 ? m_Dialogue[m_StrPos-2] : '0');
			char nextChar=m_Dialogue[m_StrPos];
			
			// prepare the sound effect to potentially play
			std::string sfx="";
			
			// increase speed
			if (curChar==TEXT_SPEED_INCR_CHAR) {
				m_FontStyle.speed-=10;
				
				// make sure to clamp the value to [0,100]
				if (m_FontStyle.speed<0)
					m_FontStyle.speed=0;
			}
			
			// decrease speed
			else if (curChar==TEXT_SPEED_DECR_CHAR) {
				m_FontStyle.speed+=10;
				
				// make sure to clamp the value to [0,100]
				if (m_FontStyle.speed>100)
					m_FontStyle.speed=100;
			}
			
			// reset speed
			else if (curChar==TEXT_SPEED_NORM_CHAR)
				m_FontStyle.speed=NORMAL_FONT_SPEED;
			
			// date string
			if (m_FontStyle.type=="date")
				sfx="sfx_typewriter";
				
			// differentiate between male and female speakers
			else if (m_FontStyle.type=="plain") {
				if (m_SpeakerGender==Character::GENDER_MALE)
					sfx="sfx_male_talk";
				else
					sfx="sfx_female_talk";
			}
			
			else if (m_FontStyle.type=="testimony-title")
				sfx="sfx_male_talk";
			
			// play a sound effect if this next ucharacter is visible
			if (shouldPlayDialogueEffect(prevChar, curChar, nextChar))
				Audio::playEffect(sfx, Audio::CHANNEL_DIALOGUE);
			
			m_StrPos++;
		}
		
		// if we are formatted to display a date or testimony title, center the string
		if (m_FontStyle.type=="date" || m_FontStyle.type=="testimony-title")
			Fonts::drawStringCentered(134+shift, m_StrPos, m_Dialogue, m_FontStyle.color);
		
		// draw the string in plain formatting otherwise
		else {
			// make the color green for cross examinations
			if (m_Game->m_State.curExamination && !m_Game->m_State.curExaminationPaused)
				m_FontStyle.color="green";
			
			// draw the string
			if (drawDialogue)
				Fonts::drawString(8, 134+shift, m_StrPos, SDL_GetVideoSurface()->w-8, m_Dialogue, m_FontStyle.color);
			
			// since the dialog is done, execute queued events
			if (m_QueuedEvent!="null" && dialogueDone()) {
				// request evidence
				if (m_QueuedEvent=="request_evidence") {
					// set the variable in the game state
					m_Game->m_State.requestingEvidence=true;
					m_Game->m_State.requestedEvidenceParams=m_QueuedEventArgs;
					
					// now, we also need to go directly to the evidence page
					int flags=STATE_TEXT_BOX | STATE_EVIDENCE_PAGE | STATE_PROFILES_PAGE;
					m_Game->m_State.drawFlags=flags;
				}
				
				// request an answer
				else if (m_QueuedEvent=="request_answer") {
					// set the variable in the game state
					m_Game->m_State.requestingAnswer=true;
					
					StringVector vec=Utils::explodeString(')', m_QueuedEventArgs);
					for (int i=0; i<vec.size(); i++) {
						// these strings have extra ucharacters, so erase them first
						int npos=vec[i].find('(');
						if (npos!=-1)
							vec[i].erase(0, npos+1);
						
						// properly formatted strings always have a leading '(' ucharacter
						else
							continue;
						
						// now we have a string in the form of a,b
						// explode it once more to get the talk option name and target
						StringVector talkOp=Utils::explodeString(',', vec[i]);
						
						// add this pair to the state talk op vector
						m_Game->m_State.talkOptions.push_back(std::make_pair<std::string, std::string>(talkOp[0], talkOp[1]));
					}
					
					// now modify flags
					int flags=STATE_TEXT_BOX | STATE_TALK;
					m_Game->m_State.drawFlags=flags;
					
					Audio::playEffect("sfx_return", Audio::CHANNEL_GUI);
				}
				
				// request to point out a contradiction with an evidence image
				else if (m_QueuedEvent=="request_image_contradiction") {
					// parameter format: image_id, rectangle [x, y, w, h], right block, wrong block
					// first, explode the string
					StringVector params=Utils::explodeString(',', m_QueuedEventArgs);
					
					// error check
					if (params.size()!=7)
						Utils::debugMessage("TextParser", "Parameters for request_image_contradiction aren't correct");
					
					else {
						// define our contradiction region
						m_Game->m_State.contradictionRegion=Rect(Point(atoi(params[1].c_str()),
											 atoi(params[2].c_str())),
											 atoi(params[3].c_str()),
											 atoi(params[4].c_str()));
						
						// set our parameters
						m_Game->m_State.requestedContrParams=params[5]+","+params[6];
						
						// and our image
						m_Game->m_State.contradictionImg=params[0];
						
						// finally, flag our state
						m_Game->m_State.drawFlags=STATE_CHECK_EVIDENCE_IMAGE | STATE_CONFIRM_BTN;
					}
				}
				
				m_QueuedEvent="null";
			}
		}
	}
	
	return "null";
}

// move on to the next break point
void TextParser::nextStep() {
	// if this block is empty and we didn't find the next one,
	// then we flag that we're done
	if (m_Block.empty() && m_NextBlock=="null" && m_StrPos==m_Dialogue.size()) {
		// draw the previous screen
		if (m_Game->m_State.prevScreen==SCREEN_EXAMINE)
			m_Game->toggle(STATE_EXAMINE | STATE_COURT_REC_BTN | STATE_BACK_BTN);
		
		else
			m_Game->toggle(STATE_COURT_REC_BTN | STATE_CONTROLS);
		
		// flag that we are done
		m_Done=true;
		m_Pause=false;
		
		// also, make sure to end any talk animations
		m_StrPos=m_Dialogue.size();
		m_Dialogue="";
		m_Speaker="none";
		
		return;
	}
	
	// if the dialogue string is still being drawn, display it all (unless it's a date/location string)
	if (m_StrPos!=m_Dialogue.size() && !m_Dialogue.empty()) {
		// although we skip the dialogue, we must execute all triggers
		for (int i=m_StrPos-1; i<m_Dialogue.size(); i++) {
			if (m_Dialogue[i]=='^')
				executeNextTrigger();
		}
		
		m_StrPos=m_Dialogue.size();
	}
	
	// once again, date/location strings have to be fully drawn
	else {
		// reset string position and clear previous formatting
		m_StrPos=1;
		clearFormatting();
		
		// if we have queued effects, execute them now
		// set fade effect
		if (m_QueuedFade!="null") {
			m_Game->m_State.fadeOut=m_QueuedFade;
			m_QueuedFade="null";
		}
		
		// display testimony from a ucharacter
		if (m_QueuedTestimony!="null") {
			// set the testimony to the game engine
			m_Game->displayTestimony(m_QueuedTestimony, false);
			
			m_QueuedTestimony="null";
		}
		
		// begin a cross examination
		else if (m_QueuedExamination!="null") {
			StringVector params=Utils::explodeString(',', m_QueuedExamination);
			
			// begin the requested cross examination of testimony
			m_Game->displayTestimony(params[0], true);
			
			// set images for lawyers
			m_Game->m_State.crossExamineLawyers.first=params[1];
			m_Game->m_State.crossExamineLawyers.second=params[2];
			
			m_QueuedExamination="null";
		}
		
		// resume a cross examination
		else if (m_QueuedResume!="null") {
			m_Game->m_State.curExaminationPaused=false;
			m_Game->m_State.curTestimonyPiece=atoi(m_QueuedResume.c_str());
			
			m_Game->m_State.fadeOut="top";
			
			// always revert back to witness stand and prepare requested piece
			m_Game->m_State.queuedLocation="witness_stand";
			m_Game->m_State.queuedBlock="INTERNAL_testimony";
			
			m_QueuedResume="null";
		}
		
		m_Pause=false;
	}
}

// see if a dialogue sound effect should be played for a given ucharacter
bool TextParser::shouldPlayDialogueEffect(char prev, char ch, char next) {
	// spaces are never played, nor trigger hooks
	if (ch==' ' || ch=='^')
		return false;
	
	// same applies for other script ucharacters
	else if (ch=='}' || ch=='{')
		return false;
	
	// literal new lines
	else if (ch=='\\' && next=='n')
		return false;
	
	// more literal new lines
	else if (ch=='n' && prev=='\\')
		return false;
	
	return true;
}

// see if a trigger should be executed right away
bool TextParser::preparseTrigger(const std::string &trigger) {
	// in order to keep the script flowing nicely, certain triggers
	// need to be executed before the script it set in motion
	if (trigger=="speaker")
		return true;
	
	else
		return false;
}

// parse a tag and apply styling
void TextParser::parseTag(const std::string &tag) {
	// blue color tag -- set blue font
	if (tag=="blue")
		m_FontStyle.color="blue";
	
	// green color tag -- set green font
	else if (tag=="green")
		m_FontStyle.color="green";
	
	// testimony title - orange font and centered
	else if (tag=="testimony-title") {
		m_FontStyle.type=tag;
		
		m_FontStyle.color="orange";
		
		m_BlockDiag=true;
	}
	
	// date tag - set green font and slow down draw speed
	else if (tag=="date") {
		m_FontStyle.type=tag;
		
		m_FontStyle.color="green";
		m_FontStyle.speed=150;
		
		m_BlockDiag=true;
	}
}

// clear current font formatting
void TextParser::clearFormatting() {
	m_FontStyle.type="plain";
	m_FontStyle.color="white";
	m_FontStyle.speed=NORMAL_FONT_SPEED;
}

void TextParser::executeNextTrigger() {
	if (m_QueuedTriggers.empty())
		return;
	
	// get the next queued trigger
	StringPair trigger=m_QueuedTriggers.front();
	
	// and execute it
	doTrigger(trigger.first, trigger.second);
	
	// get rid of it
	m_QueuedTriggers.pop();
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
	
	// go to another block following a pause
	else if (trigger=="timed_goto") {
		// split this command string
		std::vector<std::string> params=Utils::explodeString(',', command);
		std::string nextBlock=params[0];
		int pause=atoi(params[1].c_str());
		
		// set the timing
		m_TimedGoto=pause;
		m_NextBlock=nextBlock;
		
		m_Direct=true;
	}
	
	// hide the text box for a duration
	else if (trigger=="hide_text_box")
		m_Game->m_State.hideTextBox=true;
	
	// show the text box once more
	else if (trigger=="show_text_box")
		m_Game->m_State.hideTextBox=false;
	
	// add evidence to court record
	else if (trigger=="add_evidence") {
		// make sure this evidence exists at all
		if (pcase->getEvidence(command))
			m_Game->m_State.visibleEvidence.push_back(*pcase->getEvidence(command));
		else
			Utils::debugMessage("TextParser", "Unable to add unknown evidence: "+command);
	}
	
	// add a profile to court record
	else if (trigger=="add_profile") {
		// make sure this ucharacter exists
		if (pcase->getCharacter(command))
			m_Game->m_State.visibleProfiles.push_back(*pcase->getCharacter(command));
		else
			Utils::debugMessage("TextParser", "Unable to add profile for nonexistent ucharacter: "+command);
	}
	
	// show evidence on screen
	else if (trigger=="show_evidence") {
		StringVector params=Utils::explodeString(',', command);
		std::string item=params[0];
		std::string pos=params[1];
		
		// given the id, get the actual evidence struct
		if (pcase->getEvidence(item)) {
			if (pos=="right")
				m_Game->setShownEvidence(item, POSITION_RIGHT);
			else
				m_Game->setShownEvidence(item, POSITION_LEFT);
		}
		
		else
			Utils::debugMessage("TextParser", "Unable to show nonexistent evidence: "+item);
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
		std::vector<std::string> params=Utils::explodeString(',', command);
		std::string target=params[0];
		std::string location=params[1];
		
		// add the target to the location, if the location exists
		if (pcase->getLocation(location)) {
			Case::Location *tloc=pcase->getLocation(location);
			
			// add the target as an id
			tloc->moveLocations.push_back(target);
		}
		
		else
			Utils::debugMessage("TextParser", "Unable to add location '"+target+"' to non existant '"+location+"' target");
	}
	
	// set a trigger block at a location
	else if (trigger=="set_location_trigger") {
		// split this command string
		std::vector<std::string> params=Utils::explodeString(',', command);
		std::string target=params[0];
		std::string block=params[1];
		
		// get the target location
		if (pcase->getLocation(target)) {
			Case::Location *location=pcase->getLocation(target);
			
			// set the trigger here
			location->triggerBlock=block;
		}
		
		else
			Utils::debugMessage("TextParser", "Unable to set trigger in nonexistent location: "+target);
	}
	
	// set a ucharacter's animation
	else if (trigger=="set_animation") {
		// split this command string
		std::vector<std::string> params=Utils::explodeString(',', command);
		std::string ucharName=params[0];
		std::string anim=params[1];
		
		// get the ucharacter
		Character *ucharacter=pcase->getCharacter(ucharName);
		if (ucharacter)
			ucharacter->setRootAnimation(anim);
		else
			Utils::debugMessage("TextParser", "Trying to clear presentables for nonexistent ucharacter: "+command);
	}
	
	// put a ucharacter at a location
	else if (trigger=="put_ucharacter") {
		// split this command string
		std::vector<std::string> params=Utils::explodeString(',', command);
		std::string ucharacter=params[0];
		std::string target=params[1];
		
		// get the target location
		if (pcase->getLocation(target))
			pcase->getLocation(target)->ucharacter=ucharacter;
		else
			Utils::debugMessage("TextParser", "Unable to put ucharacter '"+ucharacter+"' at nonexistant location: "+target);
	}
	
	// add a talk option to a ucharacter
	else if (trigger=="add_talk_option") {
		// split this command string
		std::vector<std::string> params=Utils::explodeString(',', command);
		std::string ucharName=params[0];
		std::string viewString=params[1];
		std::string blockId=params[2];
		
		// get the target ucharacter
		Character *ucharacter=pcase->getCharacter(ucharName);
		if (ucharacter)
			ucharacter->addTalkOption(viewString, blockId);
		else
			Utils::debugMessage("TextParser", "Unable to add talk option '"+viewString+"' for nonexistent ucharacter: "+ucharName);
	}
	
	// remove a talk option
	else if (trigger=="remove_talk_option") {
		// split this command string
		std::vector<std::string> params=Utils::explodeString(',', command);
		std::string ucharName=params[0];
		std::string viewString=params[1];
		
		// get the target ucharacter
		Character *ucharacter=pcase->getCharacter(ucharName);
		if (ucharacter)
			ucharacter->removeTalkOption(viewString);
		else
			Utils::debugMessage("TextParser", "Unable to remove talk option '"+viewString+"' for nonexistent ucharacter: "+ucharName);
	}
	
	// clear talk options
	else if (trigger=="clear_talk_options") {
		// get ucharacter
		Character *ucharacter=pcase->getCharacter(command);
		if (ucharacter)
			ucharacter->clearTalkOptions();
		else
			Utils::debugMessage("TextParser", "Unable to clear talk options for nonexistent ucharacter: "+command);
	}
	
	// add a presentable piece of evidence/profile to a ucharacter
	else if (trigger=="add_presentable") {
		// split this command string
		std::vector<std::string> params=Utils::explodeString(',', command);
		std::string ucharName=params[0];
		std::string itemId=params[1];
		std::string targetBlock=params[2];
		
		// get the ucharacter
		Character *ucharacter=pcase->getCharacter(ucharName);
		if (ucharacter)
			ucharacter->addPresentable(itemId, targetBlock);
		else
			Utils::debugMessage("TextParser", "Unable to add presentable '"+itemId+"' for nonexistent ucharacter: "+ucharName);
	}
	
	// remove a piece of presentable evidence/profile
	else if (trigger=="remove_presentable") {
		// split this command string
		std::vector<std::string> params=Utils::explodeString(',', command);
		std::string ucharName=params[0];
		std::string itemId=params[1];
		
		// get the ucharacter and remove presentable item
		Character *ucharacter=pcase->getCharacter(ucharName);
		if (ucharacter)
			ucharacter->removePresentable(itemId);
		else
			Utils::debugMessage("TextParser", "Unable to remove presentables for nonexistent ucharacter: "+ucharName);
	}
	
	// clear ucharacter's presentable items
	else if (trigger=="clear_presentables") {
		// get the ucharacter requested
		Character *ucharacter=pcase->getCharacter(command);
		if (ucharacter)
			ucharacter->clearPresentableItems();
		else
			Utils::debugMessage("TextParser", "Unable to clear presentables for nonexistent ucharacter: "+command);
	}
	
	// set the block to use when a useless item was presented
	else if (trigger=="set_bad_presentable_block") {
		// split this command string
		std::vector<std::string> params=Utils::explodeString(',', command);
		std::string ucharName=params[0];
		std::string target=params[1];
		
		// get the ucharacter
		Character *ucharacter=pcase->getCharacter(ucharName);
		if (ucharacter)
			ucharacter->setBadPresentableBlock(target);
		else
			Utils::debugMessage("TextParser", "Unable to set bad presentable block for nonexistent ucharacter: "+ucharName);
	}
	
	// set music to be played a location
	else if (trigger=="set_location_music") {
		// split this command string
		std::vector<std::string> params=Utils::explodeString(',', command);
		std::string musicId=params[0];
		std::string target=params[1];
		
		// if the court was requested, set all court locations to play the same music
		if (target=="court" || target=="prosecutor_stand" || target=="defense_stand" ||
		    target=="defense_helper_stand" || target=="witness_stand" || target=="courtroom" ||
		    target=="judge_stand") {
			m_Game->m_State.continueMusic=true;
			pcase->getLocation("prosecutor_stand")->music=musicId;
			pcase->getLocation("defense_stand")->music=musicId;
			pcase->getLocation("defense_helper_stand")->music=musicId;
			pcase->getLocation("witness_stand")->music=musicId;
			pcase->getLocation("courtroom")->music=musicId;
			pcase->getLocation("judge_stand")->music=musicId;
		}
		
		// get the target location
		else if (pcase->getLocation(target))
			pcase->getLocation(target)->music=musicId;
		
		else
			Utils::debugMessage("TextParser", "Unable to set music "+musicId+" at unknown location: "+target);
	}
	
	// clear any music set at a location
	else if (trigger=="clear_location_music") {
		// get the target location
		if (pcase->getLocation(command))
			pcase->getLocation(command)->music="null";
	}
	
	// flag that requires player to present evidence or answer
	else if (trigger=="request_evidence" || trigger=="request_answer" || trigger=="request_image_contradiction") {
		m_QueuedEvent=trigger;
		m_QueuedEventArgs=command;
	}
	
	// play a sample of music
	else if (trigger=="play_music")
		Audio::playMusic(command);
	
	// halt the current music
	else if (trigger=="halt_music")
		Audio::haltMusic();
	
	// play a sound effect
	else if (trigger=="sfx") {
		// always play sound effects on their own channel
		Audio::playEffect(command, Audio::CHANNEL_SCRIPT);
	}
	
	// schedule a fade effect with given speed
	else if (trigger=="fade_out") {
		// queue this fade effect
		m_QueuedFade=command;
	}
	
	// schedule a flash effect
	else if (trigger=="flash")
		m_Game->m_State.flash="top";
	
	// schedule a special effect
	else if (trigger=="special_effect") {
		if (command=="gavel") {
			// make sure to reset the animation
			pcase->getCharacter("gavel")->getSprite()->reset();
			pcase->getCharacter("gavel")->getSprite()->setAnimation("normal_idle");
			
			// and schedule the event
			m_Game->m_State.gavel="1";
		}
	}
	
	// schedule a court camera event
	else if (trigger=="move_court_camera")
		m_Game->m_State.courtCamera=command;
	
	// set the current speaker
	else if (trigger=="speaker") {
		// copy speaker id
		m_Speaker=command;
		
		// get ucharacter gender
		if (pcase->getCharacter(m_Speaker))
			m_SpeakerGender=pcase->getCharacter(m_Speaker)->getGender();
	}
	
	// set images for courtroom overview
	else if (trigger=="set_court_overview_image") {
		// split the command string
		StringVector params=Utils::explodeString(',', command);
		std::string area=params[0];
		std::string image=params[1];
		
		// depending on area, set image
		if (area=="defense")
			m_Game->m_State.crOverviewDefense=image;
		else if (area=="prosecutor")
			m_Game->m_State.crOverviewProsecutor=image;
		else if (area=="witness")
			m_Game->m_State.crOverviewWitness=image;
		else
			Utils::debugMessage("TextParser", "No such area in courtroom overview: '"+area);
	}
	
	// set an image to be displayed over location background
	else if (trigger=="set_temp_image")
		m_Game->m_State.tempImage=command;
	
	// hide a set temporary image
	else if (trigger=="hide_temp_image")
		m_Game->m_State.tempImage="null";
	
	// display a testimony
	else if (trigger=="display_testimony") {
		m_QueuedTestimony=command;
		
		// preset a new block and start testimony directly after this block
		m_NextBlock="INTERNAL_testimony";
		m_Direct=true;
	}
	
	// begin cross examining a witness
	else if (trigger=="cross_examine") {
		m_QueuedExamination=command;
		
		// present a new block, and start cross examination afterwards
		m_NextBlock="INTERNAL_cross_examination";
		m_Direct=true;
	}
	
	// resume a cross examination
	else if (trigger=="resume_cross_examination") {
		m_QueuedResume=command;
		
		// go to a blank block to resume
		doTrigger("goto", "INTERNAL_blank");
	}
	
	return "null";
}
