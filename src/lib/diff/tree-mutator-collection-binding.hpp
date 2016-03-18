/*
  TREE-MUTATOR-COLLECTION-BINDING.hpp  -  diff::TreeMutator implementation building block

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


/** @file tree-mutator-collection-binding.hpp
 ** Special binding implementation for TreeMutator, allowing to map
 ** tree diff operations onto a STL collection of native implementation objects.
 ** TreeMutator is a customisable intermediary, which enables otherwise opaque
 ** implementation data structures to receive and respond to generic structural
 ** change messages ("tree diff").
 ** 
 ** Each concrete TreeMutator instance will be configured differently, and this
 ** adaptation is done by implementing binding templates, in the way of building
 ** blocks, attached and customised through lambdas. It is possible to layer
 ** several bindings on top of a single TreeMutator -- and this header defines
 ** a building block for one such layer, especially for binding to a representation
 ** of "child objects" managed within a typical STL container.
 **  
 ** @note the header tree-mutator-collection-binding.hpp with specific builder templates
 **       is included way down, after the class definitions. This is done so for sake
 **       of readability.
 ** 
 ** @see tree-mutator-test.cpp
 ** @see TreeMutator::build()
 ** 
 */


#ifndef LIB_DIFF_TREE_MUTATOR_COLLECTION_BINDING_H
#define LIB_DIFF_TREE_MUTATOR_COLLECTION_BINDING_H
#ifndef LIB_DIFF_TREE_MUTATOR_H
  #error "this header shall not be used standalone (see tree-mutator.hpp)"
#endif


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/meta/trait.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/opaque-holder.hpp"
//#include "lib/util.hpp"
//#include "lib/format-string.hpp"

#include <functional>
#include <utility> ////TODO
#include <string>
//#include <vector>
//#include <map>


namespace lib {
  /////////////////////////////TODO move over into opaque-holder.hpp
  /**
   * handle to allow for safe _»remote implantation«_
   * of an unknown subclass into a given OpaqueHolder buffer,
   * without having to disclose the concrete buffer type or size.
   * @remarks this is especially geared towards use in APIs, allowing
   *    a not yet known implementation to implant an agent or collaboration
   *    partner into the likewise undisclosed innards of the exposed service.
   * @warning the type BA must expose a virtual dtor, since the targetted
   *    OpaqueHolder has to take ownership of the implanted object.
   */
  template<class BA>
  class PlantingHandle
    {
      void* buffer_;
      size_t maxSiz_;
      
      ///////TODO static assert to virtual dtor??
    public:
      template<size_t maxSiz>
      PlantingHandle (InPlaceBuffer<BA, maxSiz>& targetBuffer)
        : buffer_(&targetBuffer)
        , maxSiz_(maxSiz)
        { }
      
      
      template<class SUB>
      BA&
      create (SUB&& subMutator)
        {
          if (sizeof(SUB) > maxSiz_)
            throw error::Fatal("Unable to implant implementation object of size "
                               "exceeding the pre-established storage buffer capacity."
                              ,error::LUMIERA_ERROR_CAPACITY);
          
          using Holder = InPlaceBuffer<BA, sizeof(SUB)>;
          Holder& holder = *static_cast<Holder*> (buffer_);
          
          return holder.create<SUB> (std::forward<SUB> (subMutator));
        }
      
      template<class SUB>
      bool
      canCreate()  const
        {
          return sizeof(SUB) <= maxSiz_;
        }
    };
  /////////////////////////////TODO move over into opaque-holder.hpp
namespace diff{
  
  namespace error = lumiera::error;
  
//using util::_Fmt;
  using lib::Literal;
  using std::function;
  using std::string;
  
  
  
  
  class TestMutationTarget; // for unit testing
  
  
  namespace {
    template<class PAR>
    struct Builder;
    
    using ID        = Literal;
    using Attribute = DataCap;
  }
  
  
  /**
   * Customisable intermediary to abstract mutating operations
   * on arbitrary, hierarchical object-like data.
   * The TreeMutator exposes two distinct interfaces
   * - the \em operation API -- similar to what a container exposes --
   *   is the entirety of abstract operations that can be done to the
   *   subsumed, tree like target structure
   * - the \em binding API allows to link some or all of these generic
   *   activities to concrete manipulations known within target scope.
   */
  class TreeMutator
    {
      
    public:
      
      /* ==== operation API ==== */
      
      virtual bool
      emptySrc ()
        {
          return true;
          // do nothing by default
        }
      
      /** skip next src element and advance abstract source position */
      virtual void
      skipSrc ()
        {
          // do nothing by default
        }
      
      /** establish new element at current position */
      virtual void
      injectNew (GenNode const&)
        {
          // do nothing by default
        }
      
