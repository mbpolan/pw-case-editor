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
#include <map>

#include "common.h"

class Game;
class ValueRange;

/// Struct that stores the current font style
struct _FontStyle {
	ustring type;
	Color color;
	int speed;
	ColorRangeVector colors;
};
typedef struct _FontStyle FontStyle;

/** The parser and trigger handler for the game script.
  * The TextParser class forms the foundation of the entire game engine. It 
  * handles text blocks, extracts and draws dialogue strings with correct timing, 
  * and executes triggers.
*/
class TextParser {
	public:
		/// Filters for trigger classification
		enum Filter { FILTER_NONE=0,		///< Don't filter anything
			      FILTER_CROSS_EXAMINE	///< Remove any trigger that shouldn't show up in cross examination
			    };
		
		// constants
		static const int NORMAL_FONT_SPEED=50;		///< Regular text speed
		static const uchar TEXT_SPEED_INCR_CHAR='[';	///< Character to increase text speed
		static const uchar TEXT_SPEED_DECR_CHAR=']';	///< Character to decrease text speed
		static const uchar TEXT_SPEED_NORM_CHAR='|';	///< Character to normalize text speed
		static const uchar SHAKE_SCREEN_CHAR='#';	///< Character to shake the screen
		
		/** Constructor
		  * \param game Pointer to the Game engine object
		*/
		TextParser(Game *game);
		
		/** Set a text block to parse
		  * \param block The block to parse
		*/
		void setBlock(const ustring &block);
		
		/// Reset the parser
		void reset();
		
		/** See if parsing the block has paused
		  * \return <b>true</b> if done, <b>false</b> otherwise
		*/
		bool paused() const { return m_Pause; }
		
		/** See if this parser is done with the current block
		  * \return <b>true</b> if done, <b>false</b> otherwise
		*/
		bool done() const { return m_Done; }
		
		/** See if this block's dialogue can't be skipped
		  * \return <b>true</b> if dialogue can be skipped, <b>false</b> otherwise
		*/
		bool isBlocking() const { return m_BlockDiag; }
		
		/** See if the current dialogue string is still being drawn
		  * \return <b>true</b> if done, <b>false</b> otherwise
		*/
		bool dialogueDone() const { return (m_StrPos==m_Dialogue.size()); }
		
		/** Manually lock/unlock the speaking character's _talk animation
		  * \param t <b>true</b> to lock the animation, <b>false</b> to unlock
		*/
		void lockTalk(bool t) { m_TalkLocked=t; };
		
		/** See if the script requests that dialogue not be spoken (disable _talk animation)
		  * \return <b>true</b> if the animation should be disabled, <b>false</b> otherwise
		  * \see TextParser::lockTalk()
		*/
		bool talkLocked() const { return m_TalkLocked; }
		
		/** Manually set the speaker
		  * \param speaker ID of the speaking character
		*/
		void setSpeaker(const ustring &speaker) { m_Speaker=speaker; }
		
		/** Return the currently speaking character, if any
		  * \return The speaking character
		*/
		ustring getSpeaker() const { return m_Speaker; }
		
		/** Parse the given control block
		  * \param drawDialogue Flag whether or not to draw the dialogue text
		  * \return ID of next block to parse, STR_NULL if no other block is to follow
		*/
		ustring parse(bool drawDialogue);
		
		/// Move on to the next break point
		void nextStep();
		
	private:
		/** See if a dialogue sound effect should be played for a given character
		  * \param prev The previous character
		  * \param ch The current character
		  * \param next The next character
		  * \return <b>true</b> is a sound effect should be played, <b>false</b> otherwise
		*/
		bool shouldPlayDialogueEffect(uchar prev, uchar ch, uchar next);
		
		/** See if a trigger should be executed right away
		  * \param trigger The trigger to test
		  * \return <b>true</b> to execute immediately, <b>false</b> otherwise
		*/
		bool preparseTrigger(const ustring &trigger);
		
		/** See if a trigger matches a filter
		  * \param trigger The trigger to test
		  * \param filter The filter to test against
		  * \return <b>true</b> if the trigger should be filtered, <b>false</b> otherwise
		*/
		bool filterTrigger(const ustring &trigger, const Filter &filter);
		
		/** Parse a tag and apply styling
		  * \param tag The tag to handle
		*/
		void parseTag(const ustring &tag);
		
		/// Clear current font formatting
		void clearFormatting();
		
		/// Execute the next scheduled trigger
		void executeNextTrigger();
		
		/** Execute a trigger
		  * \param trigger The trigger to execute
		  * \param command The trigger parameters
		  * \return The return value of a trigger
		*/
		ustring doTrigger(const ustring &trigger, const ustring &command);
		
		/// The current block
		ustring m_Block;
		
		/// The next block to parse
		ustring m_NextBlock;
		
		/// Internal name of current speaking character
		ustring m_Speaker;
		
		/// Gender of the current speaking character
		Character::Gender m_SpeakerGender;
		
		/// Position in block where parser was paused
		int m_BreakPoint;
		
		/// Whether or not the parser is paused
		bool m_Pause;
		
		/// Whether or not the parser is done with the current block
		bool m_Done;
		
		/// Flag if the parser should go right to the next block
		bool m_Direct;
		
		/// Flag if the dialogue should be blocked from being skipped
		bool m_BlockDiag;
		
		/// Flag if the speaking character's _talk animation should be disabled
		bool m_TalkLocked;
		
		/// Current dialogue string data
		ustring m_Dialogue;
		
		/// Position in current dialogue
		int m_StrPos;
		
		/// The last time the previous character was drawn
		int m_LastChar;
		
		/// Speed of font drawing
		int m_Speed;
		
		/// Flag whether or not the a styling tag is open
		bool m_TagOpen;
		
		/// The current styling tag, if any
		ustring m_CurTag;
		
		/// Current font formatting
		FontStyle m_FontStyle;
		
		/// Triggers to be executed in a linear manner
		std::queue<StringPair> m_QueuedTriggers;
		
		/// Scheduled fade out
		ustring m_QueuedFade;
		
		/// Scheduled testimony sprite sequence
		ustring m_QueuedTestimony;
		
		/// Scheduled cross examination sprite sequence
		ustring m_QueuedExamination;
		
		/// Scheduled cross examination to resume
		ustring m_QueuedResume;
		
		//@{
		/** Generic scheduled events to perform */
		ustring m_QueuedEvent;
		ustring m_QueuedEventArgs;
		//@}
		
		/// Wait time before parsing block
		int m_TimedGoto;
		
		/// Pointer to Game engine object
		Game *m_Game;
};

#endif
