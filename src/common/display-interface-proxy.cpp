/*
  Display(Proxy)  -  service providing access to a display for outputting frames

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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



/** @file display-interface-proxy.cpp
 ** Interface-Proxy for the lumiera::Display facade (experimental/deprecated).
 ** The purpose is to define a proxy implementation of lumiera::Display, in order to
 ** redirect any calls through the associated C Language Interface "lumieraorg_Display"
 ** 
 ** @see display-facade.hpp
 ** @see display-service.hpp actual implementation within the GUI
 ** @deprecated obsolete early design draft from 2009;
 **             as of 2018 only kept in source to compile some likewise obsolete UI widgets.
 */


#include "include/display-facade.h"
#include "include/interfaceproxy.hpp"

namespace lumiera {
  
  /// emit the vtable here into this translation unit within liblumieracommon.so...
  Display::~Display()      { }
  
  /** static storage for the facade access front-end */
  lib::Depend<Display> Display::facade;
  
} // namespace lumiera



namespace lumiera {
namespace facade {
  
  
  
  /* ==================== stage::Display ====================================== */
  
  using Interface = LUMIERA_INTERFACE_INAME(lumieraorg_Display, 0);
  using Facade    = lumiera::Display;
  
  using IHandle   = InstanceHandle<Interface, Facade>;
  
  
  template<>
  class Proxy<IHandle>
    : public Binding<IHandle>
    {
      //----Proxy-Implementation-of-lumiera::Display--------
      
      Display::Sink
      getHandle (LumieraDisplaySlot display)  override
        {
          _i_.allocate (display);
          Sink sinkHandle;
          sinkHandle.activate (display, _i_.release);
          if (lumiera_error_peek() || !sinkHandle)
            throw lumiera::error::State("failed to allocate output DisplayerSlot",
                                        lumiera_error());
          return sinkHandle;
        }
      
      
    public:
      using Binding<IHandle>::Binding;
    };
  
  
  /**  emit proxy code here... */
  template
  class Link<Interface,Facade>;
  
}} //namespace facade::lumiera
