/*
  render.cpp  -  Definition of the main workspace window object
 
  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/

#include "../gtk-lumiera.hpp"
#include "sequence-name.hpp"
#include "dialog.hpp"

using namespace Gtk;
using namespace Glib;

namespace gui {
namespace dialogs {

SequenceName::SequenceName(Window &parent, Action action,
  ustring default_title) :
  Dialog::Dialog("", parent, true),
  caption(_("Name:"))
{
  // Choose the dialog title
  ustring title;
  
  switch(action)
  {
  case AddSequence: title = _("Add Sequence"); break;
  case RenameSequence: title = _("Rename Sequence"); break;
   
  default:
    ASSERT(0); // An invalid action value was supplied
    break;
  }
    
  set_title(title);

  // Add the controls
  name.set_text(default_title);
  name.set_activates_default();
  
  hBox.pack_start(caption);
  hBox.pack_start(name);
  hBox.set_spacing(BoxSpacing);

  VBox* const v_box = get_vbox();
  ASSERT(v_box != NULL);
  v_box->pack_start(hBox);
  
  // Configure the dialog
  v_box->set_spacing(BoxSpacing);
  set_border_width(BorderPadding);
  set_resizable(false);

  // Configure the Cancel and Render buttons    
  add_button(Stock::CANCEL, RESPONSE_CANCEL);
  add_button(Stock::OK, RESPONSE_OK);
  set_default_response(RESPONSE_OK);

  show_all_children();
}

const Glib::ustring SequenceName::get_name() const
{
  return name.get_text();
}

}   // namespace dialogs
}   // namespace gui
