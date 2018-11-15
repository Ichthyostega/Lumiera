/*
  RENDER.hpp  -  dialog to define render output

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

/** @file render.hpp
 ** Dialog to set up a renter process and define output name and format.
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
