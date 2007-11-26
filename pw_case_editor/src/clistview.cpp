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
// clistview.cpp: implementation of CListView class

#include <gtkmm/treemodel.h>
#include <gtkmm/textbuffer.h>
#include <sstream>

#include "clistview.h"
#include "dialogs.h"
#include "utilities.h"

// handle highlighting text in a changed buffer
void on_buffer_changed(Glib::RefPtr<Gtk::TextBuffer> buffer) {
	if (!buffer)
		return;
	
	// iterate over text
	bool begin=false;
	Gtk::TextBuffer::iterator itb;
	for (Gtk::TextBuffer::iterator it=buffer->begin(); it!=buffer->end(); ++it) {
		// dialogue control character
		if ((*it)=='\\') {
			Gtk::TextIter end=buffer->get_iter_at_offset(it.get_offset()+2);
			
			// see if the next character is recognized
			end--;
			if ((*end)=='b' || (*end)=='n' || (*end)=='+' || (*end)=='-' || (*end)=='=') {
				end++;
				
				// apply the tag
				buffer->apply_tag(buffer->get_tag_table()->lookup("dialogue_control"), it, end);
			}
		}
		
		// if we have hit a trigger, mark it
		else if ((*it)=='{') {
			itb=it;
			begin=true;
		}
		
		// end trigger character -- we should apply the tag
		else if ((*it)=='}' && begin) {
			// increment to incorporate ending bracket
			++it;
			
			// get the range we are in
			Glib::ustring contents=buffer->get_slice(itb, it);
			
			// find the first position of trigger name
			Gtk::TextIter nBegin=buffer->get_iter_at_offset(itb.get_offset()+2); // begining of name
			
			// if we have found the delimiting : character, apply the tag
			if (contents.find(':')!=-1) {
				Gtk::TextIter nEnd=buffer->get_iter_at_offset(itb.get_offset()+contents.find(':'));
				
				// apply specific tag
				buffer->apply_tag(buffer->get_tag_table()->lookup("trigger_name"), nBegin, nEnd);
			}
			
			// apply the trigger tags to this range
			buffer->apply_tag(buffer->get_tag_table()->lookup("trigger"), itb, it);
			
			// and reset
			begin=false;
			
			// backtrack a character
			--it;
		}
	}
}

// constructor
CListView::CListView() {
	// allocate and set model
	m_Model=Gtk::TreeStore::create(m_ColumnRec);
	set_model(m_Model);
	
	// append a column
	append_column("Text Blocks", m_ColumnRec.m_Column);
	
	// append some default rows
	append_toplevel_text("Script");
	append_toplevel_text("Narration");
	append_toplevel_text("Dates");
	
	// populate menu
	Gtk::Menu::MenuList &list=m_ContextMenu.items();
	
	list.push_back(Gtk::Menu_Helpers::MenuElem("Add Text Block",
		       sigc::mem_fun(*this, &CListView::on_add_text_block)));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Remove Text Block",
		       sigc::mem_fun(*this, &CListView::on_delete_text_block)));
	list.push_back(Gtk::Menu_Helpers::MenuElem("Edit Description",
		       sigc::mem_fun(*this, &CListView::on_edit_description)));
	list.push_back(Gtk::Menu_Helpers::SeparatorElem());
	list.push_back(Gtk::Menu_Helpers::MenuElem("Follow GOTO Trigger",
		       sigc::mem_fun(*this, &CListView::on_follow_goto)));
	
	// connect selection change signal
	Glib::RefPtr<Gtk::TreeView::Selection> selection=get_selection();
	selection->signal_changed().connect(sigc::mem_fun(*this, &CListView::on_selection_changed));
}

// create a buffer ready for use in this list
Glib::RefPtr<Gtk::TextBuffer> CListView::create_buffer() {
	Glib::RefPtr<Gtk::TextBuffer> buffer=Gtk::TextBuffer::create();
	
	// font descriptors
	Pango::FontDescription tname;
	
	// create tags
	Glib::RefPtr<Gtk::TextBuffer::Tag> tag=buffer->create_tag("trigger");
	tag->property_foreground().set_value("darkgreen");
	tag->property_style().set_value(Pango::STYLE_OBLIQUE);
	
	tag=buffer->create_tag("trigger_name");
	
	tname.set_weight(Pango::WEIGHT_BOLD);
	tag->property_font_desc().set_value(tname);
	
	tag=buffer->create_tag("dialogue_control");
	tag->property_foreground().set_value("red");
	
	// connect signals
	buffer->signal_changed().connect(sigc::bind(sigc::ptr_fun(on_buffer_changed), buffer));
	
	return buffer;
}

