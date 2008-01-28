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
// frontend_gtk.cpp: the main file for frontend

#include <gtkmm/main.h>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
	Gtk::Main app(argc, argv);
	
	MainWindow mw;
	mw.show();
	
	// run the main window
	app.run(mw);
	
	// get the requested params
	PlayerParams *p=mw.get_params();
	if (p) {
		// first, we form the command string, depending on operating system
		Glib::ustring command;
#ifdef __WIN32__
		command="pw_case_player.exe ";
#else
		command="./pw_case_player ";
#endif
		
		// now run the command
		system(Glib::ustring(command+p->params+p->path).c_str());
	}
	
	return 0;
}
