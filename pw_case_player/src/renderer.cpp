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
#include <sstream>
#include "SDL_gfxPrimitives.h"

#include "font.h"
#include "renderer.h"
#include "texture.h"
#include "theme.h"
#include "utilities.h"

// draw a colored rectangle to the video surface
void Renderer::drawRect(const Rect &rect, const Color &color) {
	drawRect(SDL_GetVideoSurface(), rect.getPoint(), rect.getWidth(), rect.getHeight(), color);
}

// draw a colored rectangle
void Renderer::drawRect(SDL_Surface *surface, const Point &p, int w, int h, const Color &color) {
	SDL_Rect rect;
	rect.x=p.x();
	rect.y=p.y();
	rect.w=w;
	rect.h=h;
	
	SDL_FillRect(surface, &rect, SDL_MapRGB(SDL_GetVideoSurface()->format, color.r(), color.g(), color.b()));
}

// draw a full image at a point
void Renderer::drawImage(const Point &p, const ustring &texId) {
	// get the texture in question
	SDL_Surface *tex=Textures::queryTexture(texId);
	if (!tex) {
		Utils::debugMessage("Renderer: texture '"+texId+"' not found in stack.");
		return;
	}
	
	// draw this image
	drawImage(p, tex);
}

// draw a full image at a point
void Renderer::drawImage(const Point &p, SDL_Surface *dest, const ustring &texId) {
	// get the texture in question
	SDL_Surface *tex=Textures::queryTexture(texId);
	if (!tex) {
		Utils::debugMessage("Renderer: texture '"+texId+"' not found in stack.");
		return;
	}
	
	// draw this image
	drawImage(p, dest, tex);
}

// draw a full image at a point
void Renderer::drawImage(const Point &p, SDL_Surface *texture) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// prepare destination
	SDL_Rect destRect;
	destRect.x=p.x();
	destRect.y=p.y();
	
	// blit the surfaces
	SDL_BlitSurface(texture, NULL, screen, &destRect);
}

// draw a full image at a point onto another surface
void Renderer::drawImage(const Point &p, SDL_Surface *dest, SDL_Surface *texture) {
	SDL_Rect destRect={ p.x(), p.y() };
	SDL_BlitSurface(texture, NULL, dest, &destRect);
}

// draw a part of an image onto another
void Renderer::drawImage(const Point &p, int w, int h, SDL_Surface *src, SDL_Surface *dest) {
	// define our region
	SDL_Rect rect;
	rect.x=p.x();
	rect.y=p.y();
	rect.w=w;
	rect.h=h;
	
	SDL_BlitSurface(src, &rect, dest, NULL);
}

// draw a textured quad
void Renderer::drawImage(const Point &p1, int w, int h, const Point &p2, const ustring &texId) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// get the texture in question
	SDL_Surface *tex=Textures::queryTexture(texId);
	if (!tex) {
		Utils::debugMessage("Renderer: texture '"+texId+"' not found in stack.");
		return;
	}
	
	// rectangle blitting data
	SDL_Rect srcRect, destRect;
	
	// fill in data
	srcRect.x=p1.x();
	srcRect.y=p1.y();
	srcRect.w=w;
	srcRect.h=h;
	
	destRect.x=p2.x();
	destRect.y=p2.y();
	
	// blit the image
	SDL_BlitSurface(tex, &srcRect, screen, &destRect);
}

// draw a button with text
void Renderer::drawButton(const Point &p1, int w, const ustring &text) {
	int fsize=Fonts::FONT_BUTTON_TEXT;
	
	// verify that we have enough room for the text
	int fw=Fonts::getWidth(text, fsize);
	if (fw>w-4) {
		// step the font down a size
		fsize=Fonts::FONT_INFO_PAGE;
		fw=Fonts::getWidth(text, fsize);
	}
	
	// first, draw the border at the left
	Renderer::drawImage(p1, "tc_choice_btn_left");
	Renderer::drawImage(Point(p1.x()+w-2, p1.y()), "tc_choice_btn_right");
	
	// draw only as much of the button body as we need
	SDL_Rect srect={ 0, 0, w-4, 26 };
	SDL_Rect drect={ p1.x()+2, p1.y() };
	SDL_BlitSurface(Textures::queryTexture("tc_choice_btn_body"), &srect, SDL_GetVideoSurface(), &drect);
	
	// now draw the text
	int centerX=(p1.x()+(w/2)-(fw/2));
	int centerY=p1.y()+((26-Fonts::getHeight(fsize))/2)-2;
	Fonts::drawStringBlended(Point(centerX, centerY), text, fsize, Theme::lookup("button_text"));
}

