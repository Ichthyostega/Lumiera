/*
  ModelPortRegistry  -  creating and organising and accessing model ports

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file model-port-registry.cpp
 ** Implementation details of model port descriptors and references.
 ** Essentially the handling of the ModelPortRegistry datastructure is
 ** kept an opaque implementation detail and confined entirely within
 ** this translation unit. Both the client interface (ModelPort) and
 ** the management interface (ModelPortRegistry) are backed by this
 ** common translation unit.
 ** 
 ** @see OutputDesignation
 ** @see OutputMapping
 ** @see Timeline
 **
 */


#include "lib/error.hpp"
#include "include/logging.h"
#include "lib/sync-classlock.hpp"
#include "steam/mobject/builderfacade.hpp"
#include "steam/mobject/model-port.hpp"
#include "steam/fixture/model-port-registry.hpp"

namespace steam {
namespace fixture {
    
    namespace error = lumiera::error;
    
    using mobject::LUMIERA_ERROR_BUILDER_LIFECYCLE;
    using mobject::LUMIERA_ERROR_INVALID_MODEL_PORT;
    using mobject::LUMIERA_ERROR_UNCONNECTED_MODEL_PORT;
    
    
    using MPDescriptor = ModelPortRegistry::ModelPortDescriptor const&;
    using LockRegistry = lib::ClassLock<ModelPortRegistry>;
    
    
    /** storage for the link to the global
        Registry instance currently in charge  */
    lib::OptionalRef<ModelPortRegistry> ModelPortRegistry::theGlobalRegistry;
    
    
    
    /** globally deactivate access to model ports */
    void
    ModelPortRegistry::shutdown ()
    {
      INFO (builder, "disabling ModelPort registry....");
      LockRegistry global_lock;
      theGlobalRegistry.clear(); 
    }
    
    
    /** switch the implicit link to \em the global ModelPort registry
     *  to point to the given implementation instance. Typically used
     *  within the Builder subsystem lifecycle methods, or for
     *  temporarily exchanging the registry for unit tests
     * @return the registry instance previously in use or \c NULL
     */
    ModelPortRegistry*
    ModelPortRegistry::setActiveInstance (ModelPortRegistry& newRegistry)
    {
      LockRegistry global_lock;
      ModelPortRegistry *previous = theGlobalRegistry.isValid()?
                                      &( theGlobalRegistry()) : 0;
      INFO_IF (!previous, builder, "activating new ModelPort registry.");
      WARN_IF ( previous, builder, "switching ModelPort registry instance.");
      theGlobalRegistry.link_to (newRegistry);
      return previous;
    }
    
    
    /** access the globally valid registry instance.
     *  @throw error::State if this global registry is
     *         already closed or not yet initialised. */
    ModelPortRegistry&
    ModelPortRegistry::globalInstance()
    {
      LockRegistry global_lock;
      if (theGlobalRegistry.isValid())
        return theGlobalRegistry();
      
      throw error::State ("global model port registry is not accessible"
                         , LUMIERA_ERROR_BUILDER_LIFECYCLE); 
    }
    
    
    
    /** does the <i>transaction currently being built</i>
     *  already contain a model port registration for the given ID?
     * @note this does \em not query registration state of the
     *       global registry; use #isRegistered for that...*/
    bool
    ModelPortRegistry::contains (ID<Pipe> key)  const
    {
      return bool(key)
          && util::contains (transaction_, key); 
    }
    
    
    /** @return true if the given pipe-ID actually denotes an
     *          existing, connected and usable model port.
     *  @note reflects the state of the publicly visible
     *          model port registry, \em not any model ports
     *          being registered within a pending transaction
     *          (ongoing build process). */
    bool
    ModelPortRegistry::isRegistered (ID<Pipe> key)  const
    {
      return bool(key)
          && util::contains (currentReg_, key); 
    }
    
    
    /** basic access operation: access the descriptor
     *  of a currently valid model port.
     * @note no locking (but #accessDescriptor does lock!) 
     * @throw error::Logic if accessing a non registered port
     * @throw error::State if accessing an invalid / disconnected port
     */
    MPDescriptor
    ModelPortRegistry::get (ID<Pipe> key)  const
    {
      if (!key)
        throw error::State ("This model port is disconnected or NIL"
                           , LUMIERA_ERROR_UNCONNECTED_MODEL_PORT);
      if (!isRegistered (key))
        throw error::Logic ("Model port was never registered, or got unregistered meanwhile."
                           ,LUMIERA_ERROR_INVALID_MODEL_PORT);
      
      MPTable::const_iterator pos = currentReg_.find (key);
      ASSERT (pos != currentReg_.end());
      ASSERT (pos->second.isValid());
      return pos->second;
    }
    
    
    /** access \em the globally valid model port for the given pipe.
     *  This (static) function locks and accesses the global model port registry
     *  to fetch the descriptor record. Typically invoked by client code
     *  through the ModelPort frontend
     * @throw error::State when registry is down or the model port is disconnected
     * @throw error::Logic when the given key wasn't registered for a model port */
    MPDescriptor
    ModelPortRegistry::accessDescriptor (ID<Pipe> key)
    {
      LockRegistry global_lock;
      return theGlobalRegistry().get(key);
    }
    
    
    /* === Mutations === */
    
