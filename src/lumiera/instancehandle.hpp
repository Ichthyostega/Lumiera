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


extern "C" {
#include "lumiera/interface.h"
}

#include <boost/noncopyable.hpp>
//#include <boost/scoped_ptr.hpp>
#include <string>



namespace lumiera {
  
  using std::string;
//  using boost::scoped_ptr;
  
  namespace { // implementation details
  
    LumieraInterface
    register_and_open (LumieraInterface* descriptors)
    {
      if (!descriptors) return NULL;
      lumiera_interfaceregistry_bulkregister_interfaces (descriptors, NULL);
      LumieraInterface masterI = descriptors[0];
      return lumiera_interface_open (masterI->interface,
                                     masterI->version,
                                     masterI->size,
                                     masterI->name);
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
          >
  class InstanceHandle
    : private boost::noncopyable
    { 
      LumieraInterface* desc_;
      I* instance_;
      
    public:
      /** Set up an InstanceHandle representing a plugin.
       *  Should be placed at the client side. 
       *  @param iName unmangled name of the interface
       *  @param version major version
       *  @param minminor minimum acceptable minor version number
       *  @param impName unmangled name of the instance (implementation)
       */
      InstanceHandle (string const& iName, uint version, size_t minminor, string const& impName)
        : descriptors_(0),
          instance_(static_cast<I*> 
              (lumiera_interface_open (iName, version, minminor, impName)))
        { }
      
      /** Set up an InstanceHandle managing the 
       *  registration and deregistration of interface(s).
       *  Should be placed at the service providing side.
       *  @param descriptors zero terminated array of interface descriptors,
       *         usually available through lumiera_plugin_interfaces() 
       */
      InstanceHandle (LumieraInterface* descriptors)
        : desc_(descriptors),
          instance_(static_cast<I*> (register_and_open (desc_)))
        { }
      
      ~InstanceHandle ()
        {
          lumiera_interface_close ((LumieraInterface)instance_);
          if (desc_)
            lumiera_interfaceregistry_bulkremove_interfaces (desc_);
        }
      
      
    };
  
  
} // namespace lumiera

#endif
