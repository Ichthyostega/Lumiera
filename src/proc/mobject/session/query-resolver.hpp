/*
  QUERY-RESOLVER.hpp  -  interface for discovering contents of a scope
 
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


#ifndef MOBJECT_SESSION_QUERY_RESOLVER_H
#define MOBJECT_SESSION_QUERY_RESOLVER_H

//#include "proc/mobject/mobject.hpp"
//#include "proc/mobject/placement.hpp"
#include "lib/iter-adapter.hpp"

//#include <vector>
//#include <string>

//using std::vector;
//using std::string;

namespace mobject {
namespace session {
  
  class Scope;
  
  /**
   * TODO type comment
   */
  template<class MO>        ///////////////////////TODO: can we get rid of that type parameter? in conjunction with the virtual functions, it causes template code bloat
  class Query
    {
    public:
      virtual ~Query() {}
      
      /* results retrieval */
      
      typedef MO* Cur;
      typedef lib::IterAdapter<Cur,Query> iterator;
      
      iterator operator()(){ return this->runQuery(); }
      
      static bool hasNext  (const Query* thisQuery, Cur& pos) { return thisQuery->isValid(pos); }
      static void iterNext (const Query* thisQuery, Cur& pos) { pos = thisQuery->nextResult(); }
      
      /* diagnostics */
      static size_t query_cnt();      
      
      
    protected:
      virtual iterator runQuery()            =0;
    
      /* iteration control */
      virtual bool isValid (Cur& pos)  const =0;
      virtual Cur const& nextResult()  const =0;
    };
  
  
  /**
   * TODO type comment
   */
  class QueryResolver
    {
      
    public:
      
      virtual ~QueryResolver() {}
      
      
      /** issue a query to retrieve contents
       *  @param scope or container on which to discover contents
       *  @return an iterator to yield all elements of suitable type
       *  
       *  @todo doesn't this create a circular dependency? scope indirectly relies on QueryResolver, right??
       */
      template<class RES>
      typename Query<RES>::iterator
      query (Scope const& scope)
        {
          UNIMPLEMENTED ("create a specific contents query to enumerate contents of scope");
        }
      
      
    };
///////////////////////////TODO currently just fleshing the API
  
  
}} // namespace mobject::session
#endif
