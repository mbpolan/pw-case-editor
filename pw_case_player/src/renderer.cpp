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
// renderer.cpp: implementations of Renderer namespace

#include <cmath>
#include <GL/gl.h>
#include <sstream>
#include "SDL_gfxPrimitives.h"

#include "font.h"
#include "renderer.h"
#include "sdlcontext.h"
#include "texture.h"
#include "theme.h"
#include "utilities.h"

// reset an opengl matrix
void Renderer::resetGLMatrix(GLenum matrix) {
	glMatrixMode(matrix);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
}

// draw a colored rectangle
void Renderer::drawRect(const Rect &rect, const Color &color) {
	glDisable(GL_TEXTURE_2D);
	
	// save our current color and whatnot
	glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
	
	// we need to enable blending in this case
	if (color.a()<(char) 255) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	
	// translate according to z
	glTranslatef(0.0f, 0.0f, rect.getPoint().z());
	
	// draw the rectangle with the specified color
	glColor4ub(color.r(), color.g(), color.b(), color.a());
	
	Point p=rect.getPoint();
	glRecti(p.x(), p.y(), p.x()+rect.getWidth(), p.y()+rect.getHeight());
	
	// return to previous z
	glTranslatef(0.0f, 0.0f, -rect.getPoint().z());
	
	glPopAttrib();
	
	glEnable(GL_TEXTURE_2D);
}

// draw a full image at a point
void Renderer::drawImage(const Point &p, const ustring &texId) {
	// get the texture in question
	Textures::Texture tex=Textures::queryTexture(texId);
	
	// draw this image
	drawImage(p, tex);
}

// draw a full image at a point
void Renderer::drawImage(const Point &p, const GLuint &id) {
	// perform a query based on the GL ID instead of string id
	Textures::Texture tex=Textures::queryTexture(id);
	
	// draw the image
	drawImage(p, tex);
}

// draw a full image at a point
void Renderer::drawImage(const Point &p, const Textures::Texture &tex) {
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, tex.id);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// draw the textured quad
	glBegin(GL_QUADS); {
		glTexCoord2f(0, 0); glVertex3f(p.x(), p.y(), p.z());
		glTexCoord2f(tex.u, 0); glVertex3f(p.x()+tex.w, p.y(), p.z());
		glTexCoord2f(tex.u, tex.v); glVertex3f(p.x()+tex.w, p.y()+tex.h, p.z());
		glTexCoord2f(0, tex.v); glVertex3f(p.x(), p.y()+tex.h, p.z());
	}
	glEnd();
}

// draw a textured quad
void Renderer::drawImage(const Rect &rect, const Point &p2, const ustring &texId) {
	// get the texture in question
	Textures::Texture tex=Textures::queryTexture(texId);
	
	// FIXME
	std::cout << "TODO\n";
}

// draw a button with text
void Renderer::drawButton(const Point &p1, int w, const ustring &text) {
	int fsize=Fonts::FONT_BUTTON_TEXT;
	
	// verify that we have enough room for the text
	int fw=Fonts::getWidthTTF(text, fsize);
	if (fw>w-4) {
		// step the font down a size
		fsize=Fonts::FONT_INFO_PAGE;
		fw=Fonts::getWidthTTF(text, fsize);
	}
	
	// first, draw the border at the left
	Renderer::drawImage(p1, "tc_choice_btn_left");
	Renderer::drawImage(Point(p1.x()+w-2, p1.y(), p1.z()), "tc_choice_btn_right");
	
	// draw only as much of the button body as we need
	Textures::Texture tex=Textures::queryTexture("tc_choice_btn_body");
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex.id);
	glBegin(GL_QUADS); {
		glTexCoord2f(0, 0); glVertex3f(p1.x()+2, p1.y(), p1.z());
		glTexCoord2f(tex.u, 0); glVertex3f(p1.x()+w, p1.y(), p1.z());
		glTexCoord2f(tex.u, tex.v); glVertex3f(p1.x()+w, p1.y()+tex.h, p1.z());
		glTexCoord2f(0, tex.v); glVertex3f(p1.x()+2, p1.y()+tex.h, p1.z());
	}
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
	
	// now draw the text
	int centerX=(p1.x()+(w/2)-(fw/2));
	int centerY=p1.y()+((26-Fonts::getHeight(fsize))/2)-2;
	Fonts::drawStringBlended(Point(centerX, centerY), text, fsize, Theme::lookup("button_text"));
}

