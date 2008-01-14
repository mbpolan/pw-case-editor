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
// spriteeditor.h: the SpriteEditor class

#ifndef SPRITEEDITOR_H
#define SPRITEEDITOR_H

#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/image.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/window.h>

#include "sprite.h"

// window used to edit sprites
class SpriteEditor: public Gtk::Window {
	public:
		// constructor
		SpriteEditor();
		
		// set sprite data to use
		void set_sprite_data(const Sprite &spr);
		
		// return the working sprite
		Sprite get_sprite_data() const { return m_Sprite; }
		
	private:
		// build the ui
		void construct();
		
		// update the progress label
		void update_progress_label();
		
		// save the current sprite
		void on_save();
		
		// export the current sprite
		void on_export();
		
		// close the window
		void on_close();
		
		// preview an animation
		void on_preview_animation();
		
		// handler for loop check button toggle
		void on_loop_cb_toggled();
		
		// handler for combo box changes
		void on_anim_cb_changed();
		
		// amend button handler
		void on_amend_button_clicked();
		
		// new animation button click handler
		void on_new_animation_button_clicked();
		
		// delete animation button click handler
		void on_delete_animation_button_clicked();
		
		// add frame button click handler
		void on_add_frame_button_clicked();
		
		// delete frame button click handler
		void on_delete_frame_button_clicked();
		
		// previous frame button click handler
		void on_prev_frame_button_clicked();
		
		// next frame button click handler
		void on_next_frame_button_clicked();
		
		// labels
		Gtk::Label *m_AnimLabel;
		Gtk::Label *m_FrameLabel;
		Gtk::Label *m_TimeLabel;
		Gtk::Label *m_SfxLabel;
		
		// check buttons
		Gtk::CheckButton *m_LoopCB;
		
		// entries
		Gtk::Entry *m_TimeEntry;
		Gtk::Entry *m_SfxEntry;
		
		// image for viewing frames
		Gtk::Image *m_Image;
		
		// buttons
		Gtk::Button *m_SaveButton;
		Gtk::Button *m_ExportButton;
		Gtk::Button *m_CloseButton;
		Gtk::Button *m_PreviewButton;
		Gtk::Button *m_NewAnimButton;
		Gtk::Button *m_DeleteAnimButton;
		Gtk::Button *m_AddFrameButton;
		Gtk::Button *m_DeleteFrameButton;
		Gtk::Button *m_PrevFrameButton;
		Gtk::Button *m_NextFrameButton;
		Gtk::Button *m_AmendButton;
		
		// combo box
		Gtk::ComboBoxText *m_AnimCB;
		
		// current sprite we are editing
		Glib::ustring m_SpritePath;
		
		// sprite we are editing
		Sprite m_Sprite;
		
		// current frame of animation
		int m_CurFrame;
};

// animation player to previews
class AnimPlayer: public Gtk::Dialog {
	public:
		// constructor
		AnimPlayer(const Animation &anim);
		
	private:
		// build the interface
		void construct();
		
		// timeout for new frame
		bool on_timeout();
		
		// play button click handler
		void on_play_clicked();
		
		// stop button click handler
		void on_stop_clicked();
		
		// image for viewing a frame
		Gtk::Image *m_FrameImg;
		
		// labels
		Gtk::Label *m_AnimLabel;
		
		// progress bar
		Gtk::ProgressBar *m_ProgBar;
		
		// buttons for control
		Gtk::Button *m_PlayButton;
		Gtk::Button *m_StopButton;
		
		// connection to timer slot
		sigc::connection m_TimerSlot;
		
		// the current frame
		int m_CurFrame;
		
		// internal animation
		Animation m_Animation;
};

#endif
