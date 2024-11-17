/*
  QueryResolver  -  interface for discovering contents of a scope

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file query-resolver.cpp
 ** implementation of a framework to query and discover elements
 ** based on logical rules. This framework builds on the notion of
 ** possibly having several QueryResolver facilities to handle various
 ** kinds of queries in an uniform way. To reflect that design, the
 ** implementation is built round a DispatcherTable to forward requests
 ** to concrete entities implementing the QueryResolver interface. The
 ** actual query resolution mechanism is thus not part of the framework.
 ** 
 */


#include "lib/multifact.hpp"
#include "common/query/query-resolver.hpp"

namespace lumiera {
  
  
  
  /* generate vtables here... */
  
  Goal::~Goal() { }
  
  Resolution::~Resolution() { }
  
  QueryResolver::~QueryResolver() { }
  
  
  
  typedef Goal::QueryID const& QID;
  
  
  
  
  
  
  /* == dispatch to resolve typed queries == */
  
  using lib::factory::BuildRefcountPtr;
  using lib::factory::MultiFact;
  
  
  /** factory used as dispatcher table
   *  for resolving typed queries  */
  typedef MultiFact< Resolution*(Goal const&) // raw signature of fabrication
                   , Goal::QueryID           //  select resolution function by kind-of-Query
                   , BuildRefcountPtr       //   wrapper: manage result set by smart-ptr
                   > DispatcherTable;      //
  
  /** PImpl of the generic QueryResolver */
  class QueryDispatcher
    : public DispatcherTable
    {
    public:
      
      PReso
      handle (Goal const& query)
        {
          QID qID = query.getQID();
          ENSURE (contains (qID));
          
          return this->invokeFactory (qID, query);
        }                    // qID picks the resolution function
    };
  
  
  
  QueryResolver::QueryResolver ()
    : dispatcher_(new QueryDispatcher)
    { }
  
  
  /** @par implementation
   *  For actually building a result set, the QueryResolver base implementation
   *  uses an embedded dispatcher table. The concrete query resolving facilities,
   *  when implementing the QueryResolver interface, are expected to register
   *  individual resolution functions into this QueryDispatcher table.
   *  Whenever issuing a Goal, a suitable resolution function is picked
   *  based on the Goal::QueryID, which contains an embedded type code.
   *  Thus, the individual resolution function can (re)establish a
   *  typed context and downcast the Goal appropriately
   */
  PReso  
  QueryResolver::issue (Goal const& query)  const
  {
    REQUIRE (!dispatcher_->empty(), "attempt to issue a query without having installed any resolver (yet)");  
    
    if (!canHandle (query))
      throw lumiera::error::Invalid ("unable to resolve this kind of query"); //////////////////////////////////TICKET #197
    
    return dispatcher_->handle(query);
  }
  
  
  void
  QueryResolver::installResolutionCase (QID qID, ResolutionMechanism resolutionFun)
  {
    ENSURE (!dispatcher_->contains (qID),
            "duplicate registration of query resolution function");
    
    dispatcher_->defineProduction (qID, resolutionFun);
  }
  
  
  
  
} // namespace lumiera
