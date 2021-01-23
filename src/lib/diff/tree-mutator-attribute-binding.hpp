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
 ** a building block for one specific kind of layer, used to bind object fields
 ** through "setter" lambdas.
 ** 
 ** # architecture considerations
 ** Together with the (\ref tree-mutator-collection-binding.hpp), the attribute binding
 ** is the most relevant building block -- yet it is special in several respects. There
 ** is kind of a "impedance mismatch" between the concept of an "attribute", as used in
 ** the context of diff messages and »External Tree Description«, and the nature of
 ** data fields as used within the imperative or object oriented implementation: the
 ** latter is rooted within a _class definition_ -- which is conceived as a _type_,
 ** a conceptual entity used for construction of code, yet not really embodied into
 ** the actual code at execution time. Thus, with respect to the _behaviour_ at execution,
 ** the structure defined through typing and classes appears as static backdrop. This leads
 ** to the consequence, that, on a generic (unspecific) level, we don't have any correlate
 ** to the notion of _ordering_ and _sequence_, as found within the diff language.
 ** 
 ** On the other hand, this very notion of _ordering_ and _sequence_ is essential to the
 ** meaning of "diff", as far as collections of "children" are involved. This leaves us
 ** with the decision, either to increase complexity of the diff language's definition
 ** and concept, or to accommodate this discrepancy within the binding implementation.
 ** Deliberately, the whole concept of a "diff language" builds onto the notion of
 ** _layered semantics,_ where the precise meaning of some terms remains a private
 ** extension within specific usage context. There is a lot of leeway within the
 ** language, and the _correct usage protocol_ is linked to the actual scope of
 ** usage. We need the diff language to be a connecting medium, to link some
 ** remote partners based on a locally shared common understanding of structure.
 ** 
 ** And so we use the same approach when it comes to "attributes": We'll assume that
 ** the partners connected through diff messages are _structurally compatible_ -- thus
 ** any "change" message emitted at one side is assumed to basically make sense on the
 ** receiving side. Consequently, the binding of an "attribute" to an object or data field
 ** will either ignore or reject any specifics about field order. It will _reject_ an
 ** explicit demand to re-order a field, and it will silently pass down other notions
 ** related to ordering -- down to lower "onion layers" of the concrete binding. So
 ** it depends on the concrete setup of the data binding (TreeMutator), if some
 ** expression in diff language will be deemed incompatible -- which happens when
 ** in the end no "onion layer" of the concrete binding was able to absorb and
 ** comply to the MutationMessage.
 ** 
 ** Another architectural consideration is relevant to the way attribute bindings are
 ** constructed: we rather construct a separate binding for each individual attribute,
 ** instead of building a collective binding for all attributes of a given object.
 ** This gives us the benefit of a simple and flexible solution plus it avoids the
 ** overhead of managing a _collection of attribute definitions_ (which would likely
 ** cause a heap allocation for storage). The downside is that we loose any coherence
 ** between attributes of "the same object", we loose possible consistency checks and
 ** we get a linear search for access to any attribute binding. Moreover, we cannot
 ** protect against creation of a nonsensical binding, e.g. a binding which ties
 ** the same attribute several times in contradictory fashion. The client code
 ** constructing the concrete TreeMutator needs to have adequate understanding
 ** regarding mode of operation and "mechanics" of such a binding.
 ** 
 ** @remark for sake of completeness an alternative binding option should be mentioned:
 **       Attributes could be represented as a map of `(key,value)` pairs and then bound
 **       via the STL collection binding. This way, all the attributes of an "object"
 **       would be treated as coherent unit, within a single "onion layer". However,
 **       such a layout tends to run against the conventions and the protocol of the
 **       diff language and should be confined to cover some corner cases (e.g. to
 **       support an open ended collection of _custom properties_)
 ** 
 ** @note the header tree-mutator-attribute-binding.hpp was split off for sake of readability
 **       and is included automatically from bottom of tree-mutator.hpp
 ** 
 ** @see tree-mutator-test.cpp
 ** @see TreeMutator::build()
 ** 
 */


#ifndef LIB_DIFF_TREE_MUTATOR_ATTRIBUTE_BINDING_H
#define LIB_DIFF_TREE_MUTATOR_ATTRIBUTE_BINDING_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/diff/tree-mutator.hpp"
#include "lib/format-string.hpp"
#include "lib/idi/entry-id.hpp"

#include <utility>


namespace lib {
namespace diff{

