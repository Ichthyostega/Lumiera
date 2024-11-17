/*
  MINI-BUTTON.hpp  -  a tool button-like widget

   Copyright (C)
     2009,            Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file mini-button.hpp
 ** Adapter to wrap button widgets intended to serve as tool buttons
 ** @todo leftover from the old GTK-2 Lumiera UI as of 3/23,
 **       but should be retained and integrated into the reworked GTK-3 UI 
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
  
  
  
}}// stage::widget
#endif /*STAGE_WIDGET_MINI_BUTTON_H*/
