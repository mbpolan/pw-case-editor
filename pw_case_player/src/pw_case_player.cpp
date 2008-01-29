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
// pw_case_player.cpp: main file

#include <iostream>
#include "SDL.h"

#include "application.h"
#include "common.h"

int main(int argc, char *argv[]) {
	// we need to enable some sort of error reporting mechanism in case of
	// a crash on Windows
#ifdef __WIN32__
	signal(SIGSEGV, onSigSegv);
#endif
	
	// check arguments or display help message
	if (argc<2) {
		std::cout << "Phoenix Wright Case Player " << Application::VERSION << "\n\n";
		std::cout << "Usage: pw_case_player [OPTION]... [PATH TO CASE FILE]\n";
		std::cout << "Plays the provided case.\n";
		std::cout << "\nAvailable options:\n";
		std::cout << "  -ns,  --no-sound\tDisables audio output\n";
		std::cout << "  -d,   --debug   \tEnables debug messages\n";
		std::cout << "\n";
		std::cout << "Official website: http://pw-case-editor.sourceforge.net\n";
		return 0;
	}
	
	// create application object
	Application app(argc, argv);
	
	// run the application
	app.run();
	
	return 0;
}
