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
// spriteeditor.cpp: implementation of SpriteEditor class

#include <Magick++.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/messagedialog.h>
#include <sstream>
#include <gtkmm/table.h>

#include "dialogs.h"
#include "spriteeditor.h"

// constructor
SpriteEditor::SpriteEditor() {
	// default to no sprite path
	m_SpritePath="null";
	m_CurFrame=1;
	
	construct();
}

// set the path of a sprite to load
void SpriteEditor::set_sprite_data(const Sprite &spr) {
	// copy sprite data
	m_Sprite=spr;
	
	m_AnimCB->clear();
	
	// go over animations
	AnimationMap animations=spr.get_animations();
	for (AnimationMapIter it=animations.begin(); it!=animations.end(); ++it) {
		// append ids to combo box
		m_AnimCB->append_text((*it).second.id);
	}
	
	// set default animation
	m_AnimCB->set_active(0);
	update_progress_label();
}

// build the ui
void SpriteEditor::construct() {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_border_width(10);
	
	// allocate table
	Gtk::Table *table=manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate labels
	m_AnimLabel=manage(new Gtk::Label("Animation"));
	m_FrameLabel=manage(new Gtk::Label("1/0"));
	m_TimeLabel=manage(new Gtk::Label("Time (ms)"));
	
	// allocate entries
	m_TimeEntry=manage(new Gtk::Entry);
	m_TimeEntry->set_text("200");
	
	// allocate image
	m_Image=manage(new Gtk::Image);
	
	// allocate buttons
	m_NewAnimButton=manage(new Gtk::Button("New Animation"));
	m_DeleteAnimButton=manage(new Gtk::Button("Delete Animation"));
	m_AddFrameButton=manage(new Gtk::Button("Add Frames"));
	m_DeleteFrameButton=manage(new Gtk::Button("Delete Frame"));
	m_PrevFrameButton=manage(new Gtk::Button("<<"));
	m_NextFrameButton=manage(new Gtk::Button(">>"));
	m_AmendButton=manage(new Gtk::Button("Amend"));
	
	// connect the plethora of signals
	m_NewAnimButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteEditor::on_new_animation_button_clicked));
	m_DeleteAnimButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteEditor::on_delete_animation_button_clicked));
	m_AddFrameButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteEditor::on_add_frame_button_clicked));
	m_DeleteFrameButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteEditor::on_delete_frame_button_clicked));
	m_PrevFrameButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteEditor::on_prev_frame_button_clicked));
	m_NextFrameButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteEditor::on_next_frame_button_clicked));
	m_AmendButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteEditor::on_amend_button_clicked));
	
	// allocate check button
	m_LoopCB=manage(new Gtk::CheckButton("Loop Animation"));
	m_LoopCB->set_active(true);
	
	// connect signal
	m_LoopCB->signal_toggled().connect(sigc::mem_fun(*this, &SpriteEditor::on_loop_cb_toggled));
	
	// allocate combo box
	m_AnimCB=manage(new Gtk::ComboBoxText);
	
	// append default animation
	m_AnimCB->append_text("normal_idle");
	m_AnimCB->set_active(0);
	
	// also add default animation
	Animation animation;
	animation.id="normal_idle";
	m_Sprite.add_animation(animation);
	
	// connect signals
	m_AnimCB->signal_changed().connect(sigc::mem_fun(*this, &SpriteEditor::on_anim_cb_changed));
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// place widgets
	table->attach(*m_AnimLabel, 0, 1, 0, 1, xops, yops);
	table->attach(*m_AnimCB, 1, 3, 0, 1, xops, yops);
	table->attach(*m_NewAnimButton, 0, 1, 1, 2, xops, yops);
	table->attach(*m_DeleteAnimButton, 1, 2, 1, 2, xops, yops);
	table->attach(*m_AddFrameButton, 2, 3, 1, 2, xops, yops);
	table->attach(*m_DeleteFrameButton, 3, 4, 1, 2, xops, yops);
	table->attach(*m_PrevFrameButton, 0, 1, 2, 3, xops, yops);
	table->attach(*m_FrameLabel, 1, 3, 2, 3, xops, yops);
	table->attach(*m_NextFrameButton, 3, 4, 2, 3, xops, yops);
	table->attach(*m_Image, 0, 4, 3, 4);
	table->attach(*m_LoopCB, 0, 1, 4, 5, xops, yops);
	table->attach(*m_TimeLabel, 1, 2, 4, 5, xops, yops);
	table->attach(*m_TimeEntry, 2, 3, 4, 5, xops, yops);
	table->attach(*m_AmendButton, 3, 4, 4, 5, xops, yops);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	// add buttons
	add_button("Save", Gtk::RESPONSE_OK);
	add_button("Export", Gtk::RESPONSE_YES); // this shouldn't use RESPONSE_YES
	add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	show_all_children();
}

