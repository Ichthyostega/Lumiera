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
    class ChangeOperation
      : public PAR
      {
        using CloArgs = typename _ClosureType<CLO>::Args;
        using ValueType = typename lib::meta::Pick<CloArgs, 0>::Type;
        
        ID attribID_;      //////////////////////TODO  ....consider to build and store a BareEntryID, to include nominal target type into the match
        CLO change_;
        
        
        
        /** hard wired "selector predicate" for this binding layer.
         *  We do only handle mutation operations pertaining attributes
         *  which additionally match the key defined at binding time.
         *  Any other operations are passed down the chain.
         * @param spec target specification given within the diff verb
         * @return if this binding is in charge for handling the spec
         */
        bool
        isApplicable (GenNode const& spec)
          {
            return spec.isNamed()
               and string(attribID_) == spec.idi.getSym();  /////TODO  ....consider to build and store a BareEntryID
          }
        
      public:
        virtual void
        setAttribute (ID id, Attribute& newValue)    ///< @deprecated about to be dropped in favour of a full binding layer
          {
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
        
        /** ensure the given spec is deemed appropriate at that point.
         *  Due to the hard wired nature of an object field binding, this can
         *  only be verified passively: a spec targeted at an unknown attribute
         *  will be rejected. But since there is no notion of "ordering" for
         *  (object) data fields, we can not verify the diff's completeness. */
        virtual bool
        matchSrc (GenNode const& spec)  override
          {
            return isApplicable (spec)
                or PAR::matchSrc (spec);
          }
        
        /** accept status quo, after verifying the spec from the diff verb */
        virtual bool
        acceptSrc (GenNode const& spec)  override
          {
            return isApplicable (spec);
          }
        
        /** while, strictly speaking, one can not "insert" fields into a
         *  given class definition, this binding can tolerate an `INS` verb
         *  whenever this means to touch a field which is actually known and
         *  present in the class definition underlying this binding. In such
         *  a case, we just assign the given value. This implementation leeway
         *  is deliberate to support classes with optional / defaultable properties.
         * @throw error::Invalid on attempt to insert an unknown attribute / field.
         */
        virtual void
        injectNew (GenNode const& spec)  override
          {
            if (not isApplicable(spec))
              PAR::injectNew(spec);   ////////////////////////////////////////TODO change interface, so we can find out if anyone was able to handle this request. This allows us then to throw otherwise
            else
              {
                change_(spec.data.get<ValueType>());
                // return true;
              }
          }
        
        /** has no meaning, since object fields have no notion of "order".
         * @note it would be desirable to throw, but due to other restrictions
         *  in the "diff protocol" this operation does not get any arguments.
         *  Thus we're not able to determine, if this layer is in charge, so
         *  we need just to pass on the invocation.
         */
        virtual void
        skipSrc ()  override
          {
            PAR::skipSrc();
          }
        
        /** any reordering or deletion of object fields is prohibited
         * @throw error::Logic when this binding layer becomes responsible for
         *  handling the given diff spec, because a proper diff must be arranged
         *  in a way not to ask this binding to "reorder" a field from an
         *  existing class definition.
         */
        virtual bool
        findSrc (GenNode const& refSpec)  override
          {
            if (isApplicable(refSpec))
              throw error::Logic ("attempt to re-order attributes bound to object fields");  //////TODO can we include _Fmt and give a precise message including the key?
            else
              return PAR::findSrc (refSpec);
          }
        
        /** repeatedly accept, until after the designated location */
        virtual bool
        accept_until (GenNode const& spec)
          {
            UNIMPLEMENTED("only support UNTIL END");
          }
        
        /** invoke the setter lambda, in case this binding layer is in charge */
        virtual bool
        assignElm (GenNode const& spec)
          {
            if (not isApplicable(spec))
              return PAR::assignElm(spec);
            else
              {
                change_(spec.data.get<ValueType>());
                return true;
              }
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
            return PAR::completeScope();
          }
      };
    
    
#endif /*LIB_DIFF_TREE_MUTATOR_ATTRIBUTE_BINDING_H*/