      /** ensure the next source element matches with given spec */
      virtual bool
      matchSrc (GenNode const&)
        {
          // do nothing by default
          return false;
        }
      
      /** accept existing element, when matching the given spec */
      virtual bool
      acceptSrc (GenNode const&)
        {
          // do nothing by default
          return false;
        }
      
      /** repeatedly accept, until after the designated location */
      virtual bool
      accept_until (GenNode const&)
        {
          // do nothing by default
          return false;
        }
      
      /** locate designated element and accept it at current position */
      virtual bool
      findSrc (GenNode const&)
        {
          // do nothing by default
          return false;
        }
      
      /** locate the designated target element
       *  (must be already accepted into the target sequence).
       *  Perform an assignement with the given payload value
       * @throw when assignement fails (typically error::Logic)
       * @return false when unable to locate the target */
      virtual bool
      assignElm (GenNode const&)
        {
          // do nothing by default
          return false;
        }
      
      
      using MutatorBuffer = PlantingHandle<TreeMutator>;
      
      /** locate the designated target element
       *  and build a suittable sub-mutator for this element
       *  into the provided target buffer
       * @throw error::Fatal when buffer is insufficient
       * @return false when unable to locate the target */
      virtual bool
      mutateChild (GenNode const&, MutatorBuffer)
        {
          // do nothing by default
          return false;
        }
      
      virtual void setAttribute (ID, Attribute&) { /* do nothing by default */ }
      
      /**
       * start building a custom adapted tree mutator,
       * where the operations are tied by closures or
       * wrappers into the current implementation context.
       */
      static Builder<TreeMutator> build();
    };
  
  
  namespace { // Mutator-Builder decorator components...
    
    using lib::meta::Strip;
    
    /**
     * Type rebinding helper to pick up the actual argument type.
     * Works both for functors and for lambda expressions
     * @remarks Solution proposed 10/2011 by \link http://stackoverflow.com/users/224671/kennytm user "kennytm" \endlink
     *          in this \link http://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda/7943765#7943765
     *          answer on stackoverflow \endlink
     */
    template<typename FUN>
    struct _ClosureType
      : _ClosureType<decltype(&FUN::operator())>
      { };

    template<class C, class RET, class ARG>
    struct _ClosureType<RET (C::*)(ARG)  const>
      {
        typedef ARG ArgType;
        typedef RET ReturnType;
      };
    
    
    template<class PAR, class CLO>
    struct ChangeOperation
      : PAR
      {
        ID attribID_;
        CLO change_;
        
        virtual void
        setAttribute (ID id, Attribute& newValue)
          {
            using ValueType = typename _ClosureType<CLO>::ArgType;
            
            if (id == attribID_)
              change_(newValue.get<ValueType>());
            
            else // delegate to other closures (Decorator-style)
              PAR::setAttribute(id, newValue);
          }
        
        ChangeOperation(ID id, CLO clo, PAR const& chain)
          : PAR(chain)
          , attribID_(id)
          , change_(clo)
          { }
      };
    
    
    /**
     * Attach to collection: Concrete binding setup.
     * This record holds all the actual binding and closures
     * used to attach the tree mutator to an external pre-existing
     * STL container with child elements/objects. It serves as flexible
     * connection, configuration and adaptation element, and will be embedded
     * as a whole into the (\ref ChildCollectionMutator), which in turn implements
     * the `TreeMutator` interface. The resulting compound is able to consume
     * tree diff messages and apply the respective changes and mutations to
     * an otherwise opaque implementation data structure.
     * 
     * @tparam COLL a STL compliant collection type holding "child elements"
     * @tparam MAT a closure to determine if a child matches a diff spec (GenNode)
     * @tparam CTR a closure to construct a new child element from a given diff spec
     * @tparam SEL predicate to determine if this binding layer has to process a diff message
     * @tparam ASS a closure to assign / set a new value from a given diff spec
     * @tparam MUT a closure to construct a nested mutator for some child element
     */
    template<class COLL, class MAT, class CTR, class SEL, class ASS, class MUT>
    struct CollectionBinding
      {
        using Coll = typename Strip<COLL>::TypeReferred;
        using Elm  = typename Coll::value_type;
        
        Coll& collection;
        
        SEL isApplicable;
        MAT matches;
        CTR construct;
        ASS assign;
        MUT openSub;
        
        
        /* === content manipulation API === */
        
        Coll contentBuffer;
        
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
        void
        initMutation (string mutatorID)
          {
            prev_content_.clear();
            swap (content_, prev_content_);
            log_.event ("attachMutator "+mutatorID);
          }
        
        void
        inject (GenNode&& elm, string operationID)
          {
            content_.emplace_back (forward<GenNode>(elm));
            log_.event (operationID, renderNode (content_.back()));
          }
        
        static iterator
        search (GenNode::ID const& targetID, iterator pos)
          {
            while (pos and not pos->matches(targetID))
              ++pos;
            return pos;
          }
        