  namespace { // Mutator-Builder decorator components...
    
    
    /**
     * Generic behaviour of any binding to object fields (attributes).
     * Since object fields as such are part of the class definition, a diff
     * will never be able to add, insert, delete or re-order fields. Thus we
     * do not need to keep track of an "old" and "new" order; rather there
     * is always one single fixed element present to work on.
     * @note consequently, several diff operations are either implemented NOP,
     *       or passed to the parent (lower onion layers).
     */
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
        
        void
        __ifApplicable_refuse_to(Literal oper, GenNode const& spec)
          {
            if (this->isApplicable(spec))
              throw error::Logic (_Fmt{"attempt to %s attribute '%s', "
                                       "but this binding for '%s' is linked to a data field and "
                                       "thus does not support any notion of 'order' or 'position', "
                                       "inserting or deletion."}
                                      % oper % spec.idi % this->attribID_);
          }
        
        
        /* ==== re-Implementation of the operation API ==== */
      public:
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
        
        
        // note: hasSrc() not overridden here --> delegate to parent layer
        
        
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
        virtual void
        skipSrc (GenNode const& refSpec)  override
          {
            __ifApplicable_refuse_to ("skip or drop", refSpec);
            PAR::skipSrc (refSpec);
          }
        
        virtual bool
        findSrc (GenNode const& refSpec)  override
          {
            __ifApplicable_refuse_to ("re-order", refSpec);
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
              return PAR::accept_until(spec);
            else
              {
                __ifApplicable_refuse_to ("navigate to a position behind", spec);
                return PAR::accept_until(spec);
              }
          }
      };
    
    
    
    
    template<class PAR, class CLO>
    class ChangeOperation
      : public AttributeBindingBase<PAR>
      {
        using CloArgs = typename lib::meta::_Fun<CLO>::Args;
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
        
        /** invoke the setter lambda, when this binding layer is in charge */
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
        
        ASSERT_VALID_SIGNATURE (MUT, void(TreeMutator::Handle));

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
        mutateChild (GenNode const& spec, TreeMutator::Handle targetBuff)  override
          {
            if (not this->isApplicable(spec))
              return PAR::mutateChild(spec, targetBuff);
            
            mutatorBuilder_(targetBuff);
            return true;
          }
        
        
        /** default setup: silently absorb insert.
         * @remark this whole binding flavour (MutateOperation) deals with an
         *  odd corner case, namely an object valued attribute, which is deliberately
         *  treated as nested scope; a sub-scope accessed by name. Since attributes
         *  do not support any notion of "inserting" or "reordering" anyway, it is
         *  typical in such a situation for the implementation data structure to manage
         *  the underlying object automatically or default construct it anyway; moreover
         *  it would be pointless to define a setter, since the whole point is not to
         *  assign, but rather to enter and populate the nested target object. Yet,
         *  the diff language requires us to send an empty initial value at least
         *  once prior to mutation.
         * @note if you really _need_ a dedicated custom setter, just define it
         *  _after_ the `mutateAttribute` spec; it's implementation will then
         *  be layered on top and shadow this default case.
         */
        virtual bool
        injectNew (GenNode const& spec)  override
          {
            return this->isApplicable(spec)
                or PAR::injectNew(spec);
          }
      };
    
    
    
    /** Entry point for DSL builder */
    template<class PAR>
    template<typename CLO>
    inline auto
    Builder<PAR>::change (Symbol attributeID, CLO setterClosure)
    {
      return chainedBuilder<ChangeOperation<PAR,CLO>> (attributeID, setterClosure);
    }
    
    
    /** Entry point for DSL builder */
    template<class PAR>
    template<typename CLO>
    inline auto
    Builder<PAR>::mutateAttrib (Symbol attributeID, CLO mutatorBuilderClosure)
    {
      idi::EntryID<Rec> key{attributeID};
      return chainedBuilder<MutationOperation<PAR,CLO>> (key, mutatorBuilderClosure);
    }
    
    /** Entry point for DSL builder; variation to handle an attribute-object with given ID */
    template<class PAR>
    template<typename CLO>
    inline auto
    Builder<PAR>::mutateAttrib (idi::BareEntryID const& rawID, CLO mutatorBuilderClosure)
    {
      return chainedBuilder<MutationOperation<PAR,CLO>> (rawID, mutatorBuilderClosure);
    }
    
    
    
  }//(END)Mutator-Builder decorator components...
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_MUTATOR_ATTRIBUTE_BINDING_H*/
