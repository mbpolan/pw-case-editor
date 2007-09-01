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

#include "utilities.h"

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
