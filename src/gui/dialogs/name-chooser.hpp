/*
  name-chooser.hpp  -  Definition of the name chooser dialog object

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
/** @file name-chooser.hpp
 ** This file contains the definition of the name chooser dialog
 **
 */

#ifndef NAME_CHOOSER_H
#define NAME_CHOOSER_H

#include "gui/gtk-lumiera.hpp"

namespace gui {
namespace dialogs {

/** 
 * The name chooser dialog is a modal dialog box that prompts the user
 * to choose a string name.
 */
class NameChooser : public Gtk::Dialog
{
public:
  /**
   * Creates a name chooser dialog.
   * @param parent The window which will be the parent of this dialog.
   * @param title The string for the title of this dialog.
   * @param default_name The name that will be shown by default in the
   * edit box of the dialog.
   */
  NameChooser(Gtk::Window &parent, cuString title,
    cuString default_name);
  
  /**
   * Gets the current name of the chosen in the dialog.
   * @return Returns the name currently typed into the edit box of the
   * dialog.
   */
  cuString get_name() const;

private:
  Gtk::HBox hBox;
  Gtk::Label caption;
  Gtk::Entry name;
};

}   // namespace dialogs
}   // namespace gui

#endif // NAME_CHOOSER_H
