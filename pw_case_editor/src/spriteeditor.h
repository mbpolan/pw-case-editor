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

/** Window that displays widgets used in editing a sprite.
  * The actualy sprite can be edited using this window. It provides all of the 
  * basic frame handling, sound effect, and preview functionality.
*/
class SpriteEditor: public Gtk::Window {
	public:
		/// Constructor
		SpriteEditor();
		
		/// Clear out previous sprite data
		void clear();
		
		/** Set sprite data to use
		  * \param spr The sprite to edit
		*/
		void set_sprite_data(const Sprite &spr);
		
		/** Get the working sprite
		  * \return The updated sprite
		*/
		Sprite get_sprite_data() const { return m_Sprite; }
		
	private:
		/// Build the window's UI
		void construct();
		
		/// Update the progress label and other widgets
		void update_progress_label();
		
		/// Handler to save the current sprite
		void on_save();
		
		/// Handler to export the current sprite
		void on_export();
		
		/// Handler to close the window
		void on_close();
		
		/// Handler to preview an animation
		void on_preview_animation();
		
		/// Handler for loop check button toggles
		void on_loop_cb_toggled();
		
		/// Handler for combo box changes
		void on_anim_cb_changed();
		
		/// Handler for amend button clicks
		void on_amend_button_clicked();
		
		/// Handler for new animation button clicks
		void on_new_animation_button_clicked();
		
		/// Handler for delete animation button clicks
		void on_delete_animation_button_clicked();
		
		/// Handler for add frame button clicks
		void on_add_frame_button_clicked();
		
		/// Handler for delete frame button clicks
		void on_delete_frame_button_clicked();
		
		/// Handler for previous frame button clicks
		void on_prev_frame_button_clicked();
		
		/// Handler for next frame button clicks
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
		
		/// Current sprite path
		Glib::ustring m_SpritePath;
		
		/// The sprite we are editing
		Sprite m_Sprite;
		
		/// Current frame of animation
		int m_CurFrame;
};

/** Animation player for previewing a sprite.
  * This dialog acts like a "media player" for animations. It 
  * can start, stop, and pause animations.
*/
class AnimPlayer: public Gtk::Dialog {
	public:
		/** Constructor
		  * \param anim The animation to play
		*/
		AnimPlayer(const Animation &anim);
		
	private:
		/// Build the dialog's UI
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
		
		/// The current frame
		int m_CurFrame;
		
		/// Internal animation we're playing
		Animation m_Animation;
};

#endif
