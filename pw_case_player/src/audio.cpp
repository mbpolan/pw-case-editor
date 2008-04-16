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
#include "utilities.h"

namespace Audio {
	// global variable the stores whether or not sound is to be outputted
	bool g_Output;
	
	// music currently playing
	Sample g_Music;
}

// handler for music completion
void onMusicFinished() {
}

// load an audio sample from file
bool Audio::loadSample(const ustring &path, Audio::Sample &sample) {
	if (!Audio::g_Output)
		return true;
	
	// see if this is an effect or music
	ustring ext=path.substr(path.size()-3, path.size()-1);
	bool success=true;
	
	// differentiate between separating characters
	char sep;
#ifdef __WIN32__
	sep='\\';
#else
	sep='/';
#endif
	
	// find the basename
	ustring base=path.substr(path.rfind(sep)+1);
	
	// any file prefixed with an s_ is an effect
	if (base[0]=='s' && base[1]=='_') {
		sample.type=SAMPLE_EFFECT;
		sample.effect=Mix_LoadWAV(path.c_str());
		if (!sample.effect)
			success=false;
	}
	
	// otherwise, it's a music sample
	else {
		sample.type=SAMPLE_MUSIC;
		sample.music=path;
	}
	
	// check to see if we loaded it
	if (!success) {
		Utils::alert("Audio: Unable to load sample: '"+path+"'!\nReason: "+Mix_GetError());
		return false;
	}
	
	else
		return true;
}

// play an effect sample
void Audio::playEffect(const ustring &id, EffectChannel channel) {
	if (!Audio::g_Output)
		return;
	
	// get the sample
	Audio::Sample *audio=queryAudio(id);
	if (!audio) {
		Utils::debugMessage("Audio: sample '"+id+"' not found in stack");
		return;
	}
	
	// play this chunk once
	if (audio->type==SAMPLE_EFFECT) {
		Mix_HaltChannel(channel);
		Mix_PlayChannel(channel, audio->effect, 0);
	}
}

// play a music sample
void Audio::playMusic(const ustring &id) {
	if (!Audio::g_Output)
		return;
	
	// get the sample
	Audio::Sample *audio=queryAudio(id);
	if (!audio) {
		Utils::debugMessage("Audio: sample '"+id+"' not found in stack");
		return;
	}
	
	// and play it, if it's a music file
	if (audio->type==SAMPLE_MUSIC) {
		if (Audio::g_Music.mBuffer!=NULL) {
			Mix_HaltMusic();
			Mix_FreeMusic(Audio::g_Music.mBuffer);
		}
		
		// load our new music sample
		Audio::g_Music.mBuffer=Mix_LoadMUS(audio->music.c_str());
		if (!Audio::g_Music.mBuffer) {
			Utils::debugMessage("Audio: unable to load music: '"+audio->music+"'");
			return;
		}
		
		Mix_PlayMusic(Audio::g_Music.mBuffer, -1);
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
	if (Audio::g_Music.mBuffer)
		Mix_FreeMusic(Audio::g_Music.mBuffer);
}

// add an audio sample
void Audio::pushAudio(const ustring &id, const Audio::Sample &audio) {
	if (!Audio::g_Output)
		return;
	
	g_Audio[id]=audio;
}

// remove an audio sample
void Audio::popAudio(const ustring &id) {
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
Audio::Sample* Audio::queryAudio(const ustring &id) {
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
	
	if (Audio::g_Music.mBuffer) {
		Mix_HaltMusic();
		Mix_FreeMusic(Audio::g_Music.mBuffer);
	}
	
	g_Audio.clear();
}
