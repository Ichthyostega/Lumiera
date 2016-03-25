/*
  TREE-MUTATOR.hpp  -  flexible binding to map generic tree changing operations

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file tree-mutator.hpp
 ** Customisable intermediary to abstract generic tree mutation operations.
 ** This is the foundation for generic treatment of tree altering operations,
 ** and especially the handling of changes (diff) to hierarchical data structures.
 ** The goal is to represent a standard set of conceptual operations working on
 ** arbitrary data structures, without the need for these data structures to
 ** comply to any interface or base type. Rather, we allow each instance to
 ** define binding closures, which allows to tap into arbitrary internal data
 ** representation, without any need of disclosure. The only assumption is
 ** that the data to be treated is \em hierarchical and \em object-like,
 ** i.e. it has (named) attributes and it may have a collection of children.
 ** If necessary, typing constraints can be integrated through symbolic
 ** representation of types as chained identifiers. (path dependent types).
 ** 
 ** The interface implemented by the TreeMutator is shaped such as to support
 ** the primitives of Lumiera's tree \link diff-language.hpp diff handling language. \endlink
 ** By default, each of these primitives is implemented as a \c NOP -- but each operation
 ** can be replaced by a binding closure, which allows to invoke arbitrary code in the
 ** context of the given object's implementation internals.
 ** 
 ** ## Builder/Adapter concept
 ** TreeMutator is both an interface and a set of building blocks.
 ** On concrete usage, the (private, non disclosed) target data structure is assumed
 ** to _build a subclass of TreeMutator._ To this end, the TreeMutator is complemented
 ** by a builder API. Each call on this builder -- typically providing some closure --
 ** will add yet another decorating layer on top of the basic TreeMutator (recall all
 ** the "mutation primitives" are implemented NOP within the base class). So the actual
 ** TreeMutator will be structured like an onion, where each layer cares for the sole
 ** concrete aspect it was tied for by the supplied closure. For example, there might
 ** be a decorator to handle setting of a "foobar" attribute. Thus, when the diff
 ** dictates to mutate "foobar", the corresponding closure will be invoked.
 ** 
 ** \par test dummy target
 ** There is a special adapter binding to support writing unit tests. The corresponding
 ** API is only declared (forward) by default. The TestMutationTarget is a helper class,
 ** which can be attached through this binding and allows a unit test fixture to record
 ** and verify all the mutation operations encountered.
 **  
 ** @note to improve readability, the actual implementation of the "binding layers"
 **       is defined in separate headers and included towards the bottom of this header.
 ** 
 ** @see tree-mutator-test.cpp
 ** @see DiffDetector
 ** 
 */


#ifndef LIB_DIFF_TREE_MUTATOR_H
#define LIB_DIFF_TREE_MUTATOR_H


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
    using lib::meta::Types;
    
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

    template<class C, class RET, typename...ARGS>
    struct _ClosureType<RET (C::*)(ARGS...)  const>
      {
        using Args = typename Types<ARGS...>::Seq;
        using Ret  = RET;
        using Sig  = RET(ARGS...);
      };

    template<class RET, typename...ARGS>
    struct _ClosureType<RET (*)(ARGS...)>
      {
        using Args = typename Types<ARGS...>::Seq;
        using Ret  = RET;
        using Sig  = RET(ARGS...);
      };
    
    
    template<typename FUN, typename SIG>
    struct has_Sig
      : std::is_same<SIG, typename _ClosureType<FUN>::Sig>
      { };
    
    

    /* == implementation detail headers == */
    
#include "lib/diff/tree-mutator-attribute-binding.hpp"
#include "lib/diff/tree-mutator-collection-binding.hpp"
    
    
    
    template<class PAR>
    struct TestWireTap;
    
    
    /**
     * Builder-DSL to create and configure a concrete TreeMutator
     * @remarks all generated follow-up builders are chained and
     *          derive from the implementation of the preceding
     *          "binding layer" and the TreeMutator interface.
     */
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
        
        /** set up a binding to a structure of "child objects",
         *  implemented through a typical STL container
         * @param collectionBindingSetup as created by invoking a nested DSL,
         *   initiated by a builder function `collection(implRef)`, where `implRef`
         *   is a (language) reference to a STL compliant container existing somewhere
         *   within the otherwise opaque implementation. The type of the container and
         *   thus the type of the elements will be picked up, and the returned builder
         *   can be further outfitted with the builder methods, which take labmdas as
         *   callback into the implementation.
         */
        template<typename BIN>
        Builder<Collection<BIN>>
        attach (BIN&& collectionBindingSetup)
          {
            return Collection<BIN> (std::forward<BIN>(collectionBindingSetup), *this);
          }
        
        /** set up a diagnostic layer, binding to TestMutationTarget.
         *  This can be used to monitor the behaviour of the resulting TreeMutator for tests.
         */
        Builder<WireTap>
        attachDummy (TestMutationTarget& dummy);
        
      };
    
  }//(END) Mutator-Builder...
  
  
  Builder<TreeMutator>
  TreeMutator::build ()
  {
    return TreeMutator();
  }
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_MUTATOR_H*/
