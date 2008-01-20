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
#include <gtkmm/frame.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/separator.h>
#include <gtkmm/stock.h>
#include <gtkmm/table.h>
#include <sstream>

#include "dialogs.h"
#include "iohandler.h"
#include "spriteeditor.h"
#include "utilities.h"

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
	Gtk::VBox *vb=Gtk::manage(new Gtk::VBox);
	vb->set_spacing(5);
	
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	table->set_homogeneous(false);
	
	// allocate labels
	m_AnimLabel=Gtk::manage(new Gtk::Label("Animation"));
	m_FrameLabel=Gtk::manage(new Gtk::Label("1/0"));
	m_TimeLabel=Gtk::manage(new Gtk::Label("Time Delay (in milliseconds)"));
	m_SfxLabel=Gtk::manage(new Gtk::Label("Sound Effect"));
	
	// allocate entries
	m_TimeEntry=Gtk::manage(new Gtk::Entry);
	m_SfxEntry=Gtk::manage(new Gtk::Entry);
	m_TimeEntry->set_text("200");
	
	// allocate image
	m_Image=Gtk::manage(new Gtk::Image);
	m_Image->set_size_request(256, 192);
	
	// allocate buttons
	m_SaveButton=Gtk::manage(new Gtk::Button("Save"));
	m_CloseButton=Gtk::manage(new Gtk::Button("Close"));
	m_ExportButton=Gtk::manage(new Gtk::Button("Export"));
	m_PreviewButton=Gtk::manage(new Gtk::Button("Preview"));
	m_NewAnimButton=Gtk::manage(new Gtk::Button("New Animation"));
	m_DeleteAnimButton=Gtk::manage(new Gtk::Button("Delete Animation"));
	m_AddFrameButton=Gtk::manage(new Gtk::Button("Add Frames"));
	m_DeleteFrameButton=Gtk::manage(new Gtk::Button("Delete Frame"));
	m_PrevFrameButton=Gtk::manage(new Gtk::Button("<<"));
	m_NextFrameButton=Gtk::manage(new Gtk::Button(">>"));
	m_AmendButton=Gtk::manage(new Gtk::Button("Amend"));
	
	// connect the plethora of signals
	m_SaveButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteEditor::on_save));
	m_ExportButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteEditor::on_export));
	m_CloseButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteEditor::on_close));
	m_PreviewButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteEditor::on_preview_animation));
	m_NewAnimButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteEditor::on_new_animation_button_clicked));
	m_DeleteAnimButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteEditor::on_delete_animation_button_clicked));
	m_AddFrameButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteEditor::on_add_frame_button_clicked));
	m_DeleteFrameButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteEditor::on_delete_frame_button_clicked));
	m_PrevFrameButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteEditor::on_prev_frame_button_clicked));
	m_NextFrameButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteEditor::on_next_frame_button_clicked));
	m_AmendButton->signal_clicked().connect(sigc::mem_fun(*this, &SpriteEditor::on_amend_button_clicked));
	
	// allocate check button
	m_LoopCB=Gtk::manage(new Gtk::CheckButton("Loop Animation Upon Completion"));
	m_LoopCB->set_active(true);
	
	// connect signal
	m_LoopCB->signal_toggled().connect(sigc::mem_fun(*this, &SpriteEditor::on_loop_cb_toggled));
	
	// allocate combo box
	m_AnimCB=Gtk::manage(new Gtk::ComboBoxText);
	
	// append default animation
	m_AnimCB->append_text("normal_idle");
	m_AnimCB->set_active(0);
	
	// also add default animation
	Animation animation;
	animation.id="normal_idle";
	m_Sprite.add_animation(animation);
	
	// connect signals
	m_AnimCB->signal_changed().connect(sigc::mem_fun(*this, &SpriteEditor::on_anim_cb_changed));
	
	// button box for control buttons
	Gtk::HButtonBox *hbb=Gtk::manage(new Gtk::HButtonBox);
	hbb->set_layout(Gtk::BUTTONBOX_END);
	hbb->set_child_ipadding_x(2);
	hbb->pack_start(*m_SaveButton);
	hbb->pack_start(*m_ExportButton);
	hbb->pack_start(*m_CloseButton);
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK | Gtk::SHRINK;
	
	// create container frame and its table for frame options
	Gtk::Frame *frame=Gtk::manage(new Gtk::Frame("Frame Options"));
	Gtk::Table *ftable=Gtk::manage(new Gtk::Table);
	ftable->set_spacings(5);
	
	// place widgets in frame
	ftable->attach(*m_SfxLabel, 0, 1, 0, 1, xops, yops);
	ftable->attach(*m_SfxEntry, 1, 2, 0, 1, xops, yops);
	ftable->attach(*m_TimeLabel, 0, 1, 1, 2, xops, yops);
	ftable->attach(*m_TimeEntry, 1, 2, 1, 2, xops, yops);
	ftable->attach(*m_AmendButton, 0, 1, 2, 3, xops, yops);
	frame->add(*ftable);
	
	// place widgets
	table->attach(*m_AnimLabel, 0, 1, 0, 1, xops, yops);
	table->attach(*m_AnimCB, 1, 3, 0, 1, xops, yops);
	table->attach(*m_PreviewButton, 3, 4, 0, 1, xops, yops);
	table->attach(*m_NewAnimButton, 0, 1, 1, 2, xops, yops);
	table->attach(*m_DeleteAnimButton, 1, 2, 1, 2, xops, yops);
	table->attach(*m_AddFrameButton, 2, 3, 1, 2, xops, yops);
	table->attach(*m_DeleteFrameButton, 3, 4, 1, 2, xops, yops);
	table->attach(*m_PrevFrameButton, 0, 1, 2, 3, xops, yops);
	table->attach(*m_FrameLabel, 1, 3, 2, 3, xops, yops);
	table->attach(*m_NextFrameButton, 3, 4, 2, 3, xops, yops);
	table->attach(*m_Image, 0, 4, 3, 4);
	table->attach(*m_LoopCB, 0, 4, 4, 5, xops, yops);
	table->attach(*frame, 0, 4, 5, 6, xops, yops);
	
	vb->pack_start(*table, Gtk::PACK_SHRINK);
	
	// separator for buttons
	vb->pack_start(*Gtk::manage(new Gtk::HSeparator), Gtk::PACK_SHRINK);
	
	vb->pack_start(*hbb, Gtk::PACK_SHRINK);
	
	add(*vb);
	
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
	
	// and set the sound effect
	if (!anim.frames.empty())
		m_SfxEntry->set_text(anim.frames[m_CurFrame-1].sfx);
	
	// and update check button
	m_LoopCB->set_active(anim.loop);
}

