/*
  OUTPUT-MAPPING.hpp  -  generic interface for translation of output designations

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file output-mapping.hpp
 ** Translating and wiring output designations.
 ** OutputMapping is a complement to the OutputDesignation handles
 ** used at various places in the high-level model. It is used when
 ** translating a given output spec into another connection target
 ** - when connecting a model port to a concrete external output
 ** - when connecting a timeline to a viewer element
 ** - for implementing the viewer input selection "switchboard"
 ** - for translating output designation of virtual clips
 ** OutputMapping is to be used as value object, holding concrete
 ** connections and wiring. For each of the mentioned usage situations,
 ** it needs to be adapted specifically, which is achieved by template
 ** (generic) programming: The usage situation provides a definition
 ** context DEF to fill in the variable parts of the implementation.
 ** This definition context is actually instantiated (as base class).
 ** The mapping table actually just stores an association of hash
 ** values, which typically are interpreted as asset::ID<Pipe>.
 ** But the actual mapping result is retrieved on each access
 ** by invoking a functor on the stored hash value,
 ** thus the final resolution is done _late_.
 ** 
 ** @see OutputDesignation
 ** @see OutputMapping_test
 **
 */



#ifndef PROC_MOBJECT_OUTPUT_MAPPING_H
#define PROC_MOBJECT_OUTPUT_MAPPING_H

#include "lib/error.hpp"
#include "lib/util.hpp"
#include "lib/hash-value.h"
#include "proc/asset/pipe.hpp"
#include "common/query.hpp"

#include <boost/operators.hpp>
#include <map>



namespace proc {
namespace mobject {
  
  namespace { // Helper to extract and rebind definition types
    
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
        
        using OutputMappingMemberFunc = decltype(&DEF::output) ;
        using Rebinder = Rebind<OutputMappingMemberFunc>;
        
      public:
        using Target = typename Rebinder::Res;
        
      };
  }//(End) type rebinding helper
  
  namespace error = lumiera::error;
  using lumiera::Query;
  using lib::HashVal;
  
  
  
  
  /**
   * OutputMapping is a facility to resolve output designations.
   * The session/model uses preliminary or partial output specifications,
   * which are to be resolved to an actual system output while building
   * and preparing a render network for operation (playback/rendering).
   * For a given specification, resolution to the desired target spec
   * may be derived by querying the OutputMapping. Here, the kind of
   * the target specification is defined through the type parameter.
   * 
   * ## definition of specific mapping behaviour
   * 
   * This is an generic map-like container, acting as Interface to be used
   * in the signature of API functions either providing or requiring a Mapping.
   * For each distinct usage situation, an instantiation of this template should
   * be created, providing a <i>definition context</i> as template parameter.
   * Instances of this concrete mapping type may then be default constructed
   * and copied freely. The definition context is supposed to provide
   * - a functor `DEF::output` usable as function pipe-ID --> Target
   * - the concrete output-functor also defines the concrete Target type,
   *   which will be returned when accessing the OutputMapping
   * - a function `DEF::buildQuery (sourcePipeID,seqNr)` yielding a (defaults)
   *   query to be issued in case of accessing a non existent mapping
   * @tparam DEF static (compile-time) definition/configuration context
   */
  template<class DEF>
  class OutputMapping
    : public DEF
    {
      typedef _def<DEF> Setup;
      
      using PId   = asset::ID<asset::Pipe>;
      using PPipe = asset::PPipe;
      
      /* == mapping table storage == */
      std::map<HashVal,HashVal> table_;
      
    public:
      using Target = typename Setup::Target;
      
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
       * of the specific resolution functor, embedded in the definition context `DEF`,
       * which was given when instantiating the OutputMapping template.
       * @note depends on the template parameter of the enclosing OutputMapping type!
       */
      class Resolver
        : public boost::equality_comparable<Resolver, Target   //  final mapping result can be compared to Target...
        , boost::equality_comparable<Resolver>>               //   mapping values can be compared.
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
           *       `DEF::output` functor will yield when invoked on this ID
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
           * @return result of invoking the configured `DEF::output` functor
           * @throw  error::Logic when resolving an _unconnected_ mapping
           */
          operator Target()
            {
              if (not isValid())
                throw error::Logic ("attempt to resolve an unconnected output mapping"
                                   , error::LUMIERA_ERROR_UNCONNECTED);
              return resolve();
            }
          
          bool
          isValid()  const    ///< is this a valid \em connected mapping?
            {
              return bool(pID_);
            }
          
          explicit
          operator bool()  const
            {
              return isValid();
            }
          
          
          /* === equality comparisons (boost::operators) === */
          
          friend bool
          operator== (Resolver const& a, Resolver const& b)
          {
            return a.pID_ == b.pID_;
          }                 // note: Resolver depends on template parameter DEF
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
      Resolver operator[] (Query<asset::Pipe> query4pipe);
      
      bool
      contains (PId mapping4sourcePipeID)
        {
          return util::contains (table_, mapping4sourcePipeID);
        }
      
      bool
      contains (PPipe sourcePipe)
        {
          return !sourcePipe
              or this->contains (sourcePipe->getID());
        }
      
      
      
    private:
      Target
      resolveTarget (PId mappedPipeID)
        {
          return DEF::output (mappedPipeID);
        }
      
      Resolver
      buildResolutionWrapper (HashVal tableSlot)
        {
          ASSERT (this->contains (tableSlot));
          return Resolver (*this, table_[tableSlot]);
        }
    };
  
  
  
  
  /* ===== Implementation details ===== */
  
