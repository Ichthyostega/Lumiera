/*
  Render  -  dialog to define render output

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

* *****************************************************/


/** @file render.cpp
 ** Implementation of stage:dialog::Render, which is a Dialog
 ** to set up a renter process and define output name and format.
 ** @todo this widget is not currently used, as of 3/23;
 **    it is a leftover from the old GTK-2 GUI of Lumiera,
 **    but was retained since it might be useful in future...
 */


#include "stage/gtk-base.hpp"

#include "stage/dialog/render.hpp"
#include "stage/dialog/dialog.hpp"
#include "include/logging.h"


using namespace Gtk;         //////////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

namespace stage {
namespace dialog {
  
  Render::Render (Window& parent)
    : Dialog(_("Render"), parent, true)
    , outputFileLabel_(_("Output File:"))
    , browseButtonImage_(StockID(Stock::INDEX), ICON_SIZE_BUTTON)
    , outputFileBrowseButton_(_("_Browse..."))
    , containerFormatLabel_(_("Container Format:"))
    , renderButtonImage_(StockID(Stock::OK), ICON_SIZE_BUTTON)
    , audioFrame_(_("Audio"))
    , videoFrame_(_("Video"))
    {
      Box* v_box = get_vbox();
      REQUIRE (v_box != NULL);
      
      // The Output File Row
      outputFileHBox_.pack_start (outputFileLabel_, PACK_SHRINK);
      outputFileHBox_.pack_start (outputFilePathEntry_);
      
      outputFileBrowseButton_.set_image (browseButtonImage_);
      outputFileBrowseButton_.signal_clicked().connect(
        sigc::mem_fun(*this, &Render::on_button_browse));  
      
      outputFileHBox_.pack_start (outputFileBrowseButton_, PACK_SHRINK);
      outputFileHBox_.set_spacing(4);
      v_box->pack_start (outputFileHBox_, PACK_SHRINK);
      
      // The Container Format Row
      containerFormatHBox_.pack_start (containerFormatLabel_, PACK_SHRINK);
      containerFormatHBox_.pack_start (containerFormat_);
      containerFormatHBox_.set_spacing(4);
      v_box->pack_start (containerFormatHBox_, PACK_SHRINK);
      
      v_box->pack_start (audioFrame_);
      v_box->pack_start (videoFrame_);
      
      // Configure the dialog
      v_box->set_spacing (BoxSpacing);
      set_border_width (BorderPadding);
      set_resizable (false);
      
      // Configure the Cancel and Render buttons    
      add_button (Stock::CANCEL, RESPONSE_CANCEL);
      
      Button* render_button = add_button (Stock::OK, RESPONSE_OK);
      render_button->set_label (_("_Render"));
      render_button->set_image (renderButtonImage_);
      //render_button->set_flags(Gtk::CAN_DEFAULT);
      render_button->grab_default();
      
      show_all_children();
    }
  
  void
  Render::on_button_browse()
  {
    FileChooserDialog dialog (*this,
                              _("Select a File Name for Rendering"),
                              FILE_CHOOSER_ACTION_SAVE);
    
    // Add response buttons the the dialog:
    dialog.add_button (Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button (Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
    
    int result = dialog.run();
    INFO (stage, "%d", result);
    if (result == RESPONSE_OK)
      INFO(stage, "%s", "RESPONSE_OK");
  }
  
  
}} // namespace stage::dialog
