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
// textparser.h: the TextParser class

#ifndef TEXTPARSER_H
#define TEXTPARSER_H

#include <iostream>
#include <queue>

class Game;

// struct that stores the current font style
struct _FontStyle {
	std::string type; // built-in types: date, etc
	std::string color;
	int speed;
};
typedef struct _FontStyle FontStyle;

// class that parsers text blocks and executes them
class TextParser {
	public:
		// constants
		static const int NORMAL_FONT_SPEED=50;
		
		// constructor
		TextParser(Game *game);
		
		// set a text block to focus on
		void setBlock(const std::string &block);
		
		// reset the parser
		void reset();
		
		// see if parsing the block has paused
		bool paused() const { return m_Pause; }
		
		// see if this parser is done
		bool done() const { return m_Done; }
		
		// see if this block's dialogue can't be skipped
		bool isBlocking() const { return m_BlockDiag; }
		
		// see if the current dialogue string is still be drawn
		bool dialogueDone() const { return (m_StrPos==m_Dialogue.size()); }
		
		// manually lock/unlock the _talk animation
		void lockTalk(bool t) { m_TalkLocked=t; };
		
		// see if the script requests that dialogue not be spoken (disable _talk animation)
		bool talkLocked() const { return m_TalkLocked; }
		
		// manually set the speaker
		void setSpeaker(const std::string &speaker) { m_Speaker=speaker; }
		
		// return the currently speaking character, if any
		std::string getSpeaker() const { return m_Speaker; }
		
		// parse the given control block
		// returns: next text block to go to, std::string::null if this one is not done
		// being parsed
		std::string parse();
		
		// move on to the next break point
		void nextStep();
		
	private:
		// see if a dialogue sound effect should be played for a given character
		bool shouldPlayDialogueEffect(char prev, char ch, char next);
		
		// parse a tag and apply styling
		void parseTag(const std::string &tag);
		
		// clear current font formatting
		void clearFormatting();
		
		// execute the next trigger
		void executeNextTrigger();
		
		// execute a trigger
		std::string doTrigger(const std::string &trigger, const std::string &command);
		
		// current block
		std::string m_Block;
		std::string m_NextBlock;
		
		// current speaking character (internal name)
		std::string m_Speaker;
		Character::Gender m_SpeakerGender;
		
		// position in block where execution stopped
		int m_BreakPoint;
		bool m_Pause;
		bool m_Done;
		bool m_Direct; // flag if the parser should go right to the next block
		bool m_BlockDiag; // flag if the dialogue should be blocked from being skipped
		bool m_TalkLocked;
		
		// current dialog string data
		std::string m_Dialogue;
		int m_StrPos; // position in current dialogue
		int m_LastChar; // last time the character was drawn
		int m_Speed; // speed of font drawing
		
		// is a styling tag open?
		bool m_TagOpen;
		std::string m_CurTag;
		
		// font formatting
		FontStyle m_FontStyle;
		
		// triggers to take place
		std::queue<StringPair> m_QueuedTriggers;
		
		// queued special effects or specific end of text events
		std::string m_QueuedFade;
		std::string m_QueuedTestimony;
		
		// generic events to perform
		std::string m_QueuedEvent;
		std::string m_QueuedEventArgs;
		
		// wait times before parsing block
		int m_TimedGoto;
		
		// game engine pointer
		Game *m_Game;
};

#endif
