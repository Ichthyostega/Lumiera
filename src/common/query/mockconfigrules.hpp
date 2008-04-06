/*
  MOCKCONFIGRULES.hpp  -  mock implementation of the config rules system
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file mockconfigrules.hpp
 ** Mock/Test/Debugging Implementation of the config rules system.
 ** Instead of actually parsing/analyzing/resolving queries, this implementation
 ** uses a Table of hard wired queries together with preconfigured object instances
 ** as answer values. As of 1/2008 it is used to "keep the implementation work going"
 ** -- later on, when we use a real Prolog interpreter, it still may be useful for
 ** testing and debugging.
 **
 ** @see lumiera::Query
 ** @see lumiera::ConfigRules
 **
 */


#ifndef LUMIERA_MOCKCONFIGRULES_H
#define LUMIERA_MOCKCONFIGRULES_H

#include "proc/mobject/session.hpp"
#include "common/configrules.hpp"
#include "common/util.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/any.hpp>
#include <string>
#include <map>



namespace lumiera
  {
  
  
  namespace query
    {
    using asset::Pipe;
    using asset::ProcPatt;
    using asset::PProcPatt;
    using mobject::Session;
    
    using util::isnil;
    
    using boost::any;
    using boost::any_cast;
    
    
    
    /** a traits-class to define the smart-ptr to wrap the result */
    template<class TY>
    struct WrapReturn             { typedef shared_ptr<TY> Wrapper;  };
    
    template<>
    struct WrapReturn<ProcPatt>   { typedef PProcPatt Wrapper;  };
    
    
    /** 
     * the actual table holding preconfigured answers
     * packaged as boost::any objects. 
     */
    class MockTable : public lumiera::ConfigRules
      {
        typedef std::map<string,any> Tab;
        typedef boost::scoped_ptr<Tab> PTab;
        
        PTab answer_;
        bool isInit_;
        
      protected:
        MockTable ();
        const any& fetch_from_table_for (const string& queryStr);
        
        // special cases....
        bool fabricate_matching_new_Pipe (Query<Pipe>& q, string const& pipeID, string const& streamID);
        bool fabricate_ProcPatt_on_demand (Query<const ProcPatt>& q, string const& streamID);

        
      private:
        void fill_mock_table ();
      };
    
    
    /** 
     * building block defining how to do 
     * the mock implementation for \e one type.
     * We simply access a table holding pre-created objects.
     */
    template<class TY, class BASE>
    class LookupPreconfigured : public BASE
      {
        typedef typename WrapReturn<TY>::Wrapper Ret;
        
      public:
        /** (dummy) implementation of the QueryHandler interface */
        virtual bool 
        resolve (Ret& solution, const Query<TY>& q)
          {
            const any& entry = fetch_from_table_for (q.asKey());
            if (!isnil (entry))
              {
                const Ret& candidate (any_cast<const Ret&> (entry));
                if (! solution
                   ||(solution &&  solution == candidate)    // simulates a real unification
                   )
                  return solution = candidate;
              }
            return try_special_case(solution, q);
          }
      
      private:
        bool
        try_special_case (Ret& solution, const Query<TY>& q)
          {
            Query<TY> newQuery = q;
            if (is_defaults_query (q))  // modified query..
              return solution = Session::current->defaults (newQuery);
                                      //   may cause recursion
            if (detect_case (newQuery))
              return resolve (solution, newQuery);
            
            return solution = Ret();  
                                // fail: return default-constructed empty smart ptr
          }
        
        bool
        detect_case (Query<TY>& q);
      };
    
    
    /** Hook for treating very special cases for individual types only */
    template<class TY, class BASE>
    inline bool 
    LookupPreconfigured<TY,BASE>::detect_case (Query<TY>& q)
    {
      q.clear(); // end recursion
      return false;
    }
    template<class BASE>
    inline bool 
    LookupPreconfigured<Pipe,BASE>::detect_case (Query<Pipe>& q)
    {
      const string pipeID   = extractID("pipe", q);
      const string streamID = extractID("stream", q);
      if (!isnil(pipeID) && !isnil(streamID))
          return fabricate_matching_new_Pipe (q, pipeID, streamID);
      
      q.clear();
      return false;
    }
    template<>
    inline bool 
    LookupPreconfigured<const ProcPatt>::detect_case (Query<const ProcPatt>& q)
    {
      const string streamID = extractID("stream", q);
      if (!isnil(streamID))
          return fabricate_ProcPatt_on_demand (q, streamID);
      
      // note: we don't handle the case of "make(PP), capabilities....." specially
      // because either someone puts a special object into the mock table, or the
      // recursive query done by the StructFactory simply fails, resulting in
      // the StructFactory issuing a ProcPatt ctor call.
      
      q.clear();
      return false;
    }
    
    
    
    
    
    
    /** 
     * Facade: Dummy Implementation of the query interface.
     * Provides an explicit implementation using hard wired
     * values for some types of interest for testing and debugging.
     */
    class MockConfigRules 
      : public typelist::InstantiateChained < InterfaceTypes           
                                            , LookupPreconfigured  // building block used for each of the types
                                            , MockTable           //  for implementing the base class (interface) 
                                            >
      {
      protected:
        MockConfigRules ();                                   ///< to be used only by the singleton factory
        friend class lumiera::singleton::StaticCreate<MockConfigRules>;

        virtual ~MockConfigRules() {}
        
      public:
        
        // TODO: implementation of any additional functions on the ConfigRules inteface goes here
      };
      
    
    
  
  } // namespace query
    
} // namespace lumiera
#endif
