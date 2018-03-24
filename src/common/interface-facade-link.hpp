/*
  INTERFACE-FACADE-LINK  -  a switchable link from interface to service implementation

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file interface-facade-link.hpp
 ** Opening, accessing and closing the service access through a facade interface.
 ** Client code is assumed to access an application level service through an facade
 ** interface, while the actual implementation object remains an opaque internal detail.
 ** Moreover, services may come up and shut down, so the implementation might change
 ** during the Lifecycle. The facility defined here in this header provides a basic
 ** implementation for this access mechanism, but without any adaptation, binding
 ** or plugin access layer. It works only under the assumption that both the 
 ** interface and the actual service implementation coexist in the same
 ** executable and are written in C++, so any invocation of an 
 ** interface method boils down to a language-level call.
 ** 
 ** Usually, client code doesn't need to include this header. Clients are assumed
 ** to use the facade interface of the service in question. This facade interface
 ** contains a static member of type \c lumiera::facade::Accessor<I> (where I is
 ** the type of the facade interface). The Accessor baseclass is defined in
 ** interfaceproxy.hpp and typically included through the facade header.
 ** 
 ** @note there is a way more elaborate implementation of the same mechanism
 **       for use with the Lumiera Interface/Plugin system.
 **
 ** @see interfaceproxy.hpp description of the more general use case
 ** @see PlayService example for the simple use case
 */


#ifndef LUMIERA_FACADE_INTERFACE_FACADE_LINK_H
#define LUMIERA_FACADE_INTERFACE_FACADE_LINK_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/meta/util.hpp"
#include "include/interfaceproxy.hpp"
#include "lib/symbol.hpp"




namespace lumiera {
namespace facade {
  
  using lib::Literal;
  
  
  /********************************************************************//**
   * simple access-frontend to the implementation of a service (C++ only).
   * Usually, an instance of Accessor is placed as static member right into
   * the facade interface used to access the service. This implementation
   * of the access mechanism handles the simple case that both the facade
   * and the service implementation are written in C++ and calls happen
   * within the main executable as direct language calls, without an
   * binding layer and without involving the Interface/Plugin system.
   * 
   * Typically, the InterfaceFacadeLink becomes a member of the service
   * implementation class and is directly tied into the constructor of
   * the latter. Being a subclass of lumiera::facade::Accessor, it is
   * allowed to "open" the facade access just by setting the static
   * protected pointer Accessor::implProxy_
   */
  template<class FA>
  class InterfaceFacadeLink
    : protected Accessor<FA>
    , util::NonCopyable
    {
      string displayName_;
      
      void
      __checkLifecycle ()
        {
          if (Accessor<FA>::implProxy_)
            throw error::State("Attempt to open an already opened Facade interface."
                              , error::LUMIERA_ERROR_LIFECYCLE);
        }
      
    public:
      InterfaceFacadeLink(FA& serviceImpl, Literal interfaceName_for_Log=0)
        : displayName_(interfaceName_for_Log? string(interfaceName_for_Log)
                                            : util::typeStr<FA>())
        {
          __checkLifecycle();
          Accessor<FA>::implProxy_ = &serviceImpl;
          INFO (interface, "interface %s opened", displayName_.c_str());
        }
      
     ~InterfaceFacadeLink()
        {
          INFO (interface, "closing interface %s...", displayName_.c_str());
          Accessor<FA>::implProxy_ = 0;
        }
    };
  
  
  
  /** storage for the static access pointer */
  template<class FA>
  FA* Accessor<FA>::implProxy_;
  
  
}} // namespace lumiera::facade

#endif
