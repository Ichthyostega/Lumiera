/*
  INTERFACEPROXY  -  definition of forwarding proxies for the facade interfaces

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file interfaceproxy.hpp
 ** Implementation of C++ binding proxies on top of the (plain-C based) interface system.
 ** This is an implementation facility within the application core, which allows to embody
 ** just an ["interface instance handle"](\ref instancehandle.hpp) into the implementation
 ** os some service, in order to get RAII-style registration of interfaces and loading of
 ** plug-ins.
 ** 
 ** A *crucial requirement* for this approach to work is that any relevant interface
 ** to be bound and exposed as C++ object needs to set up a concrete specialisation of
 ** lumiera::facade::Proxy to drive instantiation of the actual binding proxy.
 ** The result of this setup is that clients can just invoke `SomeInterface::facade()`
 ** and thus call through proper C++ bindings with type safety and automatic
 ** lifecycle management.
 ** 
 ** # Interface, Plug-in, Facade interface, Instance Handle and Proxy
 ** 
 ** These are all terms related to the Interface- and Plug-in system for Lumiera.
 ** Communication between the Layers within the architecture is usually routed through
 ** *Layer Separation Interfaces*. Here we have to distinguish two different flavours
 ** of an "interface"
 ** - A Façade interface is written in C++ and is what you's usually
 **   denote with the term "interface": it defines a contract in terms of
 **   some abstract entities, without exposing implementation details.
 **   Ideally, the interface holds all you need to use a given service.
 ** - A C Language interface defined with the help of the Interface/Plugin system.
 **   It is a collection of functions and supports only the primitive types of the
 **   bare C Language. Objects need to be emulated by pointers to a struct type,
 **   and functors must be represented as static function pointers. In many cases
 **   you need to fall back to untyped `void*` unfortunately.
 ** 
 ** @todo 2018 as it stands (since 2008), the Interface/Plug-in system fulfils the basic task
 **       it was created for, but is rather cumbersome to use in practice. We should investigate
 **       to use SWIG or something similar to generate the bindings and the low-level interfaces.
 ** 
 ** The Interface/Plug-in system offers two basic usage scenarios
 ** - a CL-Interface can be published (from the service provider side).
 **   From that point on, clients can "open" that interface and talk to it.
 ** - a _client_ can use the CL-Interface of a Plug-in to _load_ a plug-in instance.
 **   From that point on, clients can talk through a interface handle to the plug-in.
 ** 
 ** An Attempt was made to simplify and unify this process with the help of an InstanceHandle.
 ** This is an RAII-style handle object, which automates the registration and instance management.
 ** 
 ** But in order to be able to actually access some service via a high-level façade interface,
 ** we still need a way to get a callable instance of the façade interface. This is where the
 ** proxy implementation comes into play. The binding proxy implements the façade and maps each
 ** high-level call into an invocation of the corresponding low-level function on the CL-interface.
 ** 
 ** Whenever InstanceHandle was created with a second template parameter defining a façade interface,
 ** it automatically attempts to instantiate a lumiera::facade::Proxy templated to the actual type
 ** of the InstanceHandle. This proxy instance is then exposed via `lib::Depend<FacadeInterface>`
 ** This way, any call will be routed through the corresponding C Language function defined within
 ** the Interface/Plugin system. Moreover, there will be another subclass of the Facade interface
 ** sitting "on the other side" of the interface barrier to _actually implement_ the functionality.
 ** 
 ** As a convention, each façade interface should hold a static accessor member named "facade" of
 ** type `lib::Depend<FacadeInterface>`, so client code can write e.g. `XYZInterface::facade()`
 ** to yield a reference to a proxy object implementing `XYZInterface`.
 ** 
 ** 
 ** # Interface Lifecycle
 ** 
 ** Instances of an Interface are either directly provided by some facility within the core,
 ** or they are loaded from a shared module (plugin). In either case this means the interface
 ** isn't accessible all the time, rather it comes up at a defined point in the application
 ** lifecycle and similarly will be shut down deliberately at some point. Beyond this time
 ** window of availability, any access through the proxy factory throws an lumiera::error::Fatal.
 ** Any sort of dependency management is outside the scope of the InstanceHandle (for the core
 ** services, it is handled by the dependency of subsystems, while the plugin loader cares
 ** for dependency issues regarding loadable modules, thereby building on the deployment
 ** descriptors.)
 ** 
 ** For the Layer separation interfaces, the process of loading and opening is abstracted as
 ** an InstanceHandle object. A service exposing an interface defines an InstanceHandle
 ** member using the appropriate template and ctor parameters; this causes registration with
 ** the Interface/Plugin and instantiates the corresponding facade::Proxy, which is then
 ** exposed through the lib::Depend front-end. Similarly, when the service implementation
 ** object is destroyed, the InstanceHandle goes out of scope, thereby detaching from the
 ** Interface/Proxy and deregistering and destroying the proxy object. Any further
 ** access beyond that point will raise an exception.
 ** 
 ** # Usage
 ** 
 ** While client code just includes the interface header (including lib/depend.hpp),
 ** there needs to be an actual implementation of each proxy object located in some translation
 ** unit, linked into the application core or `liblumieracommon.so`. This translation unit
 ** needs to specialise lumiera::facade::Proxy and then create an instance of that template.
 ** And, most importantly, such translation units (and _only such translation units_) must
 ** include this header `interfaceproxy.hpp` -- because it defines the concrete ctor
 ** and dtor of the facade::Link template and thus creates the missing "link" between
 ** the InstanceHandle and the actual proxy instantiation.
 ** 
 ** @see instancehandle.hpp
 ** @see gui-notification-facade.h usage example (facade interface)
 ** @see notification-service.hpp
 ** @see notification-service.cpp (service implementation)
 ** @see notification-interface-proxy.cpp
 */


#ifndef LUMIERA_INTERFACE_PROXY_H
#define LUMIERA_INTERFACE_PROXY_H


#include "common/instancehandle.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"


namespace lumiera {
namespace facade {

  /**
   * Implementation Base
   * for building Facade Proxy implementations.
   * Typically the purpose of such a proxy is to route
   * any calls through the C-Bindings of the Lumiera Interface system.
   * The actual instance and thus the storage for the concrete proxy object
   * is controlled via lib::DependInject::ServiceInstance, which in turn is
   * embedded into and thus linked to the lifetime of a InstanceHandle to
   * connect via Lumiera's Interface/Plug-in system. Typically the actual
   * Service implementation object will hold an instance of that InstanceHandle
   * and thus tie the opening/closing of the interface and access mechanism
   * to the service lifecycle.
   */
  template<class IHA>
  struct Binding;
  
  template<class FA, class I>
  struct Binding<InstanceHandle<I,FA>>
    : public FA
    {
      using IHandle = InstanceHandle<I,FA>;
      
      I& _i_;
      
      Binding (IHandle const& iha)
        : _i_(iha.get())
        {  }
    };
  
  
  
  template<class I, class FA>
  Link<I,FA>::Link (IH const& iha)
    : ServiceHandle<I,FA>{iha}
    { }
  
  template<class I, class FA>
  Link<I,FA>::~Link() { }
  
  
  template<class I, class FA>
  FA*
  Link<I,FA>::operator->()  const
  {
    return ServiceHandle<I,FA>::operator->();
  }

  
  
}} // namespace lumiera::facade

#endif
