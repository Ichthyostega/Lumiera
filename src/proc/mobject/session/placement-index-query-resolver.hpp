/*
  PLACEMENT-INDEX-QUERY-RESOLVER.hpp  -  using PlacementIndex to resolve scope queries
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file placement-index-query-resolver.hpp
 ** TODO WIP-WIP 
 **
 ** @see PlacementRef
 ** @see PlacementIndex_test
 **
 */



#ifndef MOBJECT_SESSION_PLACEMENT_INDEX_QUERY_RESOLVER_H
#define MOBJECT_SESSION_PLACEMENT_INDEX_QUERY_RESOLVER_H

//#include "pre.hpp"
//#include "proc/mobject/session/locatingpin.hpp"
//#include "proc/asset/pipe.hpp"
//#include "lib/util.hpp"
//#include "lib/factory.hpp"
//#include "proc/mobject/placement.hpp"
#include "proc/mobject/session/placement-index.hpp"
#include "proc/mobject/session/query-resolver.hpp"
#include "proc/mobject/session/scope-query.hpp"

//#include <tr1/memory>
//#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
//#include <vector>
#include <stack>


namespace mobject {

//  class MObject;  /////////////////////////////////////????
  
namespace session {
  
//  using lib::factory::RefcountFac;
//  using std::tr1::shared_ptr;
  using boost::scoped_ptr;
//  using std::vector;
  
  
  
