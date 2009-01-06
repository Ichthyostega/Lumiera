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
#include "lib/singletonsubclass.hpp" ////////////////////TODO
#include "include/guinotificationfacade.h"
#include "lib/util.hpp"
#include "lib/error.hpp"

extern "C" {
#include "common/interface.h"
}

using util::cStr;


namespace singleton = lumiera::singleton;

namespace gui {

  class GuiNotificationInterfaceProxy
    : public GuiNotification
    {
      LUMIERA_INTERFACE_INAME(lumieraorg_GuiNotification, 1) * interface_;
      
      GuiNotificationInterfaceProxy ()
        {
          interface_ = LUMIERA_INTERFACE_OPEN (lumieraorg_GuiNotification, 1, 2, lumieraorg_GuiNotificationFacade);
          if (!interface_)
            throw lumiera::error::State ("unable to access GuiNotificationFacade");
        }
      
      friend class singleton::StaticCreate<GuiNotificationInterfaceProxy>;
      
      
      
      /* ======== forwarding through interface ========== */
      
      void
      displayInfo (string const& text)
        {
          interface_->displayInfo (cStr(text));
        }
      
      void
      triggerGuiShutdown (string const& cause)
        {
          interface_->triggerGuiShutdown (cStr(cause));
        }
      
      
    };
  
  namespace {
  
    singleton::UseSubclass<GuiNotificationInterfaceProxy> typeinfo_proxyInstance_to_create;
  }
  
  /** storage for the facade proxy factory used by client code to invoke through the interface */
  lumiera::SingletonSub<GuiNotification> GuiNotification::facade (typeinfo_proxyInstance_to_create);
  
  ///////////////////////////////////////TODO: this solution is not correct, because it doesn't detect when the interface is shut down!
  


} // namespace gui

#include "common/instancehandle.hpp"

namespace lumiera {
  namespace facade {
  
  
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
        
        static Proxy& open(IHandle& iha)
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
        
        
        I& _i_;
        
        Holder (IHandle& iha)
          : _i_(iha.get())
          {  }
        
      };
    
    
    template<class FA>
    FA* Accessor<FA>::implProxy_;
    
    
    struct XYZ
      {
        virtual ~XYZ(){}
        
        virtual int zoing(int i)  =0;
      };
    
    struct II {};
  
    typedef InstanceHandle<II,XYZ> IIXYZ;
    
    
    template<>
    class Proxy<IIXYZ>
      : public Holder<IIXYZ>
      {
        //----Proxy-Implementation-of-XYZ--------
        
        virtual int
        zoing (int i)
          {
            return (rand() % i);
          }
        
        
      public:
        Proxy (IHandle iha) : THolder(iha) {} 
      };
    
    
    template<class IHA>
    void
    openProxy (IHA& iha)
      {
        Proxy<IHA>::open(iha);
      }
    
    template<class IHA>
    void
    closeProxy (IHA& iha)
      {
        Proxy<IHA>::close();
      }
    
    
  } // namespace facade
  
} // namespace lumiera
