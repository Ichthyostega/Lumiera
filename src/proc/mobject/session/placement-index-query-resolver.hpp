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
//#include <boost/scoped_ptr.hpp>
//#include <vector>


namespace mobject {

//  class MObject;  /////////////////////////////////////????
  
namespace session {
  
//  using lib::factory::RefcountFac;
//  using std::tr1::shared_ptr;
//  using boost::scoped_ptr;
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
      typedef session::Goal::QueryID const& QID;

      template<class MO>
      typename session::Query<Placement<MO> >::iterator
      query (PlacementMO& scope)                  const;
      
      operator string()  const { return "PlacementIndex"; }
      
      bool canHandleQuery(QID)                    const;
////////////////////////////////////////////////////////////////TODO:
        
      /**
       * on query, an individual result set is prepared
       * to be explored by the invoking client code.
       * It is built wrapping the low-level scope iterator
       * obtained from the index, controlled by an 
       * exploration strategy.
       */
      template<typename TY>
      struct ResultSet
        : Resolution
        {
          DummySolutions<TY> solutions_;
          
          typedef typename Query<TY>::Cursor Cursor;
          
          Result
          prepareResolution()
            {
              Cursor cursor;
              cursor.point_at (solutions_.next());
              return cursor;
            }
          
          void
          nextResult(Result& pos)
            {
              Cursor& cursor = static_cast<Cursor&> (pos);
              
              if (solutions_.exhausted())
                cursor.point_at (0);
              else
                cursor.point_at (solutions_.next());
            }
        };
        
      template<typename MO>
      static Resolution*
      resolutionFunction (Goal const& goal)
        {
          QID qID = goal.getQID();
          REQUIRE (qID.kind == Goal::DISCOVERY
                && qID.type == getResultTypeID<Placement<MO> >());       /////////////////////////////TODO
          REQUIRE (INSTANCEOF(ScopeQuery<MO>, &goal));
          ScopeQuery<MO> const& query = static_cast<ScopeQuery<MO> const&> (goal);
          
          Literal direction = query.searchDirection();
          PID scopeID = query.searchScope().getID();     //////////TICKET #411
          
          ///////////////////////////TODO: where to put the exploration strategy? statically, dynamically?
          //////////////////////////////// note: direction == "parents" works completely different, based on index_->getScope(..)
          
          return new ResultSet<MO>(); 
        }
      
    public:
      PlacementIndexQueryResolver (PPIdx theIndex)
        : index_(theIndex)
        {
          Goal::QueryID case1 = {Goal::DISCOVERY, getResultTypeID<int>()};           /////////////////TODO
          Goal::QueryID case2 = {Goal::DISCOVERY, getResultTypeID<string>()};
          
          installResolutionCase(case1, &resolutionFunction<int> );
          installResolutionCase(case2, &resolutionFunction<string> );
        }
    };
  
  
  
  /** @todo use query-resolver-test as an example.....
   *        return a result set object derived from Resolution
   *        For the additional type filtering: build a filter iterator,
   *        using a type-filtering predicate, based on Placement#isCompatible
   */
  template<class MO>
  inline typename session::Query<Placement<MO> >::iterator
  PlacementIndexQueryResolver::query (PlacementMO& scope)  const
  {
    UNIMPLEMENTED ("actually run the containment query");
  }
  
  
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
