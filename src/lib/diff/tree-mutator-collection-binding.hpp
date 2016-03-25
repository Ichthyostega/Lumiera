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


  
//== anonymous namespace...
    
    
    
    using lib::meta::Strip;
    using lib::diff::GenNode;
    using lib::iter_stl::eachElm;
    
    
    
    /** verify the installed functors or lambdas expose the expected signature */
#define ASSERT_VALID_SIGNATURE(_FUN_, _SIG_) \
        static_assert (has_Sig<_FUN_, _SIG_>::value, "Function " STRINGIFY(_FUN_) " unsuitable, expected signature: " STRINGIFY(_SIG_));
        
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
        
        using iterator       = typename lib::iter_stl::_SeqT<Coll>::Range;
        using const_iterator = typename lib::iter_stl::_SeqT<const Coll>::Range;
        
        
        ASSERT_VALID_SIGNATURE (MAT, bool(GenNode const& spec, Elm const& elm))
        ASSERT_VALID_SIGNATURE (CTR, Elm (GenNode const&))
        ASSERT_VALID_SIGNATURE (SEL, bool(GenNode const&))
        ASSERT_VALID_SIGNATURE (ASS, bool(Elm&, GenNode const&))
        ASSERT_VALID_SIGNATURE (MUT, bool(Elm&, TreeMutator::MutatorBuffer))
        
        
        Coll& collection;
        
        MAT matches;
        CTR construct;
        SEL isApplicable;
        ASS assign;
        MUT openSub;
        
        CollectionBinding(Coll& coll, MAT m, CTR c, SEL s, ASS a, MUT u)
          : collection(coll)
          , matches(m)
          , construct(c)
          , isApplicable(s)
          , assign(a)
          , openSub(u)
          { }
        
        
        /* === content manipulation API === */
        
        Coll contentBuffer;
        
        iterator
        initMutation ()
          {
            contentBuffer.clear();
            swap (collection, contentBuffer);
            return eachElm (contentBuffer);
          }
        
        void
        inject (Elm&& elm)
          {
            collection.emplace_back (forward<Elm>(elm));
          }
        
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
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
        using Iter = typename BIN::iterator;
        
        BIN binding_;
        Iter pos_;
        
        
      public:
        ChildCollectionMutator(BIN wiringClosures, PAR const& chain)
          : PAR(chain)
          , binding_(wiringClosures)
          , pos_(binding_.initMutation())
          { }
        
        
        
        /* ==== re-Implementation of the operation API ==== */
      
        /** skip next pending src element,
         *  causing this element to be discarded
         */
        virtual void
        skipSrc ()  override
          {
            if (pos_)
              ++pos_;
          }
        
        /** fabricate a new element, based on
         *  the given specification (GenNode),
         *  and insert it at current position
         *  into the target sequence.
         */
        virtual void
        injectNew (GenNode const& n)  override
          {
            binding_.inject (binding_.construct(n));
          }
        
        virtual bool
        emptySrc ()  override
          {
            return !pos_;
          }
        
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #992
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
        using CollectionBinding<COLL,MAT,CTR,SEL,ASS,MUT>::CollectionBinding;
        
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
    
    
    using lib::meta::enable_if;
    using lib::diff::can_wrap_in_GenNode;
    
    
    template<typename ELM, typename SEL =void>
    struct _DefaultPayload
      {
        static bool
        match (GenNode const&, ELM const&)
          {
            throw error::Logic ("unable to build a sensible default matching predicate");
          }
        
        static ELM
        construct (GenNode const&)
          {
            throw error::Logic ("unable to build a sensible default for creating new elements");
          }
      };
    
    template<typename ELM>
    struct _DefaultPayload<ELM, enable_if<can_wrap_in_GenNode<ELM>>>
      {
        static bool
        match (GenNode const& spec, ELM const& elm)
          {
            return spec.matches(elm);
          }
        
        static ELM
        construct (GenNode const& spec)
          {
            return spec.data.get<ELM>();
          }
      };
    
    /**
     * starting point for configuration of a binding to STL container.
     * When using the "nested DSL" to setup a binding to child elements
     * managed within a STL collection, all the variable and flexible
     * aspects of the binding are preconfigured to a more or less
     * disabled and inactive state. The resulting binding layer
     * offers just minimal functionality. Typically you'd use
     * the created (\ref CollectionBindingBuilder) to replace
     * those defaults with lambdas tied into the actual
     * implementation of the target data structure.
     */
    template<class COLL>
    struct _DefaultBinding
      {
        using Coll = typename Strip<COLL>::TypeReferred;
        using Elm  = typename Coll::value_type;
        
        using Payload = _DefaultPayload<Elm>;
        
        static bool
        disable_selector (GenNode const&)
          {
            return false;
          }
        
        static bool
        disable_assignment (Elm&, GenNode const&)
          {
            return false;
          }
        
        static bool
        disable_childMutation (Elm&, TreeMutator::MutatorBuffer)
          {
            return false;
          }
        
        
        using FallbackBindingConfiguration
            = CollectionBindingBuilder<Coll
                                      ,decltype(&Payload::match)
                                      ,decltype(&Payload::construct)
                                      ,decltype(&disable_selector)
                                      ,decltype(&disable_assignment)
                                      ,decltype(&disable_childMutation)
                                      >;
        
        static FallbackBindingConfiguration
        attachTo (Coll& coll)
          {
            return FallbackBindingConfiguration{ coll
                                               , Payload::match
                                               , Payload::construct
                                               , disable_selector
                                               , disable_assignment
                                               , disable_childMutation
                                               };
          }
      };
    
    
    /**
     * Entry point to a nested DSL
     * for setup and configuration of a collection binding.
     * This function shall be used right within Builder::attach()
     * and wrap a language reference to the concrete collection
     * implementing the "object children". The result is a default configured
     * binding, which should be further adapted with the builder functions,
     * using lambdas as callback into the otherwise opaque implementation code. 
     */
    template<class COLL>
    auto
    collection (COLL& coll) -> decltype(_DefaultBinding<COLL>::attachTo(coll))
    {
      return _DefaultBinding<COLL>::attachTo(coll);
    }
    
    
    
#endif /*LIB_DIFF_TREE_MUTATOR_COLLECTION_BINDING_H*/
