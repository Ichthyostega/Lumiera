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
#include "lib/symbol.hpp"

#include <tr1/functional>


namespace mobject {
namespace session {
  
  using lib::Literal;
  using std::tr1::bind;
  using std::tr1::function;
  using std::tr1::placeholders::_1;
  
  
  
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
    : public Query<Placement<MO> >
    , public Query<Placement<MO> >::iterator
    {
      typedef Query<Placement<MO> > _Query;
      
      QueryResolver const&    index_;
      PlacementMO const& startPoint_;
      Literal what_to_discover_;
      
      
      typedef typename _Query::iterator _QIter;
      typedef typename PlacementMO const& Val;
      
      
    public:
      typedef _QIter              iterator;
      typedef function<bool(Val)> ContentFilter;
      
      
      ScopeQuery (QueryResolver const& resolver,
                  PlacementMO  const& scope,
                  Literal direction)
        : _Query (_Query::defineQueryTypeID (Goal::DISCOVERY))
        , _QIter ()
        , index_(resolver)
        , startPoint_(scope)
        , what_to_discover_(direction)
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
      
      Literal
      searchDirection ()  const
        {
          return what_to_discover_;
        }
      
      ContentFilter
      contentFilter () const
        {
          return buildContentFilter();
        }
      
      
    protected:
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
      
    private:
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
        : ScopeQuery<MO> (resolver,scope, "content")
        { }
      
    };
  
  
  template<class MO>
  struct PathQuery
    : ScopeQuery<MO>
    {
      PathQuery (QueryResolver const& resolver,
                 PlacementMO  const& scope)
        : ScopeQuery<MO> (resolver,scope, "parents")
        { }
      
    };
  
  
}} // namespace mobject::session
#endif
