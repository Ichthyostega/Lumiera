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
#include "lib/bool-checkable.hpp"
#include "lib/typed-counter.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/util.hpp"

#include <tr1/memory>
//#include <vector>
//#include <string>

//using std::vector;
//using std::string;

namespace mobject {
namespace session {
  
  using util::unConst;
  
  
//  class Scope;
  class Goal;
  class QueryResolver;
  
  /** Allow for taking ownership of a result set */
  typedef std::tr1::shared_ptr<Goal> PGoal;
  
  
  
  /**
   * TODO type comment
   * Query ABC
   */
  class Goal
    {
    public:
      virtual ~Goal() {}
      
      enum Kind
        { GENERIC = 0
        , DISCOVERY
        };
      
      struct QueryID
        {
          Kind kind;
          size_t type;
        };
      
      QueryID getQID() { return id_; }
      
      
      class Result
        : public lib::BoolCheckable<Result>
        {
          void *cur_;
          
        protected:
          void pointAt(void* p) { cur_ = p; }
          
          template<typename RES>
          RES&
          access()
            {
              REQUIRE (cur_);
              return *reinterpret_cast<RES*> (cur_);
            }
          
        public:
          bool isValid()  const { return bool(cur_); }
          
          Result() : cur_(0)  { } ///< create an NIL result
        };
      
      
      static bool hasNext  (PGoal thisQuery, Result& pos) { return thisQuery->isValid(pos); }   ////TICKET #375
      static void iterNext (PGoal thisQuery, Result& pos) { thisQuery->nextResult(pos);     }
      
    protected:
      QueryID id_;
      
      Goal (QueryID qid)
        : id_(quid)
        { }
      
      /* iteration control */
      virtual bool isValid (Result& pos)             =0;      /////TODO danger of template bloat?
      virtual Result const& nextResult(Result& pos)  =0;
    };
  
  
  
  /** Context used for generating type-IDs to denote
   *  the specific result types of issued queries  */
  typedef lib::TypedContext<Goal::Result> ResultType;
  
  
  /**
   * TODO type comment
   * Concrete query to yield specifically typed result elements
   */
  template<class RES>
  class Query
    : public Goal
    {
      static QueryID
      defineQueryTypeID ()
        {
          QueryID id = {Goal::GENERIC, ResultType::ID<RES>::get()};
          return id;
        }
      
    public:
      Query()
        : Goal (defineQueryTypeID())
        { }
      
      /* results retrieval */
      class Cursor
        : public Goal::Result
        {
        public:
          RES& operator* ()   { return   access<RES>();  }
          RES* operator->()   { return & access<RES>();  }
          
          void pointAt(RES* r){ Goal::Result::pointAt(r);}
        };
      
      
      typedef lib::IterAdapter<Cursor,PGoal> iterator;
      
      iterator operator() (QueryResolver const& resolver);
      
    };
  
  
  
  /**
   * TODO type comment
   */
  class QueryResolver
    {
      
    public:
      
      virtual ~QueryResolver() ;
      
      
      struct Invocation
        {
          PGoal        resultSet;
          Goal::Result firstResult;
        };
      
      /** issue a query to retrieve contents
       *  The query is handed over internally to a suitable resolver implementation.
       *  @return Invocation data, containing a smart-pointer which \em owns the result set,
       *          and the first result or NULL result. Usable for building an IterAdapter.
       *  @throw lumiera::Error subclass if query evaluation flounders.
       *         This might be broken logic, invalid input, misconfiguration
       *         or failure of an external facility used for resolution.
       *  @note a query may yield no results, in which case the iterator is empty.
       */
      Invocation issue (Goal& query);
      
      
    protected:
      virtual bool canHandleQuery (Goal::QueryID qID)  const =0;
      
    };
///////////////////////////TODO currently just fleshing the API
  
  
  template<typename RES>
  inline typename Query<RES>::iterator
  Query<RES>::operator() (QueryResolver const& resolver)
    {
      resolver.issue (*this);
      return iterator ();
    }
  
  
}} // namespace mobject::session
#endif
