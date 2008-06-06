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

#include <glibmm/timer.h>
#include <glibmm/thread.h>
#include <gtkmm/main.h>

#include "config.h"
#include "iohandler.h"
#include "intl.h"
#include "mainwindow.h"
#include "splashscreen.h"
#include "tooltips.h"
#include "utilities.h"

#ifdef __WIN32__
#include "stdafx.h"
#endif

// routine for splash screen thread
void on_thread(SplashScreen *sp) {
	Glib::usleep(1500000);
	sp->hide();
}

// the "famous" entry point for the program
// FIXME: use of stdafx on Windows is not needed for GTK programs; get rid of this later
#ifdef __WIN32__
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	int argc=1;
	char *argv[]={""};
#else
int main(int argc, char *argv[]) {
#endif

	// before we go anywhere, we need to unpack the resource file
	if (IO::unpack_resource_file(Utils::FS::cwd()+"dat.dpkg")!=IO::CODE_OK) {
		// FIXME: maybe show a message box of some sort here?
		g_message("Unable to unpack resource file!");
		return 0;
	}
	
	// now load the configuration file
	Config::File cfile;
	IO::load_config_file("editor.cfg", cfile);
	
	// create a new instance of the configuration manager
	Config::Manager::create(cfile);
	
	// load the language file, if it's not english
	if (!Intl::set_language(Config::Manager::instance()->get_language()))
		Intl::set_language("en");
	
	// load default case blocks
	IO::load_default_blocks(Config::Manager::instance()->get_language());
	
	// initialize threads and gtkmm
	Glib::thread_init();
	Gtk::Main app(&argc, NULL);
	
	// register tooltips for application
	Tooltips::register_tooltips();
	
	Gtk::Window::set_auto_startup_notification(false);
	
	// set up the splash screen
	SplashScreen sp(Utils::FS::cwd()+".temp/splash.png");
	sp.show();
	
	Gtk::Window::set_auto_startup_notification(true);
	
	// clear out the main loop
	Utils::flush_events();
	
	// finally, show the main window
	MainWindow mw;
	mw.show();
	
	// we want the splash screen to stay shown a little bit longer
	const Glib::Thread *sthread=Glib::Thread::create(sigc::bind(sigc::ptr_fun(on_thread), &sp), false);
	
	app.run(mw);
	
	// get the new values for the configuration file and save it
	cfile=Config::Manager::instance()->serialize();
	IO::save_config_file("editor.cfg", cfile);
	
	// delete the instance of the config manager
	delete Config::Manager::instance();
	
	return 0;
}