// save the current sprite
void SpriteEditor::on_save() {
	// get the sprite
	Sprite sprite=get_sprite_data();
	
	// prepare file chooser
	Gtk::FileChooserDialog fcd(*this, "Save Sprite", Gtk::FILE_CHOOSER_ACTION_SAVE);
	fcd.add_button("Save", Gtk::RESPONSE_OK);
	fcd.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	// add a filter
	Gtk::FileFilter filter;
	filter.add_pattern("*.spr");
	filter.set_name("Sprites (*.spr)");
	fcd.add_filter(filter);
	
	// run the dialog
	if (fcd.run()==Gtk::RESPONSE_OK) {
		// get the path
		Glib::ustring path=fcd.get_filename();
		
		// check extension
		Glib::ustring ext=path.substr(path.size()-4, path.size()-1);
		ext=ext.lowercase();
		if (ext!=".spr")
			path+=".spr";
		
		// save this sprite
		if (!IO::save_sprite_to_file(path, sprite)) {
			// yet another vague error
			Gtk::MessageDialog md(*this, "Unable to save sprite.", false, Gtk::MESSAGE_ERROR);
			md.run();
		}
	}
}

// export the current sprite
void SpriteEditor::on_export() {
	// get the sprite
	Sprite sprite=get_sprite_data();
	
	// prepare file chooser
	Gtk::FileChooserDialog fcd(*this, "Export Sprite", Gtk::FILE_CHOOSER_ACTION_SAVE);
	fcd.add_button("Export", Gtk::RESPONSE_OK);
	fcd.add_button("Cancel", Gtk::RESPONSE_CANCEL);
	
	// add a filter
	Gtk::FileFilter filter;
	filter.add_pattern("*.pws");
	filter.set_name("Phoenix Wright sprites (*.pws)");
	fcd.add_filter(filter);
	
	// run the dialog
	if (fcd.run()==Gtk::RESPONSE_OK) {
		// get the path
		Glib::ustring path=fcd.get_filename();
		
		// check extension
		Glib::ustring ext=path.substr(path.size()-4, path.size()-1);
		ext=ext.lowercase();
		if (ext!=".pws")
			path+=".pws";
		
		// save this sprite
		if (!IO::export_sprite_to_file(path, sprite)) {
			// yet another vague error
			Gtk::MessageDialog md(*this, "Unable to export sprite.", false, Gtk::MESSAGE_ERROR);
			md.run();
		}
	}
}

// close the window
void SpriteEditor::on_close() {
	hide();
}