// clear all data
void CListView::clear() {
	// refresh the model with a no rows
	m_Model->clear();
	
	// also remove all buffers
	m_Buffers.clear();
	
	// add initial rows again
	append_toplevel_text("Script");
	append_toplevel_text("Narration");
	append_toplevel_text("Dates");
}

// append toplevel text to list
void CListView::append_toplevel_text(const Glib::ustring &text) {
	// append a new row
	Gtk::TreeModel::Row row=*(m_Model->append());
	row[m_ColumnRec.m_Column]=text;
}

// append text as child of a parent
void CListView::append_child_text(const Glib::ustring &parent, const Glib::ustring &id, 
				  const Glib::ustring &text, const Glib::RefPtr<Gtk::TextBuffer> &buffer) {
	// get the rows in the model
	Gtk::TreeModel::Children children=m_Model->children();
	
	// iterate over rows
	for (Gtk::TreeModel::Children::iterator it=children.begin(); it!=children.end(); ++it) {
		// see if this row matches the requested parent
		Glib::ustring row=(*it)[m_ColumnRec.m_Column];
		if (row.find(parent)!=-1) {
			// append a new row
			Gtk::TreeModel::Row nrow=*(m_Model->append((*it)->children()));
			nrow[m_ColumnRec.m_Column]=text;
			
			// and add this buffer
			Glib::ustring id=Utils::extract_block_id(text);
			m_Buffers[id]=buffer;
		}
	}
}

// remove a toplevel text element
void CListView::delete_toplevel_text(const Glib::ustring &text) {
	// get the rows in the model
	Gtk::TreeModel::Children children=m_Model->children();
	
	// iterate over rows
	for (Gtk::TreeModel::Children::iterator it=children.begin(); it!=children.end(); ++it) {
		// see if this row matches the requested parent
		Glib::ustring row=(*it)[m_ColumnRec.m_Column];
		if (row.find(text)!=-1) {
			// remove this row
			m_Model->erase(it);
			
			// go over all text buffers, and remove any that were children
			for (BufferMap::iterator itb=m_Buffers.begin();
			     itb!=m_Buffers.end(); ++itb) {
				// get id and find its root
				Glib::ustring bufferStr=(*itb).first;
				
				// extract the internal id of the parent
				int npos=bufferStr.rfind("_");
				Glib::ustring uid=bufferStr.substr(0, npos);
				
				// see if they match
				if (row.find(uid)!=-1)
					m_Buffers.erase(itb);
			}
		}
	}
}

// return buffer descriptions
std::map<Glib::ustring, Glib::ustring> CListView::get_buffer_descriptions() {
	// get the rows in the model
	Gtk::TreeModel::Children children=m_Model->children();
	
	// map for storing descriptions
	std::map<Glib::ustring, Glib::ustring> descs;
	
	// iterate over rows
	for (Gtk::TreeModel::Children::iterator it=children.begin(); it!=children.end(); ++it) {
		// if this toplevel node has any children, go over them
		if (!(*it)->children().empty()) {
			// now go over these rows
			Gtk::TreeModel::Children rowChildren=(*it)->children();
			for (Gtk::TreeModel::Children::iterator itr=rowChildren.begin(); itr!=rowChildren.end(); ++itr) {
				// get the row string
				Glib::ustring rowStr=(*itr)[m_ColumnRec.m_Column];
				
				// extract the description and id
				Glib::ustring rowId=Utils::extract_block_id(rowStr);
				
				Glib::ustring rowDesc;
				if (rowStr.find("()")!=-1)
					rowDesc="";
				else
					rowDesc=Utils::extract_block_description(rowStr);
				
				// add this description
				descs[rowId]=rowDesc;
				
			}
		}
	}
	
	return descs;
}

// handle selection changes
void CListView::on_selection_changed() {
	// get the selected character
	Glib::RefPtr<Gtk::TreeView::Selection> selection=get_selection();
	if (selection) {
		Gtk::TreeModel::iterator it=selection->get_selected();
		if (it) {
			Gtk::TreeModel::Row row=(*it);
			
			// check if this is a child row
			Gtk::TreeModel::iterator parent=it->parent();
			if (parent) {
				Glib::ustring textBlockId=Utils::extract_block_id(row[m_ColumnRec.m_Column]);
				
				// emit the display buffer signal
				m_DisplayBufferSignal.emit(textBlockId, m_Buffers[textBlockId]);
			}
		}
	}
}

