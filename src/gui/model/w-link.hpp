/*
  W-LINK.hpp  -  smart-link to GTK widget

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


/** @file w-link.hpp
 ** A smart link to an GTK widget with automatic disconnection.
 ** Allows to hold a connection to a `sigc::trackable` without ownership.
 ** The is statefull, can be reconnected, and automatically transitions into disconnected
 ** state when the target dies.
 ** 
 ** @warning this class is not threadsafe, because lib SigC++ is not either,
 **          and it can only be used reliably from within the GUI thread.
 ** 
 ** @see \ref WLink_test
 ** @see \ref NotificationHub (usage example)
 ** 
 */


#ifndef GUI_MODEL_W_LINK_H
#define GUI_MODEL_W_LINK_H


#include "lib/error.hpp"
//#include "lib/symbol.hpp"

#include <type_traits>
#include <sigc++/trackable.h>
//#include <utility>
//#include <string>


namespace gui {
namespace model {
  
//  using std::string;
//  using lib::Symbol;
  using lumiera::error::LUMIERA_ERROR_BOTTOM_VALUE;
  
  
  /**
   * Managed link to a `sigc::trackable` UI widget, without taking ownership.
   * @warning _not_ threadsafe
   */
  template<class TAR>
  class WLink
    {
      static_assert (std::is_base_of<sigc::trackable, TAR>()
                    ,"target type required to be sigc::trackable");
      
      //mutable
      TAR* widget_;
      
      
    public:
     ~WLink()
        {
          this->clear();
        }
      WLink()
        : widget_{nullptr}
        { }
      
      explicit
      WLink(TAR& targetWidget)
        : widget_{attachTo (targetWidget)}
        { }
      
      ////////////////////////////TODO copy operations
      
      explicit
      operator bool()  const
        {
          return bool(widget_);
        }
      
      TAR&
      operator* ()  const
        {
          __ensureAlive();
          return *widget_;
        }
      
      TAR*
      operator-> ()  const
        {
          __ensureAlive();
          return widget_;
        }
      
      
      void
      clear()
        {
          if (widget_)
            widget_->remove_destroy_notify_callback (&widget_);
          widget_ = nullptr;
        }
      
      void
      connect (TAR& otherTarget)
        {
          if (widget_ == &otherTarget) return;
          clear();
          widget_ = attachTo (otherTarget);
        }
      
    private:
      void
      __ensureAlive()  const
        {
          if (not widget_)
            throw lumiera::error::State ("zombie widget encountered"
                                        , LERR_(BOTTOM_VALUE));
        }
      
      TAR*
      attachTo (TAR& target)
        {
          target.add_destroy_notify_callback (&widget_
                                             ,[](void* p)
                                                {
                                                  TAR* & widgetPtr = *static_cast<TAR**>(p);
                                                  ASSERT (widgetPtr);
                                                  widgetPtr = nullptr;
                                                  return p;
                                                });
          return &target;
        }
    };
  
  
  
  /** c */
  
  
  
}} // namespace gui::model
#endif /*GUI_MODEL_W_LINK_H*/
