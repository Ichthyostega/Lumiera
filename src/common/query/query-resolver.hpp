/*
  QUERY-RESOLVER.hpp  -  framework for resolving generic queries

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

*/


#ifndef LUMIERA_QUERY_RESOLVER_H
#define LUMIERA_QUERY_RESOLVER_H

#include "lib/iter-adapter.hpp"
#include "common/query.hpp"

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <functional>
#include <memory>
#include <string>

using std::function;

namespace lumiera {
  
  using boost::noncopyable;
  using boost::scoped_ptr;
  using std::string;
  
  
  class Resolution;
  class QueryResolver;
  class QueryDispatcher;
  
  /** Allow to take and transfer ownership of a result set */
  typedef std::shared_ptr<Resolution> PReso;
  
  
  /** 
   * ABC representing the result set
   * of an individual query resolution
   */
  class Resolution
    : boost::noncopyable
    {
    public:
      typedef Goal::Result Result;
      
      virtual ~Resolution();
      
      /** IterAdapter attached here */
      friend bool
      checkPoint (PReso const&, Result const& pos)
        {
          return bool(pos);
        }
      
      friend void
      iterNext (PReso& resultSet, Result& pos)
        {
          resultSet->nextResult(pos);
        }
      
      
      virtual Result prepareResolution()    =0;
      
    protected:
      
      virtual void nextResult(Result& pos)  =0;
    };


  /**
   * Interface: a facility for resolving (some kind of) queries
   * A concrete subclass has the ability to create Resolution instances
   * in response to specific queries of some kind, \link #canHandle if applicable \endlink.
   * Every resolution mechanism is expected to enrol by calling #installResolutionCase.
   * Such a registration is considered permanent; a factory function gets stored,
   * assuming that the entity to implement this function remains available
   * up to the end of Lumiera main(). The kind of query and a suitable
   * resolver is determined by the QueryID, which includes a type-ID.
   * Thus the implementation might downcast query and resultset.
   */
  class QueryResolver
    : noncopyable
    {
      scoped_ptr<QueryDispatcher> dispatcher_;
      
      
    public:
      virtual ~QueryResolver() ;
      
      virtual operator string ()  const    =0; ///< short characterisation of the actual facility
      
      
      /** issue a query to retrieve contents
       *  The query is handed over internally to a suitable resolver implementation.
       *  @return concrete Resolution of the query (ResultSet), \em managed by smart-pointer.
       *  @throw lumiera::Error subclass if query evaluation flounders.
       *         This might be broken logic, invalid input, misconfiguration
       *         or failure of an external facility used for resolution.
       *  @note a query may yield no results, in which case the iterator is empty.
       */
      PReso issue (Goal const& query)  const;
      
      bool canHandle (Goal const&) const;
      
      
      
    protected:  /* ===== API for concrete query resolvers ===== */
      
      virtual bool canHandleQuery (Goal::QueryID const&)  const =0;
      
      using ResolutionMechanism = function<Resolution*(Goal const&)>;
      
      void installResolutionCase (Goal::QueryID const&,
                                  ResolutionMechanism);
      
      QueryResolver();
    };
  
  
  
  
  template<typename RES>
  inline typename Query<RES>::iterator
  Query<RES>::resolveBy (QueryResolver const& resolver)  const
  {
    PReso resultSet = resolver.issue (*this);
    Result first = resultSet->prepareResolution();
    Cursor& start = static_cast<Cursor&> (first);   // note: type RES must be compatible!
    return iterator (resultSet, start);
  }
  
  
  /** notational convenience shortcut,
   *  synonymous to #resolveBy */
  template<typename RES>
  inline typename Query<RES>::iterator
  Query<RES>::operator() (QueryResolver const& resolver)  const
  {
    return resolveBy (resolver);
  }
  
  
  inline bool
  QueryResolver::canHandle(Goal const& query)  const
  {
    return canHandleQuery (query.getQID());
  }
  
  
} // namespace lumiera
#endif
