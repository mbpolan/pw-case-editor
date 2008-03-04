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
// intl.cpp: Implementation of Intl namespace

#include <cstdio>

#include "intl.h"
#include "iohandler.h"

// set defaults
namespace Intl {

Glib::ustring g_Language="en";
std::map<Glib::ustring, Glib::ustring> g_Translations;

}

// set the language to use
bool Intl::set_language(const Glib::ustring &str) {
	// set the right path
	Glib::ustring path="lang/";
	path+=str;
	path+=".trs";
	
	// set the global language variable
	g_Language=str;
	
	// load the translation file
	return (IO::load_translation_file(path, Intl::g_Translations)==IO::CODE_OK ? true : false);
}

// translate an English string to its translated counterpart
Glib::ustring Intl::translate(const Glib::ustring &str) {
	Glib::ustring gstr=str;
	
	// don't bother translating if english is our default language
	if (g_Language=="en") {
		// if the string has a defining tag (such as [verb], [noun], etc), get rid of it
		int npos=gstr.find('[');
		if (npos!=-1)
			gstr.erase(npos, npos-(gstr.rfind(']')+1));
		
		return gstr;
	}
	
	// find the translation
	for (std::map<Glib::ustring, Glib::ustring>::iterator it=g_Translations.begin(); it!=g_Translations.end(); ++it) {
		if ((*it).first==gstr) {
			// defined, but not translated
			if ((*it).second.empty()) {
				g_message("Warning: %s not translated", gstr.c_str());
				return str;
			}
			
			return (*it).second;
		}
	}
	
	g_message("No match: %s", str.c_str());
	return str;
}