    /** create and register a new model port entry,
     *  within the pending transaction */
    MPDescriptor
    ModelPortRegistry::definePort (ID<Pipe> pipe, ID<Struct> element_exposing_this_port)
    {
      LockRegistry global_lock;
      if (contains (pipe))
        throw error::Logic ("attempt to register a model port with a pipe-ID, "
                            "which has already been used to register a "
                            "model port within this transaction (build process)."
                           , LUMIERA_ERROR_DUPLICATE_MODEL_PORT);
      return (transaction_[pipe] = ModelPortDescriptor(pipe, element_exposing_this_port));
    }
    
    
    /** remove a model port entry from the pending transaction */
    void
    ModelPortRegistry::remove (PID key)
    {
      LockRegistry global_lock;
      transaction_.erase (key);
    }
    
    
    /** schedule removal of all registry contents.
     *  When the currently pending transaction is committed,
     *  all registered model ports will be removed */
    void
    ModelPortRegistry::clear()
    {
      LockRegistry global_lock;
      transaction_.clear();
    }
    
    
    /** transactional switch for new/modified model ports.
     *  Promote the registered model ports from the currently
     *  pending transaction to become the globally valid model ports
     * @note automatically starts a new transaction, initialised
     *       with the now published mappings.
     */
    void
    ModelPortRegistry::commit()
    {
      LockRegistry global_lock;
      MPTable newTransaction(transaction_);
      TRACE (builder, "committing new ModelPort list....");
      swap (currentReg_, transaction_);
      swap (transaction_, newTransaction);
    }
    
    
    /** discard current transaction.
     *  The global port registration thus
     *  remains unaltered. */
    void
    ModelPortRegistry::rollback()
    {
      LockRegistry global_lock;
      TRACE (builder, "discarding changes to ModelPort list (rollback)....");
      MPTable newTransaction(currentReg_);
      swap (transaction_, newTransaction);
    }
    
    
    
    LUMIERA_ERROR_DEFINE (DUPLICATE_MODEL_PORT, "Attempt to define a new model port with an pipe-ID already denoting an existing port");
    
}// namespace fixture


namespace mobject {
  
  LUMIERA_ERROR_DEFINE (INVALID_MODEL_PORT, "Referral to unknown model port");
  LUMIERA_ERROR_DEFINE (UNCONNECTED_MODEL_PORT, "Attempt to operate on an existing but unconnected model port");
  
  
  ModelPort::ModelPort (ID<asset::Pipe> refID)
    : id_(refID)
    {
      fixture::ModelPortRegistry::accessDescriptor (refID);
    }
  
  
  /** check if the global model port registration
   *  contains a mapping for the given pipe-ID*/
  bool
  ModelPort::exists (ID<asset::Pipe> key)
  {
    return fixture::ModelPortRegistry::globalInstance().isRegistered (key);
  }
  
  
  /** access the Pipe (ID) of the global model port registered
   *  with the ID underlying this model port.
   * @throw error::Logic if no model port is registered for this Pipe-ID
   */
  ID<asset::Pipe>
  ModelPort::pipe()  const
  {
    ENSURE (this->id_ == fixture::ModelPortRegistry::accessDescriptor(this->id_).id());
    
    return fixture::ModelPortRegistry::accessDescriptor(this->id_).id();
  }
  
  
  /** access the timeline (or similar structural element) holding
   *  a global pipe which corresponds to this model port
   * @throw error::Logic if no model port is registered for this Pipe-ID
   */
  ID<asset::Struct>
  ModelPort::holder()  const
  {
    return fixture::ModelPortRegistry::accessDescriptor(this->id_).holder();
  }
  
  
  /** convenience shortcut to access the stream type
   *  associated with the pipe-ID corresponding to this model port.
   * @note no check if this model port actually is valid
   * @throw error::Invalid in case of unknown/unregistered Pipe-ID
   */
  StreamType::ID
  ModelPort::streamType() const
  {
    return this->id_.streamType();
  }
  
  
  
}} // namespace steam::mobject
