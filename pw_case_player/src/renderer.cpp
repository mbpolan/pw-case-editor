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

// draw a colored rectangle
void Renderer::drawRect(SDL_Surface *surface, int x, int y, int w, int h, Uint32 color) {
	SDL_Rect rect;
	rect.x=x;
	rect.y=y;
	rect.w=w;
	rect.h=h;
	
	SDL_FillRect(surface, &rect, color);
}

// draw a full image at a point
void Renderer::drawImage(int x, int y, const std::string &texId) {
	// get the texture in question
	SDL_Surface *tex=Textures::queryTexture(texId);
	if (!tex) {
		std::cout << "Renderer: texture '" << texId << "' not found in stack.\n";
		return;
	}
	
	// draw this image
	drawImage(x, y, tex);
}

// draw a full image at a point
void Renderer::drawImage(int x, int y, SDL_Surface *texture) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// prepare destination
	SDL_Rect destRect;
	destRect.x=x;
	destRect.y=y;
	
	// blit the surfaces
	SDL_BlitSurface(texture, NULL, screen, &destRect);
}

// draw a textured quad
void Renderer::drawImage(int x1, int y1, int w, int h, int x2, int y2, const std::string &texId) {
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
	srcRect.x=x1;
	srcRect.y=y1;
	srcRect.w=w;
	srcRect.h=h;
	
	destRect.x=x2;
	destRect.y=y2;
	
	// blit the image
	SDL_BlitSurface(tex, &srcRect, screen, &destRect);
}

// draw the evidence page
void Renderer::drawEvidencePage(const std::vector<Case::Evidence> &evidence, int page, int selected) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// draw the title bar
	Renderer::drawImage(0, 197+9, "tc_evidence_bar");
	
	// draw the background
	drawRect(screen, 24, 233, 208, 124, SDL_MapRGB(screen->format, 111, 86, 56));
	
	// draw top info bar borders
	drawRect(screen, 24, 233, 208, 20, SDL_MapRGB(screen->format, 252, 249, 244));
	drawRect(screen, 25, 234, 207, 19, SDL_MapRGB(screen->format, 168, 167, 163));
	
	// draw the top info bar
	drawRect(screen, 26, 235, 204, 16, SDL_MapRGB(screen->format, 55, 55, 55));
	
	// draw buttons
	drawImage(1, 253, "tc_large_btn_left");
	drawImage(256-16-1, 253, "tc_large_btn_right");
	
	// draw arrows on buttons if there is more than one page
	if (evidence.size()>8) {
		drawImage(4, 297, "tc_button_arrow_left");
		drawImage(256-12, 297, "tc_button_arrow_right");
	}
	
	// get the starting index for the vector
	int index=8*page;
	
	// draw 1st row of evidence slots
	int x=24+12;
	int y=259;
	for (int i=0; i<8; i++) {
		// draw the border
		drawRect(screen, x, y, 39, 39, SDL_MapRGB(screen->format, 145, 121, 93));
		
		// draw filled center
		drawRect(screen, x+2, y+2, 35, 35, SDL_MapRGB(screen->format, 117, 92, 62));
		
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
				drawRect(screen, x-1, y-1, 42, 42, SDL_MapRGB(screen->format, 255, 255, 255));
			}
			
			// draw evidence thumbnail over the empty slot borders
			SDL_Rect drect;
			drect.x=x;
			drect.y=y;
			SDL_BlitSurface(e.thumb, NULL, screen, &drect);
			
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

