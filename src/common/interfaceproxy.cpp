/*
  InterfaceProxy  -  definition of forwarding proxies for the facade interfaces

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file interfaceproxy.cpp
 ** Implementation of C++ binding proxies on top of the (plain-C based)
 ** interface system. This is an implementation facility within the application core,
 ** which allows to embody just an ["interface instance handle"](\ref instancehandle.hpp),
 ** in order to get RAII-style registration of interfaces and loading of plug-ins.
 ** 
 ** A *crucial requirement* for this approach to work is, that any relevant interface
 ** to be bound and exposed as C++ object needs to set up a concrete specialisation of
 ** lumiera::facade::Proxy to drive instantiation of the actual binding proxy.
 ** The relevant specialisations _need to be included explicitly_ into this
 ** compilation unit!
 ** 
 ** The result of this setup is that clients can just invoke `SomeInterface::facade()`
 ** and thus call through proper C++ bindings with type safety and automatic
 ** lifecycle management.
 ** 
 */


#include "include/interfaceproxy.hpp"
#include "common/instancehandle.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"

using util::cStr;

namespace lumiera{
namespace facade {
  
  
  LUMIERA_ERROR_DEFINE (FACADE_LIFECYCLE, "facade is closed; service currently not accessible");
  
  
  /**
   * Implementation Base
   * for building Facade Proxy implementations.
   * Typically the purpose of such a proxy is to route
   * any calls through the C-Bindings of the Lumiera Interface system.
   * The actual storage for the concrete proxy object is embedded,
   * inline within the #open() function. For access by the clients,
   * a frontend-object of type \c Accessor<FA> may be placed into
   * the facade interface; this accessor-frontend is basically
   * a concealed static pointer to the proxy, and will be set,
   * when the interface is opened. This opening and closing
   * of the interface itself is controlled by the
   * InstanceHandle, which in turn is typically
   * created and managed within the context
   * of the service implementation.
   */
  template<class IHA>
  class Holder;
  
  template<class FA, class I>
  class Holder<InstanceHandle<I,FA>>
    : Accessor<FA>
    , protected FA
    {
    protected:
      typedef InstanceHandle<I,FA> IHandle;
      typedef Holder<IHandle> THolder;
      typedef Proxy<IHandle> TProxy;
      typedef Accessor<FA> Access;
      
      I& _i_;
      
      Holder (IHandle const& iha)
        : _i_(iha.get())
        {  }
      
    public:
      static TProxy& open(IHandle const& iha)
        {
          static char buff[sizeof(TProxy)];
          TProxy* p = new(buff) TProxy(iha);
          Access::implProxy_ = p;
          return *p;
        }
      
      static void close()
        {
          if (!Access::implProxy_) return;
          TProxy* p = static_cast<TProxy*> (Access::implProxy_);
          Access::implProxy_ = 0;
          p->~TProxy();
        }
    };
  
  
  template<class FA>
  FA* Accessor<FA>::implProxy_;
  
  
  template<class IHA>
  void
  openProxy (IHA const& iha)
    {
//      Proxy<IHA>::open(iha);
    }
  
  template<class IHA>
  void
  closeProxy ()
    {
//      Proxy<IHA>::close();
    }
  /////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1086 : new implementation draft below....
  template<class IHA>
  class Binding;
  
  template<class FA, class I>
  class Binding<InstanceHandle<I,FA>>
    : public FA
    {
    protected:
      typedef InstanceHandle<I,FA> IHandle;
      typedef Binding<IHandle> IBinding;
      
      I& _i_;
      
      Binding (IHandle const& iha)
        : _i_(iha.get())
        {  }
    };
  
}} // namespace lumiera::facade






    /* ===================================================================== */
    /*    including explicit Proxy-Definitions for all facade interfaces     */
    /* ===================================================================== */



    /* ==================== SessionCommand ================================= */
    
#include "proc/control/session-command-interface-proxy.hpp"


    /* ==================== GuiNotification ================================ */
    
#include "gui/notification-interface-proxy.hpp"


    /* ==================== gui::Display =================================== */
    
#include "gui/display-interface-proxy.hpp"


    /* ==================== DummyPlayer ==================================== */
    
#include "proc/play/dummy-player-interface-proxy.hpp"


