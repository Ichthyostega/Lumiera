/*
  FAKE-CONFIGRULES.hpp  -  dummy implementation of the config rules system

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


/** @file fake-configrules.hpp
 ** Mock/Test/Debugging Implementation of the config rules system.
 ** Instead of actually parsing/analysing/resolving queries, this implementation
 ** uses a Table of hard wired queries together with preconfigured object instances
 ** as answer values. As of 1/2008 it is used to "keep the implementation work going"
 ** -- later on, when we use a real Prolog interpreter, it still may be useful for
 ** testing and debugging.
 ** 
 ** @todo to be removed in Alpha, when integrating a real resolution engine /////////////////TICKET #710
 ** 
 ** @see lumiera::Query
 ** @see lumiera::ConfigRules
 **
 */


#ifndef LUMIERA_FAKECONFIGRULES_H
#define LUMIERA_FAKECONFIGRULES_H

#include "proc/mobject/session.hpp"
#include "common/configrules.hpp"
#include "lib/util.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/any.hpp>
#include <string>
#include <map>



namespace lumiera {
  namespace query {
    
    namespace asset = proc::asset;
    
    using asset::Pipe;
    using asset::ProcPatt;
    using asset::PProcPatt;
    using proc::mobject::Session;
    using lib::meta::InstantiateChained;
    
    using util::isnil;
    
    using boost::any;
    using boost::any_cast;
    
    
    
    
    
    namespace { // internal details
      
      /** a traits-class to define the smart-ptr to wrap the result */
      template<class TY>
      struct WrapReturn             { typedef P<TY> Wrapper;  };
      
      template<>
      struct WrapReturn<ProcPatt>   { typedef PProcPatt Wrapper;  };
      
      
      /** helper detecting if a query actually intended to retrieve a "default" object.
       *  This implementation is quite crude, of course it would be necessary actually to
       *  parse and evaluate the query. @note query is modified if "default" ... */
      inline bool
      is_defaults_query (string& query)
      {
        return !isnil (removeTerm ("default", query));
      }
      
    } // details (end)
    
    
    
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
        template<class TY> 
        bool detect_case (typename WrapReturn<TY>::Wrapper&, Query<TY>& q);
        bool fabricate_matching_new_Pipe (Query<Pipe>& q, string const& pipeID, string const& streamID);
        bool fabricate_just_new_Pipe     (Query<Pipe>& q);
        bool fabricate_ProcPatt_on_demand (Query<const ProcPatt>& q);
        bool fabricate_Timeline_on_demand (Query<asset::Timeline>& q);
        bool fabricate_Sequence_on_demand (Query<asset::Sequence>& q);
        
        template<class TY>
        bool set_new_mock_solution (Query<TY>& q, typename WrapReturn<TY>::Wrapper& candidate);

        
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
        resolve (Ret& solution, Query<TY> const& q)
          {
            const any& entry = this->fetch_from_table_for (q.asKey());
            if (!isnil (entry))
              {
                Ret const& candidate (any_cast<Ret const&> (entry));
                if (! solution
                   ||(solution &&  solution == candidate)    // simulates a real unification
                   )
                  return solution = candidate;
              }
            return try_special_case(solution, q);
          }
      
      private:
        bool
        try_special_case (Ret& solution, Query<TY> const& q)
          {
            if (solution && isFakeBypass(q))       // backdoor for tests
              return solution;
            
            Query<TY> newQuery = q;
            if (is_defaults_query (newQuery))  // modified query..
              return solution = Session::current->defaults (newQuery);
                                             //   may cause recursion
            if (this->detect_case (solution, newQuery))
              return resolve (solution, newQuery);
            
            return solution = Ret();     // fail: return default-constructed empty smart ptr
          }
      };
    
    
    /** Hook for treating very special cases for individual types only */
    template<class TY>
    inline bool 
    MockTable::detect_case (typename WrapReturn<TY>::Wrapper&, Query<TY>& q)
    {
      q.clear(); // end recursion
      return false;
    }
    template<>
    inline bool 
    MockTable::detect_case (WrapReturn<Pipe>::Wrapper& candidate, Query<Pipe>& q)
    {
      if (!isnil (extractID("make", q)))
        // used by tests to force fabrication of a new "solution"
        return fabricate_just_new_Pipe (q);
      
      const string pipeID   = extractID("pipe", q);
      const string streamID = extractID("stream", q);
      
      if (candidate && pipeID == candidate->getPipeID())
        return set_new_mock_solution (q, candidate); // "learn" this solution to be "valid"
      
      if (!isnil(pipeID) && !isnil(streamID))
        return fabricate_matching_new_Pipe (q, pipeID, streamID);

      if (!candidate && (!isnil(streamID) || !isnil(pipeID)))
        return fabricate_just_new_Pipe (q);
      
      q.clear();
      return false;
    }
    template<>
    inline bool 
    MockTable::detect_case (WrapReturn<const ProcPatt>::Wrapper& candidate, Query<const ProcPatt>& q)
    {
      const string streamID = extractID("stream", q);
      
      if (!candidate && !isnil(streamID))
          return fabricate_ProcPatt_on_demand (q);
      
      q.clear();
      return false;
    }
    template<>
    inline bool 
    MockTable::detect_case (WrapReturn<asset::Timeline>::Wrapper& candidate, Query<asset::Timeline>& q)
    {
      if (!candidate)
          return fabricate_Timeline_on_demand (q);
      
      q.clear();
      return bool(candidate);
    }
    template<>
    inline bool 
    MockTable::detect_case (WrapReturn<asset::Sequence>::Wrapper& candidate, Query<asset::Sequence>& q)
    {
      if (!candidate)
          return fabricate_Sequence_on_demand (q);
      
      q.clear();
      return bool(candidate);
    }
    
    
    
    
    
    
    /** 
     * Facade: Dummy Implementation of the query interface.
     * Provides an explicit implementation using hard wired
     * values for some types of interest for testing and debugging.
     */
    class MockConfigRules 
      : public InstantiateChained < InterfaceTypes           
                                  , LookupPreconfigured  // building block used for each of the types
                                  , MockTable           //  for implementing the base class (interface) 
                                  >
      {
      protected:
        MockConfigRules ();                                   ///< to be used only by the singleton factory
        friend class lib::singleton::StaticCreate<MockConfigRules>;

        virtual ~MockConfigRules() {}
        
      public:
        
        // TODO: implementation of any additional functions on the ConfigRules interface goes here
      };
      
    
    
  
}} // namespace lumiera::query
#endif
