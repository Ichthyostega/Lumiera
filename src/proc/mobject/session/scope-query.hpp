/*
  CONTENTS-QUERY.hpp  -  query to discover the contents of a container-like part of the model
 
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


#ifndef MOBJECT_SESSION_SCOPE_QUERY_H
#define MOBJECT_SESSION_SCOPE_QUERY_H


#include "proc/mobject/placement.hpp"
#include "proc/mobject/session/query-resolver.hpp"

#include <tr1/functional>


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
      
      /// Assignment explicitly disallowed (but copy ctor is ok)
      DiscoveryQuery const& operator=(DiscoveryQuery const&); 
      
    protected:
      typedef Query<Placement<MO> > _Query;
      typedef typename _Query::iterator _QIter;
      
      DiscoveryQuery ()
        : _Query (_Query::defineQueryTypeID (Goal::DISCOVERY))
        , _QIter ()
        { }
      
      
      typedef PlacementMO const& Pla;
      typedef function<bool(Pla)> ContentFilter;
      
      /** yield an additional filter to be applied
       *  on the result set. */
      virtual ContentFilter  buildContentFilter()  const  =0;
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
   * Contrary to the usual handling of a generic query, a ContentsQuery
   * object holds it's own discovery iterator and thus is completely
   * self contained. The query is issued automatically on construction,
   * thus the embedded iterator immediately points at the first result.
   * Moreover, as any Lumiera Forward Iterator is \c bool checkable,
   * a ContentsQuery not yielding any results will evaluate to \c false
   * immediately after construction, allowing convenient inline checks.
   * The query can be re-issued by the function operator, and the
   * embedded result iterator can of course be copied to a bare
   * iterator instance, e.g. for passing it on (ContentsQuery
   * itself is intended to be used polymorphically and thus
   * defined to be non-copyable)
   */
  template<class MO>
  class ScopeQuery
    : public DiscoveryQuery<MO>
    {
      typedef DiscoveryQuery<MO> _Par;
      typedef typename _Par::_Query _Query;
      
      
      QueryResolver const&    index_;
      PlacementMO const& startPoint_;
      ScopeQueryKind    to_discover_;
      
    public:
      typedef typename _Par::ContentFilter ContentFilter;
      typedef typename _Query::iterator iterator;
      
      
      ScopeQuery (QueryResolver const& resolver,
                  PlacementMO  const& scope,
                  ScopeQueryKind direction)
        : index_(resolver)
        , startPoint_(scope)
        , to_discover_(direction)
        {
          resetResultIteration (_Query::resolveBy(index_));
        }
      
      
      
      iterator
      operator() ()  const
        {
          return _Query::resolveBy (index_);
        }
      
      PlacementMO const&
      searchScope ()  const
        {
          return startPoint_;
        }
      
      ScopeQueryKind
      searchDirection ()  const
        {
          return to_discover_;
        }
      
      ContentFilter
      contentFilter () const
        {
          return buildContentFilter();
        }
      
      
    private:
      /** the default implementation of the content filtering
       *  builds on the downcast-function available on each 
       *  Placement instance. By parametrising this function
       *  with our template parameter MO, we pick out only
       *  those elements of the scope being subclasses of MO
       */
      virtual ContentFilter
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
  
  
}} // namespace mobject::session
#endif
