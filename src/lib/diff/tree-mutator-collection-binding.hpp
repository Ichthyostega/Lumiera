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
 ** As a _special case_, binding to a STL map is supported, while this usage is rather
 ** discurraged, since it contradicts the diff semantics due to intrinsic ordering.
 ** 
 ** @note the header tree-mutator-collection-binding.hpp was split off for sake of readability
 **       and is included automatically from bottom of tree-mutator.hpp
 ** 
 ** @see tree-mutator-test.cpp
 ** @see TreeMutator::build()
 ** 
 */


#ifndef LIB_DIFF_TREE_MUTATOR_COLLECTION_BINDING_H
#define LIB_DIFF_TREE_MUTATOR_COLLECTION_BINDING_H


#include "lib/error.hpp"
#include "lib/meta/trait.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/diff/tree-mutator.hpp"
#include "lib/iter-adapter-stl.hpp"

#include <utility>
#include <vector>
#include <map>


namespace lib {
namespace diff{

  namespace { // Mutator-Builder decorator components...
    
    
    using std::forward;
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
     * @tparam MAT a functor to determine if a child matches a diff spec (GenNode)
     * @tparam CTR a functor to construct a new child element from a given diff spec
     * @tparam SEL predicate to determine if this binding layer has to process a diff message
     * @tparam ASS a functor to assign / set a new value from a given diff spec
     * @tparam MUT a functor to construct a nested mutator for some child element
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
        
        // allow move construction only,
        // to enable use of unique_ptr in collections
        CollectionBinding(CollectionBinding&&) = default;
        CollectionBinding(CollectionBinding&) = delete;
        
        
        
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
            emplace (collection, forward<Elm>(elm));
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
                and matches (targetSpec, recentElm(collection)))
              return recentElmIter();
            else
              return search (targetSpec, eachElm(collection));
          }
        
        
      private: /* === technicallities of container access === */
        
        /** @internal technicality
         * Our iterator is actually a Lumiera RangeIter, and thus we need
         * to construct a raw collection iterator pointing to the aftmost element
         * and then create a range from this iterator and the `end()` iterator.
         */
        iterator
        recentElmIter()
          {
            return iterator (recentElmRawIter(collection), collection.end());
          }
        
        template<class C>
        static auto
        recentElmRawIter (C& coll) ///< fallback: use first element of container
          {
            return coll.begin();
          }
        
        template<typename E>
        using Map = std::map<typename E::key_type, typename E::second_type>;
        
        template<typename E>
        static auto
        recentElmRawIter (Map<E>& map) ///< workaround for `std::Map`: lookup via reverse iterator
          {
            return map.find (recentElm(map).first);
          }
        
        static auto
        recentElmRawIter (std::vector<Elm>& vec)
          {
            return typename std::vector<Elm>::iterator (&vec.back());
          }
        
        
        template<class C>
        static Elm&
        recentElm (C& coll)
          {
            return *coll.begin();
          }
        
        template<typename E>
        static E&
        recentElm (Map<E>& map)
          {
            return *++map.rend();
          }
        
        static Elm&
        recentElm (std::vector<Elm>& vec)
          {
            return vec.back();
          }
        
        
        template<class C>
        static void
        emplace (C& coll, Elm&& elm)
          {
            coll.emplace (forward<Elm> (elm));
          }
        
