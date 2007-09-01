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

namespace Renderer {

// draw a colored rectangle
void drawRect(SDL_Surface *surface, int x, int y, int w, int h, Uint32 color);

// draw a full image at a point
void drawImage(int x, int y, const std::string &texId);

// draw a full image at a point
void drawImage(int x, int y, SDL_Surface *texture);

// draw an image
// (x1,y1) are source coordinate pair; (x2,y2) are destination
void drawImage(int x1, int y1, int w, int h, int x2, int y2, const std::string &texId);

// draw the evidence page
// page should be based on index 0, and selected should be an index between 0 and 7
void drawEvidencePage(const std::vector<Case::Evidence> &evidence, int page, int selected);

// draw evidence information page
// index is the current index into evidence vector for displayed evidence
void drawEvidenceInfoPage(const std::vector<Case::Evidence> &evidence, int index);

// draw the profiles page
void drawProfilesPage(int page);

}; // namespace Renderer

#endif
