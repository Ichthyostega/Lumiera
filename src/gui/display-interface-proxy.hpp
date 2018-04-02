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



/** @file display-interface-proxy.hpp
 ** This is an implementation fragment, intended to be included into common/interfaceproxy.cpp
 ** 
 ** The purpose is to define a proxy implementation of lumiera::Display, in order to
 ** redirect any calls through the associated C Language Interface "lumieraorg_Display"
 ** 
 ** @see display-facade.hpp
 ** @see display-service.hpp actual implementation within the GUI
 */






/* ==================== gui::Display ====================================== */

#include "include/display-facade.h"
#include "include/interfaceproxy.hpp"

namespace lumiera {
  
  /** storage for the facade proxy factory
   *  used by client code to invoke through the interface */
  lumiera::facade::Accessor<Display> Display::facade;
  
  /// emit the vtable here into this translation unit within liblumieracommon.so...
  Display::~Display()      { }
  
} // namespace lumiera



namespace lumiera {
namespace facade {
  
  typedef InstanceHandle< LUMIERA_INTERFACE_INAME(lumieraorg_Display, 0)
                        , lumiera::Display
                        > IHandle_Display;
  
  
  template<>
  class Proxy<IHandle_Display>
    : public Binding<IHandle_Display>
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
      using IBinding::IBinding;
    };
  
  
//  template  void openProxy<IHandle_Display>  (IHandle_Display const&);
//  template  void closeProxy<IHandle_Display> (void);
  
}} // namespace lumiera::facade
