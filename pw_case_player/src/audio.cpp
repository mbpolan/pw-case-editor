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

// load an audio sample from file
bool Audio::loadSample(const std::string &path, Audio::Sample &sample) {
	// see if this is an effect or music
	std::string ext=path.substr(path.size()-3, path.size()-1);
	bool success=true;
	
	// music is considered when mp3 or ogg is loaded
	if (ext=="mp3" || ext=="ogg") {
		sample.type=SAMPLE_MUSIC;
		sample.music=Mix_LoadMUS(path.c_str());
		if (!sample.music)
			success=false;
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
void Audio::playEffect(const std::string &id, int channel) {
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
	// get the sample
	Audio::Sample *audio=queryAudio(id);
	if (!audio) {
		std::cout << "Audio: sample " << id << " not found in stack\n";
		return;
	}
	
	// and play it, if it's a music file
	if (audio->type==SAMPLE_MUSIC)
		Mix_PlayMusic(audio->music, -1);
}

// halt music playback
void Audio::haltMusic() {
	Mix_HaltMusic();
}

// add an audio sample
void Audio::pushAudio(const std::string &id, const Audio::Sample &audio) {
	g_Audio[id]=audio;
}

// remove an audio sample
void Audio::popAudio(const std::string &id) {
	if (queryAudio(id)) {
		Audio::Sample *audio=queryAudio(id);
		
		// free the music chunk
		if (audio->type==SAMPLE_MUSIC)
			Mix_FreeMusic(audio->music);
		else
			Mix_FreeChunk(audio->effect);
		
		// remove it from the map
		g_Audio.erase(audio->id);
	}
}

// query an audio sample
Audio::Sample* Audio::queryAudio(const std::string &id) {
	if (g_Audio.find(id)==g_Audio.end())
		return NULL;
	else
		return &g_Audio[id];
}

// clear the audio stack
void Audio::clearAudioStack() {
	// iterate over audio stack
	for (AudioMap::iterator it=g_Audio.begin(); it!=g_Audio.end(); ++it) {
		// free the music chunk
		if ((*it).second.type==SAMPLE_MUSIC) {
			if ((*it).second.music)
				Mix_FreeMusic((*it).second.music);
		}
		
		else {
			if ((*it).second.effect)
				Mix_FreeChunk((*it).second.effect);
		}
	}
	
	g_Audio.clear();
}
