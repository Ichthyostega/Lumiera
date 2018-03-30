/*
  DEFS-MANAGER.hpp  -  access to preconfigured default objects and definitions

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file defs-manager.hpp
 ** Management of defaults and default rules.
 ** It is one of the central ideas to shape the behaviour of Lumiera
 ** not through hard wired procedures, but rather by the combination of
 ** pattern elements driven by rules. A key element within this approach
 ** is the notion of an *default*: Whenever some concrete calculation or
 ** behaviour needs an additional element as missing link, in order to
 ** reach its intrinsic goals, we (the developers) ask ourselves if the
 ** for of this additional element can be derived from first principles
 ** or if it is already determined by the input set driving the operation.
 ** If this is _not_ the case (and the expectation is that most elements
 ** are not fully determined), then we _query for an default._ This pattern
 ** creates a lot of implicit extension points, which are safe to use, since
 ** they are defined on a logical level. We ask for something to fulfil a
 ** given contract.
 ** 
 ** Contrast this to a system, which just has arbitrarily built-in flexibility
 ** by parametrisation variables: such systems are known to be brittle, since
 ** the _parameter values_ are implicitly tied to the behaviour of the operation
 ** through _shared knowledge_ of implementation details. It requires both
 ** "Druid knowledge" and an overall focus on the implementation mechanics to
 ** adjust such parameters. In this respect, a logical query for defaults is
 ** quite different, insofar it forces the implementation to respect abstractions
 ** on a very fine grained level, and it allows to express the actual defaults
 ** by rules which talk the language of the usage context, not the implementation
 ** context. The user shall be able to define generic (logical) rules how the
 ** Session should behave for the given editing project. And by combination
 ** with an additional set of logical consistency rules, a resolution engine
 ** can figure out what would be the right _default element_ to use at a
 ** given point in the implementation.
 ** 
 ** @note as of 2017, we are far from such a system, but it is of uttermost
 **       importance that we build our implementations with this goal in mind
 ** 
 ** # Configuration Query Interface
 ** The [Defaults Manager](\ref lumiera::query::DefsManager) exposes an interface
 ** similar to a database. The intended audience of this interface is the writer
 ** of low-level implementation code. This facade interface is meant to create
 ** a more familiar front-end to an essentially logic and rules driven
 ** configuration system. And, as an additional benefit, it can be implemented
 ** just by a glorified table lookup. Which indeed is what we do until the more
 ** elaborate rules based system is in place.
 ** 
 ** An implementation process in need for some missing link typically knows the
 ** _type of that missing element._ Which means, this type is possibly an abstract
 ** type and defines the contract any solution has to fulfil. Thus, the usage context
 ** can just demand "give me a suitable XYZ!". In practice, there are several flavours
 ** to this interaction, each of which is expressed by a dedicated method on the
 ** DefsManager facade interface:
 ** - we can just *retrieve* a suitable solution element, no questions asked.
 ** - we can *query* for a suitable solution, while limiting the search to what
 **   is already known and defined. This entails the possibility that there is
 **   no known solution yet.
 ** - we can *demand to fabricate* a suitable solution element. Such a call might
 **   still deliver something already fabricated, but the emphasis is on the
 **   "make it so" demand.
 ** - we can *provide and associate* a solution element we created ourselves,
 **   to be available from now on, and associated with a given query. Even
 **   this call implicates the necessity for the query to be fulfilled by
 **   the given element, which, as a consequence includes the possibility
 **   of failure.
 ** - for sake of completeness, we can also require a specific element to
 **   be purged from knowledge
 ** 
 ** ## Fake implementation
 ** As of 2018, the Lumiera project still has to reach the goal of a complete
 ** running engine; we are proceeding with partial integrations for the time being.
 ** And we postpone advanced topics, like integration of an actual rules solver
 ** to future milestones. Meanwhile, we use a [fake implementation](\ref MockConfigRules)
 ** with preconfigured, hard-wired "answers" to some frequently encountered standard queries.
 ** This Fake implementation is configured and instantiated by the [Defaults Manager](\ref DefsManager)
 ** 
 ** @see DefsManager_test
 ** @see DefsManagerImpl_test
 ** 
 */


#ifndef LUMIERA_QUERY_DEFS_MANAGER_H
#define LUMIERA_QUERY_DEFS_MANAGER_H


#include "lib/p.hpp"
#include "lib/nocopy.hpp"
#include "common/query.hpp"

#include <memory>



namespace lumiera{
namespace query  {
  
  
  using lumiera::Query;
  using std::unique_ptr;
  
  namespace impl { class DefsRegistry; }
  
  
  /**
   * Organise a collection of preconfigured default objects.
   * For various kinds of objects we can tweak the default parametrisation
   * as part of the general session configuration. A ref to an instance of
   * this class is accessible through the current session and can be used
   * to fill in parts of the configuration of new objects, if the user 
   * code didn't give more specific parameters. Necessary sub-objects 
   * will be created on demand, and any default configuration, once
   * found, will be remembered and stored with the current session.
   * 
   * @note while the logic of defaults handling can be considered
   *       roughly final, as of 12/09 most of the actual object
   *       handling is placeholder code.
   */
  class DefsManager
    : util::NonCopyable
    {
      unique_ptr<impl::DefsRegistry> defsRegistry_;
      
    public:
      
      DefsManager ()  throw();
     ~DefsManager ();
      
      /** common access point: retrieve the default object fulfilling
       *  some given conditions. May silently trigger object creation.
       *  @throw error::Config in case no solution is possible, which
       *         is considered \e misconfiguration. 
       */
      template<class TAR>
      lib::P<TAR> operator() (Query<TAR> const&);
      
      
      /** search through the registered defaults, never create anything.
       *  @return object fulfilling the query, \c empty ptr if not found. 
       */
      template<class TAR>
      lib::P<TAR> search  (Query<TAR> const&);
      
      /** retrieve an object fulfilling the query and register it as default.
       *  The resolution is delegated to the ConfigQuery system (which may cause
       *  creation of new object instances) 
       *  @return object fulfilling the query, \c empty ptr if no solution.
       */ 
      template<class TAR>
      lib::P<TAR> create  (Query<TAR> const&);
      
      /** register the given object as default, after ensuring it fulfils the
       *  query. The latter may cause some properties of the object to be set,
       *  trigger creation of additional objects, and may fail altogether.
       *  @return true if query was successful and object is registered as default
       *  @note only a weak ref to the object is stored
       */ 
      template<class TAR>
      bool define  (lib::P<TAR> const&, Query<TAR> const&  =Query<TAR>());
      
      /** remove the defaults registration of the given object, if there was such
       *  @return false if nothing has been changed because the object wasn't registered
       */
      template<class TAR>
      bool forget  (lib::P<TAR> const&);
      
      
      /** @internal for session lifecycle */
      void clear();
      
      
// Q: can we have something along the line of...?
//
//        template
//          < class TAR,                   ///< the target to query for 
//            template <class> class SMP  ///<  smart pointer class to wrap the result
//          >
//        SMP<TAR> operator() (const lumiera::Query<TAR>&);

// 12/09: according to my current knowledge of template metaprogramming, the answer is "no",
//        but we could use a traits template to set up a fixed association of smart pointers
//        and kinds of target object. This would allow to define a templated operator() returning
//        the result wrapped into the right holder. But currently I don't see how to build a sensible
//        implementation infrastructure backing such an interface.
//////////TICKET #452
      
    };
  
  
  
}} // namespace lumiera::query
#endif
