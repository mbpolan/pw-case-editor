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

namespace Audio {

// channels for sound effects
enum EffectChannel { CHANNEL_ANY=-1, CHANNEL_DIALOGUE=0, CHANNEL_SCRIPT, CHANNEL_GUI };

// types of samples
enum SampleType { SAMPLE_EFFECT, SAMPLE_MUSIC };

// audio struct representing music or effect
struct _Sample {
	std::string id; // id referenced from within the script
	SampleType type; // the type of sample
	
	Mix_Chunk *effect; // the effect to play
	Mix_Music *music; // the music sample to play
};
typedef struct _Sample Sample;

// flag whether or not to output sound
extern bool g_Output;

// map of audio
typedef std::map<std::string, Sample> AudioMap;
static AudioMap g_Audio;

// load an audio sample from file
bool loadSample(const std::string &path, Sample &sample);

// play an effect sample
void playEffect(const std::string &id, EffectChannel channel=CHANNEL_ANY);

// play a music sample
void playMusic(const std::string &id);

// see if music is playing
bool isMusicPlaying();

// halt music playback
void haltMusic();

// add an audio sample
void pushAudio(const std::string &id, const Sample &sample);

// remove an audio sample
void popAudio(const std::string &id);

// query an audio sample
Sample* queryAudio(const std::string &id);

// clear the audio stack
void clearAudioStack();

}; // namespace Audio

#endif
