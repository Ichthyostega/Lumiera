/*
  CONFIGRULES.hpp  -  interface for rule based configuration

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


/** @file configrules.hpp
 ** Interface for accessing rule based configuration.
 ** By using the Query template, you can pose a query in prolog syntax and get some
 ** existing or newly created object fulfilling the requested predicates. The actual 
 ** implementation is hidden behind the #instance (Singleton factory). As of 1/2008, 
 ** it is \e planned to use an embedded YAP Prolog system at some point in the future,
 ** for now we use a \link MockConfigRules mock implementation \endlink employing a
 ** preconfigured Map.
 **
 ** Fully implementing this facility would require the participating objects to register capabilities
 ** they want to provide, together with functors carrying out the necessary configuration steps.
 ** All details and consequences of this approach still have to be worked out...
 **
 ** @note this is rather a scrapbook and in flux... don't take this code too literal!
 ** @todo clarify the relation of config query and query-for-defaults   ///////////////TICKET #705
 **
 ** @see lumiera::Query
 ** @see mobject::session::DefsManager
 ** @see asset::StructFactory 
 ** @see fake-configrules.hpp currently used dummy-implementation
 **
 */


#ifndef LUMIERA_CONFIGRULES_H
#define LUMIERA_CONFIGRULES_H

#include "lib/p.hpp"
#include "lib/query.hpp"
#include "lib/symbol.hpp"
#include "lib/meta/generator.hpp"
#include "lib/singleton-subclass.hpp"

//TODO: is it sensible to bring in the types explicitly here? (it's not necessary, but may be convenient...)
#include "proc/mobject/session/track.hpp"
#include "proc/asset/procpatt.hpp"
#include "proc/asset/pipe.hpp"
#include "proc/asset/timeline.hpp"
#include "proc/asset/sequence.hpp"

#include <string>



namespace lumiera {  ///////TODO: shouldn't that be namespace lib? or proc?
  
  using std::string;
  using lumiera::P;
  
  
  
  
  namespace query {
    
    // The intention is to support the following style of Prolog code
    //
    //  resolve(O, Cap) :- find(O), capabilities(Cap).
    //  resolve(O, Cap) :- make(O), capabilities(Cap).
    //  capabilities(Q) :- call(Q).
    //  
    //  stream(T, mpeg) :- type(T, track), type(P, pipe), resolve(P, stream(P,mpeg)), placed_to(P, T).
    //
    // The type guard is inserted automatically, while the predicate implementations for
    // find/1, make/1, stream/2, and placed_to/2 are to be provided by the target types.
    //
    // As a example, the goal ":-retrieve(T, stream(T,mpeg))." would search a Track object, try to
    // retrieve a pipe object with stream-type=mpeg and associate the track with this Pipe. The
    // predicate "stream(P,mpeg)" needs to be implemented (natively) for the pipe object.
    
    class Resolver
      {
        
      };
    
    
    using lib::Symbol;
    using lib::Literal;
    
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
     * @todo it can't be done exactly this way, but I leave it in
     *       as a reminder for later, to show the intention
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
     * the "front side" interface: the Proc-Layer code can
     * use this QueryHandler to retrieve instances of the
     * type TY fulfilling the given Query. To start with,
     * we use a mock implementation. 
     * (this code works and is already used 2/2008)
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
        virtual bool resolve (P<TY>& solution, const Query<TY>& q) = 0;
      };
    
    // TODO: the Idea is to provide specialisations for the concrete types
    //       we want to participate in the ConfigRules system....
    //       Thus we get the possibility to create a specific return type,
    //       e.g. return a P<Pipe> but a Placement<Track>, using the appropriate factory.
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
      : public typelist::InstantiateForEach<TYPES, QueryHandler>
      {
      protected:
        ConfigRules ()         {}
        virtual ~ConfigRules() {} 
        
      public:
        // TODO: find out what operations we need to support here for the »real solution« (using Prolog)
      };
      
      
    
    
    LUMIERA_ERROR_DECLARE (CAPABILITY_QUERY);  ///< unresolvable capability query.
  
  } // namespace query
  
  
  
  
  
  /* ============= global configuration ==================== */
  
  /** 
   *  the list of all concrete types participating in the
   *  rule based config query system
   */
  typedef lumiera::typelist::Types < mobject::session::Track
                                   , asset::Pipe
                                   , const asset::ProcPatt
                                   , asset::Timeline
                                   , asset::Sequence
                                   > ::List
                                     InterfaceTypes;
  
  /** 
   * user-visible Interface to the ConfigRules subsystem.
   * Configured as Singleton (with hidden Implementation class)
   */
  class ConfigRules
    : public query::ConfigRules<InterfaceTypes>
    {
  
    public:
      static lib::SingletonSub<ConfigRules> instance;
      
    };
  
  
  
  
  namespace query {
    
    /** backdoor for tests: the next config query with this query string
     *  will magically succeed with every candidate object provided. This
     *  is currently necessary to get objects into the defaults manager,
     *  as the query system is not able to do real query resolution */
    void setFakeBypass(string const& q);
    bool isFakeBypass (string const& q);
    /////////////////////////////////////////////////////////////////////////////TICKET 710
    
  } // namespace query
  
} // namespace lumiera
#endif