// update the progress label
void SpriteEditor::update_progress_label() {
	std::stringstream ss;
	
	// get our animation
	Animation anim=m_Sprite.get_animation(m_AnimCB->get_active_text());
	
	// current frame followed by max frames
	ss << m_CurFrame << "/" << anim.frames.size();
	
	// set the label text
	m_FrameLabel->set_text(ss.str());
	
	// also, set the time for this label in the appropriate entry
	ss.str("");
	ss << anim.frames[m_CurFrame-1].time;
	m_TimeEntry->set_text(ss.str());
	
	// and update check button
	m_LoopCB->set_active(anim.loop);
}

// handler for loop check button toggle
void SpriteEditor::on_loop_cb_toggled() {
	// get our animation
	Animation &anim=m_Sprite.get_animation(m_AnimCB->get_active_text());
	
	// update loop variable
	anim.loop=m_LoopCB->get_active();
}

// handler for combo box changes
void SpriteEditor::on_anim_cb_changed() {
	// set the image frame to the first frame of this animation
	m_CurFrame=1;
	Glib::ustring id=m_AnimCB->get_active_text();
	
	if (m_Sprite.get_animation(id).frames.empty())
		m_Image->clear();
	else
		m_Image->set(m_Sprite.get_animation(id).frames[m_CurFrame-1].pixbuf);
	
	// update the progress label
	update_progress_label();
}

// amend button handler
void SpriteEditor::on_amend_button_clicked() {
	// get our current frame
	Glib::ustring id=m_AnimCB->get_active_text();
	Frame &fr=m_Sprite.get_animation(id).frames[m_CurFrame-1];
	
	// get the current frame time
	int time=atoi(m_TimeEntry->get_text().c_str());
	
	// update this frame
	fr.time=time;
}

// new animation button click handler
void SpriteEditor::on_new_animation_button_clicked() {
	// ask for animation id
	TextInputDialog td("Animation Id");
	if (td.run()==Gtk::RESPONSE_OK) {
		// get inputted text
		Glib::ustring id=td.get_text();
		
		// add this animation to the combo box
		m_AnimCB->append_text(id);
		
		// create a new Animation
		Animation anim;
		anim.id=id;
		anim.loop=true;
		
		// add it to the sprite
		m_Sprite.add_animation(anim);
	}
}

// delete animation button click handler
void SpriteEditor::on_delete_animation_button_clicked() {
	// get amount of animations
	if (m_Sprite.num_animations()==1) {
		Gtk::MessageDialog md(*this, "A sprite must have at least one animation.", false, Gtk::MESSAGE_ERROR);
		md.run();
	}
	
	else {
		Glib::ustring animId=m_AnimCB->get_active_text();
		
		// delete the sequence
		m_Sprite.remove_animation(animId);
		
		// also remove entry from combo box
		m_AnimCB->remove_text(animId);
		
		// default to first animation
		m_AnimCB->set_active(0);
	}
}

