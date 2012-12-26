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
#include "lib/query-util.hpp"
#include "lib/hash-value.h"
#include "lib/nocopy.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/operators.hpp>
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
  class QueryKey;

  
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
          
          explicit
          QueryID(Kind k =EMPTY, IxID t=1)
            : kind(k)
            , type(t)
            { }
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
  operator< (Goal::QueryID const& id1, Goal::QueryID const& id2)
  {
    return id1.kind < id2.kind
        ||(id1.kind == id2.kind && id1.type < id2.type);
  }
  
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
  
  
  
  namespace {
    /** Context used for generating type-IDs to denote
     *  the specific result types of issued queries  */
    typedef lib::TypedContext<Goal::Result> ResultType;
    
    template<typename RES>
    inline IxID
    getResultTypeID()  ///< @return unique ID denoting result type RES
    {
      return ResultType::ID<RES>::get();
    }
    
    /** includes the QueryID type distinction into the given hash value */
    inline size_t
    taggedHash (size_t hash, Goal::QueryID typeID)
    {
      lib::hash::combine (hash, typeID.kind);
      lib::hash::combine (hash, typeID.type);
      return hash;
    }
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
          QueryID id(queryType, getResultTypeID<RES>());
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
      
      explicit
      Query (QueryID typeID)
        : Goal (typeID)
        , def_(this->buildSyntacticRepresentation())
        { }
      
      Query (QueryID typeID, string querySpec)
        : Goal (defineQueryTypeID(typeID.kind))
        , def_(querySpec)
        { 
          REQUIRE (this->getQID().type == typeID.type);
        }
      
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
      
      operator QueryKey()  const;
      
      static Builder build (Kind queryType = Goal::GENERIC);
      Builder rebuild()  const;
      
      string extractID (Symbol predicate)  const;
      bool usesPredicate (Symbol predicate)  const;
      
      
      
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
        return taggedHash (hash_value(q.def_), q.id_);
      }
    };
  
  
  
  
  /** 
   * Wrapper for indexing and ordering.
   * Defines a synthetic totally ordered index value.
   * Implicitly convertible to and from Query instances.
   */
  class QueryKey
    : boost::totally_ordered<QueryKey>
    {
      Goal::QueryID id_;      
      lib::QueryText def_;
      
    public:
      QueryKey (Goal::QueryID id, lib::QueryText q)
        : id_(id)
        , def_(q)
        { }
      
      /** the empty or bottom query key */
      QueryKey()
        : id_()
        , def_("NIL")
        { }
      
      // default copyable
      
      template<class RES>
      operator Query<RES>()  const
        {
          REQUIRE (getResultTypeID<RES>() == id_.type);
          return Query<RES>::build(id_.kind).fromText(def_);
        }
      
      string
      display()  const
        {
          return "kind=" + lexical_cast<string>(id_.kind)
               +",type=" + lexical_cast<string>(id_.type)
               +",def="  + string(def_);
        }
      
      string
      getQueryString()  const
        {
          return string(def_);
        }
      
      
      uint
      degree()  const
        {
          return def_.degree_of_constriction();
        }
      
      bool
      empty()  const
        {
          return Goal::EMPTY == id_.kind;
        }
      
      
      friend bool
      operator< (QueryKey const& q1, QueryKey const& q2)
      {
        uint d1 = q1.degree();
        uint d2 = q2.degree();
        return d1 < d2
            ||(d1 == d2 && (  q1.def_ < q2.def_ 
                           ||(q1.def_ == q2.def_ && q1.id_ < q2.id_)));  
      }
      
      friend bool
      operator== (QueryKey const& q1, QueryKey const& q2)
      {
        return q1.def_ == q2.def_;  
      } 
      
      friend size_t
      hash_value (QueryKey const& q)
      {
        return taggedHash (hash_value(q.def_), q.id_);
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
      /** when done with defining or reworking the query,
       *  the result may be retrieved by type conversion */
      operator Query<RES>()
        {
          return Query<RES>(typeID_, predicateForm_);
        }
      
      
      /** @return a string representation usable for hashing
       *  @note includes the type parameter of the underlying query
       */
      string
      asKey()  const
        {
          return "type("
               + lexical_cast<string> (getResultTypeID<RES>())
               + "), "+predicateForm_;
        }
      
      
      /** extract an ID term defined as (single) parameter for the given predicate.
       *  E.g. when using the query "foo(a), bar(b)", \c extractID("bar") returns "b"
       * @param predicate symbol of the predicate to investigate
       * @warning preliminary implementation
       */
      string
      extractID (Symbol predicate)  const
        {
          return lib::query::extractID (predicate, this->predicateForm_);
        }
      
      
      /** remove the first term from this query definition,
       *  which matches the given predicate symbol
       * @warning preliminary implementation
       */
      Builder&
      removeTerm (Symbol termPredicate)
        {
          lib::query::removeTerm(termPredicate, this->predicateForm_);
          return *this;
        }
      
      Builder&
      withConditions (string additionalQueryPredicates)
        {
          this->predicateForm_ =
              lib::query::appendTerms(this->predicateForm_, additionalQueryPredicates);
          return *this;
        }
      
      Builder&
      prependConditions (string additionalQueryPredicates)
        {
          this->predicateForm_ =
              lib::query::appendTerms(additionalQueryPredicates, this->predicateForm_);
          return *this;
        }
      
      Builder&
      fromText (string queryPredicates)
        {
          this->predicateForm_ = queryPredicates;
          return *this;
        }
      
    };
  
  
  
  
  template<class RES>
  inline typename Query<RES>::Builder
  Query<RES>::build (Kind queryType)
  {
    return Builder(defineQueryTypeID (queryType));
  }
  
  
  template<class RES>
  inline typename Query<RES>::Builder
  Query<RES>::rebuild()  const
  {
    return Builder(this->id_, this->def_);
  }
  
  
  /** convenience shortcut to extract a desired name-ID.
   * @todo used extensively for the mock implementation of query resolution.
   *       For real resolution queries, such a function is quite nonsensical.
   *       To be revisited and (likely) to be removed on the long run
   * @see Query::Builder#extractID
   */
  template<class RES>
  inline string
  Query<RES>::extractID (Symbol predicate)  const
  {
    return this->rebuild().extractID (predicate);
  }
  
  
  template<class RES>
  inline bool
  Query<RES>::usesPredicate (Symbol predicate)  const
  {
    return lib::query::hasTerm(predicate, this->def_);
  }
  
  
  /** automatic conversion from Query to QueryKey for indexing and ordering.
   *  By defining a parameter of type QueryKey, any provided Query will be
   *  automatically transformed into an generic representation usable for
   *  ordered storage in sets, maps and for generation of metrics.
   */
  template<class RES>
  inline
  Query<RES>::operator QueryKey()  const
  {
    return QueryKey (this->id_, this->def_);
  }
      

  
  
  
} // namespace lumiera
#endif
