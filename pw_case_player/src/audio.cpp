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
// audio.cpp: implementation of Audio namespace

#include "SDL_mixer.h"

#include "audio.h"

namespace Audio {
	// global variable the stores whether or not sound is to be outputted
	bool g_Output;
	
	// music currently playing
	Mix_Music *g_Music=NULL;
}

// load an audio sample from file
bool Audio::loadSample(const std::string &path, Audio::Sample &sample) {
	if (!Audio::g_Output)
		return true;
	
	// see if this is an effect or music
	std::string ext=path.substr(path.size()-3, path.size()-1);
	bool success=true;
	
	// music is considered when mp3 or ogg is loaded
	if (ext=="mp3" || ext=="ogg") {
		sample.type=SAMPLE_MUSIC;
		sample.music=path;
	}
	
	// otherwise, it's an effect
	else {
		sample.type=SAMPLE_EFFECT;
		sample.effect=Mix_LoadWAV(path.c_str());
		if (!sample.effect)
			success=false;
	}
	
	// check to see if we loaded it
	if (!success) {
		std::cout << "Audio: Unable to load sample: " << path << "\nReason: " << Mix_GetError() << std::endl;
		return false;
	}
	
	else
		return true;
}

// play an effect sample
void Audio::playEffect(const std::string &id, EffectChannel channel) {
	if (!Audio::g_Output)
		return;
	
	// get the sample
	Audio::Sample *audio=queryAudio(id);
	if (!audio) {
		std::cout << "Audio: sample " << id << " not found in stack\n";
		return;
	}
	
	// play this chunk once
	if (audio->type==SAMPLE_EFFECT) {
		Mix_HaltChannel(channel);
		Mix_PlayChannel(channel, audio->effect, 0);
	}
}

// play a music sample
void Audio::playMusic(const std::string &id) {
	if (!Audio::g_Output)
		return;
	
	// get the sample
	Audio::Sample *audio=queryAudio(id);
	if (!audio) {
		std::cout << "Audio: sample " << id << " not found in stack\n";
		return;
	}
	
	// and play it, if it's a music file
	if (audio->type==SAMPLE_MUSIC) {
		if (Audio::g_Music!=NULL) {
			Mix_HaltMusic();
			Mix_FreeMusic(Audio::g_Music);
			
			// always null out the pointer
			Audio::g_Music=NULL;
		}
		
		// load our new music sample
		Audio::g_Music=Mix_LoadMUS(audio->music.c_str());
		if (!Audio::g_Music) {
			std::cout << "Audio: unable to load music: " << audio->music << std::endl;
			return;
		}
		
		Mix_PlayMusic(Audio::g_Music, -1);
	}
}

// see if music is playing
bool Audio::isMusicPlaying() {
	if (!Audio::g_Output)
		return false;
	
	return Mix_PlayingMusic();
}

// halt music playback
void Audio::haltMusic() {
	if (!Audio::g_Output)
		return;
	
	// first, halt playback
	Mix_HaltMusic();
	
	// then free any previous music
	if (Audio::g_Music) {
		Mix_FreeMusic(Audio::g_Music);
		Audio::g_Music=NULL;
	}
}

// add an audio sample
void Audio::pushAudio(const std::string &id, const Audio::Sample &audio) {
	if (!Audio::g_Output)
		return;
	
	g_Audio[id]=audio;
}

// remove an audio sample
void Audio::popAudio(const std::string &id) {
	if (!Audio::g_Output)
		return;
	
	if (queryAudio(id)) {
		Audio::Sample *audio=queryAudio(id);
		
		// free the music chunk
		if (audio->type==SAMPLE_EFFECT)
			Mix_FreeChunk(audio->effect);
		
		// remove it from the map
		g_Audio.erase(audio->id);
	}
}

// query an audio sample
Audio::Sample* Audio::queryAudio(const std::string &id) {
	if (!Audio::g_Output)
		return NULL;
	
	if (g_Audio.find(id)==g_Audio.end())
		return NULL;
	else
		return &g_Audio[id];
}

// clear the audio stack
void Audio::clearAudioStack() {
	if (!Audio::g_Output)
		return;
	
	// iterate over audio stack
	for (AudioMap::iterator it=g_Audio.begin(); it!=g_Audio.end(); ++it) {
		// free the effect
		if ((*it).second.type==SAMPLE_EFFECT) {
			if ((*it).second.effect)
				Mix_FreeChunk((*it).second.effect);
		}
	}
	
	if (Audio::g_Music) {
		Mix_HaltMusic();
		Mix_FreeMusic(Audio::g_Music);
	}
	
	g_Audio.clear();
}
