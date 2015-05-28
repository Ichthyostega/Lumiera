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


#include "gui/gtk-lumiera.hpp"

#include "gui/dialogs/render.hpp"
#include "gui/dialogs/dialog.hpp"
#include "include/logging.h"


using namespace Gtk;

namespace gui {
namespace dialogs {
  
  Render::Render (Window& parent)
    : Dialog(_("Render"), parent, true)
    , outputFileLabel(_("Output File:"))
    , browseButtonImage(StockID(Stock::INDEX), ICON_SIZE_BUTTON)
    , outputFileBrowseButton(_("_Browse..."))
    , containerFormatLabel(_("Container Format:"))
    , renderButtonImage(StockID(Stock::OK), ICON_SIZE_BUTTON)
    , audioFrame(_("Audio"))
    , videoFrame(_("Video"))
    {
      Box* v_box = get_vbox();
      REQUIRE (v_box != NULL);
      
      // The Output File Row
      outputFileHBox.pack_start (outputFileLabel, PACK_SHRINK);
      outputFileHBox.pack_start (outputFilePathEntry);
      
      outputFileBrowseButton.set_image (browseButtonImage);
      outputFileBrowseButton.signal_clicked().connect(
        sigc::mem_fun(*this, &Render::on_button_browse));  
      
      outputFileHBox.pack_start (outputFileBrowseButton, PACK_SHRINK);
      outputFileHBox.set_spacing(4);
      v_box->pack_start (outputFileHBox, PACK_SHRINK);
      
      // The Container Format Row
      containerFormatHBox.pack_start (containerFormatLabel, PACK_SHRINK);
      containerFormatHBox.pack_start (containerFormat);
      containerFormatHBox.set_spacing(4);
      v_box->pack_start (containerFormatHBox, PACK_SHRINK);
      
      v_box->pack_start (audioFrame);
      v_box->pack_start (videoFrame);
      
      // Configure the dialog
      v_box->set_spacing (BoxSpacing);
      set_border_width (BorderPadding);
      set_resizable (false);
      
      // Configure the Cancel and Render buttons    
      add_button (Stock::CANCEL, RESPONSE_CANCEL);
      
      Button* render_button = add_button (Stock::OK, RESPONSE_OK);
      render_button->set_label (_("_Render"));
      render_button->set_image (renderButtonImage);
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
    INFO (gui, "%d", result);
    if (result == RESPONSE_OK)
      INFO(gui, "%s", "RESPONSE_OK");
  }
  
  
}} // namespace gui::dialog