        iterator
        locate (GenNode::ID const& targetID)
          {
            if (!empty() and content_.back().matches(targetID))
              return lastElm();
            else
              return search (targetID, eachElm(content_));
          }
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
      };
    
    
    /**
     * Attach to collection: Building block for a concrete `TreeMutator`.
     * This decorator will be outfitted with actual binding and closures
     * and then layered on top of the (\ref TreeMutaor) base. The resulting
     * compound is able to consume tree diff messages and apply the respective
     * changes and mutations to an otherwise opaque implementation data structure.
     * @remarks in practice, this is the most relevant and typical `TreeMutator` setup.
     */
    template<class PAR, class BIN>
    class ChildCollectionMutator
      : public PAR
      {
        BIN binding_;
        
      public:
        ChildCollectionMutator(BIN wiringClosures, PAR const& chain)
          : PAR(chain)
          , binding_(wiringClosures)
          { }
        
        
        
        /* ==== re-Implementation of the operation API ==== */
      
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
        /** skip next recorded src element
         * @remarks TestWireTap adapter together with TestMutationTarget
         *      maintain a "shaddow copy" of the data and apply the detected diff
         *      against this internal copy. This allows to verify what's going on
         */
        virtual void
        skipSrc ()  override
          {
            if (pos_)
              {
                GenNode const& skippedElm = *pos_;
                ++pos_;
                target_.logSkip (skippedElm);
              }
            PAR::skipSrc();
          }
        
        /** record in the test taget
         *  that a new child element is
         *  being insertet at current position
         */
        virtual void
        injectNew (GenNode const& n)  override
          {
            target_.inject (GenNode{n}, "injectNew");
            PAR::injectNew (n);
          }
        
        virtual bool
        emptySrc ()  override
          {
            return !pos_
               and PAR::emptySrc();
          }
        
        /** ensure the next recorded source element
         *  matches on a formal level with given spec */
        virtual bool
        matchSrc (GenNode const& n)  override
          {
            return PAR::matchSrc(n)
                or pos_? n.matches(*pos_)
                       : false;
          }
        
        /** accept existing element, when matching the given spec */
        virtual bool
        acceptSrc (GenNode const& n)  override
          {
            bool isSrcMatch = TestWireTap::matchSrc(n);
            if (isSrcMatch)             // NOTE: important to call our own method here, not the virtual function
              {
                target_.inject (move(*pos_), "acceptSrc");
                ++pos_;
              }
            return PAR::acceptSrc(n)
                or isSrcMatch;
          }
        
        /** locate designated element and accept it at current position */
        virtual bool
        findSrc (GenNode const& ref)  override
          {
            Iter found = TestMutationTarget::search (ref.idi, pos_);
            if (found)
              {
                target_.inject (move(*found), "findSrc");
              }
            return PAR::findSrc(ref)
                or found;
          }
        
        /** repeatedly accept, until after the designated location */
        virtual bool
        accept_until (GenNode const& spec)
          {
            bool foundTarget = true;
            
            if (spec.matches (Ref::END))
              for ( ; pos_; ++pos_)
                target_.inject (move(*pos_), "accept_until END");
            else
              {
                string logMsg{"accept_until "+spec.idi.getSym()};
                while (pos_ and not TestWireTap::matchSrc(spec))
                  {
                    target_.inject (move(*pos_), logMsg);
                    ++pos_;
                  }
                if (TestWireTap::matchSrc(spec))
                  {
                    target_.inject (move(*pos_), logMsg);
                    ++pos_;
                  }
                else
                  foundTarget = false;
              }
            return PAR::accept_until(spec)
                or foundTarget;
          }
        
        /** locate element already accepted into the target sequence
         *  and assign the designated payload value to it. */
        virtual bool
        assignElm (GenNode const& spec)
          {
            Iter targetElm = target_.locate (spec.idi);
            if (targetElm)
              {
                string logOldPayload{render(targetElm->data)};
                *targetElm = spec;
                target_.logAssignment (*targetElm, logOldPayload);
              }
            return PAR::assignElm(spec)
                or targetElm;
          }
        
        /** locate the designated target element and build a suitable
         *  sub-mutator for this element into the provided target buffer */
        virtual bool
        mutateChild (GenNode const& spec, TreeMutator::MutatorBuffer targetBuff)
          {
            if (PAR::mutateChild (spec, targetBuff))
              return true;
            else // Test mode only --
              { //  no other layer was able to provide a mutator
                Iter targetElm = target_.locate (spec.idi);
                if (targetElm)
                  {
                    targetBuff.create (TreeMutator::build());
                    target_.logMutation (*targetElm);
                    return true;
                  }
                return false;
              }
          }
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
      };
    
    
    /**
     * Nested DSL to define the specifics of a collection binding.
     */
    template<class COLL, class MAT, class CTR, class SEL, class ASS, class MUT>
    struct CollectionBindingBuilder
      : CollectionBinding<COLL,MAT,CTR,SEL,ASS,MUT>
      {
        
        template<class FUN>
        CollectionBindingBuilder<COLL, FUN ,CTR,SEL,ASS,MUT>
        matchElement(FUN matcher)
          {
            return { this->collection
                   , matcher
                   , this->construct
                   , this->isApplicable
                   , this->assign
                   , this->openSub
                   };
          }
        
        template<class FUN>
        CollectionBindingBuilder<COLL,MAT, FUN ,SEL,ASS,MUT>
        constructFrom(FUN constructor)
          {
            return { this->collection
                   , this->matches
                   , constructor
                   , this->isApplicable
                   , this->assign
                   , this->openSub
                   };
          }
        
        template<class FUN>
        CollectionBindingBuilder<COLL,MAT,CTR, FUN ,ASS,MUT>
        isApplicableIf(FUN selector)
          {
            return { this->collection
                   , this->matches
                   , this->construct
                   , selector
                   , this->assign
                   , this->openSub
                   };
          }
        
        template<class FUN>
        CollectionBindingBuilder<COLL,MAT,CTR,SEL, FUN ,MUT>
        assignElement(FUN setter)
          {
            return { this->collection
                   , this->matches
                   , this->construct
                   , this->isApplicable
                   , setter
                   , this->openSub
                   };
          }
        
        template<class FUN>
        CollectionBindingBuilder<COLL,MAT,CTR,SEL,ASS, FUN >
        buildChildMutator(FUN childMutationBuilder)
          {
            return { this->collection
                   , this->matches
                   , this->construct
                   , this->isApplicable
                   , this->assign
                   , childMutationBuilder
                   };
          }
      };
    