        static void
        emplace (std::vector<Elm>& coll, Elm&& elm)
          {
            coll.emplace_back (forward<Elm> (elm));
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
        ChildCollectionMutator(BIN&& wiringClosures, PAR&& chain)
          : PAR(std::forward<PAR>(chain))
          , binding_(forward<BIN>(wiringClosures))
          , pos_()
          { }
        
        
        
        
        /* ==== Implementation of TreeNode operation API ==== */
        
        virtual void
        init()  override
          {
            pos_ = binding_.initMutation();
            PAR::init();
          }
        
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
                binding_.inject (std::move (binding_.construct(n)));
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
        accept_until (GenNode const& spec)  override
          {
            if (spec.matches (Ref::END)
               or
               (spec.matches (Ref::ATTRIBS)
                and binding_.isApplicable (Ref::ATTRIBS)))
              {
                for ( ; pos_; ++pos_)
                  binding_.inject (move(*pos_));
                return PAR::accept_until (spec);
              }
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
        assignElm (GenNode const& spec)  override
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
        mutateChild (GenNode const& spec, TreeMutator::Handle targetBuff)  override
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
        completeScope()  override
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
      };                                                                           /////////////////////////////////TICKET #1041  provide a shortcut for just invoking a nested DiffMutable
    
    
    /** builder function to synthesise builder type from given functors */
    template<class COLL, class MAT, class CTR, class SEL, class ASS, class MUT>
    inline auto
    createCollectionBindingBuilder (COLL& coll, MAT m, CTR c, SEL s, ASS a, MUT u)
    {
      using Coll = typename Strip<COLL>::TypeReferred;

      return CollectionBindingBuilder<Coll, MAT,CTR,SEL,ASS,MUT> {coll, m,c,s,a,u};
    }
    
    
    
    template<class ELM>
    struct _EmptyBinding
      {
        static bool
        __ERROR_missing_matcher (GenNode const&, ELM const&)
          {
            throw error::Logic ("unable to build a sensible default matching predicate");
          }
        
        static ELM
        __ERROR_missing_constructor (GenNode const&)
          {
            throw error::Logic ("unable to build a sensible default for creating new elements");
          }
        
        static bool
        ignore_selector (GenNode const& spec)
          {
            return spec != Ref::ATTRIBS;
            // by default apply diff unconditionally,
            // but don't respond to after(ATTRIBS)
          }
        
        static bool
        disable_assignment (ELM&, GenNode const&)
          {
            return false;
          }
        
        static bool
        disable_childMutation (ELM&, GenNode::ID const&, TreeMutator::Handle)
          {
            return false;
          }
        
        
        template<class COLL>
        static auto
        attachTo (COLL& coll)
          {
            return createCollectionBindingBuilder (coll
                                                  ,__ERROR_missing_matcher
                                                  ,__ERROR_missing_constructor
                                                  ,ignore_selector
                                                  ,disable_assignment
                                                  ,disable_childMutation
                                                  );
          }
      };
    
    
    
    using lib::meta::enable_if;
    using lib::diff::can_wrap_in_GenNode;
    
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
     * @note depending on the payload type within the collection,
     *       we provide some preconfigured default specialisations
     */
    template<class ELM, typename SEL =void>
    struct _DefaultBinding
      : _EmptyBinding<ELM>
      { };
    
    template<class ELM>
    struct _DefaultBinding<ELM, enable_if<can_wrap_in_GenNode<ELM>>>
      {
        template<class COLL>
        static auto
        attachTo (COLL& coll)
          {
            return _EmptyBinding<ELM>::attachTo(coll)
                      .matchElement([](GenNode const& spec, ELM const& elm)
                         {
                           return spec.matches(elm);
                         })
                      .constructFrom([](GenNode const& spec) -> ELM
                         {
                           return spec.data.get<ELM>();
                         });
          }
      };
    
    
    /** standard configuration to deal with GenNode collections.
     * @see tree-mutator-gen-node-binding.hpp */
    template<>
    struct _DefaultBinding<GenNode>
      {
        template<class COLL>
        static auto
        attachTo (COLL& coll)
          {
            return _EmptyBinding<GenNode>::attachTo(coll)
                      .matchElement([](GenNode const& spec, GenNode const& elm)
                         {
                           return spec.matches(elm);
                         })
                      .constructFrom([](GenNode const& spec) -> GenNode
                         {
                           return GenNode{spec};
                         })
                      .assignElement ([](GenNode& target, GenNode const& spec) -> bool
                         {
                           target.data = spec.data;
                           return true;
                         })
                      .buildChildMutator ([](GenNode& target, GenNode::ID const& subID, TreeMutator::Handle buff) -> bool
                         {
                           if (target.idi == subID     // require match on already existing child object
                               and target.data.isNested())
                             {
                               mutateInPlace (target.data.get<Rec>())
                                   .buildMutator(buff);
                               buff.get()->init();
                               return true;
                             }
                           else
                             return false;
                         });
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
    inline auto
    collection (COLL& coll)
    {
      using Elm  = typename COLL::value_type;
      
      return _DefaultBinding<Elm>::attachTo(coll);
    }
    
    
    
    /** Entry point for DSL builder */
    template<class PAR>
    template<class BIN>
    inline auto
    Builder<PAR>::attach (BIN&& collectionBindingSetup)
    {
      return chainedBuilder<ChildCollectionMutator<PAR,BIN>> (forward<BIN>(collectionBindingSetup));
    }
    
    
  }//(END)Mutator-Builder decorator components...
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_MUTATOR_COLLECTION_BINDING_H*/