// preview an animation
void SpriteEditor::on_preview_animation() {
	if (m_AnimCB->get_active_text().empty())
		return;
	
	// get our animation
	Animation anim=m_Sprite.get_animation(m_AnimCB->get_active_text());
	
	// we need at least 1 frame
	if (anim.frames.empty()) {
		Gtk::MessageDialog md(*this, "You cannot preview an empty animation.");
		md.run();
		return;
	}
	
	// bring up the preview dialog
	AnimPlayer player(anim);
	player.run();
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
	
	// get sound effect
	fr.sfx=m_SfxEntry->get_text();
	
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
			
			// add the animation from this gif
			m_Sprite.add_animation_from_gif(id, filenames[0]);
			
			// get the amount of frames in this animation now
			int amount=m_Sprite.get_animation(id).frames.size();
			m_CurFrame=amount;
			
			// set this new frame
			m_Image->set(m_Sprite.get_animation(id).frames[m_CurFrame-1].pixbuf);
			
			update_progress_label();
			
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

/*****************************************************************************************/

// constructor
AnimPlayer::AnimPlayer(const Animation &anim): m_Animation(anim) {
	construct();
}

// build the interface
void AnimPlayer::construct() {
	// get the vbox
	Gtk::VBox *vb=get_vbox();
	vb->set_spacing(5);
	
	// allocate table
	Gtk::Table *table=Gtk::manage(new Gtk::Table);
	table->set_spacings(5);
	
	// allocate buttons
	m_PlayButton=Gtk::manage(new Gtk::Button(Gtk::Stock::MEDIA_PLAY));
	m_StopButton=Gtk::manage(new Gtk::Button(Gtk::Stock::MEDIA_STOP));
	
	// connect button signals
	m_PlayButton->signal_clicked().connect(sigc::mem_fun(*this, &AnimPlayer::on_play_clicked));
	m_StopButton->signal_clicked().connect(sigc::mem_fun(*this, &AnimPlayer::on_stop_clicked));
	
	// allocate labels
	m_AnimLabel=Gtk::manage(new Gtk::Label);
	m_AnimLabel->set_markup("Animation: <i>"+m_Animation.id+"</i>");
	
	// allocate image
	m_FrameImg=Gtk::manage(new Gtk::Image(m_Animation.frames[0].pixbuf));
	m_FrameImg->set_size_request(256, 192);
	
	// allocate progress bar
	m_ProgBar=Gtk::manage(new Gtk::ProgressBar);
	std::stringstream ss;
	ss << "Frame 1 of " << m_Animation.frames.size();
	m_ProgBar->set_text(ss.str());
	
	// button box for media control
	Gtk::HButtonBox *hbb=Gtk::manage(new Gtk::HButtonBox);
	hbb->set_spacing(5);
	hbb->pack_start(*m_PlayButton);
	hbb->pack_start(*m_StopButton);
	
	// attach options
	Gtk::AttachOptions xops=Gtk::FILL | Gtk::EXPAND;
	Gtk::AttachOptions yops=Gtk::SHRINK;
	
	// pack widgets
	table->attach(*m_AnimLabel, 0, 1, 0, 1, xops, yops);
	table->attach(*hbb, 0, 1, 1, 2, xops, yops);
	table->attach(*m_ProgBar, 0, 1, 2, 3, xops, yops);
	table->attach(*Gtk::manage(new Gtk::HSeparator), 0, 1, 3, 4, xops, yops);
	table->attach(*m_FrameImg, 0, 1, 4, 5, xops, yops);
	
	vb->pack_start(*table);
	
	add_button("Close", Gtk::RESPONSE_OK);
	
	show_all_children();
}

// timeout for new frame
bool AnimPlayer::on_timeout() {
	// get the next frame
	if (m_CurFrame+1>=m_Animation.frames.size())
		m_CurFrame=0;
	else
		m_CurFrame++;
	
	// get timeout for this frame
	int timeout=m_Animation.frames[m_CurFrame].time;
	
	// update progress bar text and move it
	std::stringstream ss;
	ss << "Frame " << m_CurFrame+1 << " of " << m_Animation.frames.size();
	m_ProgBar->set_text(ss.str());
	
	
	Utils::flush_events();
	
	// calculate progress, and update the progress bar
	double progress=(double)(m_CurFrame+1)/m_Animation.frames.size();
	m_ProgBar->set_fraction(progress);
	
	Utils::flush_events();
	
	// disconnect old timer, and set reconnect new one
	m_TimerSlot.disconnect();
	m_TimerSlot=Glib::signal_timeout().connect(sigc::mem_fun(*this, &AnimPlayer::on_timeout), timeout);
	
	// update image
	m_FrameImg->set(m_Animation.frames[m_CurFrame].pixbuf);
	
	return true;
}

// play button click handler
void AnimPlayer::on_play_clicked() {
	// get the timeout for the first frame
	int timeout=m_Animation.frames[0].time;
	
	// connect our timer
	m_TimerSlot=Glib::signal_timeout().connect(sigc::mem_fun(*this, &AnimPlayer::on_timeout), timeout);
	
	// disable play button, enable stop
	m_PlayButton->set_sensitive(false);
	m_StopButton->set_sensitive(true);
}

// stop button click handler
void AnimPlayer::on_stop_clicked() {
	// reset frame
	m_CurFrame=0;
	
	// reset progress bar
	std::stringstream ss;
	ss << "Frame 1 of " << m_Animation.frames.size();
	m_ProgBar->set_text(ss.str());
	m_ProgBar->set_fraction(0);
	
	// disconnect our timer
	m_TimerSlot.disconnect();
	
	// and enable play button
	m_PlayButton->set_sensitive(true);
	m_StopButton->set_sensitive(false);
}
