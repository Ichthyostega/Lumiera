/*
  CONFIG-RULES.hpp  -  interface for rule based configuration

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


/** @file config-rules.hpp
 ** Interface for accessing rule based configuration.
 ** By using the Query template, you can pose a query in prolog syntax and get some
 ** existing or newly created object fulfilling the requested predicates. The actual 
 ** implementation will be hidden behind a `instance` (Singleton factory). As of 1/2008,
 ** it is _planned_ to use an embedded YAP Prolog system at some point in the future,
 ** for now we use a [mock implementation](\ref fake-configrules.hpp) based on lookup in
 ** a hard-wired, preconfigured Map.
 ** 
 ** Fully implementing this facility would require the participating objects to register capabilities
 ** they want to provide, together with functors carrying out the necessary configuration steps.
 ** All details and consequences of this approach still have to be worked out...
 ** 
 ** \par relation to Query and QueryResolver
 ** The ConfigRules resolver is just a special kind of QueryResolver, able to handle specific kinds
 ** of queries. Clients using the ConfigRules directly get a more easy to use point-and-shot style
 ** interface, allowing just to retrieve some _suitable solution_, instead of having to iterate
 ** through a result set.
 ** 
 ** @todo right now (12/2012) the above paragraph is a lie.
 **       ConfigQuery is way older than QueryResolver and will be retrofitted step by step.
 **       Not much of a problem, since the currently utilised mock implementation isn't able to
 **       deal with a real query anyway.
 **
 ** @note this is rather a concept draft and left as such for now... don't take this code too literal!
 ** @todo clarify the relation of config query and query-for-defaults   ///////////////TICKET #705
 ** @todo as of 11/2016 the situation is basically the same: this is placeholder code
 **       and just implemented enough to keep us going without violating the architecture vision
 **
 ** @see lumiera::Query
 ** @see mobject::session::DefsManager
 ** @see asset::StructFactory 
 ** @see config-resolver.hpp specialised setup for the Steam-Layer
 ** @see fake-configrules.hpp currently used dummy-implementation
 **
 */


#ifndef LUMIERA_CONFIG_RULES_H
#define LUMIERA_CONFIG_RULES_H

#include "lib/p.hpp"
#include "lib/symbol.hpp"
#include "lib/meta/generator.hpp"
#include "common/query.hpp"

#include <string>



namespace lumiera {
  
  using std::string;
  using lib::P;
  
  
  /////////////////////////////////////////////////////////////////////TICKET #705 this is draft/preview code; a real resolution system needs to be integrated
  
  namespace query {
    
    // The intention is to support the following style of Prolog code
    //
    //  resolve(O, Cap) :- find(O), capabilities(Cap).
    //  resolve(O, Cap) :- make(O), capabilities(Cap).
    //  capabilities(Q) :- call(Q).
    //  
    //  stream(T, mpeg) :- type(T, fork), type(P, pipe), resolve(P, stream(P,mpeg)), placed_to(P, T).
    //
    // The type guard is inserted automatically, while the predicate implementations for
    // find/1, make/1, stream/2, and placed_to/2 are to be provided by the target types.
    //
    // As a example, the goal ":-retrieve(T, stream(T,mpeg))." would search a Fork object (a "track"), try to
    // retrieve a pipe object with stream-type=mpeg and associate the Fork with this Pipe. The
    // predicate "stream(P,mpeg)" needs to be implemented (natively) for the pipe object.
    
    class Resolver
      {
        ///////////////////////////////////////////////////////////////TICKET #705 a real resolution system needs to be integrated
      };
    
    
    using lib::Symbol;
    using lib::Literal;
    using lumiera::Query;
    
/** placeholder definition for later.
 *  @todo intention is to integrate with lib::Symbol
 */
#define SYMBOL uint
    
    
    template
      < SYMBOL SYM,                  // Predicate symbol
        typename SIG = bool(string) //  Signature
      >
    class Pred
      { };
      
    /**
     * the "back side" interface towards the classes participating
     * in the configuration system (the config system will be
     * delivering instances of these classes for a given query). 
     * This one currently is just brainstorming. The idea is that
     * a participating class would provide such  and TypeHandler
     * implementing the predicates which make sense for this special
     * type of object. Registering  such a TypeHandler should create
     * the necessary handler functions to be installed into 
     * the Prolog system.
     * @deprecated it can't be done exactly this way, but I leave it in the
     *       current shape as a reminder for later, to show the intention...
     * @todo 6/2010 unify this with the TypeHandler in typed-id.hpp
     */  
    template<class TY>
    class TypeHandler
      {
        static const TY NIL;
        
        template<SYMBOL SYM, typename SIG>
        TY find (Pred<SYM,SIG> capability);
        
        template<SYMBOL SYM, typename SIG>
        TY make (Pred<SYM,SIG> capability, TY& refObj =NIL);
      };
    
    /** 
     * the "front side" interface: the Steam-Layer code can
     * use this QueryHandler to retrieve instances of the
     * type TY fulfilling the given Query. To start with,
     * we use a mock implementation. 
     * (this code works and is already used 2/2008)
     * @todo retrofit this to install and use a QueryResolver
     * @see lumiera::query::LookupPreconfigured
     * @see lumiera::query::MockTable
     */
    template<class TY>
    class QueryHandler
      {
      protected:
        virtual ~QueryHandler()  { }
      public:
        /** try to find or create an object of type TY 
         *  fulfilling the given query.
         *  @param solution object fulfilling the query. Will be bound or
         *         unified (in case it's already bound) with the first solution.
         *  @query any goals to be fulfilled by the solution.
         *  @return false if resolution failed. In this case, solution ptr is empty.
         */
        virtual bool resolve (P<TY>& solution, Query<TY> const& q) = 0;
      };
    
    // TODO: the Idea is to provide specialisations for the concrete types
    //       we want to participate in the ConfigRules system....
    //       Thus we get the possibility to create a specific return type,
    //       e.g. return a P<Pipe> but a Placement<Fork>, using the appropriate factory.
    //       Of course then the definitions need to be split up in separate headers.
      
      
      
      
      
    /** 
     * Generic query interface for retrieving objects matching
     * some capability query. To be instantiated using a typelist,
     * thus inheriting from the Handler classes for each type. In
     * the (future) version using YAP Prolog, this will drive the
     * generation and registration of the necessary predicate 
     * implementations for each concrete type, using the specialisations
     * given alongside with the types. For now it just serves to generate
     * the necessary resolve(Query<TY>) virtual functions (implemented
     * by MockConfigRules) 
     */
    template<typename TYPES>
    class ConfigRules
      : public lib::meta::InstantiateForEach<TYPES, QueryHandler>
      {
      protected:
        ConfigRules ()         {}
        virtual ~ConfigRules() {} 
        
      public:
        /** roll back to a pristine yet operational state.
         *  Discards all information collected through use */
        virtual void reset()   =0;
        
        // TODO: find out what operations we need to support here for the »real solution« (using Prolog)
      };
      
      
    
    
    LUMIERA_ERROR_DECLARE (CAPABILITY_QUERY);  ///< unresolvable capability query.
  
  } // namespace query
  
  
  
  
  
  
  
  namespace query {
    
    /** backdoor for tests: the next config query with this query string
     *  will magically succeed with every candidate object provided. This
     *  is currently necessary to get objects into the defaults manager,
     *  as the query system is not able to do real query resolution */
    void setFakeBypass(lumiera::QueryKey const& q);
    bool isFakeBypass (lumiera::QueryKey const& q);
    /////////////////////////////////////////////////////////////////////////////TICKET 710
    
  } // namespace query
  
} // namespace lumiera
#endif
