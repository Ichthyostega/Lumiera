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

#include "render.hpp"

using namespace Gtk;

namespace lumiera {
namespace gui {
namespace dialogs {

  Render::Render(Window &parent) :
    Dialog(_("Render"), parent, true),
    outputFileLabel(_("Output File:")),
    browseButtonImage(StockID(Stock::INDEX), ICON_SIZE_BUTTON),
    browseButtonLabel(_("_Browse..."), true),
    containerFormatLabel(_("Container Format:")),
    cancelButton(Stock::CANCEL),
    renderButtonImage(StockID(Stock::APPLY), ICON_SIZE_BUTTON),
    renderButtonLabel(_("_Render"), true)
  {
    VBox *v_box = get_vbox();
    g_assert(v_box != NULL);

    // The Output File Row
    outputFileHBox.pack_start(outputFileLabel, PACK_SHRINK);
    outputFileHBox.pack_start(outputFilePathEntry);

    browseButtonHBox.pack_start(browseButtonImage);
    browseButtonHBox.pack_start(browseButtonLabel);
    outputFileBrowseButton.add(browseButtonHBox);

    outputFileHBox.pack_start(outputFileBrowseButton, PACK_SHRINK);
    outputFileHBox.set_spacing(4);
    v_box->pack_start(outputFileHBox, PACK_SHRINK);

    // The Containter Format Row
    containerFormatHBox.pack_start(containerFormatLabel, PACK_SHRINK);
    containerFormatHBox.pack_start(containerFormat);
    containerFormatHBox.set_spacing(4);
    v_box->pack_start(containerFormatHBox, PACK_SHRINK);

    // Configure the dialog
    v_box->set_spacing(4);
    set_border_width(5);
    set_resizable(false);

    // Configure the Cancel and Render buttons
    Gtk::Box* action_area = get_action_area();
    g_assert(action_area != NULL);
    
    cancelButton.signal_clicked().connect(
              sigc::mem_fun(*this, &Render::on_button_cancel));  
    action_area->pack_start(cancelButton);

    renderButtonHBox.pack_start(renderButtonImage);
    renderButtonHBox.pack_start(renderButtonLabel);
    renderButton.add(renderButtonHBox);
    renderButton.signal_clicked().connect(
              sigc::mem_fun(*this, &Render::on_button_render));
    renderButton.set_flags(Gtk::CAN_DEFAULT);
    action_area->pack_start(renderButton);
    renderButton.grab_default();

    show_all_children();
  }

  void Render::on_button_render()
  {
    g_message("render");
    hide();
  }

  void Render::on_button_cancel()
  {
    g_message("cancel");
    hide();
  }



}   // namespace dialogs
}   // namespace gui
}   // namespace lumiera
