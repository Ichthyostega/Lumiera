/*
  OUTPUT-MAPPING.hpp  -  generic interface for translation of output designations
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file output-mapping.hpp
 ** Translating and wiring output designations.
 ** OutputMapping is a complement to the OutputDesignation handles
 ** used at various places in the high-level model. It is used when
 ** translating a given output spec into another connection target
 ** - when connecting an model port to a concrete external output
 ** - when connecting a timeline to a viewer element
 ** - for implementing the viewer input selection "switchbord"
 ** - for translating output designation of virtual clips
 ** OutputMapping is to be used as value object, holding concrete
 ** connections and wiring. For each of the mentioned usage situations,
 ** it needs to be adapted specifically, which is achieved by template
 ** (generic) programming: The usage situation provides a definition
 ** context DEF to fill in the variable parts of the implementation.
 ** This definition context is actually instantiated (as base class).
 ** The mapping table actually just stores an association of hash
 ** values, which typically are interpreted as asset::ID<Pipe>.
 ** But the actual mapping result is retrieved on each acces
 ** by invoking a functor on the stored hash value,
 ** thus the final resolution is done \em late.
 ** 
 ** @see OutputDesignation
 ** @see OutputMapping_test
 **
 */



#ifndef PROC_MOBJECT_OUTPUT_MAPPING_H
#define PROC_MOBJECT_OUTPUT_MAPPING_H

#include "lib/error.hpp"
#include "lib/bool-checkable.hpp"
#include "lib/query.hpp"
#include "lib/util.hpp"

#include <boost/operators.hpp>
#include <map>



namespace mobject {
  
  namespace { // Helper to extract and rebind definition types
    
    using util::contains;
    
    /** 
     * @internal used by OutputMapping
     * to figure out the mapping target type
     */
    template<class DEF>
    class _def
      {
        typedef asset::ID<asset::Pipe> PId;
        
        template<typename FUN>
        struct Rebind;
        
        template<typename RET>
        struct Rebind<RET(DEF::*)(PId)>
          {
            typedef RET Res;
          };
        
        typedef typeof(&DEF::output) OutputMappingMemberFunc;        // GCC extension: "typeof"
        typedef Rebind<OutputMappingMemberFunc> Rebinder;
        
      public:
        typedef typename Rebinder::Res Target;
        
      };
  }//(End) type rebinding helper
  
  namespace error = lumiera::error;
  using lumiera::Query;
  using asset::HashVal;
  
  
  
  
  /**
   * OutputMapping is a facility to resolve output designations.
   * For a given specification, resolution to the desired
   * target specification may be derived. Here, the 
   * type of the target specification is defined 
   * through the type parameter.
   * 
   * \par definition of specific mapping behaviour
   * 
   * This is an generic map-like container, acting as Interface to be used
   * in the signature of API functions either providing or requiring a Mapping.
   * For each distinct usage situation, an instantiation of this template should
   * be created, providing a <i>definition context</i> as template parameter.
   * Instances of this concrete mapping type may then be default constructed
   * and copied freely. The definition context is supposed to provide
   * - a functor \c DEF::output usable as function pipe-ID --> Target
   * - the concrete output-functor also defines the concrete Target type,
   *   which will be returned when accessing the OutputMapping
   * - a function \c DEF::buildQuery(sourcePipeID) yielding a defaults querry,
   *   to be issued in case of accessing a non existant mapping
   */
  template<class DEF>
  class OutputMapping
    : public DEF
    {
      typedef _def<DEF> Setup;
      
      typedef asset::ID<asset::Pipe> PId;
      typedef asset::PPipe         PPipe;
      
      /* == mapping table storage == */
      std::map<HashVal,HashVal> table_;
      
    public:
      typedef typename Setup::Target Target;
      
      // using default ctor and copy operations
      
      size_t size() const { return table_.size(); }
      bool empty()  const { return 0 == size(); }
      void clear()        { table_.clear(); }
      
      
      /**
       * @internal transient resolution wrapper to be exposed by map-style access.
       * A Resolver instance represents an output mapping result, yet to be fully resolved.
       * It is created on the stack by the OutputMapping container and internally wired
       * back to the container and the actually stored value (pipe-ID-hash) in the table.
       * Actually retrieving the result value by the client code triggers invocation
       * of the specific resolution functor, embedded in the definition context DEF,
       * which was given when instantiating the OutputMapping template.
       * @note depends on the template parameter of the enclosing OutputMapping type!
       */
      class Resolver
        : public lib::BoolCheckable<Resolver              // bool conversion to signal "unconnected"...
        , boost::equality_comparable<Resolver, Target,   //  final mapping result can be compared to Target...
          boost::equality_comparable<Resolver>          //   mapping values can be compared.
                                  > >                  //    
        {
          OutputMapping& thisMapping_;
          HashVal& pID_;
          
          Resolver (OutputMapping& container, HashVal& resultVal)
            : thisMapping_(container)
            , pID_(resultVal)
            { }
          
          friend class OutputMapping;
          
          /* copy by clients prohibited */
          Resolver& operator= (Resolver const&);
          
          
          Target
          resolve()  const
            {
              REQUIRE (pID_);
              PId targetPipeID (pID_);
              return thisMapping_.resolveTarget (targetPipeID);
            }
          
        public:
          /** explicitly define a new target ID for this individual mapping
           * @note the actually returned result depends on what the configured
           *       \c DEF::output functor will yield when invoked on this ID
           */
          void
          operator= (PId newId2map)
            {
              pID_ = newId2map;
            }
          
          void
          operator= (PPipe newPipe2map)
            {
              REQUIRE (newPipe2map);
              pID_ = newPipe2map->getID();
            }
          
          void
          disconnect() ///< switch this individual mapping into \em unconnected state
            {
              pID_ = 0;
            }
          
          /** actually retrieve the target object of the mapping.
           *  This operation is invoked when client code accesses
           *  the result of an OutputMapping query. 
           * @return result of invoking the configured \c DEF::output functor
           * @throw  error::Logic when resoving an \em unconnected mapping
           */
          operator Target()
            {
              if (!isValid())
                throw error::Logic ("attempt to resolve an unconnected output mapping"
                                   , error::LUMIERA_ERROR_UNCONNECTED);
              return resolve();
            }
          
          bool
          isValid()  const    ///< is this a valid \em connected mapping?
            {
              return bool(pID_);
            }
          
          
          /* === equality comparisons (boost::operators) === */
          
          friend bool
          operator== (Resolver const& a, Resolver const& b)
          {
            return a.pID_ == b.pID_; 
          }                 // note: Resolver depends on tempate parameter DEF
                           //        All instances of DEF are considered equivalent!
          friend bool
          operator== (Resolver const& rr, Target const& tval)
          {
            return rr.resolve() == tval; 
          }
        };
      
      
      
      /* === Map-style access for clients === */
      
      Resolver operator[] (PId sourcePipeID);
      Resolver operator[] (PPipe const& pipe);
      Resolver operator[] (Query<asset::Pipe> const& query4pipe);
      
      
      
    private:
      Target
      resolveTarget (PId mappedPipeID)
        {
          return DEF::output (mappedPipeID);
        }
      
      Resolver
      buildResolutionWrapper (HashVal tableSlot)
        {
          ASSERT (contains (table_, tableSlot));
          return Resolver (*this, table_[tableSlot]);
        }
    };
  
  
  
  
  /* ===== Implementation details ===== */
  
