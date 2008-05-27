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

#include <cmath>
#include <dirent.h>
#include <sstream>

// include windows.h for directory/file management functions
#ifdef __WIN32__
#include <windows.h>
#include <tchar.h>
#include <shellapi.h>
#endif

#include "font.h"
#include "utilities.h"

namespace Utils {
	bool g_DebugOn=false;
	bool g_IDebugOn=false;
}

// get the current working directory
ustring Utils::FS::cwd() {
	// NOTE: this function is currently useless, but that may not
	// be the case in the future. maybe deprecate it if there is
	// no potential use for this
	return "";
}

// move a file on the filesystem
void Utils::FS::move(const ustring &from, const ustring &to) {
	ustring cmd;
#ifndef __WIN32__
	cmd="mv ";
	cmd+=from+" "+to;
	system(cmd.c_str());
#else
	MoveFile(from.c_str(), to.c_str());
#endif
}

// check if a directory exists
bool Utils::FS::dirExists(const ustring &path) {
	// simply try to open the directory
	DIR *dir=opendir(path.c_str());
	if (dir) {
		closedir(dir);
		return true;
	}
	
	else
		return false;
}

// create a directory
void Utils::FS::makeDir(const ustring &path) {
	// no point in recreating an already existing directory
	if (!dirExists(path)) {
		
#ifndef __WIN32__
		ustring cmd="mkdir ";
		cmd+=path;
		system(cmd.c_str());
#else
		CreateDirectory(path.c_str(), NULL);
#endif
	}
}

// remove a directory
void Utils::FS::removeDir(const ustring &path) {
	// make sure that the directory even exists
	if (dirExists(path)) {
		ustring cmd;
	
#ifndef __WIN32__
		cmd="rm -rf ";
		cmd+=path;
		system(cmd.c_str());
#else
		// for consistency and compatability...
		LPCTSTR lpszPath=path.c_str();
		int length=_tcslen(lpszPath);
		
		// this is our source field
		TCHAR *from=new TCHAR[length+2];
		_tcscpy(from, lpszPath);
		
		// SHFileOperation expects this to be double 0 terminated
		from[length]=0;
		from[length+1]=0;
		
		// fill in our file operations struct
		SHFILEOPSTRUCT op;
		op.hwnd=NULL;
		op.wFunc=FO_DELETE;
		op.pFrom=from;
		op.pTo=NULL; // we're deleting; no target needed
		op.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
		op.fAnyOperationsAborted=false;
		op.lpszProgressTitle=NULL;
		op.hNameMappings=NULL;
		
		// perform this file operation
		SHFileOperation(&op);
		delete [] from;
#endif
	}
}

// display a message through the console
void Utils::message(const ustring &text) {
	std::cout << text;
}

// display an alert message to the user
void Utils::alert(const ustring &text, const MessageType &type) {
#ifndef __WIN32__
	// first, format the prefix
	ustring prefix="** ";
	switch(type) {
		default:
		case MESSAGE_CRITICAL: prefix+="CRITICAL"; break;
		case MESSAGE_WARNING: prefix+="WARNING"; break;
	}
	prefix+=" ** ";
	
	// now include the message
	prefix+=text;
	
	std::cout << prefix << std::endl;
#else
	// figure out what type of message this is
	UINT mtype=MB_OK;
	switch(type) {
		default:
		case MESSAGE_CRITICAL: mtype |= MB_ICONERROR; break;
		case MESSAGE_WARNING: mtype |= MB_ICONWARNING; break;
	}
	
	// display the message box
	MessageBox(NULL, text.c_str(), "PW Case Player", mtype);
#endif
}

// calculate the nearest power of two
int Utils::nearestPower2(int val) {
	int power=1;
	while(power<val)
		power<<=1;
	return power;
}