// draw the initial game screen
void Renderer::drawInitialScreen() {
	// first, draw the background
	Renderer::drawImage(Point(0, 197), "court_overview_g");
	
	// draw two buttons, one for New Game, and one for Continue
	UI::Manager::instance()->animateGUIButton("an_new_game_btn");
	Renderer::drawButton(Point(53, 280, 2), 150, "Continue");
	
	// draw scanlines to top it off
	Renderer::drawImage(Point(0, 197), "scanlines_overlay");
}

// draw the evidence page
void Renderer::drawEvidencePage(const std::vector<Case::Evidence*> &evidence, int page, int selected) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	
	glLoadIdentity();
	gluPerspective(45.0f, SDLContext::instance()->getWidth()/SDLContext::instance()->getHeight(), 0.1, 100.0);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -10.0f);
	
	// draw the background
	drawRect(Rect(Point(24, 233, -1.0f), 208, 124), Theme::lookup("court_record_bg"));
	
	// draw top info bar borders
	drawRect(Rect(Point(24, 233, -1.1f), 208, 20), Theme::lookup("court_record_info_bar_top"));
	drawRect(Rect(Point(25, 234, -1.2f), 207, 19), Theme::lookup("court_record_info_bar_bottom"));
	
	// draw the top info bar
	drawRect(Rect(Point(26, 235, -1.3f), 204, 16), Theme::lookup("info_bar_bg"));
	
	// draw buttons
	drawImage(Point(1, 253, 2.0f), "tc_large_btn_left");
	drawImage(Point(256-16-1, 253, 2.0f), "tc_large_btn_right");
	
	// draw arrows on buttons if there is more than one page
	if (evidence.size()>8) {
		drawImage(Point(4, 297, 2.1f), "tc_button_arrow_left");
		drawImage(Point(256-12, 297, 2.1f), "tc_button_arrow_right");
	}
	
	// get the starting index for the vector
	int index=8*page;
	
	// draw 1st row of evidence slots
	int x=36;
	int y=259;
	for (int i=0; i<8; i++) {
		// draw the border
		drawRect(Rect(Point(x, y, 1.4f), 39, 39), Theme::lookup("court_record_item_border"));
		
		// draw filled center
		drawRect(Rect(Point(x+2, y+2, 1.5f), 35, 35), Theme::lookup("court_record_bg"));
		
		// see if there is a piece of evidence at this slot
		if (index<=evidence.size()-1 && !evidence.empty()) {
			Case::Evidence *e=evidence[index];
			
			// if this is the selected evidence, write it's name in the info
			// bar, and draw selection box
			if (i==selected) {
				// get the name of this evidence
				ustring name=e->name;
				
				// calculate string length for this string
				int width=Fonts::getWidth(name, Fonts::FONT_INFO_PAGE);
				
				// find the center x position for the string
				int centerx=(int) floor((204-width)/2);
				
				// draw the string at this position
				Fonts::drawString(Point(24+centerx, 238, Z_TEXT), name, Fonts::FONT_INFO_PAGE, Fonts::COLOR_YELLOW);
				
				// draw selection box
				drawRect(Rect(Point(x-1, y-1, 1.6f), 42, 42), Theme::lookup("selection_box"));
			}
			
			// draw evidence thumbnail over the empty slot borders
			drawImage(Point(x, y, 1.7f), Textures::queryTexture(e->thumb));
			
			index++;
		}
		
		// advance to next slot
		x+=48;
		
		// reset for next row
		if (i==3) {
			y=305;
			x=36;
		}
	}
	
	glPopMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

// draw evidence information page
void Renderer::drawEvidenceInfoPage(const Case::Evidence *e) {
	// draw the info strip
	Point p(0, 206, 1.0f);
	Point n=drawInfoStrip(p, Textures::queryTexture(e->texture), e->name, e->caption, e->description, true);
	
	// draw button on left
	drawImage(Point(0, n.y()+4, 1.2f), "tc_small_btn_left");
	UI::Manager::instance()->drawAnimation("an_info_page_button_left");
	
	// draw button with arrow on right
	drawImage(Point(n.x()+3, n.y()+4, 1.2f), "tc_small_btn_right");
	UI::Manager::instance()->drawAnimation("an_info_page_button_right");
}

