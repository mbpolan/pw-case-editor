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
// tooltips.cpp: implementation of tooltip strings

#include "tooltips.h"

namespace Tooltips {

// define our map
std::map<Glib::ustring, Glib::ustring> g_TipMap;

// register some default tooltips
void register_tooltips() {
	// for TestimonyEditor class
	g_TipMap["TestimonyEditor::m_IdEntry"]="Internal ID used to reference this testimony from within the script.";
	g_TipMap["TestimonyEditor::m_TitleEntry"]="The title that will be displayed when either viewing this testimony or cross examining it.";
}

} // namespace ToolTips
