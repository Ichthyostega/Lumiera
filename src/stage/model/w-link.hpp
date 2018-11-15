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
 ** Allows to hold a connection to a [trackable] without taking ownership.
 ** The link is statefull, can be reconnected, and automatically transitions
 ** into disconnected state when the target dies. The link state can be tested
 ** by `bool` conversion. WLink objects are fully copyable, and each copy has
 ** its own attachment state and can be reconnected independently.
 ** 
 ** The purpose of WLink is to support collaborations between controllers and
 ** widgets or between widgets. Whenever some logic works with or relies on
 ** some other UI entity -- which it might even _"allocate"_ -- without taking
 ** ownership, the relation can be implemented with a WLink.
 ** 
 ** @warning this class is not threadsafe, because lib SigC++ is not either,
 **          and it can only be used reliably from within the GUI thread.
 ** @remarks the lib SigC++ is used in GTKmm for handling "Signals" and invoking
 **          "Slots" (i.e. functors) as result of such a signal. Such is dangerous
 **          within the context of an interactive UI, since widgets might go away
 **          and be destroyed due to user interaction. To deal with this problem,
 **          GTKmm derives all widgets from [trackable], which offers an auto
 **          deregistration callback, when the object is destroyed, so any
 **          remaining signal connections can be dropped to avoid calling
 **          a dead functor. We make use of the same mechanism here to
 **          install a callback to invalidate this smart-handle.
 ** 
 ** [trackable]: https://developer.gnome.org/libsigc++/stable/structsigc_1_1trackable.html#details "`sigc::trackable`"
 ** 
 ** @see \ref WLink_test
 ** @see \ref NotificationHub (usage example)
 ** 
 */


#ifndef STAGE_MODEL_W_LINK_H
#define STAGE_MODEL_W_LINK_H


#include "lib/error.hpp"
#include "lib/format-string.hpp"

#include <type_traits>
#include <sigc++/trackable.h>


namespace stage {
namespace model {
  
  namespace error = lumiera::error;
  using error::LUMIERA_ERROR_BOTTOM_VALUE;
  
  using util::_Fmt;
  
  
  /**
   * Managed link to a `sigc::trackable` UI widget, without taking ownership.
   * Automatically installs a callback to switch this link into detached state
   * when the target (widget) is destroyed.
   * @tparam TAR the actual target widget type, which can be forward declared,
   *         until the point where the widget is actually constructed or attached.
   *         This target type must derive from `sigc::trackable`
   * @warning _not_ threadsafe
   * @note only EX_SANE, since attaching, detaching and swapping might throw.
   */
  template<class TAR>
  class WLink
    {
      
      TAR* widget_;
      
      
    public:
     ~WLink()
        {
          try {
            this->clear();
          }
          ERROR_LOG_AND_IGNORE (gui, "Detaching managed WLink from Widget")
        }
      WLink()  noexcept
        : widget_{nullptr}
        { }
      
      explicit
      WLink (TAR& targetWidget)
        : widget_{attachTo (targetWidget)}
        { }
      
      WLink (WLink const& r)
        : widget_{r.widget_? attachTo(*r.widget_) : nullptr}
        { }
      
      WLink (WLink && rr)
        : widget_{rr.widget_? attachTo(*rr.widget_) : nullptr}
        {
          rr.clear();
        }
      
      WLink&
      operator= (WLink other)  ///< @warning only EX_SANE
        {
          swap (*this, other);
          return *this;
        }
      
      /** swap the pointees, including callback registration.
       * @warning only EX_SANE.
       *    Might leave the following intermediary states
       *    - only r was detached
       *    - both r and l are detached
       *    - both detached, but only l attached to the former target of r,
       *      while the former target of l is now completely detached.
       */
      friend void
      swap (WLink& l, WLink& r)
        {
          TAR* tl = l.widget_;
          TAR* tr = r.widget_;
          if (tl == tr) return;
          r.clear();
          l.clear();
          if (tr) l.widget_ = l.attachTo (*tr);
          if (tl) r.widget_ = r.attachTo (*tl);
        }
      
      
      explicit
      operator bool()  const
        {
          return bool{widget_};
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
      
      
      bool isValid()  const { return bool{*this}; }
      bool isActive() const { return bool{*this}; }
      
      
      /** detach and deactivate this link
       * @note EX_STRONG (but only under the assumption that also
       *       sigc::trackable::remove_destroy_notify_cllback is EX_STRONG)
       */
      void
      clear()
        {
          if (widget_)
            widget_->remove_destroy_notify_callback (&widget_);
          widget_ = nullptr;
        }
      
      /** (re)connect this smart link to the given target.
       *  Any previously existing link is detached beforehand
       * @note EX_SANE only (assuming `sigc::trackable` is sane)
       *    Might leave this WLink in disconnected state when throwing
       * @throws error::External when registration with `sigc::trackable`  fails
       */
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
            throw error::State ("zombie widget encountered"
                               , LERR_(BOTTOM_VALUE));
        }
      
      /** @internal installs the necessary callback
       *   to detach this link in case the target is destroyed
       *  @note EX_STRONG
       */
      TAR*
      attachTo (TAR& target)
        {
          static_assert (std::is_base_of<sigc::trackable, TAR>()
                        ,"target type required to be sigc::trackable");
          
          try {
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
          catch (std::exception& problem)
            {
              throw error::External (problem, _Fmt{"WLink could not attach to %s. Problem is %s"} % target % problem);
            }
          catch (...)
            {
              ERROR (gui, "Unknown exception while attaching WLink");
              throw error::External (_Fmt{"WLink could not attach to %s due to unidentified Problems"} % target);
            }
        }
    };
  
  
}} // namespace stage::model
#endif /*STAGE_MODEL_W_LINK_H*/
