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
 ** it is \i planned to use an embedded YAP Prolog system at some point in the future,
 ** for now we use a \link MockConfigRules mock implementation \endlink employing a
 ** preconfigured Map.
 **
 ** Fully implementing this facility would require the participating objects to register capabilities
 ** they want to provide, together with functors carrying out the neccessary configuration steps.
 ** All details and consequences of this approach still have to be worked out...
 **
 ** @see cinelerra::Query
 ** @see mobject::session::DefsManager
 ** @see asset::StructFactory 
 **
 */


#ifndef CINELERRA_CONFIGRULES_H
#define CINELERRA_CONFIGRULES_H

#include "common/query.hpp"
#include "common/singleton.hpp"
#include "common/typelist.hpp"

#include "proc/mobject/session/track.hpp"
#include "proc/asset/port.hpp"


#include <string>



namespace cinelerra
  {
  using std::string;
  namespace query { class MockConfigRules; }
  
  
    
  namespace query
    {
    // The intention is to support the following style of Prolog code
    //
    // retrieve(O, Cap) :- find(T), capabilities(Cap).
    // retrieve(O, Cap) :- make(T), capabilities(Cap).
    // capabilities(Q) :- call(Q).
    //
    // stream(T, mpeg) :- type(T, track), type(P, port), retrieve(P, stream(P,mpeg)), place_to(P, T).
    //
    // The type guard is inserted auomatically, while the predicate implementations for
    // find/1, make/1, stream/2, and place_to/2 are to be provided by the target types.
    //
    // As a example, the goal ":-retrieve(T, stream(T,mpeg))." would search a Track object, try to
    // retrieve a port object with stream-type=mpeg and associate the track with this Port. The
    // predicate "stream(P,mpeg)" needs to be implemented (natively) for the port object.
    
    class Resolver
      {
        
      };
      
    typedef const char * const Symbol;  
      
    template
      < const Symbol SYM,            // Predicate symbol
        typename SIG = bool(string) //  Signature
      >
    class Pred
      {
        
      };
      
    template<class TY>
    class TypeHandler
      {
        static const TY NIL;
        
        template<Symbol SYM, typename SIG>
        TY find (Pred<SYM,SIG> capability);
        
        template<Symbol SYM, typename SIG>
        TY make (Pred<SYM,SIG> capability, TY& refObj =NIL);
      };
      
    template<class TY>
    class QueryHandler
      {
      protected:
        virtual ~QueryHandler();
      public:
        virtual TY resolve (Query<TY> q);
      };

    // TODO: the Idea is to provide specialisations for the concrete types
    //       we want to participate in the ConfigRules system....
    //       Thus we get the possibility to create a specific return type,
    //       e.g. return a shared_ptr<Port> but a Placement<Track>, using the appropriate factory.
    //       Of course then the definitions need to be split up in separate headers.
      
      
      
      
    using cinelerra::typelist::Node;
    using cinelerra::typelist::NullType;
      
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
    template<class TYPES>
    class ConfigRulesInterface;
      
    template<>
    class ConfigRulesInterface<NullType> 
      {
      protected:
        virtual ~ConfigRulesInterface();
      };
      
    template<class TY, typename TYPES>
    class ConfigRulesInterface<Node<TY, TYPES> > 
      : public QueryHandler<TY>,
        public ConfigRulesInterface<TYPES>
      { };
      
    
    template
      < typename TYPES,
        class IMPL
      >
    class ConfigRules
      : public ConfigRulesInterface<typename TYPES::List> 
      {
      protected:
        ConfigRules ()         {}
        virtual ~ConfigRules() {} 
  
      public:
        static cinelerra::Singleton<IMPL> instance;
        
        // TODO: find out what operations we need to support here for the »real solution« (using Prolog)
      };
      
    /** storage for the Singleton instance factory */
    template<typename TYPES, class IMPL>
    cinelerra::Singleton<IMPL> ConfigRules<TYPES,IMPL>::instance; 

    
  
  } // namespace query
  
  
  
  
  
  /* ============= global configuration ==================== */
  
  /** 
   *  the list of all concrete types participating in the
   *  rule based config query system
   */
  typedef cinelerra::typelist::Types < mobject::session::Track
                                     , asset::Port
                                     >
                                       InterfaceTypes;
  
  typedef query::ConfigRules< InterfaceTypes,          // List of Types to generate interface functions
                              query::MockConfigRules  //  actual Implementation to use
                            > 
                              ConfigRules;          //    user-visible Interface to the ConfigRules subsystem.
  
  
} // namespace cinelerra
#endif
