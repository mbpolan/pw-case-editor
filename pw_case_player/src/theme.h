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
// theme.h: theme functions

#ifndef THEME_H
#define THEME_H

#include <iostream>
#include <map>

#include "common.h"

/// Namespace for functions that handle the theme in the player
namespace Theme {

/// Typedef'd map for key,color combinations
typedef std::map<ustring, Color> ColorMap;

/// The global theme map
extern ColorMap g_Theme;

/** Get a color based on theme key
  * \param key The key to look up
  * \return The color from the theme
*/
Color lookup(const ustring &key);

}; // namespace Theme

#endif
