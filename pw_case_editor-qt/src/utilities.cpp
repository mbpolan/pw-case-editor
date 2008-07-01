/***************************************************************************
 *   Copyright (C) 2008 by Mike Polan                                      *
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
// utilities.cpp: implementation of Utilities namespace

#include "utilities.h"

// extract the root ID of a block
QString Utils::extractID(const QString &id) {
	int i=id.indexOf("_");
	if (i==-1)
		return id;
	
	return id.left(i);
}

// extract a description from block name
QString Utils::extractDescription(const QString &name) {
	int npos=name.lastIndexOf("(");
	if (npos==-1)
		return "";
	
	return name.mid(npos+1, (name.lastIndexOf(")")-(npos+1)));
}

// strip a block name of its description
QString Utils::stripDescription(const QString &name) {
	int npos=name.indexOf("(");
	if (npos==-1)
		return name;
	
	return name.left(npos-1);
}
