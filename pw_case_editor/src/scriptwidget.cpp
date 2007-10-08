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
// scriptwidget.cpp: implementation of ScriptWidget class

#include <sstream>

#include "scriptwidget.h"

// constructor
ScriptWidget::ScriptWidget(Case::LawSystem system) {
	m_LawSystem=system;
	construct();
}

// destructor
ScriptWidget::~ScriptWidget() {
	for (int i=0; i<6; i++)
		delete m_TreeViews[i];
}

// clear all data
void ScriptWidget::clear(Case::LawSystem system) {
	// reset law system
	m_LawSystem=system;
	
	// first, clear all lists
	for (int i=0; i<6; i++)
		m_TreeViews[i]->clear();
	
	// reset combo box
	reset_combo_box();
}

// add a character to the list
void ScriptWidget::add_character(int day, int stage, const Glib::ustring &name, const Glib::ustring &internal) {
	// form the string
	Glib::ustring str=name;
	str+=" (";
	str+=internal;
	str+=")";
	
	// add to all lists
	if (day==-1 && stage==-1) {
		for (int i=0; i<6; i++)
			m_TreeViews[i]->append_toplevel_text(str);
	}
	
	else {
		// get the list
		CListView *list=m_TreeViews[day*2+stage];
		list->append_toplevel_text(str);
	}
}

// remove a character from the list
void ScriptWidget::remove_character(const Glib::ustring &name) {
	get_current_list()->delete_toplevel_text(name);
}

// add a text block under an exiting category
void ScriptWidget::add_text_block(int day, int stage, const Glib::ustring &parent,
				  const Glib::ustring &blockName, const Glib::ustring &desc,
				  const Glib::RefPtr<Gtk::TextBuffer> &buffer) {
	// form the string
	Glib::ustring str=blockName;
	str+=" (";
	str+=desc;
	str+=")";
	
	// append this text
	CListView *list=m_TreeViews[day*2+stage];
	list->append_child_text(parent, blockName, str, buffer);
}

// get a complete map of all buffers
BufferMap ScriptWidget::get_buffers() const {
	BufferMap bmap;
	
	// get amount of lists
	int amount;
	if (m_LawSystem==Case::TWO_DAY)
		amount=4;
	else if (m_LawSystem==Case::THREE_DAY)
		amount=6;
	else
		amount=2;
	
	// combine maps
	for (int i=0; i<amount; i++) {
		BufferMap tmpMap=m_TreeViews[i]->get_buffers();
		
		// iterate over this buffer map
		for (BufferMap::iterator it=tmpMap.begin(); it!=tmpMap.end(); ++it) {
			Glib::ustring id=(*it).first;
			
			// preprend an identifier for the day and stage of this buffer
			std::stringstream pre;
			
			// day of case
			int day;
			
			// day 1
			if (i>=0 && i<2)
				day=0;
			else if (i>=2 && i<4)
				day=1;
			else if (i>=4 && i<6)
				day=2;
			
			// trial
			if (i%2!=0)
				pre << 't' << day;
				
			// investigation
			else
				pre << 'i' << day;
			
			// preprend this string to the id
			Glib::ustring tmp=id;
			tmp+="_"+pre.str();
			
			bmap[tmp]=(*it).second;
		}
	}
	
	return bmap;
}

// return buffer descriptions
std::map<Glib::ustring, Glib::ustring> ScriptWidget::get_buffer_descriptions() {
	std::map<Glib::ustring, Glib::ustring> bmap;
	
	// get amount of lists
	int amount;
	if (m_LawSystem==Case::TWO_DAY)
		amount=4;
	else if (m_LawSystem==Case::THREE_DAY)
		amount=6;
	else
		amount=2;
	
	// combine maps
	for (int i=0; i<amount; i++) {
		std::map<Glib::ustring, Glib::ustring> tmpMap=m_TreeViews[i]->get_buffer_descriptions();
		
		// iterate over buffer descriptions
		for (std::map<Glib::ustring, Glib::ustring>::iterator it=tmpMap.begin(); it!=tmpMap.end(); ++it)
			bmap[(*it).first]=(*it).second;
	}
	
	return bmap;
}

