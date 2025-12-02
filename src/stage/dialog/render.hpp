/*
  RENDER.hpp  -  dialog to define render output

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file render.hpp
 ** Dialog to set up a renter process and define output name and format.
 ** @todo this widget is not currently used, as of 3/23;
 **    it is a leftover from the old GTK-2 GUI of Lumiera,
 **    but was retained since it might be useful in future...
 **
 */


#ifndef STAGE_DIALOG_RENDER_H
#define STAGE_DIALOG_RENDER_H

#include "stage/gtk-base.hpp"


namespace stage {
namespace dialog {
  
  
  /** 
   * A dialog to choose render output format and name
   */
  class Render
    : public Gtk::Dialog
    {
      Gtk::HBox  outputFileHBox_;
      Gtk::Label outputFileLabel_;
      Gtk::Entry outputFilePathEntry_;
    
      Gtk::Image  browseButtonImage_;
      Gtk::Button outputFileBrowseButton_;
    
      Gtk::HBox  containerFormatHBox_;
      Gtk::Label containerFormatLabel_;
      Gtk::ComboBox containerFormat_;
      
      Gtk::Image renderButtonImage_;
    
      Gtk::Frame audioFrame_;
      Gtk::Frame videoFrame_;
      
    public:
      Render (Gtk::Window& parent);
      
    private:
      void on_button_browse();
    };
  
  
}} // namespace stage::dialog
#endif /*STAGE_DIALOG_RENDER_H*/
