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
#include <GL/gl.h>
#include "SDL.h"

#include "case.h"
#include "common.h"
#include "texture.h"
#include "uimanager.h"

/// Namespace for all higher level drawing functions
namespace Renderer {

/** Resets an OpenGL matrix back to its identity, 
  * then returns to modelview matrix
  * \param matrix Enum for the matrix to reset
*/
void resetGLMatrix(GLenum matrix);

/** Draw a colored rectangle
  * \param rect The rectangle to draw
  * \param color The color for the rectangle
*/
void drawRect(const Rect &rect, const Color &color);

/** Draw a full image from the stack at a point
  * \param p The point to draw the image at
  * \param texId The ID of the image to draw
*/
void drawImage(const Point &p, const ustring &texId);

/** Draw a full image from the stack at a point
  * \param p THe point to draw the image at
  * \param id The GL ID of the texture
*/
void drawImage(const Point &p, const GLuint &id);

/** Draw a full image at a point
  * \param p The point to draw the image at
  * \param texture The texture to draw
*/
void drawImage(const Point &p, const Textures::Texture &texture);

/** Draw part of an image to a specific point onto the screen
  * \param rect The part of the region to draw
  * \param p2 The destination point
  * \param texId The ID of the image
*/
void drawImage(const Rect &rect, const Point &p2, const ustring &texId);

/** Draw a flat button with text
  * \param p1 The point at which to draw the button at
  * \param w The width of the button
  * \param text The text for the button
*/
void drawButton(const Point &p1, int w, const ustring &text);

/// Draw the initial game screen
void drawInitialScreen();

/** Draw the evidence Court Record page
  * \param evidence Vector of evidence in the Court Record
  * \param page Selected page, should be 0 based
  * \param selected Selected evidence item, should be 0 based and in the range [0,7]
*/
void drawEvidencePage(const std::vector<Case::Evidence*> &evidence, int page, int selected);

/** Draw evidence information page in Court Record
  * \param evidence Pointer to displayed evidence
*/
void drawEvidenceInfoPage(const Case::Evidence *evidence);

/** Draw the profiles page in the Court Record
  * \param uchars Vector of characters in the Court Record
  * \param page Selected page, should be 0 based
  * \param selected Selected profile, should be 0 based and in the range [0,7]
*/
void drawProfilesPage(const std::vector<Character*> &uchars, int page, int selected);

/** Draw the profile information page in the Court Record
  * \param character Pointer to displayed character profile
*/
void drawProfileInfoPage(const Character *character);

/** Draw the strip containing evidence or profile
  * \param p The point at which to draw the strip at
  * \param image The image to display in the small box
  * \param name The name of the evidence or profile
  * \param caption Text to display in the green box
  * \param desc Description text to display below the image and caption
  * \param description Whether or not to include the description
  * \return Point with the current, updated drawing coordinates relative to <i>p</i>
*/
Point drawInfoStrip(const Point &p, const Textures::Texture &image, const ustring &name, const ustring &caption, const ustring &desc, bool description);

/** Draw the examination scene
  * \param bg The background for the location
  * \param cursor Position of the examine cursor
  * \param slideBG Flag whether or not to show the animation of the background sliding down
*/
void drawExamineScene(const Textures::Texture &bg, const Point &cursor, bool slideBG=true);

/** Draw the movement scene
  * \param locations Vector of location IDs to draw
  * \param lmap Map of all locations
  * \param selected 0 based index of selected location
*/
void drawMoveScene(const std::vector<ustring> &locations, LocationMap lmap, int selected);

/** Draw the talk scene
  * \param options String vector of talk options
  * \param selected 0 based index of selected option
  * \param centered Whether or not to draw the buttons centered on screen
*/
void drawTalkScene(const std::vector<StringPair> &options, int selected, bool centered=false);

}; // namespace Renderer

#endif
