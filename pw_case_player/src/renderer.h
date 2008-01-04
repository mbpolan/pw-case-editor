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
// renderer.h: various higher rendering functions

#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>
#include "SDL.h"

#include "case.h"
#include "common.h"
#include "uimanager.h"

namespace Renderer {

// draw a colored rectangle
void drawRect(SDL_Surface *surface, const Point &p, int w, int h, Uint32 color);

// draw a full image at a point
void drawImage(const Point &p, const std::string &texId);

// draw a full image at a point
void drawImage(const Point &p, SDL_Surface *texture);

// draw a part of an image onto another
void drawImage(const Point &p, int w, int h, SDL_Surface *src, SDL_Surface *dest);

// draw an image
// (x1,y1) are source coordinate pair; (x2,y2) are destination
void drawImage(const Point &p1, int w, int h, const Point &p2, const std::string &texId);

// generate a correctly rendered court panorama based on shown sprites
SDL_Surface* generateCourtPanorama(Case::Case *pcase, const std::string &prosecutor, 
				   const std::string &attorney, const std::string &witness);

// draw the evidence page
// page should be based on index 0, and selected should be an index between 0 and 7
void drawEvidencePage(const std::vector<Case::Evidence> &evidence, int page, int selected);

// draw evidence information page
// index is the current index into evidence vector for displayed evidence
void drawEvidenceInfoPage(UI::Manager *ui, const std::vector<Case::Evidence> &evidence, int index);

// draw the profiles page
// page should be based on index 0, and selected should be an index between 0 and 7
void drawProfilesPage(const std::vector<Character> &chars, int page, int selected);

// draw profile information page
// index is the current index into profiles vector
void drawProfileInfoPage(UI::Manager *ui, const std::vector<Character> &chars, int index);

// draw the examination scene
void drawExamineScene(const Point &cursor);

// draw the movement scene
void drawMoveScene(const std::vector<std::string> &locations, LocationMap lmap, int selected);

// draw talk scene
void drawTalkScene(const std::vector<StringPair> &options, int selected, bool centered=false);

}; // namespace Renderer

#endif