    template<class COLL>
    struct _DefaultBinding
      {
        using Coll = typename Strip<COLL>::TypeReferred;
        using Elm  = typename Coll::value_type;
        
        static bool
        disable_selector (GenNode const& spec)
          {
            UNIMPLEMENTED ("dont discriminate by default");
          }
        
        static bool
        default_contantMatch (Elm const& elm)
          {
            UNIMPLEMENTED ("fallback matcher");
          }
        
        static Elm
        default_construct_from_payload (GenNode const& spec)
          {
            UNIMPLEMENTED ("default construct from spec payload");
          }
        
        static bool
        disable_assignment (GenNode const& spec)
          {
            UNIMPLEMENTED ("disabled assignment");
          }
        
        static bool
        disable_childMutation (GenNode const& spec, TreeMutator::MutatorBuffer targetBuff)
          {
            UNIMPLEMENTED ("inactive mutator builder");
          }
        
        
        using FallbackBindingConfiguration
            = CollectionBindingBuilder<Coll
                                      ,decltype(disable_selector)
                                      ,decltype(default_contantMatch)
                                      ,decltype(default_construct_from_payload)
                                      ,decltype(disable_assignment)
                                      ,decltype(disable_childMutation)
                                      >;
        
        static FallbackBindingConfiguration
        attachTo (Coll& coll)
          {
            return { coll
                   , disable_selector
                   , default_contantMatch
                   , default_construct_from_payload
                   , disable_assignment
                   , disable_childMutation
                   };
          }
      };
    
    
    template<class COLL>
    auto
    collection (COLL& coll) -> decltype(_DefaultBinding<COLL>::attachTo(coll))
    {
      return _DefaultBinding<COLL>::attachTo(coll);
    }
    
    
    
    
    template<class PAR>
    struct TestWireTap;
    
    
    template<class PAR>
    struct Builder
      : PAR
      {
        Builder(PAR par)
          : PAR(par)
          { }
        
        template<class CLO>
        using Change = ChangeOperation<PAR,CLO>;
        
        template<class BIN>
        using Collection = ChildCollectionMutator<PAR,BIN>;
        
        using WireTap = TestWireTap<PAR>;
        
        
        /* ==== binding API ==== */
        
        template<typename CLO>
        Builder<Change<CLO>>
        change (Literal attributeID, CLO closure)
          {
            return Change<CLO> (attributeID, closure, *this);
          }
        
        template<typename BIN>
        Builder<Collection<BIN>>
        attach (BIN&& collectionBindingSetup)
          {
            return Collection<BIN> (std::forward<BIN>(collectionBindingSetup));
          }
        
        Builder<WireTap>
        attachDummy (TestMutationTarget& dummy);
        
      };
    
  }//(END) Mutator-Builder decorator components...
  
  
  Builder<TreeMutator>
  TreeMutator::build ()
  {
    return TreeMutator();
  }
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_MUTATOR_COLLECTION_BINDING_H*/
