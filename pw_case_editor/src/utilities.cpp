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
// utilities.cpp: implementation of Util namespace

#include <cstdlib>
#include <dirent.h>
#include <glibmm.h>
#include <gtkmm/main.h>
#include <sstream>

#include "utilities.h"

#ifdef __WIN32__
#include "stdafx.h"
#include <shellapi.h>
#endif

// get the current working directory
Glib::ustring Utils::FS::cwd() {
	// NOTE: this function is kinda useless since the current directory will be
	// automatically detected in 99.9% of cases. maybe deprecate this function
	// in the future
	return "";
}

// move a file on the filesystem
void Utils::FS::move(const Glib::ustring &from, Glib::ustring &to) {
	Glib::ustring cmd;

#ifdef __WIN32__
	MoveFileA(from.c_str(), to.c_str());
#else
	cmd="mv ";
	cmd+=from+" "+to;
	
	system(cmd.c_str());
#endif
}

// check if a directory exists
bool Utils::FS::dir_exists(const Glib::ustring &path) {
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
void Utils::FS::make_dir(const Glib::ustring &path) {
	// no point in recreating an already existing directory
	if (!dir_exists(path)) {
		
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
void Utils::FS::remove_dir(const Glib::ustring &path) {
	// make sure that the directory even exists
	if (dir_exists(path)) {
		Glib::ustring cmd;
		
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

// flush gui events that may still be pending in the main loop
void Utils::flush_events() {
	while(Gtk::Main::events_pending())
		Gtk::Main::iteration();
}

// calculate center of screen coordinates for a window
void Utils::calculate_center(int width, int height, int &x, int &y) {
	// we assume the default screen
	Glib::RefPtr<Gdk::Screen> screen=Gdk::Screen::get_default();
	
	x=(screen->get_width()/2)-(width/2);
	y=(screen->get_height()/2)-(height/2);
}

// convert an I/O error code into a string error message
Glib::ustring Utils::io_error_to_str(const IO::Code &code) {
	switch(code) {
		case IO::CODE_OK: return ""; break; // this shouldn't be handled
		case IO::CODE_OPEN_FAILED: return " Unable to open file."; break;
		case IO::CODE_WRONG_MAGIC_NUM: return " File's header is incorrect or corrupt."; break;
		case IO::CODE_WRONG_VERSION: return " This file version is not supported."; break;
		case IO::CODE_VALIDATE_FAILED: return " The file is corrupt and cannot be opened."; break;
	}
}

// format an exception string
Glib::ustring Utils::exception_string(const Glib::ustring &reason, const Glib::ustring &file, int line) {
	std::stringstream ss;
	ss << "Fatal exception: " << reason << ".";
	ss << "\nFile: ";
	ss << file;
	ss << "\nLine: ";
	ss << line;
	ss << "\nThe program will now terminate.";
	
	return ss.str();
}

// convert int to string
Glib::ustring Utils::to_string(int val) {
	std::stringstream ss;
	ss << val;
	
	return ss.str();
}

// extract a text block's id from a full string
Glib::ustring Utils::extract_block_id(const Glib::ustring str) {
	Glib::ustring fstr=str;
	
	int npos1=fstr.find("(")-1;
	return fstr.substr(0, npos1);
}

// extract a text block's description from a full string
Glib::ustring Utils::extract_block_description(const Glib::ustring &str) {
	Glib::ustring fstr=str;
	
	if (fstr.find("()")!=-1)
		return "";
	
	int npos1=fstr.find("(");
	int npos2=fstr.find(")");
	
	if (npos1==-1 || npos2==-1)
		return fstr;
	
	Glib::ustring desc=fstr.substr(npos1+1, npos2);
	
	if (desc.find(")")!=-1)
		desc.erase(desc.size()-1, desc.size());
	
	return desc;
}