// generate a correctly rendered court panorama based on shown sprites
SDL_Surface* Renderer::generateCourtPanorama(Case::Case *pcase, const ustring &prosecutor, 
					     const ustring &defense, const ustring &witness) {
	// get the base panorama
	SDL_Surface *bg=Textures::queryTexture("court_panorama");
	if (!bg)
		return NULL;
	
	// first, create a new surface
	SDL_Surface *panorama=Textures::queryTexture("court_panorama_filled");
	
	// clear the image
	SDL_FillRect(panorama, NULL, 0);
	
	// draw the panorama onto the blank
	Renderer::drawImage(Point(0, 0), bg->w, bg->h, bg, panorama);
	
	// now draw the sprites themselves, if they are requested
	if (prosecutor!=STR_NULL && pcase->getCharacter(prosecutor)) {
		// get the character's sprite
		Sprite *sProsecutor=pcase->getCharacter(prosecutor)->getSprite();
		
		// draw the prosecutor on the right
		SDL_Surface *image=sProsecutor->getCurrentFrame()->image;
		Renderer::drawImage(Point(bg->w-256, 0), image->w, image->h, image, panorama);
	}
	
	if (defense!=STR_NULL && pcase->getCharacter(defense)) {
		// get the character's sprite
		Sprite *sDefense=pcase->getCharacter(defense)->getSprite();
		
		// draw the defense attorney on the left
		SDL_Surface *image=sDefense->getCurrentFrame()->image;
		Renderer::drawImage(Point(0, 0), image->w, image->h, image, panorama);
	}
	
	if (witness!=STR_NULL && pcase->getCharacter(witness)) {
		// get the character's sprite
		Sprite *sDefense=pcase->getCharacter(witness)->getSprite();
		
		// draw the witness in the center
		SDL_Surface *image=sDefense->getCurrentFrame()->image;
		Renderer::drawImage(Point(472, 0), image->w, image->h, image, panorama);
	}
	
	// draw prosecuter side bench
	SDL_Surface *pb=Textures::queryTexture("prosecutor_bench");
	Renderer::drawImage(Point(bg->w-256, 0), pb->w, pb->h, pb, panorama);
	
	// draw defense side bench
	SDL_Surface *db=Textures::queryTexture("defense_bench");
	Renderer::drawImage(Point(0, 0), db->w, db->h, db, panorama);
	
	// draw witness bench
	SDL_Surface *wb=Textures::queryTexture("witness_bench");
	Renderer::drawImage(Point(472, 0), wb->w, wb->h, wb, panorama);
	
	return panorama;
}

// draw the initial game screen
void Renderer::drawInitialScreen() {
	// first, draw the background
	Renderer::drawImage(Point(0, 197), "court_overview_g");
	
	// draw two buttons, one for New Game, and one for Continue
	//Renderer::drawButton(Point(53, 240), 150, "New Game");
	UI::Manager::instance()->animateGUIButton("an_new_game_btn");
	Renderer::drawButton(Point(53, 280), 150, "Continue");
	
	// draw scanlines to top it off
	Renderer::drawImage(Point(0, 197), "scanlines_overlay");
}

