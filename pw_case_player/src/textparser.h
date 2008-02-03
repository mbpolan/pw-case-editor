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
	ustring type; // built-in types: date, etc
	Color color;
	int speed;
};
typedef struct _FontStyle FontStyle;

// class that parsers text blocks and executes them
class TextParser {
	public:
		// filters for trigger classification
		enum Filter { FILTER_NONE=0,		// don't filter anything
			      FILTER_CROSS_EXAMINE	// any trigger that shouldn't show up in cross examination
			    };
		
		// constants
		static const int NORMAL_FONT_SPEED=50;
		static const uchar TEXT_SPEED_INCR_CHAR='[';
		static const uchar TEXT_SPEED_DECR_CHAR=']';
		static const uchar TEXT_SPEED_NORM_CHAR='|';
		static const uchar SHAKE_SCREEN_CHAR='#';
		
		// constructor
		TextParser(Game *game);
		
		// set a text block to focus on
		void setBlock(const ustring &block);
		
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
		void setSpeaker(const ustring &speaker) { m_Speaker=speaker; }
		
		// return the currently speaking character, if any
		ustring getSpeaker() const { return m_Speaker; }
		
		// parse the given control block
		// returns: next text block to go to, ustring::null if this one is not done
		// being parsed
		ustring parse(bool drawDialogue);
		
		// move on to the next break point
		void nextStep();
		
	private:
		// see if a dialogue sound effect should be played for a given character
		bool shouldPlayDialogueEffect(uchar prev, uchar ch, uchar next);
		
		// see if a trigger should be executed right away
		bool preparseTrigger(const ustring &trigger);
		
		// see if a trigger matches a filter
		bool filterTrigger(const ustring &trigger, const Filter &filter);
		
		// parse a tag and apply styling
		void parseTag(const ustring &tag);
		
		// clear current font formatting
		void clearFormatting();
		
		// execute the next trigger
		void executeNextTrigger();
		
		// execute a trigger
		ustring doTrigger(const ustring &trigger, const ustring &command);
		
		// current block
		ustring m_Block;
		ustring m_NextBlock;
		
		// current speaking character (internal name)
		ustring m_Speaker;
		Character::Gender m_SpeakerGender;
		
		// position in block where execution stopped
		int m_BreakPoint;
		bool m_Pause;
		bool m_Done;
		bool m_Direct; // flag if the parser should go right to the next block
		bool m_BlockDiag; // flag if the dialogue should be blocked from being skipped
		bool m_TalkLocked;
		
		// current dialog string data
		ustring m_Dialogue;
		int m_StrPos; // position in current dialogue
		int m_LastChar; // last time the character was drawn
		int m_Speed; // speed of font drawing
		
		// is a styling tag open?
		bool m_TagOpen;
		ustring m_CurTag;
		
		// font formatting
		FontStyle m_FontStyle;
		
		// triggers to take place
		std::queue<StringPair> m_QueuedTriggers;
		
		// queued special effects or specific end of text events
		ustring m_QueuedFade;
		ustring m_QueuedTestimony;
		ustring m_QueuedExamination;
		ustring m_QueuedResume;
		
		// generic events to perform
		ustring m_QueuedEvent;
		ustring m_QueuedEventArgs;
		
		// wait times before parsing block
		int m_TimedGoto;
		
		// game engine pointer
		Game *m_Game;
};

#endif
