/*
  ModelPortRegistry  -  creating and organising and accessing model ports
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file model-port-registry.cpp 
 ** Implementation details of model port descriptors and references.
 ** Especially, the handling of the ModelPortTable datastructure is
 ** kept an opaque implementation detail and confined entirely within
 ** this translation unit.
 ** 
 ** TODO: comment necessary?
 ** 
 ** @see OutputDesignation
 ** @see OutputMapping
 ** @see Timeline
 **
 */


#include "lib/error.hpp"
//#include "lib/symbol.hpp"//
#include "proc/mobject/model-port.hpp"
#include "proc/mobject/builder/model-port-registry.hpp"

//#include <boost/functional/hash.hpp>
//#include <cstdlib>
#include <boost/noncopyable.hpp>

//using lumiera::query::QueryHandler;
//using lumiera::query::removeTerm;
//using lumiera::query::extractID;
//using lumiera::ConfigRules;

//using lumiera::Symbol;

namespace mobject {
  namespace builder {
    
    
    /**
     * TODO type comment
     */
    class ModelPortTable
      : boost::noncopyable
      {
        
      public:
      };
    
      
    typedef ModelPortRegistry::ModelPortDescriptor const& MPDescriptor;
    
      inline MPDescriptor
      accessDescriptor() 
      {
        
      }
    
    ModelPortRegistry&
    ModelPortRegistry::setActiveInstance (ModelPortRegistry& newRegistry)
    {
      UNIMPLEMENTED ("handling of active model port registry"); 
    }
    
    
    /** */
    ModelPortRegistry&
    ModelPortRegistry::globalInstance()
    {
      UNIMPLEMENTED ("access the globally valid registry instance"); 
    }
   
    
    
    /** */
    bool
    ModelPortRegistry::contains (ID<Pipe> pID)  const
    {
      UNIMPLEMENTED ("diagnostics querying the state of the pending transaction"); 
    }
    
    
    /** @return true if the given pipe-ID actually denotes an
     *          existing, connected and usable model port. 
     *  @note reflects the state of the publicly visible 
     *          model port registry, \em not any model ports
     *          being registered within a currently pending
     *          transaction (ongoing build process). */
    bool
    ModelPortRegistry::isRegistered (ID<Pipe> key)  const
    {
      if (!key) return false;
      
      UNIMPLEMENTED ("query the publicly valid contents"); 
    }
    
    
    /** */
    MPDescriptor
    ModelPortRegistry::operator[] (ID<Pipe> key)  const
    {
      UNIMPLEMENTED ("access registered model port");
    }
    
    
    /** */
    MPDescriptor
    ModelPortRegistry::accessDescriptor (ID<Pipe> key)
    {
      UNIMPLEMENTED ("access the current global registry and fetch model port");
    }
    
    
    /** */
    MPDescriptor
    ModelPortRegistry::definePort (ID<Pipe> pipeA, ID<Struct> element_exposing_this_port)
    {
      UNIMPLEMENTED ("create and register a new model port entry, within the pending transaction"); 
    }
    
    
    /** */
    void
    ModelPortRegistry::remove (PID key)
    {
      UNIMPLEMENTED ("remove a model port entry from the pending transaction"); 
    }
    
    
    /** */
    void
    ModelPortRegistry::clear()
    {
      UNIMPLEMENTED ("schedule removal of all registry contents into the pending transaction"); 
    }
    
    
    /** */
    void
    ModelPortRegistry::commit()
    {
      UNIMPLEMENTED ("transactional switch for new/modified model ports"); 
    }
    
    
    /** */
    void
    ModelPortRegistry::rollback()
    {
      UNIMPLEMENTED ("discard current transaction"); 
    }
  
    
    
    LUMIERA_ERROR_DEFINE (DUPLICATE_MODEL_PORT, "Attempt to define a new model port with an pipe-ID already denoting an existing port");
  
  } // namespace builder  
  
  
  
  
  LUMIERA_ERROR_DEFINE (INVALID_MODEL_PORT, "Referral to unknown model port");
  LUMIERA_ERROR_DEFINE (UNCONNECTED_MODEL_PORT, "Attempt to operate on an existing but unconnected model port");

  
  ModelPort::ModelPort (ID<asset::Pipe> refID)
    : id_(refID)
    {
      builder::ModelPortRegistry::accessDescriptor (refID);
    }
  
  
  /** */
  bool
  ModelPort::exists (ID<asset::Pipe> key)
  {
    return builder::ModelPortRegistry::globalInstance().isRegistered (key);
  }
  
  
  /** */
  ID<asset::Pipe>
  ModelPort::pipe()  const
  {
    ENSURE (this->id_ == builder::ModelPortRegistry::accessDescriptor(this->id_).id);
    
    return builder::ModelPortRegistry::accessDescriptor(this->id_).id;
  }
  
  
  /** */
  ID<asset::Struct>
  ModelPort::holder()  const
  {
    return builder::ModelPortRegistry::accessDescriptor(this->id_).holder;
  }
  
  
  /** */
  StreamType::ID
  ModelPort::streamType() const
  {
    return this->id_.streamType();
  }


  
  
} // namespace mobject