// draw the evidence page
void Renderer::drawEvidencePage(const std::vector<Case::Evidence*> &evidence, int page, int selected) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// draw the background
	drawRect(screen, Point(24, 233), 208, 124, Theme::lookup("court_record_bg"));
	
	// draw top info bar borders
	drawRect(screen, Point(24, 233), 208, 20, Theme::lookup("court_record_info_bar_top"));
	drawRect(screen, Point(25, 234), 207, 19, Theme::lookup("court_record_info_bar_bottom"));
	
	// draw the top info bar
	drawRect(screen, Point(26, 235), 204, 16, Theme::lookup("info_bar_bg"));
	
	// draw buttons
	drawImage(Point(1, 253), "tc_large_btn_left");
	drawImage(Point(256-16-1, 253), "tc_large_btn_right");
	
	// draw arrows on buttons if there is more than one page
	if (evidence.size()>8) {
		drawImage(Point(4, 297), "tc_button_arrow_left");
		drawImage(Point(256-12, 297), "tc_button_arrow_right");
	}
	
	// get the starting index for the vector
	int index=8*page;
	
	// draw 1st row of evidence slots
	int x=24+12;
	int y=259;
	for (int i=0; i<8; i++) {
		// draw the border
		drawRect(screen, Point(x, y), 39, 39, Theme::lookup("court_record_item_border"));
		
		// draw filled center
		drawRect(screen, Point(x+2, y+2), 35, 35, Theme::lookup("court_record_bg"));
		
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
				Fonts::drawString(Point(24+centerx, 238), name, Fonts::FONT_INFO_PAGE, Fonts::COLOR_YELLOW);
				
				// draw selection box
				drawRect(screen, Point(x-1, y-1), 42, 42, Theme::lookup("selection_box"));
			}
			
			// draw evidence thumbnail over the empty slot borders
			SDL_Rect drect;
			drect.x=x;
			drect.y=y;
			SDL_BlitSurface(e->thumb, NULL, screen, &drect);
			
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

// draw evidence information page
void Renderer::drawEvidenceInfoPage(const Case::Evidence *e) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// draw the info strip
	Point p(0, 206);
	Point n=drawInfoStrip(p, e->texture, e->name, e->caption, e->description, true);
	
	// draw button on left
	drawImage(Point(0, n.y()+4), "tc_small_btn_left");
	UI::Manager::instance()->drawAnimation("an_info_page_button_left");
	
	// draw button with arrow on right
	drawImage(Point(n.x()+3, n.y()+4), "tc_small_btn_right");
	UI::Manager::instance()->drawAnimation("an_info_page_button_right");
}

// draw the profiles page
void Renderer::drawProfilesPage(const std::vector<Character*> &uchars, int page, int selected) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// draw the background
	drawRect(screen, Point(24, 233), 208, 124, Theme::lookup("court_record_bg"));
	
	// draw top info bar borders
	drawRect(screen, Point(24, 233), 208, 20, Theme::lookup("court_record_info_bar_top"));
	drawRect(screen, Point(25, 234), 207, 19, Theme::lookup("court_record_info_bar_bottom"));
	
	// draw the top info bar
	drawRect(screen, Point(26, 235), 204, 16, Theme::lookup("info_bar_bg"));
	
	// draw buttons
	drawImage(Point(1, 253), "tc_large_btn_left");
	drawImage(Point(239, 253), "tc_large_btn_right");
	
	// draw arrows on buttons if there is more than one page
	if (uchars.size()>8) {
		drawImage(Point(4, 297), "tc_button_arrow_left");
		drawImage(Point(244, 297), "tc_button_arrow_right");
	}
	
	// get the starting index for the vector
	int index=8*page;
	
	// draw 1st row of profile slots
	int x=36;
	int y=259;
	for (int i=0; i<8; i++) {
		// draw the border
		drawRect(screen, Point(x, y), 39, 39, Theme::lookup("court_record_item_border"));
		
		// draw filled center
		drawRect(screen, Point(x+2, y+2), 35, 35, Theme::lookup("court_record_bg"));
		
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
				Fonts::drawString(Point(24+centerx, 238), name, Fonts::FONT_INFO_PAGE, Fonts::COLOR_YELLOW);
				
				// draw selection box
				drawRect(screen, Point(x-1, y-1), 42, 42, Theme::lookup("selection_box"));
			}
			
			// draw profile thumbnail over the empty slot borders
			SDL_Rect drect;
			drect.x=x;
			drect.y=y;
			SDL_BlitSurface(c->getHeadshotThumb(), NULL, screen, &drect);
			
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
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// draw the info strip
	Point p(0, 206);
	Point n=drawInfoStrip(p, const_cast<Character*> (c)->getHeadshot(), c->getName(), c->getCaption(), c->getDescription(), true);
	
	// draw button on left
	drawImage(Point(0, n.y()+4), "tc_small_btn_left");
	UI::Manager::instance()->drawAnimation("an_info_page_button_left");
	
	// draw button with arrow on right
	drawImage(Point(n.x()+3, n.y()+4), "tc_small_btn_right");
	UI::Manager::instance()->drawAnimation("an_info_page_button_right");
}

