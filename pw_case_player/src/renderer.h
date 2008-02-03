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
void drawRect(SDL_Surface *surface, const Point &p, int w, int h, const Color &color);

// draw a full image at a point
void drawImage(const Point &p, const ustring &texId);

// draw a full image at a point
void drawImage(const Point &p, SDL_Surface *dest, const ustring &texId);

// draw a full image at a point
void drawImage(const Point &p, SDL_Surface *texture);

// draw a full image at a point onto another surface
void drawImage(const Point &p, SDL_Surface *dest, SDL_Surface *texture);

// draw a part of an image onto another
void drawImage(const Point &p, int w, int h, SDL_Surface *src, SDL_Surface *dest);

// draw an image
// (x1,y1) are source coordinate pair; (x2,y2) are destination
void drawImage(const Point &p1, int w, int h, const Point &p2, const ustring &texId);

// draw a button with text
void drawButton(const Point &p1, int w, const ustring &text);

// generate a correctly rendered court panorama based on shown sprites
SDL_Surface* generateCourtPanorama(Case::Case *pcase, const ustring &prosecutor, 
				   const ustring &attorney, const ustring &witness);

// draw the initial game screen
void drawInitialScreen(UI::Manager *ui);

// draw the evidence page
// page should be based on index 0, and selected should be an index between 0 and 7
void drawEvidencePage(const std::vector<Case::Evidence*> &evidence, int page, int selected);

// draw evidence information page
void drawEvidenceInfoPage(Case::Evidence *evidence);

// draw the profiles page
// page should be based on index 0, and selected should be an index between 0 and 7
void drawProfilesPage(const std::vector<Character*> &uchars, int page, int selected);

// draw profile information page
void drawProfileInfoPage(Character *character);

// draw the examination scene
void drawExamineScene(SDL_Surface *bg, const Point &cursor, bool slideBG=true);

// draw the movement scene
void drawMoveScene(const std::vector<ustring> &locations, LocationMap lmap, int selected);

// draw talk scene
void drawTalkScene(const std::vector<StringPair> &options, int selected, bool centered=false);

}; // namespace Renderer

#endif
