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
// utilities.h: various extra functions

#ifndef UTILITIES_H
#define UTILITIES_H

#include <QString>

/// Namespace for Utilities functions
namespace Utils {

/** Extract the root ID of a block
  * \param id The block's ID
  * \return Extracted block ID
*/
QString extractID(const QString &id);

/** Extract a description string from a block name
  * \param name The block name
  * \return The block's description
*/
QString extractDescription(const QString &name);

/** Extract the block ID, minus its description
  * \param name The full block name
  * \return The block ID
*/
QString stripDescription(const QString &name);

} // namespace Utils

#endif
