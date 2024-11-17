/*
  NameChooser  -  dialog to enter a string name

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file name-chooser.cpp
 ** Implementation details of NameChooser,
 ** a dialog to enter a name string.
 ** @todo this widget is not currently used, as of 3/23;
 **    it is a leftover from the old GTK-2 GUI of Lumiera,
 **    but was retained since it might be useful in future...
 */


#include "stage/gtk-base.hpp"
#include "stage/dialog/name-chooser.hpp"
#include "stage/dialog/dialog.hpp"

using namespace Gtk;         //////////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace Glib;        //////////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

namespace stage {
namespace dialog {
  
  NameChooser::NameChooser (Window &parent,
                            cuString title,
                            cuString defaultName)
    : Dialog::Dialog(title, parent, true)
    , caption_(_("Name:"))
    {
      // Add the controls
      name_.set_text (defaultName);
      name_.set_activates_default();
      
      hBox_.pack_start (caption_);
      hBox_.pack_start (name_);
      hBox_.set_spacing (BoxSpacing);
    
      Box* const v_box = get_vbox();
      REQUIRE (v_box != NULL);
      v_box->pack_start (hBox_);
      
      // Configure the dialog
      v_box->set_spacing (BoxSpacing);
      set_border_width (BorderPadding);
      set_resizable (false);
    
      // Configure the Cancel and Render buttons    
      add_button (Stock::CANCEL, RESPONSE_CANCEL);
      add_button (Stock::OK, RESPONSE_OK);
      set_default_response (RESPONSE_OK);
    
      show_all_children();
    }
  
  
  
}} // namespace stage::dialog