// draw evidence information page
void Renderer::drawEvidenceInfoPage(const std::vector<Case::Evidence> &evidence, int index) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// draw the title bar
	Renderer::drawImage(0, 197+9, "tc_evidence_bar");
	
	// get evidence to draw
	Case::Evidence e=evidence[index];
	
	// keep track of y changes
	int x=0;
	int y=197+9;
	
	// draw transparent background
	SDL_Surface *opaqueBlack=Textures::queryTexture("opaque_black");
	SDL_SetAlpha(opaqueBlack, SDL_SRCALPHA, 80); // set transparent alpha value
	drawImage(0, 197, opaqueBlack);
	
	// draw info strip background
	drawRect(screen, 0, y+25, 256, 70+6, SDL_MapRGB(screen->format, 111, 86, 56));
	
	// draw upper border
	drawRect(screen, 0, y+25, 256, 6, SDL_MapRGB(screen->format, 218, 218, 218));
	y+=25+6;
	
	// draw button on left
	drawImage(0, y, "tc_small_btn_left");
	drawImage(3, y+26, "tc_button_arrow_left");
	
	// draw the evidence
	drawImage(19, y, e.texture);
	x+=19+70+3;
	
	// draw info box's border
	drawRect(screen, x, y, 148, 70, SDL_MapRGB(screen->format, 255, 255, 255));
	
	// draw info box title bar
	drawRect(screen, x+2, y+2, 144, 15, SDL_MapRGB(screen->format, 55, 55, 55));
	
	// calculate center position for name
	int centerx=(Fonts::getWidth("orange", e.name)/3)+x;
	
	// draw evidence name in title bar
	Fonts::drawString(centerx, y+4, e.name, "orange");
	
	// draw info box body
	drawRect(screen, x+2, y+2+15, 144, 45, SDL_MapRGB(screen->format, 153, 192, 145));
	
	// draw evidence caption in this area
	Fonts::drawString(x+4, y+2+16, -1, x+2+145, e.caption, "black");
	
	// moving right along...
	x+=148;
	
	// draw button with arrow on right
	drawImage(x+3, y, "tc_small_btn_right");
	drawImage(x+7, y+26, "tc_button_arrow_right");
	
	// draw lower border
	drawRect(screen, 0, 301, 256, 6, SDL_MapRGB(screen->format, 218, 218, 218));
	
	// draw evidence description in bottom area
	Fonts::drawString(20, y+75, e.description, "white");
}

// draw the profiles page
void Renderer::drawProfilesPage(const std::vector<Character> &chars, int page, int selected) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// draw the title bar
	Renderer::drawImage(0, 197+9, "tc_profiles_bar");
	
	// draw the background
	drawRect(screen, 24, 233, 208, 124, SDL_MapRGB(screen->format, 111, 86, 56));
	
	// draw top info bar borders
	drawRect(screen, 24, 233, 208, 20, SDL_MapRGB(screen->format, 252, 249, 244));
	drawRect(screen, 25, 234, 207, 19, SDL_MapRGB(screen->format, 168, 167, 163));
	
	// draw the top info bar
	drawRect(screen, 26, 235, 204, 16, SDL_MapRGB(screen->format, 55, 55, 55));
	
	// draw buttons
	drawImage(1, 253, "tc_large_btn_left");
	drawImage(256-16-1, 253, "tc_large_btn_right");
	
	// draw arrows on buttons if there is more than one page
	if (chars.size()>8) {
		drawImage(4, 297, "tc_button_arrow_left");
		drawImage(256-12, 297, "tc_button_arrow_right");
	}
	
	// get the starting index for the vector
	int index=8*page;
	
	// draw 1st row of profile slots
	int x=24+12;
	int y=259;
	for (int i=0; i<8; i++) {
		// draw the border
		drawRect(screen, x, y, 39, 39, SDL_MapRGB(screen->format, 145, 121, 93));
		
		// draw filled center
		drawRect(screen, x+2, y+2, 35, 35, SDL_MapRGB(screen->format, 117, 92, 62));
		
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
				drawRect(screen, x-1, y-1, 42, 42, SDL_MapRGB(screen->format, 255, 255, 255));
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
void Renderer::drawProfileInfoPage(const std::vector<Character> &chars, int index) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// draw the title bar
	Renderer::drawImage(0, 197+9, "tc_profiles_bar");
	
	// get character to draw
	Character c=chars[index];
	
	// keep track of y changes
	int x=0;
	int y=197+9;
	
	// draw transparent background
	SDL_Surface *opaqueBlack=Textures::queryTexture("opaque_black");
	SDL_SetAlpha(opaqueBlack, SDL_SRCALPHA, 80); // set transparent alpha value
	drawImage(0, 197, opaqueBlack);
	
	// draw info strip background
	drawRect(screen, 0, y+25, 256, 70+6, SDL_MapRGB(screen->format, 111, 86, 56));
	
	// draw upper border
	drawRect(screen, 0, y+25, 256, 6, SDL_MapRGB(screen->format, 218, 218, 218));
	y+=25+6;
	
	// draw button on left
	drawImage(0, y, "tc_small_btn_left");
	drawImage(3, y+26, "tc_button_arrow_left");
	
	// draw the profile
	drawImage(19, y, c.getHeadshot());
	x+=19+70+3;
	
	// draw info box's border
	drawRect(screen, x, y, 148, 70, SDL_MapRGB(screen->format, 255, 255, 255));
	
	// draw info box title bar
	drawRect(screen, x+2, y+2, 144, 15, SDL_MapRGB(screen->format, 55, 55, 55));
	
	// calculate center position for name
	int centerx=(Fonts::getWidth("orange", c.getName())/3)+x;
	
	// draw evidence name in title bar
	Fonts::drawString(centerx, y+4, c.getName(), "orange");
	
	// draw info box body
	drawRect(screen, x+2, y+2+15, 144, 45, SDL_MapRGB(screen->format, 153, 192, 145));
	
	// draw character caption in this area
	Fonts::drawString(x+4, y+2+16, -1, x+2+145, c.getCaption(), "black");
	
	// moving right along...
	x+=148;
	
	// draw button with arrow on right
	drawImage(x+3, y, "tc_small_btn_right");
	drawImage(x+7, y+26, "tc_button_arrow_right");
	
	// draw lower border
	drawRect(screen, 0, 301, 256, 6, SDL_MapRGB(screen->format, 218, 218, 218));
	
	// draw character description in bottom area
	Fonts::drawString(20, y+75, c.getDescription(), "white");
}

