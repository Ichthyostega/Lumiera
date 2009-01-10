/*
  InterfaceProxy  -  definition of forwarding proxies for the facade interfaces
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
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


#include "include/guinotificationfacade.h"

namespace gui {
  
  /** storage for the facade proxy factory used by client code to invoke through the interface */
  lumiera::facade::Accessor<GuiNotification> GuiNotification::facade;

} // namespace gui




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
        typedef Proxy<IHandle> Proxy;
        typedef Accessor<FA> Access;
        
        I& _i_;
        
        Holder (IHandle const& iha)
          : _i_(iha.get())
          {  }
        
      public:
        static Proxy& open(IHandle const& iha)
          {
            static char buff[sizeof(Proxy)];
            Proxy* p = new(buff) Proxy(iha);
            Access::implProxy_ = p;
            return *p;
          }
        
        static void close()
          {
            if (!Access::implProxy_) return;
            Proxy* p = static_cast<Proxy*> (Access::implProxy_);
            Access::implProxy_ = 0;
            p->~Proxy();
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
    
    
  
    typedef InstanceHandle< LUMIERA_INTERFACE_INAME(lumieraorg_GuiNotification, 1)
                          , gui::GuiNotification
                          > Handle_GuiNotification;
    
    
    template<>
    class Proxy<Handle_GuiNotification>
      : public Holder<Handle_GuiNotification>
      {
        //----Proxy-Implementation-of-GuiNotification--------
        
        void displayInfo (string const& text)           { _i_.displayInfo (cStr(text)); }
        void triggerGuiShutdown (string const& cause)   { _i_.triggerGuiShutdown (cStr(cause)); }
        
        
      public:
        Proxy (IHandle const& iha) : THolder(iha) {} 
      };
    
    
    
    
    template  void openProxy<Handle_GuiNotification>  (Handle_GuiNotification const&);
    template  void closeProxy<Handle_GuiNotification> (void);
    
    
  } // namespace facade
  
} // namespace lumiera
