/*
  TREE-MUTATOR-ATTRIBUTE-BINDING.hpp  -  diff::TreeMutator implementation building block

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file tree-mutator-attribute-binding.hpp
 ** Special binding implementation for TreeMutator, allowing to map
 ** tree diff operations onto native object attributes. TreeMutator is a
 ** customisable intermediary, which enables otherwise opaque implementation
 ** data structures to receive and respond to generic structural change messages
 ** ("tree diff").
 ** 
 ** Each concrete TreeMutator instance will be configured differently, and this
 ** adaptation is done by implementing binding templates, in the way of building
 ** blocks, attached and customised through lambdas. It is possible to layer
 ** several bindings on top of a single TreeMutator -- and this header defines
 ** a building block for one such layer, especially for binding to object fields
 ** through getter / setter lambdas.
 **  
 ** @note the header tree-mutator-attribute-binding.hpp with specific builder templates
 **       is included way down, after the class definitions. This is done so for sake
 **       of readability.
 ** 
 ** @see tree-mutator-test.cpp
 ** @see TreeMutator::build()
 ** 
 */


#ifndef LIB_DIFF_TREE_MUTATOR_ATTRIBUTE_BINDING_H
#define LIB_DIFF_TREE_MUTATOR_ATTRIBUTE_BINDING_H
#ifndef LIB_DIFF_TREE_MUTATOR_H
  #error "this header shall not be used standalone (see tree-mutator.hpp)"
#endif


  
//== anonymous namespace...
    
    
    template<class PAR, class CLO>
    struct ChangeOperation
      : PAR
      {
        ID attribID_;
        CLO change_;
        
        virtual void
        setAttribute (ID id, Attribute& newValue)
          {
            using Args = typename _ClosureType<CLO>::Args;
            using ValueType = typename lib::meta::Pick<Args, 0>::Type;
            
            if (id == attribID_)
              change_(newValue.get<ValueType>());
            
            else // delegate to other closures (Decorator-style)
              PAR::setAttribute(id, newValue);
          }
        
        ChangeOperation(ID id, CLO clo, PAR&& chain)
          : PAR(std::forward<PAR>(chain))
          , attribID_(id)
          , change_(clo)
          { }
        
        
        
        /* ==== re-Implementation of the operation API ==== */
        
        /** this binding to an object field can not support any reordering,
         *  inserting or deletion of "Elements", since the structure of an object
         *  is fixed through the underlying class definition. For this reason,
         *  we do not keep track of an "old" and "new" order; rather there
         *  is always one single fixed element present to work on.
         * @return `true` always
         */
        virtual bool
        hasSrc ()  override
          {
            return true;
          }
        
        /** ensure the next recorded source element
         *  matches on a formal level with given spec */
        virtual bool
        matchSrc (GenNode const& spec)  override
          {
            UNIMPLEMENTED("check applicability");
          }
        
        /** accept existing element, when matching the given spec */
        virtual bool
        acceptSrc (GenNode const& n)  override
          {
            UNIMPLEMENTED("check applicability, then NOP");
          }
        
        /** fabricate a new element, based on
         *  the given specification (GenNode),
         *  and insert it at current position
         *  into the target sequence.
         */
        virtual void
        injectNew (GenNode const& n)  override
          {
            UNIMPLEMENTED("accept, then assignElm");
          }
        
        /** skip next pending src element,
         *  causing this element to be discarded
         */
        virtual void
        skipSrc ()  override
          {
            UNIMPLEMENTED("prohibited /raise error");
          }
        
        /** locate designated element and accept it at current position */
        virtual bool
        findSrc (GenNode const& refSpec)  override
          {
            UNIMPLEMENTED("prohibited / raise error");
          }
        
        /** repeatedly accept, until after the designated location */
        virtual bool
        accept_until (GenNode const& spec)
          {
            UNIMPLEMENTED("only support UNTIL END");
          }
        
        /** locate element already accepted into the target sequence
         *  and assign the designated payload value to it. */
        virtual bool
        assignElm (GenNode const& spec)
          {
            UNIMPLEMENTED("invoke setter");
          }
        
        /** locate the designated target element and build a suitable
         *  sub-mutator for this element into the provided target buffer */
        virtual bool
        mutateChild (GenNode const& spec, TreeMutator::MutatorBuffer targetBuff)
          {
            UNIMPLEMENTED("invoke mutator builder");
          }
        
        /** verify all our pending (old) source elements where mentioned.
         * @note allows chained "onion-layers" to clean-up and verify.*/
        virtual bool
        completeScope()
          {
            UNIMPLEMENTED("NOP and succeed");
          }
      };
    
    
#endif /*LIB_DIFF_TREE_MUTATOR_ATTRIBUTE_BINDING_H*/
