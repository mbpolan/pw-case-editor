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
// audio.h: various Audio related functions

#ifndef AUDIO_H
#define AUDIO_H

#include <iostream>
#include <map>
#include "SDL_mixer.h"

#include "case.h"

/// Namespace for audio sample output and management
namespace Audio {

/// Channels for sound effects
enum EffectChannel { CHANNEL_DIALOGUE=0, CHANNEL_SCRIPT, CHANNEL_GUI };

/// Types of audio samples
enum SampleType { SAMPLE_EFFECT, SAMPLE_MUSIC };

/// Audio struct representing music or sound effect
struct _Sample {
	ustring id; ///< ID referenced from within the script
	SampleType type; ///< The type of sample
	
	Mix_Chunk *effect; ///< The effect to play
	ustring music; ///< The music sample to play
};
typedef struct _Sample Sample;

/// Flag whether or not to output sound
extern bool g_Output;

/// Typedef'd map for audio samples
typedef std::map<ustring, Sample> AudioMap;

/// Internal map of audio
static AudioMap g_Audio;

/// Typedef'd map of sound effects
typedef std::map<int, Mix_Chunk*> ChannelMap;

/// Internal map of sound effects
static ChannelMap Channels;

/// The music currently playing
extern Mix_Music *g_Music;

/** Load an audio sample from file
  * \param path The path to the audio sample
  * \param sample The audio sample object to load the data into
  * \return <b>true</b> if there were no errors, <b>false</b> otherwise
*/
bool loadSample(const ustring &path, Sample &sample);

/** Play a sound effect sample
  * \param id The ID of the sample
  * \param channel The channel to play the sound effect on
*/
void playEffect(const ustring &id, EffectChannel channel);

/** Play a music sample
  * \param id The ID of the sample
*/
void playMusic(const ustring &id);

/** See if music is playing
  * \return <b>true</b> if music is playing, <b>false</b> otherwise
*/
bool isMusicPlaying();

/// Halt music playback
void haltMusic();

/** Add an audio sample to the internal map
  * \param id The ID of the audio sample
  * \param sample The audio sample to add
*/
void pushAudio(const ustring &id, const Sample &sample);

/** Remove an audio sample from the internal map
  * \param id The ID of the sample to remove
*/
void popAudio(const ustring &id);

/** Get an audio sample from the internal map
  * \param id The ID of the sample to get
  * \return Pointer to the requested audio sample
*/
Sample* queryAudio(const ustring &id);

/// Clear the audio stack
void clearAudioStack();

}; // namespace Audio

#endif
