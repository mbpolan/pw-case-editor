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
// intl.h: Internationalization functions

#ifndef INTL_H
#define INTL_H

#include <map>

#include "common.h"

/// Namespace for internationalization functions
namespace Intl {

/// Macro to simplify marking strings for translation
#define _(x) Intl::translate(x)

/// The current language, set as ISO letter code (pl, jp, etc)
extern ustring g_Language;

/// Map of translations
extern std::map<ustring, ustring> g_Translations;

/** Set the language to use
  * \param language String containing ISO letter code of the requested language
  * \return <b>true</b> if the language can be used, <b>false</b> otherwise
*/
bool setLanguage(const ustring &str);

/** Map and English key string to its translated counterpart
  * \param str The string to translate
  * \return The translated string, or if the string has no translation, the original is returned
*/
ustring translate(const ustring &str);

} // namespace Intl

#endif
