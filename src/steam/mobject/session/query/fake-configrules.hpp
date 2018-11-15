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
 ** @remarks the primary purpose of this header and fake-configrules.cpp is to define
 **          the type specialisations of the \c QueryHandler<TY>::resolve(solution,query)
 **          function(s). Below, there is a really confusing and ugly ping-pong game involving
 **          the faked solutions and the mocked defaults manager. This is spaghetti code, written
 **          for the reason everyone writes spaghetti code: to get away with it. So please look
 **          away, some day the real thing will be there, displacing this mess without further notice.
 ** @todo to be removed in Alpha, when integrating a real resolution engine /////////////////TICKET #710
 ** 
 ** @see lumiera::Query
 ** @see lumiera::ConfigRules
 **
 */


#ifndef MOBJECT_SESSION_QUERY_FAKECONFIGRULES_H
#define MOBJECT_SESSION_QUERY_FAKECONFIGRULES_H

#include "steam/mobject/session.hpp"
#include "steam/config-resolver.hpp"
#include "lib/depend-inject.hpp"
#include "lib/query-util.hpp"
#include "lib/util.hpp"
#include "lib/p.hpp"

#include <boost/any.hpp>
#include <string>
#include <map>



namespace steam {
namespace mobject {
namespace session {
  namespace query {
    
    namespace asset = proc::asset;
    
    using asset::Pipe;
    using asset::ProcPatt;
    using asset::PProcPatt;
    using proc::mobject::Session;
    using lib::meta::InstantiateChained;
    using lib::P;
    
    using lumiera::Query;
    using lumiera::QueryKey;
    using lumiera::query::isFakeBypass;      /////////TODO placeholder until there is a real resolution engine
    
    using util::contains;
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
      template<typename TY>
      inline bool
      is_defaults_query (Query<TY> const& querySpec)
      {
        return querySpec.usesPredicate ("default");
      }
      
      template<typename WRA>
      inline bool
      exists(WRA const& solution)
      {
        return bool(solution);
      }
      
    } // details (end)
    
    
    
    /** 
     * the actual table holding preconfigured answers
     * packaged as boost::any objects. MockTable is the implementation base;
     * subclasses for the individual types are layered below to define the
     * \c resolve(..) functions. Finally #MockConfigRules wraps things up.
     * 
     * The implementation relies on boost::any records to stash the objects
     * in automatically managed heap memory. 
     */
    class MockTable
      : public proc::ConfigResolver
      {
        typedef std::map<QueryKey,any> Tab;
        
        Tab  answer_;
        bool isInit_;
        
      protected:
        MockTable ();
        virtual void reset();
        any const& fetch_from_table_for (QueryKey const& query);
        
        // special cases....
        template<class TY> 
        bool detect_case (typename WrapReturn<TY>::Wrapper&, Query<TY> const&);
        bool fabricate_matching_new_Pipe  (Query<Pipe> const& q, string const& pipeID, string const& streamID);
        bool fabricate_just_new_Pipe      (Query<Pipe> const& q);
        bool fabricate_ProcPatt_on_demand (Query<const ProcPatt> const& q);
        bool fabricate_Timeline_on_demand (Query<asset::Timeline> const& q);
        bool fabricate_Sequence_on_demand (Query<asset::Sequence> const& q);
        
        template<class TY>
        bool set_new_mock_solution (Query<TY> const& q, typename WrapReturn<TY>::Wrapper& candidate);

        
      private:
        void fill_mock_table ();
        
        /** shortcut for simply accessing a table entry */ 
        template<class STRU>
        any&
        item (string const& querySpec)
        {
          return answer_[Query<STRU>(querySpec)];
        }
      };
    
    
    /** 
     * building block providing the 
     * mock implementation for a \em single type.
     * We simply access a table holding pre-created objects.
     */
    template<class TY, class BASE>
    class LookupPreconfigured : public BASE
      {
        typedef typename WrapReturn<TY>::Wrapper Ret;
        
        
        /** (dummy) implementation of the QueryHandler interface */
        virtual bool 
        resolve (Ret& solution, Query<TY> const& q)
          {
            any const& entry = this->fetch_from_table_for(q);
            if (!isnil (entry))
              {
                Ret const& candidate (any_cast<Ret const&> (entry));
                if (! solution
                   ||(solution &&  solution == candidate)      // simulates a real unification
                   )
                  return exists (solution = candidate);
              }
            return try_special_case(solution, q);
          }
      
