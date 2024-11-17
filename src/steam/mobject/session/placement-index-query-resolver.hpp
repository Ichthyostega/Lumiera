/*
  PLACEMENT-INDEX-QUERY-RESOLVER.hpp  -  using PlacementIndex to resolve scope queries

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file placement-index-query-resolver.hpp
 ** Implementing resolution of "discover contents"-queries based on PlacementIndex.
 ** This wrapper adds a service to resolve queries for exploring the contents or
 ** the parent path of a given scope; the actual implementation relies on the 
 ** basic operations provided by the PlacementIndex; usually this wrapper is
 ** instantiated as one of the SessionServices for use by Steam-Layer internals.
 ** The PlacementIndex to use for the implementation is handed in to the ctor.
 ** 
 ** As any of the QueryResolver services, the actual resolution is completely
 ** decoupled from the querying client code, which retrieves the query results
 ** through an iterator. Parametrisation is transmitted to the resolver using a
 ** special subclass of Goal, a ScopeQuery. Especially, besides a filter to apply
 ** on the results to retrieve, the direction and way to search can be parametrised: 
 ** - ascending to the parents of the start scope
 ** - enumerating the immediate child elements of the scope
 ** - exhaustive depth-first search to get any content of the scope
 ** 
 ** \par how the actual result set is created
 ** On initialisation, a table with preconfigured resolution functions is built,
 ** in order to re-gain the fully typed context when receiving a query. From within
 ** this context, the concrete Query instance can be investigated to define a
 ** constructor function for the actual result set, determining the way how further
 ** results will be searched and extracted. The further exploration is driven by the
 ** client pulling values from the iterator until exhaustion.  
 **
 ** @see PlacementRef
 ** @see PlacementIndex_test
 **
 */



#ifndef MOBJECT_SESSION_PLACEMENT_INDEX_QUERY_RESOLVER_H
#define MOBJECT_SESSION_PLACEMENT_INDEX_QUERY_RESOLVER_H

#include "steam/mobject/session/placement-index.hpp"
#include "steam/mobject/session/scope-query.hpp"

#include "common/query.hpp"

#include <functional>


namespace steam {
namespace mobject {
namespace session {
 
  using std::function;
  using lumiera::Goal;
  
  typedef PlacementIndex& IndexLink(void);
  
  class Explorer;
  
  
  /**
   * Wrapper for the PlacementIndex, allowing to resolve scope contents discovery
   * - handles queries for placements of
   *   * MObjcect
   *   * Clip
   *   * Effect
   * - is able to process
   *   * ContentsQuery for retrieving full contents of a scope depth-first
   *   * PathQuery for retrieving all the parent scopes
   *   * more generally, any ScopeQuery with these properties, in some variations
   */
  class PlacementIndexQueryResolver
    : public lumiera::QueryResolver
    {
      
      function<IndexLink> _getIndex;
      
      
      virtual bool canHandleQuery(Goal::QueryID const&)  const override;
      
      virtual operator string()  const override { return "PlacementIndex"; }
      
      
      Explorer* setupExploration (PlacementIndex::ID startID, ScopeQueryKind direction);
      
      void preGenerateInvocationContext();
      
      template<typename MO>
      void defineHandling();
        
      template<typename MO>
      lumiera::Resolution* resolutionFunction (Goal const& goal);
      
      
    public:
      PlacementIndexQueryResolver (PlacementIndex& theIndex);
      PlacementIndexQueryResolver (function<IndexLink> const& accessIndex);
    };
  
  
}}} // namespace steam::mobject::session
#endif