  namespace _mapping {
    
    /** yield a suitable table slot for this query */
    HashVal slot (Query<asset::Pipe> const&);
    
    /** delegate target pipe resolution to the rules system */
    HashVal resolveQuery (Query<asset::Pipe> const&);
  }
  
  
  template<class DEF>
  inline typename OutputMapping<DEF>::Resolver
  OutputMapping<DEF>::operator[] (PId sourcePipeID)
  {
    if (!contains (table_, sourcePipeID))
      {
        // issue a defaults query to resolve this mapping first
        Query<asset::Pipe> query4pipe = DEF::buildQuery (sourcePipeID);
        table_[sourcePipeID] = _mapping::resolveQuery (query4pipe);
      }
    return buildResolutionWrapper (sourcePipeID);
  }
  
  
  template<class DEF>
  inline typename OutputMapping<DEF>::Resolver
  OutputMapping<DEF>::operator[] (PPipe const& pipe)
  {
    REQUIRE (pipe);
    return (*this) [pipe->getID()];
  }
  
  
  /** determine an OutputMapping by resolving a complex query,
   *  instead of just picking a mapped pipe (which is the default usage).
   *  Accessing the OutputMapping this way by query enables all kinds of
   *  extended usages: It suffices that the given query somehow yields a Pipe,
   *  which then is considered the mapped result and handed over to the \c DEF::output
   *  functor for resolution to a result object to be returned. 
   *  @note the mapped result is remembered within this mapping. Further invocations
   *        with the \em same query will just fetch this stored pipe-ID and hand it
   *        to the functor, without resolving the query again. You might want to
   *        \link Resolver::disconnect remove \endlink this specific mapping 
   *        in order to force re-evaluation of the query.  
   * @param Query for a pipe, which is handed over as-is to the rules engine.
   * @warning depending on the actual query, there might be no solution,
   *        in which case an \em unconnected marker is retrieved and
   *        stored. Thus the yielded Resolver should be checked,
   *        if in doubt.
   */
  template<class DEF>
  inline typename OutputMapping<DEF>::Resolver
  OutputMapping<DEF>::operator[] (Query<asset::Pipe> const& query4pipe)
  {
    HashVal hash4query = _mapping::slot (query4pipe);
    if (!contains (table_, hash4query))
      // need to resolve this query first
      table_[hash4query] = _mapping::resolveQuery (query4pipe);
    
    ENSURE (contains (table_, hash4query));
    return buildResolutionWrapper (hash4query);
  }
  
  
} // namespace mobject
#endif
