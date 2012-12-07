/*
  QUERY.hpp  -  interface for generic queries

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


#ifndef LUMIERA_QUERY_H
#define LUMIERA_QUERY_H


#include "lib/bool-checkable.hpp"
#include "lib/typed-counter.hpp"
#include "lib/iter-adapter.hpp"
#include "lib/query-text.hpp"
#include "lib/nocopy.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
#include <tr1/memory>
#include <typeinfo>
#include <cctype>
#include <string>


namespace lumiera {
  
  using lib::IxID;
  using lib::Symbol;
  using lib::Literal;
  using util::unConst;
  using boost::lexical_cast;
  using std::string;
  
  
  /* ==== common definitions for rule based queries ==== */
  
  class Goal;
  class Resolution;
  class QueryResolver;
  
  /** Allow for taking ownership of a result set */
  typedef std::tr1::shared_ptr<Resolution> PReso;

  
  
  /**
   * Query ABC: unspecific goal for resolution or retrieval.
   * Goal elements are used within the backbone of a generic query system
   * to access individual resolution mechanisms based on an internal classification
   * of the type of query.
   */
  class Goal
    : util::no_copy_by_client
    {
    public:
      virtual ~Goal(); ///< this is a marker baseclass
      
      enum Kind
        { EMPTY      = 0
        , GENERIC    = 1
        , DISCOVERY
        };
      
      struct QueryID
        {
          Kind kind;
          IxID type;
        };
      
      QueryID const&
      getQID()  const
        {
          return id_;
        }
      
      
      /** 
       * Single Solution, possibly part of a result set.
       * A pointer-like object, usually to be down-casted
       * to a specifically typed Query::Cursor
       * @see Resolution
       */
      class Result
        : public lib::BoolCheckable<Result>
        {
          void* cur_;
          
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
  
  
  inline bool
  operator== (Goal::QueryID const& id1, Goal::QueryID const& id2)
  {
    return id1.kind == id2.kind
        && id1.type == id2.type;
  }
  
  inline bool
  operator!= (Goal::QueryID const& id1, Goal::QueryID const& id2)
  {
    return ! (id1  == id2);
  }
  
  
  
  /** Context used for generating type-IDs to denote
   *  the specific result types of issued queries  */
  typedef lib::TypedContext<Goal::Result> ResultType;
  
  template<typename RES>
  inline IxID
  getResultTypeID()  ///< @return unique ID denoting result type RES
  {
    return ResultType::ID<RES>::get();
  }
  
  
  
  
  /**
   * Generic interface to express a query
   * for specifically typed result elements
   * providing some capabilities or fulfilling
   * some properties. This is a generic umbrella
   * for several kinds of queries and provides a
   * mechanism for uniform usage of various
   * resolution mechanisms.
   * 
   * Any query bears an internal type classification and can be
   * represented in a common syntactical form based on predicate logic.
   * Query instances are created by some facilities allowing to query for objects.
   * These query providers \em do know the specific kind (type) of query to expose.
   * While client code uses these queries only by reference, there is the possibility
   * to involve a generic QueryResolver, which -- behind the scenes -- manages a registry
   * of specific resolution mechanisms. This way, clients may retrieve a set of results,
   * each representing a possible solution to the posed query.
   * 
   * @note until really integrating a rules based system
   *       this is largely dummy placeholder implementation.
   *       Some more specific query resolvers are available already,
   *       so, depending on the circumstances the actual resolution might be
   *       substantial or just a fake.  
   * @warning especially the classical resolution-type queries are just
   *       faked and use  the given query-string as-is, without any normalisation.
   *       Moreover, as especially the fake-configrules match by string comparison,
   *       this may led to unexpected mis-matches.
   */
  template<class RES>
  class Query
    : public Goal
    {
      /** generic syntactical definition */
      lib::QueryText def_;
      
    protected:
      static QueryID
      defineQueryTypeID (Kind queryType = Goal::GENERIC)
        {
          QueryID id = {queryType, getResultTypeID<RES>() };
          return id;
        }
      
      /**
       * Extension point for specific kinds of queries
       * to generate a generic definition from some specialised
       * internal representation.
       * @return a complete definition of this query in predicate form
       */
      virtual lib::QueryText
      buildSyntacticRepresentation()
        {
          return this->def_;
        }
      
      class Builder;

      Query (QueryID qID)
        : Goal (qID)
        , def_(this->buildSyntacticRepresentation())
        { }
      
      friend class Builder;
      
      
    public:
      Query()
        : Goal (defineQueryTypeID(Goal::EMPTY))
        { }
      
      explicit
      Query (string querySpec)
        : Goal (defineQueryTypeID(Goal::GENERIC))
        , def_(querySpec)
        { }
      
      static Builder
      build (Kind queryType = Goal::GENERIC);
      
      Builder
      rebuild()  const;
      
      
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
          void point_at(RES& r){ Goal::Result::point_at(&r);}
        };
      
      
      typedef lib::IterAdapter<Cursor,PReso> iterator;
      
      iterator operator() (QueryResolver const& resolver)  const;
      iterator resolveBy  (QueryResolver const& resolver)  const;
      
      friend size_t
      hash_value (Query const& q)
      {
        return hash_value (q.def_);
      }
      
      friend bool
      operator== (Query const& q1, Query const& q2)
      {
        return q1.def_ == q2.def_;
      }
    };
  
  
  
  
    /** 
     * Helper for establishing,
     * reworking and remolding queries. 
     */
    template<class RES>
    class Query<RES>::Builder
      {
        QueryID typeID_;
        string predicateForm_;
        
        Builder (QueryID kind, string baseDef ="")
          : typeID_(kind)
          , predicateForm_(baseDef)
          { }
        
        friend class Query<RES>;
        
        
      public:
        
      const string
      asKey()  const
        {
          return "type("
               + lexical_cast<string> (getResultTypeID<RES>())
               + "), "+predicateForm_;
        }
      };
  
  
  
} // namespace lumiera
#endif
