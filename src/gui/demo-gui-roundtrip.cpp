/*
  DemoGuiRoundtrip  -  placeholder to further progress of the GUI-Session connection

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file demo-gui-roundtrip.cpp
 ** Dummy and scaffolding to help development of the UI - Session connection.
 ** The Goal is to establish a full messaging round-trip between UI and Proc-Layer
 ** - a menu entry
 ** - which issues a Proc-Command
 ** - which in turn accesses the Notification-facade
 ** - and sends a "mark" message towards some UI demo widget
 ** - which in turn displays the mark text
 ** 
 ** @todo WIP as of 8/20176          ////////////////////////////////////////////////////////////////////////TICKET #1099
 ** 
 ** @see CallQueue_test
 ** @see notification-service.hpp
 ** 
 */

#include "gui/gtk-base.hpp"

#include "lib/error.hpp"
#include "lib/depend.hpp"
#include "gui/ui-bus.hpp"
#include "gui/ctrl/ui-manager.hpp"
#include "lib/nocopy.hpp"

#include <string>



//using lumiera::error::LUMIERA_ERROR_STATE;

using std::string;


namespace gui {
  
  namespace { // implementation details
  
//using util::isnil;
    using std::string;
    
  } // (End) impl details
  
  
  
  
  
  /**
   * Quick-n-dirty hack to establish a prototypic connection between Application Core and UI...
   * 
   * @todo be commendable
   * @todo don't misbehave
   * 
   * @see SomeSystem
   * @see CallQueue_test
   */
  class DemoGuiRoundtrip
    : util::NonCopyable
    {
      string nothing_;

      DemoGuiRoundtrip();
     ~DemoGuiRoundtrip();

      friend class lib::DependencyFactory;

    public:
      /** access point to set up the scaffolding.
       * @internal this is a development helper and will be remoulded all the time.
       */
      static lib::Depend<DemoGuiRoundtrip> instance;
      
      
      /* == X interface for == */

      /* == Adapter interface for == */

#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1099
      void setSolution (string const& solution = "")
        {
          UNIMPLEMENTED ("tbw");
          if (isDeaf())
          this->transmogrify (solution);
        }
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1099
    };
  
  
  
} // namespace gui
