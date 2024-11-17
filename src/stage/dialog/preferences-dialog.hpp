/*
  PREFERENCES-DIALOG.hpp  -  dialog for application preferences

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file preferences-dialog.hpp
 ** Dialog to manage application preferences.
 ** @todo as of 2016, this is more of a place-holder, since we also
 **       do not have decided on the implementation approach for
 **       application preferences in general.
 */


#ifndef STAGE_DIALOG_PREFERENCES_DIALOG_H
#define STAGE_DIALOG_PREFERENCES_DIALOG_H


#include "stage/gtk-base.hpp"
#include "stage/dialog/dialog.hpp"

namespace stage {
namespace dialog {
  
  
  class PreferencesDialog
    : public Gtk::Dialog
    {
      Gtk::Notebook notebook_;
    
      Gtk::VBox     interfaceBox_;
      Gtk::ComboBox interfaceThemeCombo_;
      
    public:
      PreferencesDialog (Gtk::Window &parent)
        : Dialog(_("Preferences"), parent, true)
        {
          using namespace Gtk;   /////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
          
          Box *v_box = get_vbox();
          REQUIRE (v_box != NULL);
        
          interfaceBox_.pack_start (interfaceThemeCombo_, PACK_SHRINK);
          interfaceBox_.set_spacing(4);
          interfaceBox_.set_border_width(5);
        
          notebook_.append_page (interfaceBox_, _("Interface"));
          
          v_box->pack_start (notebook_);
        
          // Configure the dialog
          v_box->set_spacing (BoxSpacing);
          set_border_width (BorderPadding);
          set_resizable (false);
        
          // Configure the Cancel and OK buttons    
          add_button (Stock::CANCEL, RESPONSE_CANCEL);
          add_button (Stock::OK, RESPONSE_OK);
        
          show_all_children();
        }
    };
  
  
}} // namespace stage::dialog
#endif /*STAGE_DIALOG_PREFERENCES_DIALOG_H*/
