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

// get the current working directory
Glib::ustring Utils::FS::cwd() {
#ifndef __WIN32__
	char path[255];
	return Glib::ustring(getcwd(path, 255))+"/";
#else
	// TODO: Windows code for getting working directory
#endif
}

// move a file on the filesystem
void Utils::FS::move(const Glib::ustring &from, Glib::ustring &to) {
	Glib::ustring cmd;
#ifndef __WIN32__
	cmd="mv ";
#else
	cmd="move ";
#endif
	cmd+=from+" "+to;
	
	system(cmd.c_str());
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
void Utils::FS::mkdir(const Glib::ustring &path) {
	// no point in recreating an already existing directory
	if (!dir_exists(path)) {
		Glib::ustring cmd="mkdir ";
		cmd+=path;
		system(cmd.c_str());
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
#else
		// TODO: Windows code for removing directory
#endif
		
		system(cmd.c_str());
	}
}

// flush gui events that may still be pending in the main loop
void Utils::flush_events() {
	while(Gtk::Main::events_pending())
		Gtk::Main::iteration();
}

// format an exception string
Glib::ustring Utils::exceptionString(const Glib::ustring &reason, const Glib::ustring &file, int line) {
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
