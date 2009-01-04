/*
  INSTANCEHANDLE.hpp  -  automatically handling interface lifecycle 
 
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
 ** @see interfaceproxy.cpp
 */


#ifndef LUMIERA_INSTANCEHANDLE_H
#define LUMIERA_INSTANCEHANDLE_H


#include "include/nobugcfg.h"

extern "C" {
#include "common/interface.h"
#include "common/interfaceregistry.h"
}

#include <boost/noncopyable.hpp>
//#include <boost/scoped_ptr.hpp>
#include <string>



namespace lumiera {
  
  using std::string;
//  using boost::scoped_ptr;
  
  namespace { // implementation details
    
    void
    throwIfError() 
    {
      if (lumiera_error_peek())
        throw lumiera::error::Config("failed to open interface or plugin.",lumiera_error());
    }
  
    /** takes a bunch of instance definitions, as typically created
     *  when defining interfaces for external use, and registers them
     *  with the InterfaceSystem. Then uses the data found in the
     *  \em first descriptor to open an instance handle.  
     */
    LumieraInterface
    register_and_open (LumieraInterface* descriptors)
    {
      if (!descriptors) return NULL;
      lumiera_interfaceregistry_bulkregister_interfaces (descriptors, NULL);
      throwIfError();
      LumieraInterface masterI = descriptors[0];
      return lumiera_interface_open (masterI->interface,
                                     masterI->version,
                                     masterI->size,
                                     masterI->name);
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
  
  
  
  /**
   * Handle tracking the registration of an interface, deregistering it on deletion.
   * Depending on which flavour of the ctor is used, either (bulk) registration of interfaces
   * or plugin loading is triggered. The interface type is defined by type parameter.
   * @todo when provided with the type of an facade interface class, care for enabling/disabling
   *       access through the facade proxy singleton when opening/closing the registration.
   */
  template< class I         ///< fully mangled name of the interface type
          , class FA = I    ///< facade interface type to be used by clients
          >
  class InstanceHandle
    : private boost::noncopyable
    { 
      LumieraInterface* desc_;
      I* instance_;
      
      typedef InstanceHandle<I,FA> _ThisType;
      
    public:
      /** Set up an InstanceHandle representing a plugin.
       *  Should be placed at the client side. 
       *  @param iName unmangled name of the interface
       *  @param version major version
       *  @param minminor minimum acceptable minor version number
       *  @param impName unmangled name of the instance (implementation)
       */
      InstanceHandle (string const& iName, uint version, size_t minminor, string const& impName)
        : desc_(0),
          instance_(reinterpret_cast<I*> 
              (lumiera_interface_open (iName.c_str(), version, minminor, impName.c_str())))
        { 
          throwIfError();
        }
      
      /** Set up an InstanceHandle managing the 
       *  registration and deregistration of interface(s).
       *  Should be placed at the service providing side.
       *  @param descriptors zero terminated array of interface descriptors,
       *         usually available through lumiera_plugin_interfaces() 
       */
      InstanceHandle (LumieraInterface* descriptors)
        : desc_(descriptors),
          instance_(reinterpret_cast<I*> (register_and_open (desc_)))
        { 
          throwIfError();
        }
      
      ~InstanceHandle ()
        {
          lumiera_interface_close (&instance_->interface_header_);
          if (desc_)
            lumiera_interfaceregistry_bulkremove_interfaces (desc_);
        }
      
      
      
      /** act as smart pointer providing access through the facade. 
       *  @todo implement the case where the Facade differs from I
       *  @note we don't provide operator*                      */
      FA * operator-> ()  const { return accessFacade(); }      
      
      /** directly access the instance via the CL interface */
      I& get ()  const { ENSURE(instance_); return *instance_; }
      
      
      
      typedef I* _ThisType::*unspecified_bool_type;
      
      /** implicit conversion to "bool" */
      operator unspecified_bool_type()  const // never throws
        { return isValid()?  &_ThisType::instance_ : 0; }
      
      bool operator! ()  const { return !isValid();     }
      
      
    private:
      FA *
      accessFacade()  const
        {
          ENSURE (instance_);
          return static_cast<FA *> (instance_);    /////////////////TODO: actually handle the case when the facade differs from the interface by using the proxy
        }
      
      bool 
      isValid()  const
        { 
          return instance_ 
              && verify_validity (&instance_->interface_header_);
        }
    };
  
  
} // namespace lumiera

#endif
