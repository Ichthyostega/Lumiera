/*
  FLASH-DECO.hpp  -  widget decorator to add a visual flash action

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file flash-deco.hpp
 ** Decorator to add the ability to display a visual flash action to a GTK widget.
 ** The protocol for operating UI elements connected to the [UI-Bus](\ref ui-bus.hpp) includes
 ** a message to make the receiving widget flash for a short time, to mandate the users attention.
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


#ifndef GUI_MODEL_FLASH_DECO_H
#define GUI_MODEL_FLASH_DECO_H


#include "gui/gtk-base.hpp"
#include "gui/style-scheme.hpp"

//#include <utility>


namespace gui {
namespace model {

//  using std::move;
  
  
  /**
   * Decorator for a Gtk::Widget to add a visual flash action.
   * @tparam WIT type of the target widget to decorate. All ctors are passed through.
   * @remark invoke the #flash function to trigger
   * @see \ref gui::CSS_CLASS_FLASH
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
          styleContext->add_class (CSS_CLASS_FLASH);
          
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
          styleContext->remove_class (CSS_CLASS_FLASH);
        }
    };
  
  
  
}} // namespace gui::model
#endif /*GUI_MODEL_FLASH_DECO_H*/