// add a text block handler
void CListView::on_add_text_block() {
	// get the selected character
	Glib::RefPtr<Gtk::TreeView::Selection> selection=get_selection();
	if (selection) {
		Gtk::TreeModel::iterator it=selection->get_selected();
		if (it) {
			Gtk::TreeModel::Row row=(*it);
			
			// check if this is indeed a toplevel row
			Gtk::TreeModel::iterator parent=it->parent();
			if (!parent) {
				// get this character's internal name
				Glib::ustring str=row[m_ColumnRec.m_Column];
				
				// internal name is encased in ()
				int npos1=str.find("(");
				Glib::ustring iname;
				bool character=true;
				if (npos1==-1) {
					iname=str;
					character=false;
				}
				
				else {
					int npos2=str.find(")");
					iname=str.substr(npos1+1, str.size()-1);
					
					// remove any stray characters
					if (iname[iname.size()-1]==')')
						iname.erase(iname.size()-1, iname.size());
				}
				
				// emit signal
				m_AddBlockSignal.emit(iname, character, this);
			}
		}
	}
}

// delete a text block handler
void CListView::on_delete_text_block() {
	// get the selected character
	Glib::RefPtr<Gtk::TreeView::Selection> selection=get_selection();
	if (selection) {
		Gtk::TreeModel::iterator it=selection->get_selected();
		if (it && (*it)->parent()) {
			Glib::ustring id=Utils::extract_block_id((*it)[m_ColumnRec.m_Column]);
			
			m_Buffers.erase(id);
			m_Model->erase(it);
		}
	}
}

// edit a text block's description
void CListView::on_edit_description() {
	// get the selected character
	Glib::RefPtr<Gtk::TreeView::Selection> selection=get_selection();
	if (selection) {
		Gtk::TreeModel::iterator it=selection->get_selected();
		if (it && (*it)->parent()) {
			// get old description, if any
			Glib::ustring old=Utils::extract_block_description((*it)[m_ColumnRec.m_Column]);
			
			// ask for description
			TextInputDialog tid("Description", old);
			if (tid.run()==Gtk::RESPONSE_OK) {
				// get new description
				Glib::ustring updated=tid.get_text();
				
				// get the id of the block
				Glib::ustring id=Utils::extract_block_id((*it)[m_ColumnRec.m_Column]);
				
				// form new string
				Glib::ustring nStr=id;
				nStr+=" (";
				nStr+=updated;
				nStr+=")";
				
				// and update the original row text
				(*it)[m_ColumnRec.m_Column]=nStr;
			}
			
		}
	}
}

// button press handler
bool CListView::on_button_press_event(GdkEventButton *e) {
	// call base function
	bool ret=Gtk::TreeView::on_button_press_event(e);
	
	// check which button was pressed
	if (e->button==3 && e->type==GDK_BUTTON_PRESS)
		m_ContextMenu.popup(e->button, e->time);
	
	return ret;
}

// follow a block's GOTO trigger to its target block
void CListView::on_follow_goto() {
	// get the selected block
	Glib::RefPtr<Gtk::TreeView::Selection> selection=get_selection();
	if (selection) {
		Gtk::TreeModel::iterator it=selection->get_selected();
		if (it && (*it)->parent()) {
			// get the block id
			Glib::ustring id=Utils::extract_block_id((*it)[m_ColumnRec.m_Column]);
			
			// get the buffer
			Glib::RefPtr<Gtk::TextBuffer> buffer=m_Buffers[id];
			
			// parse it for the GOTO trigger
			Glib::ustring text=buffer->get_text();
			while(1) {
				// find trigger character
				int npos=text.find('{');
				if (npos==-1)
					break;
				
				// find the command start
				int npos1=text.find(':', npos);
				if (npos1==-1)
					break;
				
				// find trigger end character
				int npos2=text.find('}', npos);
				if (npos2==-1)
					break;
				
				// make sure that this is indeed the right trigger
				Glib::ustring trig=text.substr(npos+2, npos1-npos-2);
				trig=trig.lowercase();
				if (trig=="goto") {
					// get the target block
					Glib::ustring target=text.substr(npos1+1, npos1-npos2);
					target.erase(target.size()-3, target.size()-1);
					
					// request the row to be selected
					m_SelectSignal.emit(it);
					
					// ask for this buffer to be displayed
					m_DisplayBufferSignal.emit(id, buffer);
					
					return;
				}
				
				else
					text.erase(0, npos2+1);
			}
		}
	}
}