// build the ui
void ScriptWidget::construct() {
	// allocate label
	m_CurBlockLabel=manage(new Gtk::Label("Current Text Block: "));
	
	// allocate the text view widget
	m_TextView=manage(new Gtk::TextView);
	
	// disable editing
	m_TextView->set_editable(false);
	
	// allocate vbox
	Gtk::VBox *vb=manage(new Gtk::VBox);
	
	// allocate its containing window
	m_TextSWindow=manage(new Gtk::ScrolledWindow);
	m_TextSWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
	
	// pack widgets
	m_TextSWindow->add(*m_TextView);
	vb->pack_start(*m_CurBlockLabel, Gtk::PACK_SHRINK);
	vb->pack_start(*m_TextSWindow);
	
	// at most, we can only have 6 tree views
	for (int i=0; i<6; i++) {
		// allocate tree view
		CListView *view=new CListView;
	
		// connect treeview signals
		view->signal_display_buffer().connect(sigc::mem_fun(*this, &ScriptWidget::on_display_buffer));
		view->signal_select().connect(sigc::mem_fun(*this, &ScriptWidget::on_select_row));
		view->signal_add_text_block().connect(sigc::mem_fun(*this, &ScriptWidget::on_list_add_text_block));
		
		m_TreeViews.push_back(view);
	}
	
	// allocate combo box
	m_DayCB=manage(new Gtk::ComboBoxText);
	reset_combo_box();
	
	// connect signals
	m_DayCB->signal_changed().connect(sigc::mem_fun(*this, &ScriptWidget::on_combo_box_changed));
	
	// allocate notebook
	m_StageNB=manage(new Gtk::Notebook);
	
	// allocate scrolled window
	Gtk::ScrolledWindow *sw1=manage(new Gtk::ScrolledWindow);
	Gtk::ScrolledWindow *sw2=manage(new Gtk::ScrolledWindow);
	sw1->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	sw2->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	
	// allocate left vbox
	Gtk::VBox *leftVb=manage(new Gtk::VBox);
	leftVb->pack_start(*m_DayCB, Gtk::PACK_SHRINK);
	leftVb->pack_start(*m_StageNB);
	
	// add default lists
	sw1->add(*m_TreeViews[0]);
	sw2->add(*m_TreeViews[1]);
	
	// append default pages
	m_StageNB->append_page(*sw1, "Investigation");
	m_StageNB->append_page(*sw2, "Trial");
	
	// allocate parent pane
	m_HPane=manage(new Gtk::HPaned);
	m_HPane->pack1(*leftVb);
	m_HPane->pack2(*vb);
	
	// set an initial position
	m_HPane->set_position(10);
	
	pack_start(*m_HPane);
	
	show_all_children();
}

// get the current tree view
CListView* ScriptWidget::get_current_list() {
	// get the active item
	Glib::ustring str=m_DayCB->get_active_text();
	int item;
	if (str.find("2")!=-1)
		item=1;
	else if (str.find("3")!=-1)
		item=2;
	else
		item=0;
	
	// return the appropriate lis
	return m_TreeViews[item*2+m_StageNB->get_current_page()];
}

// get a unique id for a text block
Glib::ustring ScriptWidget::unique_id(const Glib::ustring &rootString) {
	std::stringstream ss;
	
	// always start counter with 1
	int count=1;
	
	// iterate over map and increment counter
	for (int i=0; i<6; i++) {
		CListView *list=m_TreeViews[i];
		
		// now iterate over buffers
		BufferMap buffers=list->get_buffers();
		for (BufferMap::iterator it=buffers.begin(); it!=buffers.end(); ++it) {
			Glib::ustring id=(*it).first;
			if (id.find(rootString)!=-1)
				count++;
		}
	}
	
	// form a string that is the collective of all ids
	ss << rootString << "_" << count;
	return ss.str();
}

// clistview handler to add a new text block
void ScriptWidget::on_list_add_text_block(const Glib::ustring &root, bool isCharacter, CListView *list) {
	// first, generate a unique id
	Glib::ustring unique=unique_id(root);
	
	// create a new text buffer
	Glib::RefPtr<Gtk::TextBuffer> buffer=CListView::create_buffer();
	
	// if this buffer is for a character, then automatically append the speaker tag
	if (isCharacter)
		buffer->set_text("{*speaker:"+root+";*}\n");
	
	// add this block
	list->append_child_text(root, unique, unique+" ()", buffer);
}

// reset the combo box
void ScriptWidget::reset_combo_box() {
	// clear the contents
	m_DayCB->clear();
	
	// add text based on law system
	int amount;
	switch(m_LawSystem) {
		case Case::TWO_DAY: amount=2; break;
		case Case::THREE_DAY: amount=3; break;
	}
	
	for (int i=0; i<amount; i++) {
		// format and append text
		std::stringstream ss;
		ss << "Day " << i+1;
		m_DayCB->append_text(ss.str());
		
		// set default
		if (i==0)
			m_DayCB->set_active_text(ss.str());
		
		ss.str("");
	}
}

// combo box selection handler
void ScriptWidget::on_combo_box_changed() {
	// get the active item
	Glib::ustring str=m_DayCB->get_active_text();
	int item;
	if (str.find("2")!=-1)
		item=1;
	else if (str.find("3")!=-1)
		item=2;
	else
		item=0;
	
	// get our scrolled windows
	Gtk::ScrolledWindow *sw1=(Gtk::ScrolledWindow*) m_StageNB->get_nth_page(0);
	Gtk::ScrolledWindow *sw2=(Gtk::ScrolledWindow*) m_StageNB->get_nth_page(1);
	
	// remove previously visible lists
	sw1->remove();
	sw2->remove();
	
	// get the new lists based on page and add them
	sw1->add(*m_TreeViews[item*2]);
	sw2->add(*m_TreeViews[item*2+1]);
	
	show_all_children();
}

// display buffer handler
void ScriptWidget::on_display_buffer(Glib::ustring id, Glib::RefPtr<Gtk::TextBuffer> buffer) {
	// make this the current buffer to edit
	m_TextView->set_buffer(buffer);
	
	// update label
	Glib::ustring str="Current Text Block: ";
	str+=id;
	m_CurBlockLabel->set_text(str);
	
	// enable editing
	m_TextView->set_editable(true);
}

// select a row
void ScriptWidget::on_select_row(Gtk::TreeModel::iterator it) {
	get_current_list()->get_selection()->select(it);
}
