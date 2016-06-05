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
    
    
    template<class PAR>
    class AttributeBindingBase
      : public PAR
      {
        
        BareEntryID attribID_;
        
      protected:
        AttributeBindingBase (BareEntryID attribID, PAR&& chain)
          : PAR(std::forward<PAR>(chain))
          , attribID_(attribID)
          { }
        
        /** hard wired "selector predicate" for this binding layer.
         *  We handle only mutation operations pertaining attributes
         *  which additionally match the key defined at binding time.
         *  Any other operations are passed down the chain.
         * @param spec target specification given within the diff verb
         * @return if this binding is in charge for handling the spec
         */
        bool
        isApplicable (GenNode const& spec)
          {
            return spec.isNamed()
               and attribID_ == spec.idi;
          }
        
        
        /* ==== re-Implementation of the operation API ==== */
      public:
        /** this binding to an object field can not support any reordering,
         *  inserting or deletion of "Elements", since the structure of an object
         *  is fixed through the underlying class definition. For this reason,
         *  we do not keep track of an "old" and "new" order; rather there
         *  is always one single fixed element present to work on.
         * @return `true` always
         */
        virtual bool
        hasSrc()  override
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
            return isApplicable (spec)
                or PAR::acceptSrc (spec);
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
              throw error::Logic (_Fmt{"attempt to re-order attribute '%s', "
                                       "which is bound to an object field and "
                                       "thus does not respond to ordering."}
                                      % refSpec.idi.getSym());
            else
              return PAR::findSrc (refSpec);
          }
        
        /** there is no real support for navigating to a 'position',
         *  since attribute / data field binding has no notion of ordering.
         *  An attempt to fast-forward to "the end" is tolerated though.
         * @throw error::Logic when this binding becomes responsible and a
         *        request to navigate to some specific attribute is detected
         * @note  the diff spec `Ref::END` or `Ref::ATTRIBS` is tolerated
         *        and implemented as NOP (since there is no 'position'
         *        incorporated into the binding implementation.
         */
        virtual bool
        accept_until (GenNode const& spec)  override
          {
            if (Ref::END == spec or Ref::ATTRIBS == spec)
              return true;
            else
              if (isApplicable(spec))
                throw error::Logic (_Fmt{"attempt to navigate to a position behind attribute '%s', "
                                         "but this binding for '%s' is linked to a data field and "
                                         "thus does not support any notion of 'order' or 'position'."}
                                        % spec.idi.getSym()
                                        % this->attribID_);
              else
                return PAR::accept_until(spec);
          }
      };
    
    
    
    
    template<class PAR, class CLO>
    class ChangeOperation
      : public AttributeBindingBase<PAR>
      {
        using CloArgs = typename _ClosureType<CLO>::Args;
        using ValueType = typename lib::meta::Pick<CloArgs, 0>::Type;
        using ID = idi::EntryID<ValueType>;
        
        
        CLO setter_;
        
      public:
        ChangeOperation (Symbol attribKey, CLO clo, PAR&& chain)
          : AttributeBindingBase<PAR>(ID{attribKey}, std::forward<PAR>(chain))
          , setter_(clo)
          { }
        
        
        /* ==== Implementation of value assignment operation ==== */
        
        /** while, strictly speaking, one can not "insert" fields into a
         *  given class definition, this binding can tolerate an `INS` verb
         *  whenever this means to touch a field which is actually known and
         *  present in the class definition underlying this binding. In such
         *  a case, we just assign the given value. This implementation leeway
         *  is deliberate to support classes with optional / defaultable properties.
         */
        virtual bool
        injectNew (GenNode const& spec)  override
          {
            if (not this->isApplicable(spec))
              return PAR::injectNew(spec);
            
            setter_(spec.data.get<ValueType>());
            return true;
          }
        
        /** invoke the setter lambda, in case this binding layer is in charge */
        virtual bool
        assignElm (GenNode const& spec)  override
          {
            if (not this->isApplicable(spec))
              return PAR::assignElm(spec);
            
            setter_(spec.data.get<ValueType>());
            return true;
          }
      };
    
    
    
    template<class PAR, class MUT>
    class MutationOperation
      : public AttributeBindingBase<PAR>
      {
        
        ASSERT_VALID_SIGNATURE (MUT, void(TreeMutator::MutatorBuffer));

        MUT mutatorBuilder_;
        
        
      public:
        MutationOperation (BareEntryID const& attribID, MUT clo, PAR&& chain)
          : AttributeBindingBase<PAR>(attribID, std::forward<PAR>(chain))
          , mutatorBuilder_(clo)
          { }
        
        
        /** if this binding layer is in charge, then invoke the closure,
         *  which is assumed to construct a nested TreeMutator into the provided Buffer,
         *  able to deal with the nested attribute object referred by this binding */
        virtual bool
        mutateChild (GenNode const& spec, TreeMutator::MutatorBuffer targetBuff)  override
          {
            if (not this->isApplicable(spec))
              return PAR::mutateChild(spec, targetBuff);
            
            mutatorBuilder_(targetBuff);
            return true;
          }
      };
    
    
#endif /*LIB_DIFF_TREE_MUTATOR_ATTRIBUTE_BINDING_H*/