// add frame button click handler
void SpriteEditor::on_add_frame_button_clicked() {
	// prepare file chooser
	Gtk::FileChooserDialog fcd(*this, "Open Frame Image", Gtk::FILE_CHOOSER_ACTION_OPEN);
	fcd.set_select_multiple(true);
	fcd.add_button("Open", Gtk::RESPONSE_OK);
	fcd.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	// run the dialog
	if (fcd.run()==Gtk::RESPONSE_OK) {
		std::vector<Glib::ustring> filenames=fcd.get_filenames();
		
		// check for gifs
		if (filenames.size()==1 && filenames[0].substr(filenames[0].size()-4, 4)==".gif") {
			// first, get the id of our current animation
			Glib::ustring id=m_AnimCB->get_active_text();
			
			// vector of frames in this animation
			std::vector<Magick::Image> frames;
			
			// read in the frames
			Magick::readImages(&frames, filenames[0]);
			
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
				m_Sprite.add_frame(id, delay, pixbuf);
				
				// get the amount of frames in this animation now
				int amount=m_Sprite.get_animation(id).frames.size();
				m_CurFrame=amount;
				
				// set this new frame
				m_Image->set(m_Sprite.get_animation(id).frames[m_CurFrame-1].pixbuf);
				
				update_progress_label();
			}
			
			// we don't need to deal with this image anymore
			return;
		}
		
		// iterate over filenames
		for (int i=0; i<filenames.size(); i++) {
			// create a pixbuf from the file
			Glib::RefPtr<Gdk::Pixbuf> pixbuf=Gdk::Pixbuf::create_from_file(filenames[i]);
			if (pixbuf) {
				// first, get the id of our current animation
				Glib::ustring id=m_AnimCB->get_active_text();
				
				// add the frame
				m_Sprite.add_frame(id, 200, pixbuf);
				
				// get the amount of frames in this animation now
				int amount=m_Sprite.get_animation(id).frames.size();
				m_CurFrame=amount;
				
				// set this new frame
				m_Image->set(m_Sprite.get_animation(id).frames[m_CurFrame-1].pixbuf);
				
				update_progress_label();
			}
		}
	}
}

// delete frame button click handler
void SpriteEditor::on_delete_frame_button_clicked() {
	// get animation id
	Glib::ustring id=m_AnimCB->get_active_text();
	
	// if there are no frames, ignore the request
	if (m_Sprite.get_animation(id).frames.empty())
		return;
	
	// delete current frame
	m_Sprite.remove_frame(id, m_CurFrame-1);
	
	// see if there are any frames left
	if (!m_Sprite.get_animation(id).frames.empty()) {
		// then there should be a frame 0
		m_Image->set(m_Sprite.get_animation(id).frames[0].pixbuf);
		m_CurFrame=1;
	}
	
	else
		m_Image->clear();
	
	update_progress_label();
	
}

// previous frame button click handler
void SpriteEditor::on_prev_frame_button_clicked() {
	if (m_CurFrame>1) {
		// move the the frame before this one
		m_CurFrame-=1;
		
		// get the previous frame pixbuf
		Glib::RefPtr<Gdk::Pixbuf> pixbuf=m_Sprite.get_animation(m_AnimCB->get_active_text()).frames[m_CurFrame-1].pixbuf;
		
		// update image
		m_Image->set(pixbuf);
	}
	
	update_progress_label();
}

// next frame button click handler
void SpriteEditor::on_next_frame_button_clicked() {
	// get amount of frames in animation
	Animation anim=m_Sprite.get_animation(m_AnimCB->get_active_text());
	if (m_CurFrame<anim.frames.size()) {
		m_CurFrame+=1;
		
		// get the previous frame pixbuf
		Glib::RefPtr<Gdk::Pixbuf> pixbuf=m_Sprite.get_animation(m_AnimCB->get_active_text()).frames[m_CurFrame-1].pixbuf;
		
		// update image
		m_Image->set(pixbuf);
	}
	
	update_progress_label();
}