      private:
        bool
        try_special_case (Ret& solution, Query<TY> const& q)
          {
            if (solution && isFakeBypass(q))        // backdoor for tests
              return exists (solution);
            
            if (is_defaults_query (q))
              {
                Query<TY> defaultsQuery = q.rebuild().removeTerm("default");
                return exists (solution = Session::current->defaults (defaultsQuery));
              }                             //  may lead to recursion
                                            
            if (this->detect_case (solution, q))
              return resolve (solution, q);
            
            return exists (solution = Ret());  // fail: return default-constructed empty smart ptr
          }
      };
    
    
    /** Hook for treating very special cases for individual types only */
    template<class TY>
    inline bool 
    MockTable::detect_case (typename WrapReturn<TY>::Wrapper&, Query<TY> const&)
    {
      return false;
    }
    template<>
    inline bool 
    MockTable::detect_case (WrapReturn<Pipe>::Wrapper& candidate, Query<Pipe> const& q)
    {
      if (q.usesPredicate ("make"))
        // used by tests to force fabrication of a new "solution"
        return fabricate_just_new_Pipe (q);
      
      const string pipeID   = q.extractID("pipe");
      const string streamID = q.extractID("stream");
      
      if (candidate && pipeID == candidate->getPipeID())
        return set_new_mock_solution (q, candidate); // "learn" this solution to be "valid"
      
      if (!isnil(pipeID) && !isnil(streamID))
        return fabricate_matching_new_Pipe (q, pipeID, streamID);

      if (!candidate && (!isnil(streamID) || !isnil(pipeID)))
        return fabricate_just_new_Pipe (q);
      
      return false;
    }
    
    template<>
    inline bool 
    MockTable::detect_case (WrapReturn<const ProcPatt>::Wrapper& candidate, Query<const ProcPatt> const& q)
    {
      const string streamID = q.extractID("stream");
      
      if (!candidate && !isnil(streamID))
          return fabricate_ProcPatt_on_demand (q);
      return false;
    }
    
    template<>
    inline bool 
    MockTable::detect_case (WrapReturn<asset::Timeline>::Wrapper& candidate, Query<asset::Timeline> const& q)
    {
      if (!candidate)
          return fabricate_Timeline_on_demand (q);
      return bool(candidate);
    }
    
    template<>
    inline bool 
    MockTable::detect_case (WrapReturn<asset::Sequence>::Wrapper& candidate, Query<asset::Sequence> const& q)
    {
      if (!candidate)
          return fabricate_Sequence_on_demand (q);
      return bool(candidate);
    }
    
    
    
    
    
    
    /** 
     * Facade: Dummy Implementation of the query interface.
     * Provides an explicit implementation using hard wired
     * values for some types of interest for testing and debugging.
     */
    class MockConfigRules 
      : public InstantiateChained < proc::InterfaceTypes           
                                  , LookupPreconfigured  // building block used for each of the types
                                  , MockTable           //  for implementing the base class (interface) 
                                  >
      {
      protected:
        MockConfigRules ();                         ///< to be used only by the singleton factory
        friend class lib::DependencyFactory<MockConfigRules>;
        friend class lib::DependInject<ConfigResolver>::Local<MockConfigRules>;
      public:
        virtual ~MockConfigRules() {}           ///< extends the ConfigResolver interface
        
        
        /* === implementation of additional functions on the ConfigRules interface goes here === */
      };
      
    
    
  } // namespace query
}}} // namespace steam::mobject::session
#endif
