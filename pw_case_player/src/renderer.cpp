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
#include "SDL_gfxPrimitives.h"

#include "font.h"
#include "renderer.h"
#include "texture.h"
#include "utilities.h"

// draw a colored rectangle
void Renderer::drawRect(SDL_Surface *surface, const Point &p, int w, int h, Uint32 color) {
	SDL_Rect rect;
	rect.x=p.x();
	rect.y=p.y();
	rect.w=w;
	rect.h=h;
	
	SDL_FillRect(surface, &rect, color);
}

// draw a full image at a point
void Renderer::drawImage(const Point &p, const std::string &texId) {
	// get the texture in question
	SDL_Surface *tex=Textures::queryTexture(texId);
	if (!tex) {
		std::cout << "Renderer: texture '" << texId << "' not found in stack.\n";
		return;
	}
	
	// draw this image
	drawImage(p, tex);
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
void Renderer::drawImage(const Point &p1, int w, int h, const Point &p2, const std::string &texId) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// get the texture in question
	SDL_Surface *tex=Textures::queryTexture(texId);
	if (!tex) {
		std::cout << "Renderer: texture '" << texId << "' not found in stack.\n";
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

// generate a correctly rendered court panorama based on shown sprites
SDL_Surface* Renderer::generateCourtPanorama(Case::Case *pcase, const std::string &prosecutor, 
					     const std::string &defense, const std::string &witness) {
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
	if (prosecutor!="null" && pcase->getCharacter(prosecutor)) {
		// get the character's sprite
		Sprite *sProsecutor=pcase->getCharacter(prosecutor)->getSprite();
		
		// draw the prosecutor on the right
		SDL_Surface *image=sProsecutor->getCurrentFrame()->image;
		Renderer::drawImage(Point(bg->w-256, 0), image->w, image->h, image, panorama);
	}
	
	if (defense!="null" && pcase->getCharacter(defense)) {
		// get the character's sprite
		Sprite *sDefense=pcase->getCharacter(defense)->getSprite();
		
		// draw the defense attorney on the left
		SDL_Surface *image=sDefense->getCurrentFrame()->image;
		Renderer::drawImage(Point(0, 0), image->w, image->h, image, panorama);
	}
	
	if (witness!="null" && pcase->getCharacter(witness)) {
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

// draw the evidence page
void Renderer::drawEvidencePage(const std::vector<Case::Evidence> &evidence, int page, int selected) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// draw the background
	drawRect(screen, Point(24, 233), 208, 124, SDL_MapRGB(screen->format, 134, 118, 69));
	
	// draw top info bar borders
	drawRect(screen, Point(24, 233), 208, 20, SDL_MapRGB(screen->format, 252, 249, 244));
	drawRect(screen, Point(25, 234), 207, 19, SDL_MapRGB(screen->format, 168, 167, 163));
	
	// draw the top info bar
	drawRect(screen, Point(26, 235), 204, 16, SDL_MapRGB(screen->format, 55, 55, 55));
	
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
		drawRect(screen, Point(x, y), 39, 39, SDL_MapRGB(screen->format, 211, 197, 148));
		
		// draw filled center
		drawRect(screen, Point(x+2, y+2), 35, 35, SDL_MapRGB(screen->format, 134, 118, 69));
		
		// see if there is a piece of evidence at this slot
		if (index<=evidence.size()-1 && !evidence.empty()) {
			Case::Evidence e=evidence[index];
			
			// if this is the selected evidence, write it's name in the info
			// bar, and draw selection box
			if (i==selected) {
				// get the name of this evidence
				std::string name=e.name;
				
				// calculate string length for this string
				int width=Fonts::getWidth("orange", name);
				
				// find the center x position for the string
				int centerx=(int) floor((204-width)/2);
				
				// draw the string at this position
				Fonts::drawString(24+centerx, 238, name, "orange");
				
				// draw selection box
				drawRect(screen, Point(x-1, y-1), 42, 42, SDL_MapRGB(screen->format, 255, 255, 255));
			}
			
			// draw evidence thumbnail over the empty slot borders
			SDL_Rect drect;
			drect.x=x;
			drect.y=y;
			SDL_BlitSurface(e.thumb, NULL, screen, &drect);
			
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
void Renderer::drawEvidenceInfoPage(UI::Manager *manager, const std::vector<Case::Evidence> &evidence, int index) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// get evidence to draw
	Case::Evidence e=evidence[index];
	
	// keep track of y changes
	int x=0;
	int y=206;
	
	// draw transparent background
	SDL_Surface *opaqueBlack=Textures::queryTexture("opaque_black");
	SDL_SetAlpha(opaqueBlack, SDL_SRCALPHA, 80); // set transparent alpha value
	drawImage(Point(0, 197), opaqueBlack);
	
	// draw info strip background
	drawRect(screen, Point(0, y+25), 256, 77, SDL_MapRGB(screen->format, 134, 118, 69));
	drawRect(screen, Point(8, y+101), 240, 52, SDL_MapRGB(screen->format, 134, 118, 69));
	
	// draw upper border
	drawRect(screen, Point(0, y+25), 256, 6, SDL_MapRGB(screen->format, 218, 218, 218));
	y+=31;
	
	// draw button on left
	drawImage(Point(0, y+4), "tc_small_btn_left");
	manager->drawAnimation("an_info_page_button_left");
	
	// draw the evidence
	drawImage(Point(19, y), e.texture);
	x+=92;
	
	// draw info box's border
	drawRect(screen, Point(x, y), 148, 70, SDL_MapRGB(screen->format, 255, 255, 255));
	
	// draw info box title bar
	drawRect(screen, Point(x+2, y+2), 144, 15, SDL_MapRGB(screen->format, 55, 55, 55));
	
	// calculate center position for name
	int centerx=(x+72)-(Fonts::getWidth("orange", e.name)/2);
	
	// draw evidence name in title bar
	Fonts::drawString(centerx, y+4, e.name, "orange");
	
	// draw info box body
	drawRect(screen, Point(x+2, y+17), 144, 51, SDL_MapRGB(screen->format, 153, 192, 145));
	
	// draw evidence caption in this area
	Fonts::drawTTF(Point(x+5, y+18), e.caption);
	
	// moving right along...
	x+=148;
	
	// draw button with arrow on right
	drawImage(Point(x+3, y+4), "tc_small_btn_right");
	manager->drawAnimation("an_info_page_button_right");
	
	// draw lower border
	drawRect(screen, Point(0, 308), 256, 6, SDL_MapRGB(screen->format, 218, 218, 218));
	
	// draw evidence description in bottom area
	Fonts::drawString(16, y+81, e.description, "white");
}

// draw the profiles page
void Renderer::drawProfilesPage(const std::vector<Character> &chars, int page, int selected) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// draw the background
	drawRect(screen, Point(24, 233), 208, 124, SDL_MapRGB(screen->format, 134, 118, 69));
	
	// draw top info bar borders
	drawRect(screen, Point(24, 233), 208, 20, SDL_MapRGB(screen->format, 252, 249, 244));
	drawRect(screen, Point(25, 234), 207, 19, SDL_MapRGB(screen->format, 168, 167, 163));
	
	// draw the top info bar
	drawRect(screen, Point(26, 235), 204, 16, SDL_MapRGB(screen->format, 55, 55, 55));
	
	// draw buttons
	drawImage(Point(1, 253), "tc_large_btn_left");
	drawImage(Point(239, 253), "tc_large_btn_right");
	
	// draw arrows on buttons if there is more than one page
	if (chars.size()>8) {
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
		drawRect(screen, Point(x, y), 39, 39, SDL_MapRGB(screen->format, 211, 197, 148));
		
		// draw filled center
		drawRect(screen, Point(x+2, y+2), 35, 35, SDL_MapRGB(screen->format, 134, 118, 69));
		
		// see if there is a profile at this slot
		if (index<=chars.size()-1 && !chars.empty()) {
			Character c=chars[index];
			
			// if this is the selected profile, write his name in the info
			// bar, and draw selection box
			if (i==selected) {
				// get the name of this character
				std::string name=c.getName();
				
				// calculate string length for this string
				int width=Fonts::getWidth("orange", name);
				
				// find the center x position for the string
				int centerx=(int) floor((204-width)/2);
				
				// draw the string at this position
				Fonts::drawString(24+centerx, 238, name, "orange");
				
				// draw selection box
				drawRect(screen, Point(x-1, y-1), 42, 42, SDL_MapRGB(screen->format, 255, 255, 255));
			}
			
			// draw profile thumbnail over the empty slot borders
			SDL_Rect drect;
			drect.x=x;
			drect.y=y;
			SDL_BlitSurface(c.getHeadshotThumb(), NULL, screen, &drect);
			
			index++;
		}
		
		// advance to next slot
		x+=40+8;
		
		// reset for next row
		if (i==3) {
			y=305;
			x=24+12;
		}
	}
}

// draw the profile info page
void Renderer::drawProfileInfoPage(UI::Manager *manager, const std::vector<Character> &chars, int index) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// get character to draw
	Character c=chars[index];
	
	// keep track of y changes
	int x=0;
	int y=206;
	
	// draw transparent background
	SDL_Surface *opaqueBlack=Textures::queryTexture("opaque_black");
	SDL_SetAlpha(opaqueBlack, SDL_SRCALPHA, 80); // set transparent alpha value
	drawImage(Point(0, 197), opaqueBlack);
	
	// draw info strip background
	drawRect(screen, Point(0, y+25), 256, 77, SDL_MapRGB(screen->format, 134, 118, 69));
	drawRect(screen, Point(8, y+101), 240, 52, SDL_MapRGB(screen->format, 134, 118, 69));
	
	// draw upper border
	drawRect(screen, Point(0, y+25), 256, 6, SDL_MapRGB(screen->format, 218, 218, 218));
	y+=31;
	
	// draw button on left
	drawImage(Point(0, y+4), "tc_small_btn_left");
	manager->drawAnimation("an_info_page_button_left");
	
	// draw the profile
	drawImage(Point(19, y), c.getHeadshot());
	x+=92;
	
	// draw info box's border
	drawRect(screen, Point(x, y), 148, 70, SDL_MapRGB(screen->format, 255, 255, 255));
	
	// draw info box title bar
	drawRect(screen, Point(x+2, y+2), 144, 15, SDL_MapRGB(screen->format, 55, 55, 55));
	
	// calculate center position for name
	int centerx=(x+72)-(Fonts::getWidth("orange", c.getName())/2);
	
	// draw evidence name in title bar
	Fonts::drawString(centerx, y+4, c.getName(), "orange");
	
	// draw info box body
	drawRect(screen, Point(x+2, y+17), 144, 51, SDL_MapRGB(screen->format, 153, 192, 145));
	
	// draw character caption in this area
	Fonts::drawTTF(Point(x+5, y+18), c.getCaption());
	
	// moving right along...
	x+=148;
	
	// draw button with arrow on right
	drawImage(Point(x+3, y+4), "tc_small_btn_right");
	manager->drawAnimation("an_info_page_button_right");
	
	// draw lower border
	drawRect(screen, Point(0, 308), 256, 6, SDL_MapRGB(screen->format, 218, 218, 218));
	
	// draw evidence description in bottom area
	Fonts::drawString(16, y+81, c.getDescription(), "white");
}

// draw the examination scene
void Renderer::drawExamineScene(const Point &cursor) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
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
void Renderer::drawMoveScene(const std::vector<std::string> &locations, LocationMap lmap, int selected) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// keep track of x,y changes
	int x=256/3;
	int y=236;
	
	// go over locations
	for (int i=0; i<locations.size(); i++) {
		Case::Location location=lmap[locations[i]];
		
		// see if this is the selected button
		if (selected==i) {
			// draw a gold border around it
			Renderer::drawRect(screen, Point(x, y), 150, 20, SDL_MapRGB(screen->format, 255, 148, 57));
			
			// draw the preview to the left
			Renderer::drawImage(Point(0, 263), location.bgScaled);
		}
		
		// otherwise, draw only a white border
		else
			Renderer::drawRect(screen, Point(x, y), 150, 20, SDL_MapRGB(screen->format, 189, 148, 132));
		
		// draw the button
		Renderer::drawRect(screen, Point(x+1, y+1), 148, 18, SDL_MapRGB(screen->format, 255, 255, 255));
		
		// draw the string
		int centerx=(x+(150/2))-(Fonts::getTTFWidth(location.name)/2)-4;
		Fonts::drawTTF(Point(centerx, y+1), location.name);
		
		// increment y
		y+=25;
	}
}

// draw talk scene
void Renderer::drawTalkScene(const std::vector<StringPair> &options, int selected, bool centered) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// keep track of drawing
	int x=5;
	int y=236;
	
	// if centering was requested, set a center x point
	if (centered)
		x=28;
	
	// iterate over options
	for (int i=0; i<options.size(); i++) {
		// if this is the selected option, draw a gold border around it
		if (selected==i)
			Renderer::drawRect(screen, Point(x, y), 200, 20, SDL_MapRGB(screen->format, 255, 148, 57));
			
		// otherwise, draw a white border
		else
			Renderer::drawRect(screen, Point(x, y), 200, 20, SDL_MapRGB(screen->format, 189, 148, 132));
		
		// draw the button
		Renderer::drawRect(screen, Point(x+1, y+1), 200-2, 18, SDL_MapRGB(screen->format, 255, 255, 255));
		
		// calculate length of string
		int length=Fonts::getTTFWidth(options[i].first);
		
		// draw the text, centered on the button
		int centerx;
		if (centered)
			centerx=128-(length/2);
		else
			centerx=100-(length/2);
		
		Fonts::drawTTF(Point(centerx, y+1), options[i].first);
		
		// move down to next slot
		y+=25;
	}
}