  namespace _mapping {
    
    /** yield a suitable table slot for this query */
    HashVal slot (Query<asset::Pipe> const&);
    
    /** delegate target pipe resolution to the rules system */
    HashVal resolveQuery (Query<asset::Pipe> const&);
    
    /** detect the special case, when actually the Nth
     *  solution of a defaults query is requested */
    uint is_defaults_query_with_channel (Query<asset::Pipe> const&);
    
    /** ..and build the corresponding defaults source query for this case */
    Query<asset::Pipe> build_corresponding_sourceQuery (Query<asset::Pipe> const&);
  }
  
  
  /** standard map-style access to an OutputMapping.
   *  For the given source pipe-ID the mapped target pipe-ID is fetched
   *  and then handed over to the configured \c DEF::output functor, which
   *  is assumed to calculate or retrieve the actual result object.
   *  
   *  \par default mappings
   *  whenever accessing an yet non-existent mapping, a query is issued
   *  behind the scenes to establish a suitable default mapping. The actual
   *  query is built from a query template by the \c DEF::buildQuery function
   *  and thus can be configured for the concrete usage situation of the mapping.
   * @warning depending on the actually configured defaults query, there might be
   *        no solution, in which case an \em unconnected marker is retrieved and
   *        stored. Thus the yielded Resolver should be checked, if in doubt.
   */
  template<class DEF>
  inline typename OutputMapping<DEF>::Resolver
  OutputMapping<DEF>::operator[] (PId sourcePipeID)
  {
    if (not contains (sourcePipeID))
      {
        // issue a defaults query to resolve this mapping first
        Query<asset::Pipe> query4pipe = DEF::buildQuery (sourcePipeID);
        table_[sourcePipeID] = _mapping::resolveQuery (query4pipe);
      }
    return buildResolutionWrapper (sourcePipeID);
  }
  
  
  /** similar to the standard map-style access, but accepts
   *  a source pipe object instead of just a pipe-ID */
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
   *  which then is considered the mapped result and handed over to the `DEF::output`
   *  functor for resolution to a result object to be returned.
   *  
   *  ## Query for the Nth default instance
   *  OutputMapping provides a special behaviour for retrieving "the Nth default pipe".
   *  The rationale being the request for connection to the Nth bus of a given kind, like
   *  e.g. the 3rd audio subgroup or the 2nd video master. This special behaviour is triggered
   *  by the predicate "ord(##)" in the query. The _remainder of the query_ is supposed to
   *  designate a _default_ in this case, rather then querying directly for the result of
   *  the mapping. Thus this remainder of the query is used to retrieve a _source pipe_,
   *  which then is treated as if accessing a non-existent mapping: a suitable default
   *  solution for this mapping is retrieved, but in this special case, we append the
   *  given sequence number to the ID of the retrieved pipe, i.e. we get the Nth
   *  (identical) solution to the aforementioned query for a default pipe.
   *  
   *  @note the mapped result is remembered within this mapping. Further invocations
   *        with the _same query_ will just fetch this stored pipe-ID and hand it
   *        to the functor, without resolving the query again. You might want to
   *        [remove](\ref Resolver::disconnect) this specific mapping
   *        in order to force re-evaluation of the query.
   * @param Query for a pipe, which is handed over as-is to the rules engine.
   * @warning depending on the actual query, there might be no solution,
   *        in which case an _unconnected marker_ is retrieved and
   *        stored. Thus the yielded Resolver should be checked,
   *        if in doubt.
   */
  template<class DEF>
  inline typename OutputMapping<DEF>::Resolver
  OutputMapping<DEF>::operator[] (Query<asset::Pipe> query4pipe)
  {
    HashVal hash4query = _mapping::slot (query4pipe);
    if (not contains (hash4query))
      {
        // need to resolve this query first
        auto addEntry = [&](auto query)
                          {
                            table_[hash4query] = _mapping::resolveQuery (query);
                          };
        
        if (uint seqNr = _mapping::is_defaults_query_with_channel (query4pipe))
          {
            // treat the special case
            // when actually requesting the "Nth default of this kind"
            PPipe corresponding_sourcePipe
              = asset::Struct::retrieve (
                  _mapping::build_corresponding_sourceQuery (query4pipe));
            ENSURE (corresponding_sourcePipe);
            
            PId sourcePipeID = corresponding_sourcePipe->getID();
            addEntry (DEF::buildQuery (sourcePipeID, seqNr));
          }
        else
          addEntry (query4pipe);
      }
    
    ENSURE (this->contains (hash4query));
    return buildResolutionWrapper (hash4query);
  }
  
  
}} // namespace proc::mobject
#endif
