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
 ** by a **builder DSL**. Each call on this builder -- typically providing some closure --
 ** will add yet another _decorating layer_ on top of the basic TreeMutator (recall all
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
 ** ## Lifecycle
 ** The TreeMutator is conceived as a disposable, one-way-off object. On initialisation,
 ** it will _"grab" the contents of its target_ and push them back into place one by one
 ** while consuming a mutation diff. For this reason, TreeMutator is made **non-copyable**,
 ** just supporting move construction, as will happen when using the DSL functions on
 ** the builder. This is also the only supported usage pattern: you create an anonymous
 ** TreeMutator sub type by using the Builder functions right within the scope about to
 ** consume one strike of DiffStep entries from a MutationMessage. These diff steps
 ** should cover anything to confirm or reshape _all of the target's contents_. After that,
 ** you must not refer to the exhausted TreeMutator anymore, just let it fall out of scope.
 ** Incidentally, this also means that _any failure or exception encountered_ while applying
 ** a diff will leave a **corrupted target data structure**. The basic assumption is that
 ** - the target data structure will actually be built through diff messages solely
 ** - and that all received diff messages are sane, as being drawn from a
 **   semantically and structurally equivalent source structure
 ** If unable to uphold this consistency assumption, it is the client's responsibility
 ** to care for _transactional behaviour,_ i.e. create a clone copy of the data structure
 ** beforehand, and "commit" or "roll back" the result atomically.
 **  
 ** @note to improve readability, the actual implementation of the "binding layers"
 **       is defined in separate headers and included towards the bottom of this header.
 ** 
 ** @see tree-mutator-test.cpp
 ** @see tree-mutator-binding-test.cpp
 ** @see [usage for tree diff application](\ref tree-diff-application.hpp)
 ** @see diff-language.hpp
 ** @see DiffDetector
 ** 
 */


#ifndef LIB_DIFF_TREE_MUTATOR_H
#define LIB_DIFF_TREE_MUTATOR_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/symbol.hpp"
#include "lib/meta/trait.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/opaque-holder.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/format-string.hpp"
#include "lib/idi/entry-id.hpp"

#include <utility>
#include <string>


namespace lib {
namespace diff{
  
  namespace error = lumiera::error;
  
  using lib::Symbol;
  using std::string;
  using util::_Fmt;
  using lib::idi::BareEntryID;
  
  
  
  
  class TestMutationTarget; // for unit testing
  
  
  namespace {
    template<class PAR>
    struct Builder;
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
    : util::MoveOnly
    {
      
    public:
      virtual ~TreeMutator(); ///< this is an interface
      
      // only default and move construction allowed
      TreeMutator ()                    =default;
      TreeMutator (TreeMutator&&)       =default;
      
      
      
      /* ==== operation API ==== */
      
      /** initialisation immediately before start of diff application
       * @remark allows for setup of state which is dependent on memory location,
       *         like e.g. iterators. Due to the invocation via Builder DSL, the
       *         implementation object may be moved after construction, but prior
       *         to invoking this hook
       */
      virtual void
      init() { }
      
      virtual bool
      hasSrc () ////////////////////////////////////TODO questionable if we need it. Can not be sensibly implemented on multiple onion-layers!
        {
          return false;
          // do nothing by default
        }
      
      /** establish new element at current position
       * @return `true` when successfully inserted something */
      virtual bool
      injectNew (GenNode const&)
        {
          // do nothing by default
          return false;
        }
      
      /** ensure the next source element matches with given spec */
      virtual bool
      matchSrc (GenNode const&)
        {
          // do nothing by default
          return false;
        }
      
      /** skip next src element and advance abstract source position.
       *  The argument shall be used to determine applicability
       * @remarks this operation is used both to implement the `del` verb
       *          and the `skip` verb. Since the latter discards garbage
       *          left back by `find` we must not touch the contents,
       *          to prevent a SEGFAULT. Thus `skipSrc` can not match
       *          and thus can not return anything. Consequently the
       *          `del` implementation has to use `matchSrc` explicitly,
       *          and the latter must invoke the selector prior to
       *          performing the local match. */
      virtual void
      skipSrc (GenNode const&)
        {
          // do nothing by default
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
      accept_until (GenNode const& spec)
        {
          return (Ref::END == spec or Ref::ATTRIBS == spec);
              // contents are exhausted by default,
             //  yet we're unable to find something specific
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
       *  Perform an assignment with the given payload value
       * @throw when assignment fails (typically error::Logic)
       * @return false when unable to locate the target */
      virtual bool
      assignElm (GenNode const&)
        {
          // do nothing by default
          return false;
        }
      
      
      using Handle = PlantingHandle<TreeMutator>;
      
      /** locate the designated target element
       *  and build a suitable sub-mutator for this element
       *  into the provided target buffer
       * @throw error::Fatal when buffer is insufficient
       * @return `false` when unable to locate the target */
      virtual bool
      mutateChild (GenNode const&, Handle)
        {
          // do nothing by default
          return false;
        }
      
      /** ensure the scope addressed by this TreeMutator
       *  was processed and exhausted without mismatch
       * @return `true` when all "open ends" are closed
       *  and no pending work remains to be done. */
      virtual bool
      completeScope()
        {
          // nothing to clean-up or verify by default
          return true;
        }
      
      
      
      /**
       * DSL: start building a custom adapted tree mutator,
       * where the operations are tied by closures or
       * wrappers into the current implementation context.
       */
      static Builder<TreeMutator> build();
    };
  
  
  
  
  
  namespace { // Mutator-Builder decorator components...
    
    using lib::meta::_Fun;
    using std::forward;
    using std::move;
    
    
    template<typename FUN, typename SIG>
    struct has_Sig
      : std::is_same<SIG, typename _Fun<FUN>::Sig>
      { };
    
    /** verify the installed functors or lambdas expose the expected signature */
#define ASSERT_VALID_SIGNATURE(_FUN_, _SIG_) \
        static_assert (has_Sig<_FUN_, _SIG_>::value, "Function " STRINGIFY(_FUN_) " unsuitable, expected signature: " STRINGIFY(_SIG_));
    
    
    
    
    
    
    
    
    /**
     * Builder-DSL to create and configure a concrete TreeMutator
     * @remarks all generated follow-up builders are chained and
     *          derive from the implementation of the preceding
     *          "binding layer" and the TreeMutator interface.
     * @note on each chained builder call, the compound is
     *          moved "inside out" into the next builder.
     */
    template<class PAR>
    struct Builder
      : PAR
      {
        Builder(PAR&& par)
          : PAR{forward<PAR> (par)}
          { }
        
        template<typename BIN, typename...ARGS>
        Builder<BIN>
        chainedBuilder (ARGS&&...args)
          {
            return Builder<BIN> (BIN{forward<ARGS>(args)..., move(*this)});
          }
        
        
        
        /* ==== binding API ==== */
        
        /** set up a binding to represent an "attribute"
         *  through a data or object field. This binding will allow
         *  to apply basic diff operations, _but no re-ordering or deletion._
         *  Rationale is the fixed nature of a class definition, which does not
         *  support any notion of ordering, or adding and removal of members.
         * @param attributeID symbolic key to denote this "attribute"
         * @param setterClosure functor or lambda to apply a new value
         * @note the nominal value type of the "attribute" is picked up from
         *       the setterClosure's (single) argument. It must be one of the
         *       types supported as payload for GenNode. In case the target
         *       data field needs any other value type, it is the closure's
         *       responsibility to convert appropriately.
         * @note the combination of attributeID and nominal value type is used
         *       to build an (\ref EntryID). The hash of this EntryID needs to
         *       match the GenNode::ID in any diff verb considered to be
         *       "applicable" to this attribute and binding. Similar to
         *       GenNode, the provided attributeID is used as-is,
         *       without further sanitising.
         * @return a _chained builder,_ which establishes this building and
         *       can then be used to define additional binding layers on top
         */
        template<typename CLO>
        auto change (Symbol attributeID, CLO setterClosure);
        
        
        /** set up a binding for an object valued "attribute" or _named scope_.
         *  This covers the rather special case, where some relevant sub object is
         *  accessed as a (named) property of a managing parent object. On implementation
         *  level, this corresponds to using a _getter_ to access a subcomponent or "PImpl".
         *  On a formal level, for tree diff handling, such counts as _attribute_, yet with
         *  the special twist that we can not just assign a new "value", but rather have to
         *  enter a sub scope and handle a nested diff -- similar to how nested child objects
         *  are dealt with in general. Thus, all we need here is a way how to build a nested
         *  TreeMutator for this sub-scope.
         * @param attributeID symbolic key to denote this "attribute"
         * @param mutatorBuilderClosure functor or lambda to emplace a custom sub TreeMutator
         *        into the given buffer (handle). Such a nested mutator shall be wired internally
         *        to the object representation of the attribute in question.
         * @see CollectionBindingBuilder::buildChildMutator
         */
        template<typename CLO>
        auto mutateAttrib (Symbol attributeID, CLO mutatorBuilderClosure);
        
        /**
         * @param rawID the explicitly given ID of an attribute object,
         *              used literally to match the attribute in question
         */
        template<typename CLO>
        auto mutateAttrib (idi::BareEntryID const& rawID, CLO mutatorBuilderClosure);
        
        
        /** set up a binding to a structure of "child objects",
         *  implemented through a typical STL container
         * @param collectionBindingSetup as created by invoking a nested DSL,
         *   initiated by a builder function `collection(implRef)`, where `implRef`
         *   is a (language) reference to a STL compliant container existing somewhere
         *   within the otherwise opaque implementation. The type of the container and
         *   thus the type of the elements will be picked up, and the returned builder
         *   can be further outfitted with the builder methods, which take lambdas as
         *   callback into the implementation.
         *   - the _matcher closure_ (CollectionBindingBuilder::matchElement) defines
         *     how to determine, if an implementation data element "matches" a given diff spec
         *   - the _constructor closure_ (CollectionBindingBuilder::constructFrom) defines how
         *     to build a new implementation data element from the spec of an `INS` diff verb.
         *     Note: the result will be moved (move-constructed) into the target container.
         *   - the optional _selector closure_ (CollectionBindingBuilder::isApplicableIf)
         *     allows to limit applicability of this whole binding (layer) to only some
         *     diff specs. E.g., we may set up a binding for elements with value semantics
         *     and another binding layer on top to deal with object like children (sub scopes).
         *     Please note that this selector also gets to judge upon the Ref::ATTRIBS spec,
         *     which indicates if this layer's contents can be considered "attributes".
         *   - the optional _setter closure_ (CollectionBindingBuilder::assignElement) accepts
         *     a diff spec (GenNode) and should assign an equivalent value to the internal
         *     data representation of the corresponding element (typically by constructing
         *     an implementation data element and then invoking the corresponding setter)
         *   - the optional _mutator closure_ (CollectionBindingBuilder::buildChildMutator)
         *     allows for recursive descent into nested child scopes. On invocation, it has
         *     to build a suitable custom TreeMutator implementation into the provided buffer
         *     (handle), and this nested TreeMutator should be wired with the internal
         *     representation of the nested scope to enter. The code invoking this closure
         *     typically pushes the buffer on some internal stack and switches then to use
         *     this nested mutator until encountering the corresponding `EMU` bracket verb.
         * @note the `after(Ref::ATTRIBS)` verb can only processed if the selector responds
         *     correct to a Ref::ATTRIBS spec. The implicit default selector does so, i.e.
         *     it rejects `Ref::ATTRIBS`. Please be sure to accept this token _only_ if
         *     your layer indeed holds something meant to implement "attributes", because
         *     in that case, the verb `after(Ref::ATTRIBS)` will fast forward and accept
         *     all the current contents of this layer
         * @warning please note the _nested DSL_. The builder functions used to define
         *     the various closures are to be invoked on the _argument_ ("`collection(xyz)`"),
         *     not on the top level builder...
         */
        template<typename BIN>
        auto attach (BIN&& collectionBindingSetup);
        
        
        /** set up binding to a GenNode tree: Special setup to build a concrete `TreeMutator`.
         *  This decorator is already outfitted with the necessary closures to work on a
         *  diff::Record<GenNode> -- which is typically used as "meta representation" of
         *  object-like structures. Thus this binding allows to apply a MutationMessage onto
         *  such a given »External Tree Description«, mutating it into new shape.
         * @remarks our meta representation of "objects" is based on Record<GenNode>, which
         *          is implemented through two STL collections, one for the attributes and
         *          one for the child elements. Thus we'll using two binding layers, based
         *          on the ChildCollectionMutator, configured with the necessary lambdas.
         */
        auto attach (Rec::Mutator& targetTree);
        
        
        /** set up a diagnostic layer, binding to TestMutationTarget.
         *  This can be used to monitor the behaviour of the resulting TreeMutator for tests.
         */
        auto attachDummy (TestMutationTarget& dummy);
        
        
        /** set up a catch-all and ignore-everything layer */
        auto ignoreAllChanges();
      };
    
  }//(END) Mutator-Builder...
  
  
  inline Builder<TreeMutator>
  TreeMutator::build ()
  {
    return TreeMutator();
  }
  
  
}} // namespace lib::diff
#endif /*LIB_DIFF_TREE_MUTATOR_H*/


         /* == implementation detail headers == */

#include "lib/diff/tree-mutator-gen-node-binding.hpp"
#include "lib/diff/tree-mutator-attribute-binding.hpp"
#include "lib/diff/tree-mutator-collection-binding.hpp"
#include "lib/diff/tree-mutator-noop-binding.hpp"

