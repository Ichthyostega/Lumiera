/*
  preferences-dialog.cpp  -  Implementation of the application preferences dialog

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

#include "gui/dialogs/preferences-dialog.hpp"
#include "gui/dialogs/dialog.hpp"

using namespace Gtk;

namespace gui {
namespace dialogs {

PreferencesDialog::PreferencesDialog(Window &parent) :
  Dialog(_("Preferences"), parent, true)
{
  Box *v_box = get_vbox();
  REQUIRE(v_box != NULL);

  interfaceBox.pack_start(interfaceThemeCombo, PACK_SHRINK);
  interfaceBox.set_spacing(4);
  interfaceBox.set_border_width(5);

  notebook.append_page(interfaceBox, _("Interface"));
  
  v_box->pack_start(notebook);

  // Configure the dialog
  v_box->set_spacing(BoxSpacing);
  set_border_width(BorderPadding);
  set_resizable(false);

  // Configure the Cancel and OK buttons    
  add_button(Stock::CANCEL, RESPONSE_CANCEL);
  add_button(Stock::OK, RESPONSE_OK);

  show_all_children();
}

}   // namespace dialogs
}   // namespace gui