  /**
   * TODO type comment
   */
  class PlacementIndexQueryResolver
    : public session::QueryResolver
    {
      PPIdx index_;
      
      typedef PlacementIndex::ID PID;
////////////////////////////////////////////////////////////////TODO: moved in from PlacementIndex
      typedef Goal::QueryID QueryID;
      typedef QueryID const& QID;

      template<class MO>
      typename session::Query<Placement<MO> >::iterator
      query (PlacementMO& scope)                  const;
      
      operator string()  const { return "PlacementIndex"; }
      
      bool canHandleQuery(QID)                    const;
////////////////////////////////////////////////////////////////TODO:

      
      typedef PlacementIndex::iterator PIter;
      
      
      /** Interface: strategy for exploring the structure */
      class Explorer
        {
        public:
          virtual ~Explorer() { };
          virtual bool exhausted ()          =0;
          virtual PlacementMO& operator() () =0;
        };
      
      /**
       * Strategy: explore the structure
       * just by following the given iterator;
       * usually this yields an element's children
       */
      class ChildExplorer
        {
          PIter tip_;
          
          bool
          exhausted() 
            {
              return !tip_;
            }
          
          PlacementMO&
          operator() ()
            {
              REQUIRE (tip_);
              PlacementMO& pos = *tip_;
              ++tip_;
              return pos;
            }
          
        public:
          ChildExplorer(PIter start)
            : tip_(start)
            { }
        };
      
      /**
       * Strategy: explore the structure depth first.
       * After returning an element, delve into the scope
       * defined by this element and so on recursively.
       */
      class DeepExplorer
        {
          PPIdx index_;
          std::stack<PIter> scopes_;
          
          bool
          exhausted() 
            {
              while (!scopes_.empty() && !scopes_.top())
                scopes_.pop();
              return scopes_.empty();
            }
          
          PlacementMO&
          operator() ()
            {
              REQUIRE (!scopes_.empty() && scopes_.top());
              PlacementMO& pos = *scopes_.top();
              ++scopes_.top();
              scopes_.push(index_->getReferrers(pos.getID()));
              return pos;
            }
          
        public:
          DeepExplorer(PIter start, PPIdx idx)
            : index_(idx)
            {
              scopes_.push(start);
            }
        };
      
      
      /**
       * Strategy: explore the structure upwards,
       * ascending until reaching the root element.
       */
      class UpExplorer
        {
          PPIdx index_;
          PlacementMO* tip_;
          
          bool
          exhausted() 
            {
              return !tip_;
            }
          
          PlacementMO&
          operator() ()
            {
              REQUIRE (tip_);
              PlacementMO& pos = *tip_;
              tip_ = &index_->getScope(*tip_);
              if (tip_ == &pos)
                tip_ = 0;
              return pos;
            }
          
        public:
          UpExplorer(PlacementMO& start, PPIdx idx)
            : index_(idx)
            , tip_(&start)
            { }
        };
      
      typedef function<bool(Val const&)> ContentFilter;
      typedef function<Explorer*()> ExplorerBuilder;
      
      
      /**
       * on query, an individual result set is prepared
       * to be explored by the invoking client code.
       * It is built wrapping the low-level scope iterator
       * obtained from the index, controlled by an 
       * exploration strategy.
       */
      struct ResultSet
        : Resolution
        {
          typedef PlacementMO Val;
          
          ContentFilter acceptable_;
          ExplorerBuilder buildExploartion_;
          scoped_ptr<Explorer> explore_;
          
          void
          exploreNext (Result& res)
            {
              typedef typename Query<Val>::Cursor Cursor;
              Cursor& cursor = static_cast<Cursor&> (res);
              
              while (!explore_->exhausted() )
                {
                  Val& elm (*explore_());
                  if (acceptable_(elm))
                    {
                      cursor.point_at (elm);
                      return;
                    }
                }
              
              ASSERT (explore_->exhausted());
              cursor.point_at (0);
            }   
          
          Result
          prepareResolution()
            {
              explore_.reset (buildExploartion_());
              
              Result cursor;
              exploreNext (cursor);
              return cursor;
            }
          
          void
          nextResult(Result& pos)
            {
              exploreNext (pos);
              return pos;
            }
          
        public:
          ResultSet (ExplorerBuilder b
                    ,ContentFilter a)
            : acceptable_(a)
            , buildExploartion_(b)
            , explore_()
            { }
        };
        
      Explorer*
      setupExploration (PID startID, Literal direction)
        {
          if      (direction == "contents")
            return new DeepExplorer(index_->getReferrers(startID), index_);
          
          else if (direction == "children") 
            return new ChildExplorer(index_->getReferrers(startID));
          
          else if (direction == "parents") 
            return new UpExplorer(index_->getScope(startID),index_);
          
          else if (direction == "path") 
            return new UpExplorer(index_->find(startID),index_); 
        }
      
      template<typename MO>
      void
      defineHandling()
        {
          installResolutionCase( whenQueryingFor<MO>()
                               , bind (&PlacementIndexQueryResolver::resolutionFunction<MO>,
                                       this, _1 )
                               );
        }
        
      template<typename MO>
      Resolution*
      resolutionFunction (Goal const& goal)
        {
          QID qID = goal.getQID();
          REQUIRE (qID.kind == Goal::DISCOVERY
                && qID.type == getResultTypeID<Placement<MO> >());       /////////////////////////////TODO
          REQUIRE (INSTANCEOF(ScopeQuery<MO>, &goal));
          ScopeQuery<MO> const& query = static_cast<ScopeQuery<MO> const&> (goal);
          
          Literal direction = query.searchDirection();
          PID scopeID = query.searchScope().getID();     //////////TICKET #411
          
          return new ResultSet( bind (&PlacementIndexQueryResolver::setupExploration, 
                                      this, scopeID, direction)
                              , getContentFilter(query)
                              );
        }
      
      template<typename QUERY>
      ContentFilter
      getContentFilter (QUERY query)
        {
          return query.contentFilter();
        }
      
      ContentFilter
      getContentFilter (ScopeQuery<MObject>) ///< especially queries for MObjects need not be filtered
        {
          ContentFilter acceptAllObjects = bind (val(true), _1);
          return acceptAllObjects;
        }
      
      template<typename MO>
      static QueryID
      whenQueryingFor()
        {
          QueryID qID = {Goal::DISCOVERY, getResultTypeID<Placement<MO> >()};
          return qID;
        }
      
    public:
      PlacementIndexQueryResolver (PPIdx theIndex)
        : index_(theIndex)
        {
          defineHandling<MObject>();
          defineHandling<Clip>();
          defineHandling<Effect>();
          ///////////////////////////////////////TICKET #414
        }
    };
  
  
  
  bool
  PlacementIndexQueryResolver::canHandleQuery (QID qID) const
  {
    UNIMPLEMENTED ("decide by hard-wired check if the given Query can be resolved by PlacementIndex");
    return session::Goal::GENERIC == qID.kind;
        // thats not enough! need to check the typeID (match to Placement<MOX>, with some fixed MOX values)
  }
  
  
  
////////////////////////////////////////////////////////////////TODO:      
  

  
}} // namespace mobject::session
#endif
