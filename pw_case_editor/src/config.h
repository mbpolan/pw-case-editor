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
// config.h: various configuration file stuff

#ifndef CONFIG_H
#define CONFIG_H

#include <glibmm/ustring.h>
#include <map>

/// Namespace for all configuration related code
namespace Config {

/** Struct used to store values to save to the config file
*/
struct _File {
	/// The language to set for the editor
	Glib::ustring language;
	
	/// Map of keys and values
	std::map<Glib::ustring, Glib::ustring> keys;
};
typedef struct _File File;

/** The manager class for the configuration of the editor.
  * This class exists as a stand alone object, which can be
  * accessed via the static Config::Manager::instance() function
*/
class Manager {
	public:
		/** Create a new config file manager
		  * \param file A completed File struct
		  * \return A pointer to the new Manager instance
		*/
		static Manager* create(const File &file);
		
		/** Get the single instance of this config manager
		  * \return Pointer to an instance of this class
		*/
		static Manager* instance();
		
		/** Save all values into a Config::File struct
		  * \return Completed Config::File struct
		*/
		File serialize();
		
		/** Set the language to use in the config file
		  * \param lang ISO code of language to set
		*/
		void set_language(const Glib::ustring &lang) { m_Language=lang; }
		
		/** Get the language set by the config file
		  * \return ISO code of language
		*/
		Glib::ustring get_language() const { return m_Language; }
		
		/** Set or edit a key in the config
		  * \param key The key to create or edit
		  * \param value The value to assign
		*/
		void set_key(const Glib::ustring &key, const Glib::ustring &value) { m_Keys[key]=value; }
		
		/** Get a key from the config
		  * \param key The key whose value to return
		*/
		Glib::ustring get_key(const Glib::ustring &key) { return (m_Keys.find(key)==m_Keys.end() ? "-1" : m_Keys[key]); }
		
	private:
		/// Hidden constructor
		Manager(const File &file);
		
		/// The language set in the config file
		Glib::ustring m_Language;
		
		/// Map of key:value pairs
		std::map<Glib::ustring, Glib::ustring> m_Keys;
};

}; // namespace Config

#endif
