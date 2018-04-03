/*
  INSTANCEHANDLE.hpp  -  automatically handling interface lifecycle

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

*/

/** @file instancehandle.hpp
 ** A handle template for automatically dealing with interface and plugin
 ** registration and deregistration. By placing an instance of this template,
 ** parametrised with the appropriate interface type, the respective interface
 ** and instance is loaded and opened through the Lumiera Interface system.
 ** It will be closed and unregistered automatically when the handle goes
 ** out of scope. Additionally, access via an (existing) interface proxy
 ** may be enabled and disabled alongside with the loading and unloading.
 **
 ** @see gui::GuiFacade usage example
 ** @see interface.h
 ** @see interfaceproxy.hpp (more explanations)
 ** @see session-command-interface-proxy.cpp (Proxy implementation example)
 **
 */


#ifndef LUMIERA_INSTANCEHANDLE_H
#define LUMIERA_INSTANCEHANDLE_H


#include "include/logging.h"
#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/depend-inject.hpp"

extern "C" {
#include "common/interface.h"
#include "common/interfaceregistry.h"
}

#include <string>



namespace lumiera {
  
  using std::string;
  
  template<class I, class FA>
  class InstanceHandle;
  
  
  namespace { // implementation details
    
    void
    throwIfError()
    {
      if (lumiera_error_peek())
        throw lumiera::error::Config("failed to open interface or plugin.",lumiera_error());
    }
    
    /** takes a (single) instance definitions, as typically created
     *  when defining interfaces for external use, and registers it
     *  with the InterfaceSystem. Then uses the data found in the
     *  \em given instance descriptor to open an instance handle.
     *  @throws error::Config when the registration process fails
     */
    LumieraInterface
    register_and_open (LumieraInterface descriptor)
    {
      if (!descriptor) return NULL;
      lumiera_interfaceregistry_register_interface (descriptor, NULL);
      throwIfError();
      return lumiera_interface_open (descriptor->interface,
                                     descriptor->version,
                                     descriptor->size,
                                     descriptor->name);
    }
    
    /** do a lookup within the interfaceregistry
     *  using the name/version found within the interface
     *  handle, to ensure it is still valid and registered */
    bool
    verify_validity (LumieraInterface ifa)
    {
      REQUIRE (ifa);
      return (ifa == lumiera_interfaceregistry_interface_find (ifa->interface,
                                                               ifa->version,
                                                               ifa->name));
    }
    
  } // (End) impl details
  
  
  
  
  namespace facade {

    /**
     * to be specialised and implemented for each
     * individual interface and facade interface.
     * The actual proxy implements the facade interface
     * and reroutes each call to the corresponding function
     * on the CL-Interface for the Lumiera interface system.
     */
    template<class IHA>
    class Proxy;
    
    /** The ServiceHandle automatically creates and manages the Proxy instance */
    template<class I, class FA>
    using ServiceHandle = typename lib::DependInject<FA>::template ServiceInstance<Proxy<InstanceHandle<I,FA>>>;
    
    
    /**
     * @internal Helper/Adapter for establishing a link
     * between an InstanceHandle and a facade interface,
     * which is going to be implemented through the given
     * interface/plugin. This way, creating the InstanceHandle
     * automatically creates a lumiera::facade::Proxy, to route
     * any facade calls through the interface/plugin. Similarly,
     * when destroying the InstanceHandle, the proxy will be closed.
     */
    template<class I, class FA>
    struct Link
      : ServiceHandle<I,FA>
      {
        using IH = InstanceHandle<I,FA>;
        
        Link (IH const& iha);
       ~Link ();
        
        FA* operator->()  const;
      };
    
    
    /**
     * @internal when the InstanceHandle isn't associated with a
     * facade interface, then this specialisation switches
     * the facade::Link into "NOP" mode.
     */
    template<class I>
    struct Link<I,I>
      : util::NonCopyable
      {
        using IH = InstanceHandle<I,I>;
        IH& ih_;
        
        Link (IH& ih)
          : ih_{ih}
        { }
        
        I*
        operator->()  const
          {
            return & ih_.get();
          }
      };
    
  } // namespace facade (impl details)
  
  
  
  
  
  /**
   * Handle tracking the registration of an interface, deregistering it on deletion.
   * Depending on which flavour of the ctor is used, either (bulk) registration of interfaces
   * or plugin loading is triggered. The interface type is defined by type parameter.
   * Additionally, choosing a facade interface as second type parameter causes installation
   * of a proxy, which implements the facade by routing calls through the basic interface
   * represented by this handle. This proxy will be "closed" automatically when this
   * InstanceHandle goes out of scope. Of course, the proxy needs to be implemented
   * somewhere, typically in interfaceproxy.cpp
   */
  template< class I         ///< fully mangled name of the interface type
          , class FA = I    ///< facade interface type to be used by clients
          >
  class InstanceHandle
    : util::NonCopyable
    {
      LumieraInterface desc_;
      I*                instance_;
      facade::Link<I,FA> facadeLink_;
      
    public:
      /** Set up an InstanceHandle representing a plugin.
       *  Should be placed at the client side.
       *  @param iName unmangled name of the interface
       *  @param version major version
       *  @param minminor minimum acceptable minor version number
       *  @param impName unmangled name of the instance (implementation)
       */
      InstanceHandle (string const& iName, uint version, size_t minminor, string const& impName)
        : desc_(0)
        , instance_(reinterpret_cast<I*>
              (lumiera_interface_open (iName.c_str(), version, minminor, impName.c_str())))
        , facadeLink_(*this)
        {
          throwIfError();
        }
      
      /** Set up an InstanceHandle managing the
       *  registration and deregistration of interface(s).
       *  Should be placed at the service providing side.
       *  @param a (single) interface descriptor, which can be created with
       *         LUMIERA_INTERFACE_INSTANCE and referred to by LUMIERA_INTERFACE_REF
       */
      InstanceHandle (LumieraInterface descriptor)
        : desc_(descriptor)
        , instance_(reinterpret_cast<I*> (register_and_open (desc_)))
        , facadeLink_(*this)
        {
          throwIfError();
        }
      
      ~InstanceHandle ()
        {
          lumiera_interface_close (&instance_->interface_header_);
          if (desc_)
            lumiera_interfaceregistry_remove_interface (desc_);
        }
      
      
      
      /** act as smart pointer to allow access through the facade.
       *  @note we don't provide `operator*`
       */
      FA*
      operator-> ()  const
        {
          return facadeLink_.operator ->();
        }
      
      /** directly access the instance via the CL interface */
      I&
      get ()  const
        {
          ENSURE(instance_);
          return *instance_;
        }
      
      
      explicit
      operator bool()  const
        {
          return isValid();
        }
      bool
      operator!() const
        {
          return not isValid();
        }
      
      
    private:
      bool
      isValid()  const
        {
          return instance_
              && verify_validity (&instance_->interface_header_);
        }
    };
  
  
} // namespace lumiera

#endif
