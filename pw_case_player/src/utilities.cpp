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
// utilities.cpp: implementation of Utils namespace

#include "utilities.h"

// convert a court camera script string to animation limits
void Utils::scriptToLimits(const std::string &str, UI::Limit &start, UI::Limit &end) {
	// first split the string
	StringVector vec=explodeString(',', str);
	
	// strings must not match
	if (vec[0]==vec[1])
		start=end=UI::LIMIT_NONE;
	
	// otherwise, compare strings and set appropriate limits
	for (int i=0; i<2; i++) {
		UI::Limit limit;
		
		if (vec[i]=="prosecutor_stand")
			limit=UI::LIMIT_PROSECUTOR_STAND;
		
		else if (vec[i]=="defense_stand")
			limit=UI::LIMIT_DEFENSE_STAND;
		
		else if (vec[i]=="witness_stand")
			limit=UI::LIMIT_WITNESS_STAND;
		
		else
			std::cout << "Warning: unknown animation limit: " << vec[i] << std::endl;
		
		// set the limit
		if (i==0)
			start=limit;
		else
			end=limit;
	}
}

// create a blank surface
SDL_Surface* Utils::createSurface(int width, int height) {
	return SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0, 0, 0, 0);
}

// break a string apart based on a delimiting character
StringVector Utils::explodeString(char delimiter, const std::string &strc) {
	std::string str=strc;
	StringVector split;
	
	while(1) {
		// find next separator
		int npos=str.find(delimiter);
		if (npos==-1) {
			// erase to end for last parameter
			split.push_back(str.substr(0, str.size()));
			
			break;
		}
		
		// get the string
		std::string s=str.substr(0, npos);
		split.push_back(s);
		
		str.erase(0, npos+1);
	}
	
	return split;
}