// draw the profiles page
void Renderer::drawProfilesPage(const std::vector<Character*> &uchars, int page, int selected) {
	// draw the background
	drawRect(Rect(Point(24, 233, 1.0f), 208, 124), Theme::lookup("court_record_bg"));
	
	// draw top info bar borders
	drawRect(Rect(Point(24, 233, 1.1f), 208, 20), Theme::lookup("court_record_info_bar_top"));
	drawRect(Rect(Point(25, 234, 1.2f), 207, 19), Theme::lookup("court_record_info_bar_bottom"));
	
	// draw the top info bar
	drawRect(Rect(Point(26, 235, 1.3f), 204, 16), Theme::lookup("info_bar_bg"));
	
	// draw buttons
	drawImage(Point(1, 253, 2.0f), "tc_large_btn_left");
	drawImage(Point(239, 253, 2.0f), "tc_large_btn_right");
	
	// draw arrows on buttons if there is more than one page
	if (uchars.size()>8) {
		drawImage(Point(4, 297, 2.1f), "tc_button_arrow_left");
		drawImage(Point(244, 297, 2.1f), "tc_button_arrow_right");
	}
	
	// get the starting index for the vector
	int index=8*page;
	
	// draw 1st row of profile slots
	int x=36;
	int y=259;
	for (int i=0; i<8; i++) {
		// draw the border
		drawRect(Rect(Point(x, y, 1.4f), 39, 39), Theme::lookup("court_record_item_border"));
		
		// draw filled center
		drawRect(Rect(Point(x+2, y+2, 1.5f), 35, 35), Theme::lookup("court_record_bg"));
		
		// see if there is a profile at this slot
		if (index<=uchars.size()-1 && !uchars.empty()) {
			Character *c=uchars[index];
			
			// if this is the selected profile, write his name in the info
			// bar, and draw selection box
			if (i==selected) {
				// get the name of this character
				ustring name=c->getName();
				
				// calculate string length for this string
				int width=Fonts::getWidth(name, Fonts::FONT_INFO_PAGE);
				
				// find the center x position for the string
				int centerx=(int) floor((204-width)/2);
				
				// draw the string at this position
				Fonts::drawString(Point(24+centerx, 238, Z_TEXT), name, Fonts::FONT_INFO_PAGE, Fonts::COLOR_YELLOW);
				
				// draw selection box
				drawRect(Rect(Point(x-1, y-1, 1.6f), 42, 42), Theme::lookup("selection_box"));
			}
			
			// draw profile thumbnail over the empty slot borders
			Renderer::drawImage(Point(x, y, 1.7f), Textures::queryTexture(c->getHeadshotThumb()));
			
			index++;
		}
		
		// advance to next slot
		x+=48;
		
		// reset for next row
		if (i==3) {
			y=305;
			x=36;
		}
	}
}

// draw the profile info page
void Renderer::drawProfileInfoPage(const Character *c) {
	// draw the info strip
	Point p(0, 206, 1.0f);
	Point n=drawInfoStrip(p, Textures::queryTexture(const_cast<Character*> (c)->getHeadshot()), 
			      c->getName(), c->getCaption(), c->getDescription(), true);
	
	// draw button on left
	drawImage(Point(0, n.y()+4, 1.2f), "tc_small_btn_left");
	UI::Manager::instance()->drawAnimation("an_info_page_button_left");
	
	// draw button with arrow on right
	drawImage(Point(n.x()+3, n.y()+4, 1.2f), "tc_small_btn_right");
	UI::Manager::instance()->drawAnimation("an_info_page_button_right");
}

