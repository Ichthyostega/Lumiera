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
        ASSERT_VALID_SIGNATURE (MUT, bool(Elm&, GenNode::ID const&, TreeMutator::Handle))
        
        
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
        
        iterator
        search (GenNode const& targetSpec, iterator pos)
          {
            while (pos and not matches(targetSpec, *pos))
              ++pos;
            return pos;
          }
        
        iterator
        locate (GenNode const& targetSpec)
          {
            if (not collection.empty()
                and (Ref::THIS.matches(targetSpec.idi)
                     or matches (targetSpec, collection.back())))
              return lastElm();
            else
              return search (targetSpec, eachElm(collection));
          }
        
      private:
        /** @internal technicality
         * Our iterator is actually a Lumiera RangeIter, and thus we need
         * to construct a raw collection iterator pointing to the aftmost element
         * and then create a range from this iterator and the `end()` iterator.
         */
        iterator
        lastElm()
          {
            using raw_iter = typename CollectionBinding::Coll::iterator;
            return iterator (raw_iter(&collection.back()), collection.end());
          }
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
        ChildCollectionMutator(BIN wiringClosures, PAR&& chain)
          : PAR(std::forward<PAR>(chain))
          , binding_(wiringClosures)
          , pos_(binding_.initMutation())
          { }
        
        
        
        
        /* ==== re-Implementation of the operation API ==== */
        
        /** fabricate a new element, based on
         *  the given specification (GenNode),
         *  and insert it at current position
         *  into the target sequence.
         */
        virtual bool
        injectNew (GenNode const& n)  override
          {
            if (binding_.isApplicable(n))
              {
                binding_.inject (binding_.construct(n));
                return true;
              }
            else
              return PAR::injectNew (n);
          }
        
        virtual bool
        hasSrc ()  override
          {
            return bool(pos_) or PAR::hasSrc();
          }
        
        /** ensure the next recorded source element
         *  matches on a formal level with given spec */
        virtual bool
        matchSrc (GenNode const& spec)  override
          {
            if (binding_.isApplicable(spec))
              return pos_ and binding_.matches (spec, *pos_);
            else
              return PAR::matchSrc (spec);
          }
        
        /** skip next pending src element,
         *  causing this element to be discarded
         * @note can not perform a match on garbage data
         */
        virtual void
        skipSrc (GenNode const& n)  override
          {
            if (binding_.isApplicable(n))
              {
                if (pos_)
                    ++pos_;
              }
            else
              PAR::skipSrc (n);
          }
        
        /** accept existing element, when matching the given spec */
        virtual bool
        acceptSrc (GenNode const& n)  override
          {
            if (binding_.isApplicable(n))
              {
                bool isSrcMatch = pos_ and binding_.matches (n, *pos_);
                if (isSrcMatch) //NOTE: crucial to perform only our own match check here 
                  {
                    binding_.inject (move(*pos_));
                    ++pos_;
                  }
                return isSrcMatch;
              }
            else
              return PAR::acceptSrc (n);
          }
        
        /** locate designated element and accept it at current position */
        virtual bool
        findSrc (GenNode const& refSpec)  override
          {
            if (binding_.isApplicable(refSpec))
              {
                Iter found = binding_.search (refSpec, pos_);
                if (found)
                  {
                    binding_.inject (move(*found));
                  }
                return found;
              }
            else
              return PAR::findSrc (refSpec);
          }
        
        /** repeatedly accept, until after the designated location */
        virtual bool
        accept_until (GenNode const& spec)
          {
            if (spec.matches (Ref::END))
              {
                for ( ; pos_; ++pos_)
                  binding_.inject (move(*pos_));
                return true;
              }
            else
            if (spec.matches (Ref::ATTRIBS))
                return PAR::accept_until (spec);
            else
              if (binding_.isApplicable(spec))
                {
                  bool foundTarget = false;
                  while (pos_ and not binding_.matches (spec, *pos_))
                    {
                      binding_.inject (move(*pos_));
                      ++pos_;
                    }
                  if (binding_.matches (spec, *pos_))
                    {
                      binding_.inject (move(*pos_));
                      ++pos_;
                      foundTarget = true;
                    }
                  return foundTarget;
                }
              else
                return PAR::accept_until (spec);
          }
        
        /** locate element already accepted into the target sequence
         *  and assign the designated payload value to it. */
        virtual bool
        assignElm (GenNode const& spec)
          {
            if (binding_.isApplicable(spec))
              {
                Iter target_found = binding_.locate (spec);
                return target_found and binding_.assign (*target_found, spec);
              }
            else
              return PAR::assignElm (spec);
          }
        
        /** locate the designated target element and build a suitable
         *  sub-mutator for this element into the provided target buffer */
        virtual bool
        mutateChild (GenNode const& spec, TreeMutator::Handle targetBuff)
          {
            if (binding_.isApplicable(spec))
              {
                Iter target_found = binding_.locate (spec);
                return target_found and binding_.openSub (*target_found, spec.idi, targetBuff);
              }
            else
              return PAR::mutateChild (spec, targetBuff);
          }
        
        /** verify all our pending (old) source elements where mentioned.
         * @note allows chained "onion-layers" to clean-up and verify.*/
        virtual bool
        completeScope()
          {
            return PAR::completeScope()
               and isnil(this->pos_);
          }
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
        matchElement (FUN matcher)                               ///< expected lambda: `bool(GenNode const& spec, Elm const& elm)`
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
        constructFrom (FUN constructor)                          ///< expected lambda: `Elm (GenNode const&)`
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
        isApplicableIf (FUN selector)                            ///< expected lambda: `bool(GenNode const&)`
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
        assignElement (FUN setter)                               ///< expected lambda: `bool(Elm&, GenNode const&)`
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
        buildChildMutator (FUN childMutationBuilder)             ///< expected lambda: `bool(Elm&, GenNode::ID const&, TreeMutator::Handle)`
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
        ignore_selector (GenNode const&)
          {
            return true; // by default apply diff unconditionally
          }
        
        static bool
        disable_assignment (Elm&, GenNode const&)
          {
            return false;
          }
        
        static bool
        disable_childMutation (Elm&, GenNode::ID const&, TreeMutator::Handle)
          {
            return false;
          }
        
        
        using FallbackBindingConfiguration
            = CollectionBindingBuilder<Coll
                                      ,decltype(&Payload::match)
                                      ,decltype(&Payload::construct)
                                      ,decltype(&ignore_selector)
                                      ,decltype(&disable_assignment)
                                      ,decltype(&disable_childMutation)
                                      >;
        
        static FallbackBindingConfiguration
        attachTo (Coll& coll)
          {
            return FallbackBindingConfiguration{ coll
                                               , Payload::match
                                               , Payload::construct
                                               , ignore_selector
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
