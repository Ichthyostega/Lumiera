/*
  FLASH-DECO.hpp  -  widget decorator to add a visual flash action

   Copyright (C)
     2018,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file flash-deco.hpp
 ** Decorator to add the ability to display a visual flash action to a GTK widget.
 ** The protocol for operating UI elements connected to the [UI-Bus](\ref ui-bus.hpp) includes
 ** a message to make the receiving widget flash for a short time, to mandate the user's attention.
 ** The visual change itself is effected by adding a CSS class, while adding a timeout callback
 ** to revert to normal display after a short timespan. In itself, this modification sequence
 ** is entirely generic, and can thus be added by decorating the widget to affect; moreover,
 ** adding such a callback need to be done properly, to avoid a crash in case the widget
 ** is destroyed during the timeout period.
 ** 
 ** @see [UI-Element protocol](\ref tangible.hpp)
 ** @see [usage example](\ref error-log-display.hpp)
 ** 
 */


#ifndef STAGE_MODEL_FLASH_DECO_H
#define STAGE_MODEL_FLASH_DECO_H


#include "stage/gtk-base.hpp"
#include "stage/style-scheme.hpp"


namespace stage {
namespace model {
  
  
  /**
   * Decorator for a Gtk::Widget to add a visual flash action.
   * @tparam WIT type of the target widget to decorate. All ctors are passed through.
   * @remark invoke the #flash function to trigger
   * @see \ref stage::CSS_CLASS_FLASH
   */
  template<class WIT>
  class FlashDeco
    : public WIT
    {
      static_assert (std::is_base_of<Gtk::Widget, WIT>()
                    ,"wrapped target type required to be a Gtk::Widget");
    public:
      using WIT::WIT;
      
      void
      flash()
        {
          auto styleContext = this->get_style_context();
          styleContext->add_class (CLASS_indication_flash);
          
          Glib::signal_timeout()
            .connect_once (sigc::mem_fun(*this, &FlashDeco::flashback)
                          ,STYLE_FLASH_DURATION_in_ms
                          ,Glib::PRIORITY_LOW);   // after all pending drawing tasks
        }
      
    private:
      void
      flashback()
        {
          auto styleContext = this->get_style_context();
          styleContext->remove_class (CLASS_indication_flash);
        }
    };
  
  
}} // namespace stage::model
#endif /*STAGE_MODEL_FLASH_DECO_H*/