// draw the strip containing evidence or profile
Point Renderer::drawInfoStrip(const Point &p, const Textures::Texture &image, const ustring &name,
			      const ustring &caption, const ustring &desc, bool description) {
	int x=p.x();
	int y=p.y();
	
	// draw info strip background
	drawRect(Rect(Point(x+0, y+25, 1.0f), 256, 77), Theme::lookup("court_record_bg"));
	
	// if descriptions are to be drawn, lengthen the background
	if (description)
		drawRect(Rect(Point(x+8, y+101, 1.1f), 240, 52), Theme::lookup("court_record_bg"));
	
	// draw upper border
	drawRect(Rect(Point(x+0, y+25, 1.2f), 256, 6), Theme::lookup("info_box_border"));
	y+=31;
	
	// draw the item
	drawImage(Point(x+19, y, 1.3f), image);
	x+=92;
	
	// draw info box's border
	drawRect(Rect(Point(x, y, 1.4f), 148, 70), Theme::lookup("info_box_outline"));
	
	// draw info box title bar
	drawRect(Rect(Point(x+2, y+2, 1.5f), 144, 15), Theme::lookup("info_bar_bg"));
	
	// calculate center position for name
	int centerx=(x+72)-(Fonts::getWidth(name, Fonts::FONT_INFO_PAGE)/2);
	
	// draw name in title bar
	Fonts::drawString(Point(centerx, y+4, 1.6f), name, Fonts::FONT_INFO_PAGE, Fonts::COLOR_YELLOW);
	
	// draw info box body
	drawRect(Rect(Point(x+2, y+17, 1.7f), 144, 51), Theme::lookup("info_box_bg"));
	
	// draw caption in this area
	Fonts::drawStringBlended(Point(x+5, y+18, 1.8f), caption, Fonts::FONT_INFO_PAGE, Fonts::COLOR_BLACK);
	
	// moving right along...
	x+=148;
	
	// draw lower border
	drawRect(Rect(Point(p.x(), y+70, 1.9f), 256, 6), Theme::lookup("info_box_border"));
	
	// draw description in bottom area
	if (description)
		Fonts::drawString(Point(p.x()+16, y+81, 2.0f), desc, Fonts::FONT_INFO_PAGE, Fonts::COLOR_WHITE);
	
	// return the modified coordinates
	return Point(x, y, 2.0f);
}

// draw the examination scene
void Renderer::drawExamineScene(const Textures::Texture &bg, const Point &cursor, bool slideBG) {
	// slide the background
	UI::Manager::instance()->slideBG("an_bg_slide", bg.id);
	
	// save our current state attributes
	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
	
	// disable textures
	glDisable(GL_TEXTURE_2D);
	
	// and set a pulsing blue color
	static int red=255, green=255, v=-1;
	
	// red and green are our only variables
	red+=v*3;
	green+=v*3;
	
	// max of 255, min of 110
	if (red>=255)
		v=-1;
	else if (red<=110)
		v=1;
	
	glColor3ub(256-red, 256-green, 255);
	
	// y coordinate is not relative to lower screen, so cache the real point
	Point p(cursor.x(), cursor.y()+197);
	
	// draw the two crosshair lines
	glBegin(GL_LINES); {
		// draw the vertical crosshair first
		glVertex3f(p.x(), 197, 1.6f);
		glVertex3f(p.x(), 389, 1.6f);
		
		// then the horizontal
		glVertex3f(0, p.y(), 1.7f);
		glVertex3f(256, p.y(), 1.7);
	}
	glEnd();
	
	// now draw the rectangle in the center
	glBegin(GL_LINE_STRIP); {
		// the rectangle has dimensions of 12x12 pixels
		glVertex3f(p.x()-6, p.y()-6, 1.8f);
		glVertex3f(p.x()-6, p.y()+6, 1.8f);
		glVertex3f(p.x()+6, p.y()+6, 1.8f);
		glVertex3f(p.x()+6, p.y()-6, 1.8f);
		glVertex3f(p.x()-6, p.y()-6, 1.9f);
	}
	glEnd();
	
	glPopAttrib();
}

// draw the movement scene
void Renderer::drawMoveScene(const std::vector<ustring> &locations, LocationMap lmap, int selected) {
	// go over locations
	for (int i=0; i<locations.size(); i++) {
		Case::Location location=lmap[locations[i]];
		
		std::stringstream ss;
		ss << "an_move_loc" << i+1 << "_btn";
		
		// draw the preview to the left
		if (selected==i)
			Renderer::drawImage(Point(0, 263), location.bgScaled);
		
		// update text in button
		UI::Manager::instance()->setGUIButtonText(ss.str(), location.name);
		
		// and draw the button
		UI::Manager::instance()->animateGUIButton(ss.str());
	}
}

// draw talk scene
void Renderer::drawTalkScene(const std::vector<StringPair> &options, int selected, bool centered) {
	// iterate over options, and draw each one
	for (int i=0; i<options.size(); i++) {
		std::stringstream ss;
		ss << "an_talk_op" << i+1 << "_btn";
		
		// update the text
		UI::Manager::instance()->setGUIButtonText(ss.str(), options[i].first);
		
		// and draw the button itself
		UI::Manager::instance()->animateGUIButton(ss.str());
	}
}