// draw the examination scene
void Renderer::drawExamineScene(SDL_Surface *background, int cursorX, int cursorY) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// get opaque screen and make it completely transparent
	SDL_Surface *overlay=Textures::queryTexture("transparent");
	SDL_FillRect(overlay, NULL, 0);
	
	// draw crosshairs
	vlineRGBA(overlay, cursorX, 0, 192, 0, 0, 255, 200);
	hlineRGBA(overlay, 0, 256, cursorY, 0, 0, 255, 200);
	
	// draw center rectangle
	rectangleRGBA(overlay, cursorX-6, cursorY-6, cursorX+6, cursorY+6, 0, 0, 255, 200);
	
	// draw overlay
	Renderer::drawImage(0, 197, overlay);
}

// draw the movement scene
void Renderer::drawMoveScene(const std::vector<std::string> &locations, LocationMap lmap, int selected) {
	// get pointer to screen surface
	SDL_Surface *screen=SDL_GetVideoSurface();
	if (!screen)
		return;
	
	// keep track of x,y changes
	int x=256/3;
	int y=197+34+5;
	
	// go over locations
	for (int i=0; i<locations.size(); i++) {
		Case::Location location=lmap[locations[i]];
		
		// see if this is the selected button
		if (selected==i) {
			// draw a gold border around it
			Renderer::drawRect(screen, x, y, 150, 20, SDL_MapRGB(screen->format, 255, 148, 57));
			
			// draw the preview to the left
			Renderer::drawImage(0, 263, location.bgScaled);
		}
		
		// otherwise, draw only a white border
		else
			Renderer::drawRect(screen, x, y, 150, 20, SDL_MapRGB(screen->format, 189, 148, 132));
		
		// draw the button
		Renderer::drawRect(screen, x+1, y+1, 150-2, 18, SDL_MapRGB(screen->format, 255, 255, 255));
		
		// draw the string
		int centerx=(x+(150/2))-(Fonts::getWidth("black", location.name)/2)-4;
		Fonts::drawString(centerx, y+4, location.name, "black");
		
		// increment y
		y+=25;
	}
}