// draw the strip containing evidence or profile
Point Renderer::drawInfoStrip(const Point &p, SDL_Surface *image, const ustring &name,
			      const ustring &caption, const ustring &desc, bool description) {
	int x=p.x();
	int y=p.y();
	
	// draw info strip background
	drawRect(Rect(Point(x+0, y+25), 256, 77), Theme::lookup("court_record_bg"));
	
	// if descriptions are to be drawn, lengthen the background
	if (description)
		drawRect(Rect(Point(x+8, y+101), 240, 52), Theme::lookup("court_record_bg"));
	
	// draw upper border
	drawRect(Rect(Point(x+0, y+25), 256, 6), Theme::lookup("info_box_border"));
	y+=31;
	
	// draw the item
	drawImage(Point(x+19, y), image);
	x+=92;
	
	// draw info box's border
	drawRect(Rect(Point(x, y), 148, 70), Theme::lookup("info_box_outline"));
	
	// draw info box title bar
	drawRect(Rect(Point(x+2, y+2), 144, 15), Theme::lookup("info_bar_bg"));
	
	// calculate center position for name
	int centerx=(x+72)-(Fonts::getWidth(name, Fonts::FONT_INFO_PAGE)/2);
	
	// draw name in title bar
	Fonts::drawString(Point(centerx, y+4), name, Fonts::FONT_INFO_PAGE, Fonts::COLOR_YELLOW);
	
	// draw info box body
	drawRect(Rect(Point(x+2, y+17), 144, 51), Theme::lookup("info_box_bg"));
	
	// draw caption in this area
	Fonts::drawStringBlended(Point(x+5, y+18), caption, Fonts::FONT_INFO_PAGE, Fonts::COLOR_BLACK);
	
	// moving right along...
	x+=148;
	
	// draw lower border
	drawRect(Rect(Point(p.x(), y+70), 256, 6), Theme::lookup("info_box_border"));
	
	// draw description in bottom area
	if (description)
		Fonts::drawString(Point(p.x()+16, y+81), desc, Fonts::FONT_INFO_PAGE, Fonts::COLOR_WHITE);
	
	// return the modified coordinates
	return Point(x, y);
}

// draw the examination scene
void Renderer::drawExamineScene(SDL_Surface *bg, const Point &cursor, bool slideBG) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	if (bg)
		UI::Manager::instance()->slideBG("an_bg_slide", bg);
	
	// get opaque screen and make it completely transparent
	SDL_Surface *overlay=Textures::queryTexture("transparent");
	SDL_FillRect(overlay, NULL, 0);
	
	// draw crosshairs
	vlineRGBA(overlay, cursor.x(), 0, 192, 0, 0, 255, 200);
	hlineRGBA(overlay, 0, 256, cursor.y(), 0, 0, 255, 200);
	
	// draw center rectangle
	rectangleRGBA(overlay, cursor.x()-6, cursor.y()-6, cursor.x()+6, cursor.y()+6, 0, 0, 255, 200);
	
	// draw overlay
	Renderer::drawImage(Point(0, 197), overlay);
}

// draw the movement scene
void Renderer::drawMoveScene(const std::vector<ustring> &locations, LocationMap lmap, int selected) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	
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
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
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