// convert a court camera script string to animation limits
void Utils::scriptToLimits(const ustring &str, UI::Limit &start, UI::Limit &end) {
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

// map a char representing a text color to the actual color
Color Utils::keyToColor(char ch) {
	switch(ch) {
		default:
		case 'w': return Fonts::COLOR_WHITE; break;
		case 'c': return Fonts::COLOR_BLUE; break;
		case 'g': return Fonts::COLOR_GREEN; break;
		case 'o': return Fonts::COLOR_ORANGE; break;
	}
}

// print a debug message
void Utils::debugMessage(const ustring &msg) {
	// the last message displayed
	static ustring last="";
	
	if (Utils::g_DebugOn) {
		// prevent the same message from coming up repeatedly
		if (last!=msg) {
			alert(msg, MESSAGE_WARNING);
			last=msg;
		}
	}
}

// see if a point is in a rectangle
bool Utils::pointInRect(const Point &p, const Rect &rect) {
	Point corner;
	int w, h;
	rect.getGeometry(corner, w, h);
	
	if ((p.x()>=corner.x() && p.x()<=corner.x()+w) &&
	    (p.y()>=corner.y() && p.y()<=corner.y()+h))
		return true;
	
	else
		return false;
}

// calculate a random offset point (used during shake animations)
Point Utils::calculateShakePoint(int highestRadius) {
	// get a random angle and radii for x and y
	int alpha=Utils::randomRange(1, 360); // full circle shaking motion
	int rX=Utils::randomRange(1, highestRadius);
	int rY=Utils::randomRange(1, highestRadius);
	
	// and calculate x and y based on angle and provided radii
	int sx=cos(alpha)*rX;
	int sy=sin(alpha)*rY;
	
	// form a movement point
	return Point(sx, sy);
}

// set color masks
void Utils::setRGBAMasks(Uint32 &r, Uint32 &g, Uint32 &b, Uint32 &a) {
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	r=0x000000FF;
	g=0x0000FF00;
	b=0x00FF0000;
	a=0xFF000000;
#else
	r=0xFF000000;
	g=0x00FF0000;
	b=0x0000FF00;
	a=0x000000FF;
#endif
}

// get a random number in the provided range
int Utils::randomRange(int min, int max) {
	// programmer stupidity check
	if (min>max) {
		int tmp=max;
		max=min;
		min=tmp;
	}
	
	// ditto
	if (min==max)
		return min;
	
	// return a randomized number
	int num=(rand()%(max-min+1))+min;
	return num;
}

// get the location of the mouse pointer
Point Utils::getMouseLocation(Uint8 *modState) {
	int x, y;
	Uint8 state=SDL_GetMouseState(&x, &y);
	if (modState)
		(*modState)=state;
	return Point(x, y);
}

// create a blank surface
SDL_Surface* Utils::createSurface(int width, int height) {
	return SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0, 0, 0, 0);
}

// convert a uchar to a string
ustring Utils::ucharToStr(uchar ch) {
	ustring str="";
	str+=ch;
	return str;
}

// convert an integer to string
ustring Utils::itoa(int num) {
	std::stringstream ss;
	ss << num;
	return ss.str();
}

// convert a ustring to array of Uint16's
Uint16* Utils::ustringToArray(const ustring &str) {
	Uint16 *array=new Uint16[str.size()+1];
	for (int i=0; i<str.size(); i++)
		array[i]=str[i];
	array[str.size()]='\0';
	
	return array;
}

// break a string apart based on a delimiting string
StringVector Utils::explodeString(const ustring &delimiter, const ustring &strc) {
	ustring str=strc;
	StringVector split;
	
	do {
		// find next separator
		int npos=str.find(delimiter);
		if (npos==-1) {
			// erase to end for last parameter
			split.push_back(str.substr(0, str.size()));
			
			break;
		}
		
		// get the string
		ustring s=str.substr(0, npos);
		split.push_back(s);
		
		str.erase(0, npos+delimiter.size());
	} while(1);
	
	return split;
}

// break a string apart based on a delimiting character
StringVector Utils::explodeString(uchar delimiter, const ustring &strc) {
	ustring d;
	d+=delimiter;
	return explodeString(d, strc);
}

// count occurrences of a substring in a string
int Utils::amountInString(const ustring &sub, const ustring &str) {
	int c=0;
	int i=0;
	while((i=str.find(sub, i))!=-1) {
		c++;
	}
	
	return c;
}
