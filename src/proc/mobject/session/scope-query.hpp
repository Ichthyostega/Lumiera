/*
  SCOPE-QUERY.hpp  -  query to discover the contents of a container-like part of the model
 
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


/** @file scope-query.hpp
 ** Specific queries to explore contents of a scope within the high-level model.
 ** This is an application of the QueryResolver facility, and used heavily to
 ** provide the various search and exploration functions on the session API.
 ** It is implemented by accessing a SessionService, which resolves the
 ** queries by iteration on the PlacementIndex behind the scenes.
 ** 
 ** @see query-focus.hpp
 ** @see query-resolver.hpp
 ** @see scope-query-test.cpp
 ** @see placement-index-query-test.cpp
 ** @see PlacementIndexQueryResolver
 **
 */



#ifndef MOBJECT_SESSION_SCOPE_QUERY_H
#define MOBJECT_SESSION_SCOPE_QUERY_H


#include "proc/mobject/placement.hpp"
#include "proc/mobject/session/query-resolver.hpp"

#include <tr1/functional>
#include "lib/meta/function-closure.hpp"  //////////////////////TODO


namespace mobject {
namespace session {
  
  using std::tr1::bind;
  using std::tr1::function;
  using std::tr1::placeholders::_1;
  
  
  
  /**
   * Mix-in ABC, combining a Query for placement-attached objects
   * with an embedded result set iterator. Thus, an concrete (derived)
   * DiscoveryQuery can be created by the invoking client code, and
   * then immediately explored until exhaustion of the result iterator. 
   */
  template<class MO>
  class DiscoveryQuery
    : public Query<Placement<MO> >
    , public Query<Placement<MO> >::iterator
    {
      typedef Query<Placement<MO> > _Query;
      
      
    public:      
      typedef typename _Query::iterator iterator;
      
      typedef function<bool(PlacementMO const&)> ContentFilter;
      
      
      ContentFilter
      contentFilter () const
        {
          return buildContentFilter();
        }

    protected:
      /** yield additional filter to be applied to the result set. */
      virtual ContentFilter  buildContentFilter()  const  =0;
      
      
      
      
      DiscoveryQuery ()
        : _Query (_Query::defineQueryTypeID (Goal::DISCOVERY))
        , _QIter ()
        { }
      
    private:
      /// Assignment explicitly disallowed (but copy ctor is ok)
      DiscoveryQuery const& operator=(DiscoveryQuery const&);
    };
  
  
  enum ScopeQueryKind
    { CONTENTS = 0      ///< discover any contained objects depth-first
    , CHILDREN          ///< discover the immediate children 
    , PARENTS           ///< discover the enclosing scopes
    , PATH              ///< discover the path to root
    };
  
  
  /**
   * Query a scope to discover it's contents or location.
   * This is a special kind of query, wired up such as to enumerate
   * the contents or parents of a scope, filtered by a subtype-check.
   * For the actual resolution of the elements to discover, this query
   * relies on an index like facility (usually Session's PlacementIndex),
   * which is abstracted as a QueryResolver, but actually is expected to
   * cooperate especially with this Query subclass to retrieve the scope
   * to be enumerated and the definition of the actual filter predicate.
   * Currently (11/09), there is a special, hard-wired Query-kind-ID
   * \c Goal::DISCOVERY to distinguish this special kind of a Query.
   * 
   * Contrary to the usual handling of a generic query, a ScopeQuery
   * object holds it's own discovery iterator and thus is completely
   * self contained. The query is issued automatically on construction,
   * thus the embedded iterator immediately points at the first result.
   * Moreover, as any Lumiera Forward Iterator is \c bool checkable,
   * a ScopeQuery not yielding any results will evaluate to \c false
   * immediately after construction, allowing convenient inline checks.
   * The query can be re-issued by the function operator, and the
   * embedded result iterator can of course be copied to a bare
   * iterator instance, e.g. for passing it on (ScopeQuery
   * itself is intended to be used polymorphically and
   * thus defined to be not assignable)
   */
  template<class MO>
  class ScopeQuery
    : public DiscoveryQuery<MO>
    {
      typedef DiscoveryQuery<MO>   _Parent;
      typedef Query<Placement<MO> > _Query;
      
      
      QueryResolver const&    index_;
      PlacementMO::ID    startPoint_;
      ScopeQueryKind    to_discover_;
      
    public:
      typedef typename _Parent::iterator      iterator;
      typedef typename _Parent::ContentFilter ContentFilter;
      
      
      ScopeQuery (QueryResolver const& resolver,
                  PlacementMO  const& scope,
                  ScopeQueryKind direction)
        : index_(resolver)
        , startPoint_(scope)
        , to_discover_(direction)
        {
          resetResultIteration (_Query::resolveBy(index_)); ///////////////////////////////TICKET #642  this call causes the problem!!!
        }
      
      
      
      iterator
      operator() ()  const
        {
          return _Query::resolveBy (index_);
        }
      
      PlacementMO::ID const&
      searchScope ()  const
        {
          return startPoint_;
        }
      
      ScopeQueryKind
      searchDirection ()  const
        {
          return to_discover_;
        }
      
      
      
    private:
      /** the default implementation of the content filtering
       *  builds on the downcast-function available on each 
       *  Placement instance. By parametrising this function
       *  with our template parameter MO, we pick out only
       *  those elements of the scope being subclasses of MO
       */
      ContentFilter
      buildContentFilter()  const
        {
          return bind (&PlacementMO::isCompatible<MO>, _1 );
        }
      
      void
      resetResultIteration (iterator const& newQueryResults)
        {
          static_cast<iterator&> (*this) = newQueryResults;
        }
    };
  
  
  template<class MO>
  struct ContentsQuery
    : ScopeQuery<MO>
    {
      ContentsQuery (QueryResolver const& resolver,
                     PlacementMO  const& scope)
        : ScopeQuery<MO> (resolver,scope, CONTENTS)
        { }
      
    };
  
  
  template<class MO>
  struct PathQuery
    : ScopeQuery<MO>
    {
      PathQuery (QueryResolver const& resolver,
                 PlacementMO  const& scope)
        : ScopeQuery<MO> (resolver,scope, PARENTS)
        { }
      
    };
  
  
  template<class MO>
  class SpecificContentsQuery
    : public ContentsQuery<MO>
    {
      typedef ContentsQuery<MO> _Parent;
      typedef typename _Parent::ContentFilter ContentFilter;
      
      typedef Placement<MO> const& TypedPlacement; 
      
      typedef function<bool(TypedPlacement)> SpecialPredicate;
      
      class SpecialTest
        {
          SpecialPredicate predicate_;
        public:
          SpecialTest (SpecialPredicate const& pred)
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
      
      SpecialTest specialTest_;
      
      ContentFilter
      buildContentFilter()  const
        {
          return specialTest_;
        }
      
    public:
      SpecificContentsQuery (QueryResolver const& resolver
                            ,PlacementMO  const& scope
                            ,SpecialPredicate const& specialPred)
        : ContentsQuery<MO> (resolver,scope)
        , specialTest_(specialPred)
        { }
    };
  
  
}} // namespace mobject::session
#endif
