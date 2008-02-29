/*
  CONFIGRULES.hpp  -  interface for rule based configuration
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
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
 ** they want to provide, together with functors carrying out the neccessary configuration steps.
 ** All details and consequences of this approach still have to be worked out...
 **
 ** @note this is rather a scrapbook and in flux... don't take this code too literal!
 **
 ** @see cinelerra::Query
 ** @see mobject::session::DefsManager
 ** @see asset::StructFactory 
 **
 */


#ifndef CINELERRA_CONFIGRULES_H
#define CINELERRA_CONFIGRULES_H

#include "common/query.hpp"
#include "common/typelistutil.hpp"
#include "common/singletonsubclass.hpp"

//TODO: is it sensible to bring in the types explicitly here? (it's not necessary, but may be convenient...)
#include "proc/mobject/session/track.hpp"
#include "proc/asset/procpatt.hpp"
#include "proc/asset/pipe.hpp"
#include "proc/asset/track.hpp"

#include <string>
#include <tr1/memory>



namespace cinelerra
  {
  using std::string;
  using std::tr1::shared_ptr;

  
  
    
  namespace query
    {
    // The intention is to support the following style of Prolog code
    //
    //  resolve(O, Cap) :- find(O), capabilities(Cap).
    //  resolve(O, Cap) :- make(O), capabilities(Cap).
    //  capabilities(Q) :- call(Q).
    //  
    //  stream(T, mpeg) :- type(T, track), type(P, pipe), resolve(P, stream(P,mpeg)), place_to(P, T).
    //
    // The type guard is inserted auomatically, while the predicate implementations for
    // find/1, make/1, stream/2, and place_to/2 are to be provided by the target types.
    //
    // As a example, the goal ":-retrieve(T, stream(T,mpeg))." would search a Track object, try to
    // retrieve a pipe object with stream-type=mpeg and associate the track with this Pipe. The
    // predicate "stream(P,mpeg)" needs to be implemented (natively) for the pipe object.
    
    class Resolver
      {
        
      };
    
    
    template
      < const Symbol SYM,            // Predicate symbol
        typename SIG = bool(string) //  Signature
      >
    class Pred
      {
        
      };
      
    /**
     * the "backside" interface towards the classes participating
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
        
        template<Symbol SYM, typename SIG>
        TY find (Pred<SYM,SIG> capability);
        
        template<Symbol SYM, typename SIG>
        TY make (Pred<SYM,SIG> capability, TY& refObj =NIL);
      };
    
    /** 
     * the "frontside" interface: the Proc-Layer code can
     * use this QueryHandler to retrieve instances of the
     * type TY fulfilling the given Query. To start with,
     * we use a mock implementation. 
     * (this code works and is already used 2/2008)
     * @see cinelerra::query::LookupPreconfigured
     * @see cinelerra::query::MockTable
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
        virtual bool resolve (shared_ptr<TY> solution, const Query<TY>& q) = 0;
      };

    // TODO: the Idea is to provide specialisations for the concrete types
    //       we want to participate in the ConfigRules system....
    //       Thus we get the possibility to create a specific return type,
    //       e.g. return a shared_ptr<Pipe> but a Placement<Track>, using the appropriate factory.
    //       Of course then the definitions need to be split up in separate headers.
      
      
      
      
      
    /** 
     * Generic query interface for retrieving objects matching
     * some capability query. To be instantiated using a typelist,
     * thus inheriting from the Handler classes for each type. In
     * the (future) version using YAP Prolog, this will drive the
     * generation and registration of the necessary predicate 
     * implementations for each concrete type, using the speicalisations
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
      

    
    
    CINELERRA_ERROR_DECLARE (CAPABILITY_QUERY);  ///< unresolvable capability query.
  
  } // namespace query
  
  
  
  
  
  /* ============= global configuration ==================== */
  
  /** 
   *  the list of all concrete types participating in the
   *  rule based config query system
   */
  typedef cinelerra::typelist::Types < mobject::session::Track
                                     , asset::Track
                                     , asset::Pipe
                                     , const asset::ProcPatt
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
      static SingletonSub<ConfigRules> instance;
      
    };
  
  
} // namespace cinelerra
#endif
