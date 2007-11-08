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

#include <Magick++.h>
#include <glibmm/fileutils.h>
#include <gtkmm/main.h>
#include "dialogs.h"
#include "sprite.h"

// create a sprite from gifs in a directory
bool Sprite::create_from_gifs(const Glib::ustring &path) {
	// create dialog to show progress
	ProgressDialog pd("Creating sprite...");
	pd.show();
	
	// make sure to show the dialog
	while(Gtk::Main::events_pending())
		Gtk::Main::iteration();
	
	// read the directory and copy file names
	Glib::Dir dir(path);
	std::vector<Glib::ustring> files(dir.begin(), dir.end());
	
	// correct path name
	Glib::ustring rpath=path;
	if (rpath[rpath.size()-1]!='/')
		rpath+='/';
	
	// iterate over files
	for (int i=0; i<files.size(); i++) {
		// first, see if this file is a gif
		if (files[i].substr(files[i].size()-4, 4)!=".gif")
			continue;
		Glib::ustring name=files[i];
		
		// calculate completed progress
		double prog=(double) i/files.size();
		
		// set the new progress
		pd.set_progress(prog);
		
		// process events to show this one
		while(Gtk::Main::events_pending())
			Gtk::Main::iteration();
		
		// we now need to extract the id of this animation
		// first, get rid of the extension
		name.erase(name.size()-4, name.size());
		
		// if this animation has an idle and talk parts, then it should have a
		// set of parenthesis at the end
		Glib::ustring part;
		int npos=name.find('(');
		
		// set a default part of animation
		if (npos==-1)
			part="idle";
		
		else {
			// now get the next character
			if (name[npos+1]=='a')
				part="idle";
			else if (name[npos+1]=='b')
				part="talk";
			
			// erase the parenthesis
			name.erase(npos, 3);
		}
		
		// complete id string
		Glib::ustring id=name+"_"+part;
		
		// form complete path to gif
		Glib::ustring filePath=rpath+files[i];
		
		// create the animation
		add_animation_from_gif(id, filePath);
	}
	
	pd.hide();
	
	return true;
}

// add an animation from a gif
void Sprite::add_animation_from_gif(const Glib::ustring &id, const Glib::ustring &path) {
	// add this animation if it doesn't exist
	if (m_Animations.find(id)==m_Animations.end()) {
		// create animation object
		Animation anim;
		anim.id=id;
		
		// and add it
		add_animation(anim);
	}
	
	// vector of frames in this animation
	std::vector<Magick::Image> frames;
	
	// read in the frames
	Magick::readImages(&frames, path);
	
	// iterate over each frame and process it
	for (int i=0; i<frames.size(); i++) {
		Magick::Image frame=frames[i];
		
		// get the delay for this frame
		int delay=frame.animationDelay()*10;
		
		// if this image has transparency, flatten it
		if (frame.matte()) {
			// vector of layers
			std::vector<Magick::Image> flatten;
			
			// we add a new, blank image that is completely green
			flatten.push_back(Magick::Image(frame.size(), Magick::ColorRGB(0, 255, 0)));
			flatten.push_back(frame);
			
			// flatten these images
			Magick::flattenImages(&frame, flatten.begin(), flatten.end());
		}
		
		// dump this image to file
		frame.write("tmp.png");
		
		// and create a pixbuf from it
		Glib::RefPtr<Gdk::Pixbuf> pixbuf=Gdk::Pixbuf::create_from_file("tmp.png");
		
		// once we are through with this file, get rid of it
		::remove("tmp.png");
		
		// add this frame
		add_frame(id, delay, pixbuf);
	}
}

// add a frame to an animation
void Sprite::add_frame(const Glib::ustring &id, int time, const Glib::RefPtr<Gdk::Pixbuf> &pixbuf) {
	Frame fr;
	fr.time=time;
	fr.pixbuf=pixbuf;
	
	m_Animations[id].frames.push_back(fr);
}

// remove a frame from an animation sequence
void Sprite::remove_frame(const Glib::ustring &id, int index) {
	Animation &anim=m_Animations[id];
	int c=0;
	for (std::vector<Frame>::iterator it=anim.frames.begin(); it!=anim.frames.end(); ++it) {
		// erase this element if it matches the index
		if (c==index) {
			it=anim.frames.erase(it);
			break;
		}
		c+=1;
	}
}
