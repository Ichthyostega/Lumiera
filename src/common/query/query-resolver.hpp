/*
  QUERY-RESOLVER.hpp  -  framework for resolving generic queries

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file query-resolver.hpp
 ** framework and to resolve logical queries.
 ** This header defines a family of interfaces and classes
 ** to integrate resolution of logical, rules based queries into generic
 ** implementation code. The concrete facility actually to resolve such queries
 ** is abstracted away as QueryResolver. A prominent usage example is the session,
 ** which allows to query for elements "somewhere within the model"
 ** 
 */


#ifndef LUMIERA_QUERY_RESOLVER_H
#define LUMIERA_QUERY_RESOLVER_H

#include "lib/iter-adapter.hpp"
#include "common/query.hpp"
#include "lib/nocopy.hpp"

#include <functional>
#include <memory>
#include <string>

using std::function;

namespace lumiera {
  
  using std::unique_ptr;
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
    : util::NonCopyable
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
   * in response to specific queries of some kind, [if applicable](\ref QueryResolver::canHandle).
   * Every resolution mechanism is expected to enrol by calling #installResolutionCase.
   * Such a registration is considered permanent; a factory function gets stored,
   * assuming that the entity to implement this function remains available
   * up to the end of Lumiera main(). The kind of query and a suitable
   * resolver is determined by the QueryID, which includes a type-ID.
   * Thus the implementation might downcast query and resultset.
   */
  class QueryResolver
    : util::NonCopyable
    {
      unique_ptr<QueryDispatcher> dispatcher_;
      
      
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
   *  synonymous to Query<RES>::resolveBy() */
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
