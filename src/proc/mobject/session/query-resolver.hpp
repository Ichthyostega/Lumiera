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

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <tr1/functional>
#include <tr1/memory>
//#include <vector>
//#include <string>

//using std::vector;
//using std::string;

namespace mobject {
namespace session {
  
  using util::unConst;
  using boost::noncopyable;
  using boost::scoped_ptr;
  using std::tr1::function;
  
  
  class Goal;
  class Resolution;
  class QueryResolver;
  class QueryDispatcher;
  
  /** Allow for taking ownership of a result set */
  typedef std::tr1::shared_ptr<Resolution> PReso;
  
  
  
  /**
   * TODO type comment
   * Query ABC
   */
  class Goal
    : noncopyable
    {
    public:
      virtual ~Goal() ;
      
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
      
      
      /** 
       * Single Solution, possibly part of a result set.
       * A pointer-like object, usually to be down-casted
       * to a specifically typed Query::Cursor
       * @see Resolution
       */
      class Result
        : public lib::BoolCheckable<Result>
        {
          void *cur_;
          
        protected:
          void point_at(void* p) { cur_ = p; }
          
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
      
      
      
    protected:
      QueryID id_;
      
      Goal (QueryID qid)
        : id_(qid)
        { }
      
    };
  
  
  
  /** Context used for generating type-IDs to denote
   *  the specific result types of issued queries  */
  typedef lib::TypedContext<Goal::Result> ResultType;
  
  template<typename RES>
  inline size_t
  getResultTypeID()  ///< @return unique ID denoting result type RES
  {
    return ResultType::ID<RES>::get();
  }
  
  
  
  
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
          QueryID id = {Goal::GENERIC, getResultTypeID<RES>() };
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
          typedef RES value_type;
          typedef RES& reference;
          typedef RES* pointer;
          
          RES& operator* ()    { return   access<RES>();  }
          RES* operator->()    { return & access<RES>();  }
          
          void point_at(RES* r){ Goal::Result::point_at(r);}
        };
      
      
      typedef lib::IterAdapter<Cursor,PReso> iterator;
      
      iterator
      operator() (QueryResolver const& resolver);
      
    };
  
  
  
  
  
  /** 
   * ABC denoting the result set
   * of an individual query resolution
   */
  class Resolution
    : noncopyable
    {
    public:
      typedef Goal::Result Result;
      
      virtual ~Resolution();
      
      
      friend bool
      hasNext  (PReso const&, Result const& pos)          ////TICKET #375
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
   * Interface: a facility for resolving (some) queries
   * TODO type comment
   */
  class QueryResolver
    : noncopyable
    {
      scoped_ptr<QueryDispatcher> dispatcher_;
      
      
    public:
      virtual ~QueryResolver() ;
      
      
      /** issue a query to retrieve contents
       *  The query is handed over internally to a suitable resolver implementation.
       *  @return concrete Resolution of the query (ResultSet), \em managed by smart-pointer.
       *  @throw lumiera::Error subclass if query evaluation flounders.
       *         This might be broken logic, invalid input, misconfiguration
       *         or failure of an external facility used for resolution.
       *  @note a query may yield no results, in which case the iterator is empty.
       */
      PReso issue (Goal& query)  const;
      
      
      
    protected: /* === API for concrete query resolvers === */
      
      virtual bool canHandleQuery (Goal::QueryID)  const =0;
      
      void installResolutionCase (Goal::QueryID const&,
                                  function<Resolution*(Goal&)>);
      
      QueryResolver();
    };
///////////////////////////TODO currently just fleshing the API
  
  
  template<typename RES>
  inline typename Query<RES>::iterator
  Query<RES>::operator() (QueryResolver const& resolver)
    {
      PReso resultSet = resolver.issue (*this);
      Result first = resultSet->prepareResolution();
      Cursor& start = static_cast<Cursor&> (first);
      return iterator (resultSet, start);
    }
  
  
}} // namespace mobject::session
#endif
