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


#include "include/interfaceproxy.hpp"
#include "common/instancehandle.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"

using util::cStr;

namespace lumiera {
  namespace facade {
    
    
    LUMIERA_ERROR_DEFINE (FACADE_LIFECYCLE, "facade interface currently not accessible");  
    
    
    template<class IHA>
    class Holder;
    
    template<class FA, class I>
    class Holder<InstanceHandle<I,FA> >
      : Accessor<FA>,
        protected FA
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
        Proxy<IHA>::open(iha);
      }
    
    template<class IHA>
    void
    closeProxy ()
      {
        Proxy<IHA>::close();
      }
    
   } // namespace facade
  
} // namespace lumiera






    /* ===================================================================== */
    /*    including explicit Proxy-Definitions for all facade interfaces     */
    /* ===================================================================== */



    /* ==================== GuiNotification ================================ */
    
#include "gui/notification-interface-proxy.hpp"


    /* ==================== gui::Display =================================== */
    
#include "gui/display-interface-proxy.hpp"


    /* ==================== DummyPlayer ==================================== */
    
#include "proc/play/dummy-player-interface-proxy.hpp"


