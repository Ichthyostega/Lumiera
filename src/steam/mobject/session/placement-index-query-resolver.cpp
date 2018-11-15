/*
  placementIndexQueryResolver  -  using PlacementIndex to resolve scope queries

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


/** @file placement-index-query-resolver.cpp
 ** @todo WIP implementation of session core from 2010
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#include "steam/mobject/session/placement-index-query-resolver.hpp"
#include "steam/mobject/session/scope-query.hpp"
#include "steam/mobject/placement.hpp"
#include "steam/mobject/session/clip.hpp"
#include "steam/mobject/session/effect.hpp"

#include "steam/mobject/test-dummy-mobject.hpp"  /////////////////////////////////////TICKET #532

#include <memory>
#include <stack>


namespace steam {
namespace mobject {
namespace session {
  
  using std::unique_ptr;
  
  using lumiera::Goal;
  using lumiera::Query;
  using lumiera::Resolution;
  using lumiera::getResultTypeID;
  
  
  typedef PlacementIndex::ID PID;
  typedef Goal::QueryID QueryID;
  typedef QueryID const& QID;
  
  typedef PlacementIndex& PIdx;
  typedef PlacementIndex::iterator PIter;
  
  /** @note all of this search implementation works on Placement<MObject> refs.
   *        Only finally, when accessing the iterator, a downcast to a more specific
   *        object type may happen. In this case, there is also a ContentFilter to
   *        prevent accessing a placement of a non-matching object type, by trying
   *        a dynamic cast beforehand. The instantiation of a suitably typed
   *        PlacementIndexQueryResolver::resolutionFunction ensures that these
   *        types match reliably the type of the issued query.
   */
  typedef PlacementMO Pla;
  
  
  
  /** Interface: strategy for exploring the structure */
  class Explorer
    {
    public:
      virtual ~Explorer() { };
      virtual bool exhausted ()  =0;
      virtual Pla& step ()       =0;
    };
  
  
  
  /* ==== special strategies to choose from ==== */
  
  /**
   * Strategy: explore the structure
   * just by following the given iterator;
   * usually this yields an element's children
   */
  class ChildExplorer
    : public Explorer
    {
      PIter tip_;
      
      bool
      exhausted() 
        {
          return !tip_;
        }
      
      Pla&
      step ()
        {
          REQUIRE (tip_);
          Pla& pos = *tip_;
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
    : public Explorer
    {
      PIdx index_;
      std::stack<PIter> scopes_;
      
      bool
      exhausted() 
        {
          while (!scopes_.empty() && !scopes_.top())
            scopes_.pop();
          return scopes_.empty();
        }
      
      Pla&
      step ()
        {
          REQUIRE (!scopes_.empty() && scopes_.top());
          Pla& pos = *scopes_.top();
          ++scopes_.top();
          scopes_.push(index_.getReferrers(pos.getID()));
          return pos;
        }
      
    public:
      DeepExplorer(PIter start, PIdx idx)
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
    : public Explorer
    {
      PIdx index_;
      Pla* tip_;
      
      bool
      exhausted() 
        {
          return !tip_;
        }
      
      Pla&
      step ()
        {
          REQUIRE (tip_);
          Pla& pos = *tip_;
          tip_ = &index_.getScope(*tip_);
          if (tip_ == &pos)
            tip_ = 0;
          return pos;
        }
      
    public:
      UpExplorer(Pla& start, PIdx idx)
        : index_(idx)
        , tip_(&start)
        { }
    };
  
  
  typedef function<bool(Pla const&)> ContentFilter;
  typedef function<Explorer*()>    ExplorerBuilder;
  
  
  /**
   * on query, an individual result set is prepared
   * to be explored by the invoking client code.
   * It is built wrapping the low-level scope iterator
   * obtained from the index, controlled by an 
   * exploration strategy. Embedded into the iterator
   * there is a smart-ptr managing this ResultSet.
   */
  class ResultSet
    : public lumiera::Resolution
    {
      ContentFilter acceptable_;
      ExplorerBuilder buildExploartion_;
      unique_ptr<Explorer> explore_;
      
      
      virtual Result
      prepareResolution()
        {
          explore_.reset (buildExploartion_());
          
          Result cursor;
          exploreNext (cursor);
          return cursor;
        }
      
      virtual void
      nextResult(Result& pos)
        {
          exploreNext (pos);
        }
      
      void
      exploreNext (Result& res)
        {
          typedef Query<Pla>::Cursor Cursor;
          Cursor& cursor = static_cast<Cursor&> (res);
          
          while (!explore_->exhausted() )
            {
              Pla& elm (explore_->step());
              if (acceptable_(elm))
                {
                  cursor.point_at (elm);
                  return;
                }
            }
          
          ASSERT (explore_->exhausted());
          cursor.point_at (0);
        }   
      
    public:
      ResultSet (ExplorerBuilder b
                ,ContentFilter a)
        : acceptable_(a)
        , buildExploartion_(b)
        , explore_()
        { }
    };
  
  
  
  
  namespace { // Helpers for wiring up a suitable resolutionFunction....
    
    
    bool acceptAllObjects_(Pla) { return true; }
    
    /** use filter predicate provided by the concrete query */
    template<typename QUERY>
    ContentFilter
    getContentFilter (QUERY const& query)
    {
      return query.contentFilter();
    }
    
    /** especially queries for MObjects need not be filtered */
    ContentFilter
    getContentFilter (ScopeQuery<MObject> const&)
    {
      static ContentFilter acceptAllObjects = bind (&acceptAllObjects_, _1);
      return acceptAllObjects;
    }
    
    /** shortcut for a suitable QueryID */
    template<typename MO>
    QueryID
    whenQueryingFor()
      {
        QueryID qID(Goal::DISCOVERY, getResultTypeID<Placement<MO>>());
        return qID;
      }
    
    struct
    UseThisIndex
      {
        UseThisIndex (PlacementIndex& idx) : refIndex_(idx) {}
        
        PlacementIndex& refIndex_;  
        PlacementIndex& operator() (void) { return refIndex_; }
      };
  } //(END) Helpers
  
  
  
  
  PlacementIndexQueryResolver::PlacementIndexQueryResolver (PlacementIndex& theIndex)
    : _getIndex(UseThisIndex (theIndex))
    {
      preGenerateInvocationContext();
    }
  
  
  PlacementIndexQueryResolver::PlacementIndexQueryResolver (function<IndexLink> const& accessIndex)
    : _getIndex(accessIndex)
    {
      preGenerateInvocationContext();
    }
  
  
  void
  PlacementIndexQueryResolver::preGenerateInvocationContext()
  {
    defineHandling<MObject>();
    defineHandling<Clip>();
    defineHandling<Effect>();
                       /////////////////////////////////////////////////////////////////TICKET #414
      
    defineHandling<mobject::test::DummyMO    >();
    defineHandling<mobject::test::TestSubMO1 >();
    defineHandling<mobject::test::TestSubMO2 >();
    defineHandling<mobject::test::TestSubMO21>();
                       /////////////////////////////////////////////////////////////////TICKET #532
  }
      

  bool
  PlacementIndexQueryResolver::canHandleQuery(QID qID)  const
  {
    return qID.kind == Goal::DISCOVERY
       &&( qID.type == getResultTypeID<Placement<MObject>>()
         ||qID.type == getResultTypeID<Placement<Clip>>()
         ||qID.type == getResultTypeID<Placement<Effect>>()
                       /////////////////////////////////////////////////////////////////TICKET #414
         
         ||qID.type == getResultTypeID<Placement<mobject::test::DummyMO    >>()
         ||qID.type == getResultTypeID<Placement<mobject::test::TestSubMO1 >>()
         ||qID.type == getResultTypeID<Placement<mobject::test::TestSubMO2 >>()
         ||qID.type == getResultTypeID<Placement<mobject::test::TestSubMO21>>()
                       /////////////////////////////////////////////////////////////////TICKET #532
         );
  }
  
  
  template<typename MO>
  void
  PlacementIndexQueryResolver::defineHandling()
  {
    installResolutionCase( whenQueryingFor<MO>()
                         , bind (&PlacementIndexQueryResolver::resolutionFunction<MO>,
                                 this, _1 )
                         );
  }
  
  
  /** an instance of this function is installed for each specifically typed
   *  kind of query to be handled. This allows the client code to retrieve
   *  just placements of this special type (e.g. Placement<Clip>) in a 
   *  typesafe manner. We ensure a suitable ContentFilter will be installed,
   *  dropping any other query results (of other type) before the point
   *  where they may get casted to the desired result type. The key for
   *  picking the right resolutionFunction is getResultTypeID<TYPE>()
   */
  template<typename MO>
  Resolution*
  PlacementIndexQueryResolver::resolutionFunction (Goal const& goal)
  {
    QID qID = goal.getQID();
    REQUIRE (qID == whenQueryingFor<MO>());
    REQUIRE (INSTANCEOF(ScopeQuery<MO>, &goal));
    
    ScopeQuery<MO> const& query = static_cast<ScopeQuery<MO> const&> (goal);
    ScopeQueryKind direction = query.searchDirection();
    PID scopeID = query.searchScope();
    
    return new ResultSet( bind (&PlacementIndexQueryResolver::setupExploration, 
                                this, scopeID, direction)
                        , getContentFilter(query)
                        );
  }
  
  
  /** the builder function used to set up an concrete result set object
   *  when issuing the query. It is preconfigured by the resolutionFunction.
   *  The object returned from this function is taken over and managed by a
   *  smart-ptr, which is embedded within the iterator given to the client.
   */
  Explorer*
  PlacementIndexQueryResolver::setupExploration (PID startID, ScopeQueryKind direction)
  {
    PIdx index = _getIndex(); // access the currently configured PlacementIndex (Session)
    switch (direction)
      {
      case CONTENTS:  return new DeepExplorer(index.getReferrers(startID), index);
      case CHILDREN:  return new ChildExplorer(index.getReferrers(startID));
      case PARENTS:   return new UpExplorer(index.getScope(startID),index);
      case PATH:      return new UpExplorer(index.find(startID),index);
      
      default:
        throw lumiera::error::Invalid("unknown query direction");    //////TICKET #197
      }
  }
  
  
  
}}} // namespace steam::mobject::session
