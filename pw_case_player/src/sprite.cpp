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
// sprite.cpp: implementation of Sprite class

#include "audio.h"
#include "renderer.h"
#include "sprite.h"
#include "textparser.h"

// constructor
Sprite::Sprite() {
	reset();
}

// toggle loop animation
void Sprite::toggleLoop(bool b) {
	// set looping for current animation
	if (getAnimation(m_CurAnim))
		getAnimation(m_CurAnim)->loop=b;
}

// see if looping is enabled
bool Sprite::loop() {
	if (!getAnimation(m_CurAnim))
		return getAnimation(m_CurAnim)->loop;
	else
		return false;
}

// see if this sprite has finished playing
bool Sprite::done() {
	if (!getAnimation(m_CurAnim) || getAnimation(m_CurAnim)->loop)
		return false;
	
	else
		return m_CurFrame>=getAnimation(m_CurAnim)->frames.size()-1;
}

// set animation to play
void Sprite::setAnimation(const std::string &animation) {
	Animation *anim=getAnimation(animation);
	if (!anim)
		return;
	
	// don't bother reseting this animation if it's already
	// being played
	else if (m_CurAnim==animation && anim->loop)
		return;
	
	// set the animation to use and reset frames
	m_CurAnim=animation;
	m_CurFrame=0;
}

// animate the sprite
void Sprite::animate(int x, int y) {
	// get the animation
	Animation *anim=getAnimation(m_CurAnim);
	if (!anim) {
		std::cout << "Sprite: current animation is nonexistant: " << m_CurAnim << std::endl;
		return;
	}
	
	// get current frame
	Frame *frame=getCurrentFrame();
	if (!frame)
		return;
	
	// get the last frame time and compare it with now
	int now=SDL_GetTicks();
	if (now-m_LastFrame>=frame->time) {
		// save this time
		m_LastFrame=now;
		
		// move on to the next frame
		m_CurFrame++;
		
		// loop if needed
		if (m_CurFrame>anim->frames.size()-1 && anim->loop)
			m_CurFrame=0;
		
		// if this frame has a sound effect requested, play it now
		if (frame->sfx!="")
			Audio::playEffect(frame->sfx, SCRIPT_SFX_CHANNEL);
	}
	
	// draw the current frame
	Renderer::drawImage(x, y, frame->image);
}

// render a single frame of current animation
void Sprite::renderFrame(int x, int y, int frame) {
	// get the current animation
	Animation *anim=getAnimation(m_CurAnim);
	if (!anim)
		return;
	
	// default to first frame if unavailable
	if (anim->frames.empty())
		 frame=0;
	
	// don't draw anything if this frame is nonexistant
	if (frame>=anim->frames.size()-1)
		return;
	
	// draw the requested frame
	Renderer::drawImage(x, y, anim->frames[frame].image);
}

// get an animation sequence
Animation* Sprite::getAnimation(const std::string &id) {
	if (m_Animations.find(id)!=m_Animations.end())
		return &m_Animations[id];
	else
		return NULL;
}

// get the current frame
Frame* Sprite::getCurrentFrame() {
	Animation *anim=getAnimation(m_CurAnim);
	if (!anim)
		return NULL;
	
	// return the frame
	else {
		if (m_CurFrame>anim->frames.size()-1)
			return NULL;
		
		return &anim->frames[m_CurFrame];
	}
}

// add a frame to an animation sequence
void Sprite::addFrame(const std::string &id, int time, SDL_Surface *frame) {
	// get the target animation
	Animation *anim=getAnimation(id);
	if (anim) {
		// fill in data
		Frame fr;
		fr.time=time;
		fr.image=frame;
		
		anim->frames.push_back(fr);
	}
}

// reset the sprite
void Sprite::reset() {
	m_CurFrame=0;
	m_CurAnim="idle";
	m_LastFrame=0;
}
