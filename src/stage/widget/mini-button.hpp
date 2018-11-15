/*
  MINI-BUTTON.hpp  -  a tool button-like widget

  Copyright (C)         Lumiera.org
    2009,               Joel Holdsworth <joel@airwebreathe.org.uk>

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


/** @file mini-button.hpp
 ** Adapter to wrap button widgets intended to serve as tool buttons
 */



#ifndef STAGE_WIDGET_MINI_BUTTON_H
#define STAGE_WIDGET_MINI_BUTTON_H

#include "stage/gtk-base.hpp"


namespace stage {
namespace widget {
  
  /**
   * A wrapper for ToolButton-like Button widgets
   */
  template<class T>
  class MiniWrapper
    : public T
    {
      /** image widget for the button. */
      Gtk::Image image_;
      
    public:
      /**
       * Creates a new Button containing the image and text from a stock item.
       * @param stock_id The stock_id of the image.
       * @param icon_size The size of the image to show.
       * @remarks Stock IDs have identifiers like \c Gtk::Stock::OK and \c Gtk::Stock::APPLY.
       */
      MiniWrapper (Gtk::StockID const& stock_id
                  ,const Gtk::IconSize icon_size = Gtk::ICON_SIZE_LARGE_TOOLBAR)
        : image_(stock_id, icon_size)
        {
          T::add (image_);
          T::set_relief (Gtk::RELIEF_NONE);
          T::set_focus_on_click (false);
        }
      
      
      /**
       * Sets a new image from a stock-id for this button.
       * @param stock_id The stock_id of the image.
       * @param icon_size The size of the image to show.
       */
      void
      setStockID (Gtk::StockID const& stock_id
                 ,const Gtk::IconSize icon_size = Gtk::ICON_SIZE_LARGE_TOOLBAR)
        {
          image_.set (stock_id, icon_size);
        }
    };
  
  
  
  /** A ToolButton-like widget */
  using MiniButton = MiniWrapper<Gtk::Button>;
  
  /** A ToggleToolButton-like widget */
  using MiniToggleButton = MiniWrapper<Gtk::ToggleButton>;
  
  
  
}}// gui::widget
#endif /*STAGE_WIDGET_MINI_BUTTON_H*/
