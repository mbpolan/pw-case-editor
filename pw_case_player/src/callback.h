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
// callback.h: C++ typesafe callback templates

#ifndef CALLBACK_H
#define CALLBACK_H

/// Callback with one parameter
template <class Object, typename RetType, typename Param1=void>
class Callback {
	public:
		/// Typedef'd slot for cleaner code
		typedef RetType (Object::*Slot)(Param1);
		
		/** Constructor
		 * \param caller The object which is responsible for this callback
		 * \param slotFunc Function to bind to
		*/
		Callback(Object *caller, Slot slotFunc) {
			m_Caller=caller;
			m_Slot=slotFunc;
		}
		
		/** Emits the callback
		  * \param param1 The value for the first parameter
		*/
		RetType emit(Param1 param1) {
			return (m_Caller->*m_Slot)(param1);
		}
		
	private:
		Object *m_Caller;
		Slot m_Slot;
};

/// Callback with no parameters
template <class Object, typename RetType>
class Callback<Object, RetType> {
	public:
		/// Typedef'd slot for cleaner code
		typedef RetType (Object::*Slot)();
		
		/** Constructor
		  * \param caller The object which is responsible for this callback
		  * \param slotFunc Function to bind to
		*/
		Callback(Object *caller, Slot slotFunc) {
			m_Caller=caller;
			m_Slot=slotFunc;
		}
		
		/// Emits the callback
		RetType emit() {
			return (m_Caller->*m_Slot)();
		}
		
	private:
		Object *m_Caller;
		Slot m_Slot;
};

#endif
