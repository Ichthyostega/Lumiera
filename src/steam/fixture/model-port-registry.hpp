/*
    MODEL-PORT-REGISTRY.hpp  -  creating and organising and accessing model ports

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file model-port-registry.hpp
 ** Mutation and management facility for model ports within the builder.
 ** Model ports denote the points where output might possibly be produced.
 ** While client code accesses model ports only as immutable descriptors handled
 ** through an (opaque) reference, the builder is in charge of detecting and organising
 ** any (new) model ports arising as the result of the build process. Changes to the set
 ** of current model ports are to be activated with an atomic <i>transactional switch.</i>
 ** 
 ** builder::ModelPortRegistry thus acts as management interface and factory for model ports.
 ** A given instance of this registry can be promoted to be "the" model port registry reflecting
 ** the current active model ports. Within the Lumiera application, the builder subsystem cares
 ** for setting up such a registry, while all other parts of the system just access the current
 ** model ports through the mobject::ModelPort frontend.
 ** 
 ** @note the locking is rather coarse grained; basically we're using just one
 **       single global lock for all ModelPortRegistry instances and all access/mutations,
 **       as well as for accessing the globally valid Registry through the ModelPort frontend.
 **       Assumed that usually there is just one Registry maintained by the builder, this is
 **       likely to be sufficient.
 ** 
 ** @see ModelPort
 ** @see OutputDesignation
 ** @see ModelPortRegistry_test
 */


#ifndef STEAM_FIXTURE_MODEL_PORT_REGISTRY_H
#define STEAM_FIXTURE_MODEL_PORT_REGISTRY_H

#include "lib/error.hpp"
#include "lib/optional-ref.hpp"
#include "steam/asset/pipe.hpp"
#include "steam/asset/struct.hpp"
#include "steam/mobject/model-port.hpp"

#include <map>

namespace steam {
namespace fixture {
  
  using asset::ID;
  using asset::Pipe;
  using asset::Struct;
  
  LUMIERA_ERROR_DECLARE (DUPLICATE_MODEL_PORT); ///< Attempt to define a new model port with an pipe-ID already denoting an existing port
  
  
  /**
   * Management facility for tracking model ports.
   * ModelPort handles are exposed as frontend for usage
   * by client code. Model ports are discovered by the builder
   * when re-creating the low-level model; during such an ongoing
   * build process, newly discovered ports are accumulated within
   * a transaction, which then gets committed atomically when the
   * new model is complete and ready for use.
   */
  class ModelPortRegistry
    : util::NonCopyable
    {
      
      typedef ID<Pipe>   PID;
      typedef ID<Struct> StID;
      
    public:
      
      /** @internal record to describe a model port */
      class ModelPortDescriptor;
      
      
      static void shutdown ();
      
      static ModelPortRegistry*
      setActiveInstance (ModelPortRegistry& newRegistry);
      
      static ModelPortRegistry&
      globalInstance();
      
      static ModelPortDescriptor const&
      accessDescriptor (PID); 
      
      
      ModelPortDescriptor const&
      definePort (PID pipe, StID element_exposing_this_port);
      
      bool contains (PID)     const;
      bool isRegistered (PID) const;
      
      ModelPortDescriptor const&
      get (PID)  const;
      
      
      void remove (PID);
      void clear();
      
      
      /** activate pending model port changes.
       *  Any accumulated changes and newly defined model ports
       *  are promoted to become part of the current active configuration
       *  with a single atomic (transactional) switch.
       */
      void commit();
      
      /** discard pending changes.
       *  Silently drop model port definition changes since the last commit.
       */
      void rollback();
      
      
    private:
      static lib::OptionalRef<ModelPortRegistry> theGlobalRegistry;
      
      typedef std::map<PID, ModelPortDescriptor> MPTable;
      
      MPTable currentReg_;
      MPTable transaction_;
    };
  
  
  
  
  /** ModelPortDescriptor records are used as actual storage
   *  within the model port registration table; they are immutable
   *  value objects and never exposed to client code directly.
   */
  class ModelPortRegistry::ModelPortDescriptor
    {
      PID  id_;
      StID holder_;
      
    protected:
      ModelPortDescriptor (PID pipe, StID element_exposing_this_port)
        : id_(pipe)
        , holder_(element_exposing_this_port)
        { }
      
      friend class ModelPortRegistry;
      
    public:
      ModelPortDescriptor()
        : id_(PID::INVALID)
        , holder_(StID::INVALID)
        { }
      
      // default copy operations permitted
      
      bool
      isValid()  const
        {
          return bool(id_);
        }
      
      const PID  id()      const { return id_; }
      const StID holder()  const { return holder_; }
    };
  
  
  
}} // namespace steam::fixture
#endif /*STEAM_FIXTURE_MODEL_PORT_REGISTRY_H*/
