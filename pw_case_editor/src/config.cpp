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
// config.cpp: implementation of Config namespace

#include "config.h"

Config::Manager *g_Manager=NULL;

// create an instance of this manager
Config::Manager* Config::Manager::create(const Config::File &file) {
	g_Manager=new Manager(file);
	return g_Manager;
}

// get a pointer to an instance of this manager
Config::Manager* Config::Manager::instance() {
	return g_Manager;
}

// save values into a Config::File struct
Config::File Config::Manager::serialize() {
	File cfg;
	cfg.language=m_Language;
	
	return cfg;
}

// constructor
Config::Manager::Manager(const Config::File &file) {
	m_Language=file.language;
}
