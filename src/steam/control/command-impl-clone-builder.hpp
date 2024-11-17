/*
  COMMAND-IMPL-CLONE-BUILDER.hpp  -  Cloning command implementation without disclosing concrete type

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file command-impl-clone-builder.hpp
 ** Helper for creating an implementation clone, based on the visitor pattern.
 ** This file deals with the problem of creating a clone from top level without
 ** any specific type information. While generally this means passing down the
 ** allocation interface, the specific problem here is that multiple parts of the
 ** command implementation need to be cloned and re-wired with the cloned partners,
 ** which requires re-creating the specifically typed context used at initial setup.
 ** 
 ** @todo Ticket #301 : it may well be that the need for such a facility is a symptom
 ** of misaligned design, but I rather doubt so -- because both the memento holder and
 ** the command closure need a specifically typed context, and there is no reason
 ** for combining them into a single facility.
 ** 
 ** @see CommandRegistry#createCloneImpl
 ** @see CommandImpl
 ** @see StorageHolder#createClone
 ** @see command-clone-builder-test.cpp
 **
 */



#ifndef CONTROL_COMMAND_IMPL_CLONE_BUILDER_H
#define CONTROL_COMMAND_IMPL_CLONE_BUILDER_H

#include "steam/control/command-mutation.hpp"
#include "lib/typed-allocation-manager.hpp"
#include "lib/opaque-holder.hpp"
#include "lib/nocopy.hpp"


namespace steam {
namespace control {
  
  using lib::TypedAllocationManager;
  using lib::InPlaceBuffer;
  
  
  namespace impl { // Helper: type erased context for creating command clones
    
    struct CloneContext  ///< Interface and empty state
      {
        virtual ~CloneContext() {}
        
        virtual UndoMutation const& getUndoFunc() { NOTREACHED(); }
        virtual PClo         const& getClosure()  { NOTREACHED(); }
        virtual bool                isValid()     { return false; }
      };
    
    class ClonedContext
      : public CloneContext
      {
        PClo newClosure_;
        UndoMutation newUndoFunctor_;
        
        virtual UndoMutation const& getUndoFunc() { return newUndoFunctor_; }
        virtual PClo         const& getClosure()  { return newClosure_; }
        virtual bool                isValid()     { return true; }
        
        
        /** helper for accessing the new cloned closure
         *  through the specific concrete type. As we need
         *  to access the closure later, after discarding the
         *  exact type, we can store only a baseclass pointer.
         */
        template<typename ARG>
        ARG&
        downcast()
          {
            REQUIRE (INSTANCEOF (ARG, newClosure_.get()));
            
            return static_cast<ARG&> (*newClosure_);
          }
        
        
      public:
        
        template<typename ARG>
        ClonedContext ( ARG const& origArgHolder
                      , TypedAllocationManager& allocator
                      )
          : newClosure_(allocator.create<ARG> (origArgHolder))
          , newUndoFunctor_(downcast<ARG>().getMementoWiring())
          { }
      };
    
  }//(End) impl namespace
  
  
  
  
  
  /**
   * Visitor to support creating a CommandImpl clone.
   * Created and managed by CommandRegistry, on clone creation
   * an instance of this builder object is passed down to re-gain
   * a fully typed context, necessary for re-wiring the undo functors
   * and the memento storage within the cloned parts.
   */
  class CommandImplCloneBuilder
    : util::NonCopyable
    {
      typedef InPlaceBuffer<impl::CloneContext, sizeof(impl::ClonedContext)> ContextHolder;
      
      TypedAllocationManager& allocator_;
      ContextHolder newContext_;
      
    public:
      CommandImplCloneBuilder (TypedAllocationManager& allo)
        : allocator_(allo)
        { }
      
      
      /** to be executed from within the specifically typed context
       *  of a concrete command StorageHolder; allocate a clone copy
       *  and then prepare a new UNDO-Functor, which is correctly wired
       *  with the memento holder within this new \em clone closure.
       *  After that point, these prepared parts can be retrieved
       *  through the public accessor functions; they will be 
       *  used by the command registry to put together a complete
       *  clone copy of the original CommandImpl.
       */
      template<typename ARG>
      void
      buildCloneContext (ARG const& origArgHolder)
        {
          REQUIRE (!newContext_->isValid(), "Lifecycle-Error");
          
          newContext_.create<impl::ClonedContext> (origArgHolder, allocator_);
        }
      
      
      
      /** after visitation: use pre-built bits to provide a cloned UndoFunctor */
      UndoMutation const&
      clonedUndoMutation ()
        {
          REQUIRE (newContext_->isValid());
          return newContext_->getUndoFunc();
        }
      
      
      /** after visitation: provide cloned StorageHolder,
       *  but already stripped down to the generic usage type */
      PClo const&
      clonedClosuere ()
        {
          REQUIRE (newContext_->isValid());
          return newContext_->getClosure();
        }
      
    };
  
  
  
  
  
}} // namespace steam::control
#endif
