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

#include <string>



namespace cinelerra
  {
  using std::string;
  namespace query { class MockConfigRules; }
  
  
  /** 
   * Generic query interface for retrieving objects matching
   * some capability query
   */
  class ConfigRules
    {
    protected:
      ConfigRules ()         {}
      virtual ~ConfigRules() {} 

    public:
      static Singleton<query::MockConfigRules> instance;
      
      // TODO: find out what operations we need to support here for the »real solution« (using Prolog)
    };

    
  namespace query
    {
    // The intention is to support the following style of Prolog code
    //
    // retrieve(T) :- type(T, track), find(T), capabilities(T).
    // retrieve(T) :- type(T, track), make(T), capabilities(T).
    //
    // capabilities(T) :- stream(T,mpeg).
    // stream(T, mpeg) :- type(T, track), type(P, port), retrieve(P), place_to(P, T).
    //
    // The type guard is inserted auomatically, while the predicate implementations for
    // find/1, make/1, stream/2, and place_to/2 are to be provided by the target types.
    
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
        TY resolve (Query<TY> q);
      };
    
    
  
  } // namespace query
    
} // namespace cinelerra
#endif
