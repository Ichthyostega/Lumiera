/*
  render.hpp  -  Definition of the render output dialog

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
 ** This file contains the definition of the render output dialog
 **
 */

#ifndef RENDER_HPP
#define RENDER_HPP

#include "gui/gtk-lumiera.hpp"

namespace gui {
namespace dialogs {

/** 
 * The defintion of render output dialog class
 */
class Render : public Gtk::Dialog
{
public:
  Render(Gtk::Window &parent);

protected:
  void on_button_browse();

protected:
  Gtk::HBox outputFileHBox;
  Gtk::Label outputFileLabel;
  Gtk::Entry outputFilePathEntry;

  Gtk::Image browseButtonImage;
  Gtk::Button outputFileBrowseButton;

  Gtk::HBox containerFormatHBox;
  Gtk::Label containerFormatLabel;
  Gtk::ComboBox containerFormat;
  
  Gtk::Image renderButtonImage;

  Gtk::Frame audioFrame;
  Gtk::Frame videoFrame;
};

}   // namespace dialogs
}   // namespace gui

#endif // RENDER_HPP
