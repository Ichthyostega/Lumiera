/*
  SPECIFIC-CONTENTS-QUERY.hpp  -  pick specific contents from the model, using a filter

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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


#ifndef MOBJECT_SESSION_SPECIFIC_CONTENTS_QUERY_H
#define MOBJECT_SESSION_SPECIFIC_CONTENTS_QUERY_H


#include "proc/mobject/session/scope-query.hpp"
#include "proc/mobject/placement.hpp"
#include "common/query/query-resolver.hpp"

#include <functional>


namespace proc {
namespace mobject {
namespace session {
  
  using std::function;
  
  using lumiera::QueryResolver;
  
  
  /**
   * from the session, based on a filter predicate. As the parent type,
   * ContentsQuery, the resolution of this query requires to explore the
   * given scope depth first; but in addition to filter based on type,
   * a client-provided predicate is applied to each result.
   * @note this may degenerate on large sessions.
   * @todo develop a system of sub-indices and specialised queries
   */
  template<class MO>
  class SpecificContentsQuery
    : public ContentsQuery<MO>
    {
      typedef typename ContentsQuery<MO>::ContentFilter ContentFilter;
      
      typedef Placement<MO> const& TypedPlacement; 
      
      typedef function<bool(TypedPlacement)> SpecialPredicate;
      
      /**
       * Filter functor, built on top of a predicate,
       * which is provided by the client on creation of this
       * SpecivicContentsQuery instance. This allows for filtering
       * based on operations of the specific type MO, as opposed to
       * just using the bare MObject interface.
       */
      class Filter
        {
          SpecialPredicate predicate_;
          
        public:
          Filter (SpecialPredicate const& pred)
            : predicate_(pred)
            { }
          
          bool
          operator() (PlacementMO const& anyMO)
            {
              if (!anyMO.isCompatible<MO>())
                return false;
              
              TypedPlacement interestingObject = static_cast<TypedPlacement> (anyMO);
              return predicate_(interestingObject);
            }
        };
      
      Filter specialTest_;
      
      /** using a specialised version of the filtering,
       *  which doesn't only check the concrete type,
       *  but also applies a custom filter predicate
       *  @return function object, embedding a copy
       *          of the Filter functor. */
      ContentFilter
      buildContentFilter()  const
        {
          return specialTest_;
        }
      
      
      
    public:
      SpecificContentsQuery (PlacementMO  const& scope
                            ,SpecialPredicate const& specialPred)
        : ContentsQuery<MO> (scope)
        , specialTest_(specialPred)
        { }
    };
  
  
  
  namespace { // type matching helper
                                                              ///////////////////////////////TICKET #644  combine/clean up! see also element-query.hpp
    template<class PRED>
    struct _PickResult;
    
    template<class MO>
    struct _PickResult<function<bool(Placement<MO> const&)> >
      {
        typedef MO Type;
        typedef SpecificContentsQuery<MO> FilterQuery;
        typedef typename ScopeQuery<MO>::iterator Iterator;
      };
    
    template<class MO>
    struct _PickResult<bool(&)(Placement<MO> const&)>
      {
        typedef MO Type;
        typedef SpecificContentsQuery<MO> FilterQuery;
        typedef typename ScopeQuery<MO>::iterator Iterator;
      };
    
    template<class MO>
    struct _PickResult<bool(*)(Placement<MO> const&)>
      {
        typedef MO Type;
        typedef SpecificContentsQuery<MO> FilterQuery;
        typedef typename ScopeQuery<MO>::iterator Iterator;
      };
  }
  
  
  
  
  /** convenience shortcut to issue a SpecificContentsQuery,
   *  figuring out the actual return/filter type automatically,
   *  based on the predicate given as parameter
   */
  template<typename FUNC>
  inline typename _PickResult<FUNC>::FilterQuery
  pickAllSuitable(PlacementMO const& scope, FUNC predicate)
  {
    typedef typename _PickResult<FUNC>::FilterQuery Query;
    
    return Query(scope, predicate);
  }
  
  /** convenience shortcut (variant), automatically to build
   *  and execute a suitable SpecificContentsQuery
   *  @return iterator yielding placements of the type as
   *          defined through the parameter of the predicate
   */
  template<typename FUNC>
  inline typename _PickResult<FUNC>::Iterator
  pickAllSuitable(PlacementMO const& scope, FUNC predicate, QueryResolver const& resolver)
  {
    typedef typename _PickResult<FUNC>::FilterQuery Query;
    
    return Query(scope, predicate ).resolveBy(resolver);
  }
  
  
  
}}} // namespace proc::mobject::session
#endif
