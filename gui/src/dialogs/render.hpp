/*
  render.hpp  -  Definition of the render output dialog
 
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
 
*/
/** @file render.hpp
 ** This file contains the definition of the render output dialog
 **
 */

#ifndef RENDER_H
#define RENDER_H

#include <gtkmm.h>

using namespace Gtk;

namespace lumiera {
namespace gui {
namespace dialogs {

  /** 
   * The defintion of render output dialog class
   */
  class Render : public Dialog
  {
  public:
    Render(Window &parent);

  protected:
    void on_button_render();

    void on_button_cancel();

  protected:
    HBox outputFileHBox;
    Label outputFileLabel;
    Entry outputFilePathEntry;

    HBox browseButtonHBox;
    Image browseButtonImage;
    Label browseButtonLabel;
    Button outputFileBrowseButton;

    HBox containerFormatHBox;
    Label containerFormatLabel;
    ComboBox containerFormat;

    Button cancelButton;

    HBox renderButtonHBox;
    Image renderButtonImage;
    Label renderButtonLabel;
    Button renderButton;
  };

}   // namespace dialogs
}   // namespace gui
}   // namespace lumiera

#endif // RENDER_H
