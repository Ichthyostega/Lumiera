/*
  OutputDesignation  -  specifying a desired output destination

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

* *****************************************************/


/** @file output-designation.cpp 
 ** Implementation details of OutputDesignation and OutputMapping.
 ** Both of these are mostly intended as interface elements to represent
 ** the intention to connect to another MObject, or a translation and mapping
 ** of such connection intentions. But parts of the implementation are kept
 ** here in a translation unit separate of the usage site: The implementation
 ** of the various kinds of OutputDesignation spec (absolute, indirect, relative)
 ** and the connection between OutputMapping and the rules based system.
 ** 
 ** @see OutputDesignation
 ** @see OutputMapping
 ** @see OutputMapping_test
 **
 */


#include "lib/error.hpp"
#include "lib/symbol.hpp"
#include "lib/query-util.hpp"
#include "common/query/query-resolver.hpp"
#include "steam/mobject/mobject.hpp"
#include "steam/mobject/placement-ref.hpp"
#include "steam/mobject/output-designation.hpp"
#include "steam/mobject/output-mapping.hpp"
#include "steam/config-resolver.hpp"
#include "lib/util.hpp"

#include <boost/functional/hash.hpp>
#include <cstdlib>

using lumiera::Symbol;
using lumiera::query::QueryHandler;
using lib::query::removeTerm;
using lib::query::extractID;
using proc::ConfigResolver;
using lib::HashVal;
using util::uNum;

namespace steam {
namespace mobject {
  
  typedef OutputDesignation::PPipe PPipe;
  typedef OutputDesignation::PID PID;
  typedef OutputDesignation::TargetSpec TargetSpec;
  
  
  struct AbsoluteSpec
    : TargetSpec
    {
      PID target_;
      
      AbsoluteSpec (PID explicitTarget)
        : target_(explicitTarget)
        { }
      
      PID resolve (PPipe) { return target_; }
    };
  
  struct IndirectSpec
    : TargetSpec
    {
      RefPlacement mediator_;
      
      IndirectSpec (RefPlacement const& indirectTarget)
        : mediator_(indirectTarget)
        { }
      
      PID
      resolve (PPipe)
        {
          REQUIRE (mediator_);
          UNIMPLEMENTED ("how to query a placement for output designation");
        }
    };
  
  struct RelativeSpec
    : TargetSpec
    {
      uint busNr_;
      
      RelativeSpec (uint relative_busNr)
        : busNr_(relative_busNr)
        { }
      
      PID
      resolve (PPipe)
        {
          UNIMPLEMENTED ("how the hell can we get a grip on the target to resolve the bus??");
        }
    };
  
  
  
  
  OutputDesignation::TargetSpec::~TargetSpec() { } 
  
  
  /** create an output designation by directly
   *  specifying the target to connect 
   */
  OutputDesignation::OutputDesignation (PID explicitTarget)
    : spec_(AbsoluteSpec (explicitTarget))
    { }
  
  
  /** create an output designation indirectly
   *  to be resolved by forwarding the resolution
   *  to the given reference scope / mediator. 
   */
  OutputDesignation::OutputDesignation (RefPlacement const& indirectTarget)
    : spec_(IndirectSpec (indirectTarget))
    { }
  
  
  /** create an output designation by relative specification,
   *  to be resolved based on the stream type and the actual
   *  default target object at hand when resolving.
   *  @param relative_busNr within the collection of target pipes
   *         available for the actual stream type to connect
   *  @note as the relative bus/pipe number defaults to 0,
   *        effectively this becomes a default ctor, denoting
   *        "connect me to the first bus suitable for my stream type" 
   */
  OutputDesignation::OutputDesignation (uint relative_busNr)
    : spec_(RelativeSpec (relative_busNr))
    { }
  
  
  
  
  
  
  namespace _mapping {
    
    /** @internal to allow for the use of queries mixed with normal Pipe-IDs
     *  in a single table, we rely on the \c hash_value() function, to be
     *  picked up by ADL */
    HashVal
    slot (Query<asset::Pipe> const& query)
    {
      return hash_value (query);
    }
    
    /** @param Query for pipe, which is handed over as-is to the rules engine.
     * @return key for a table slot to hold the associated mapping. This slot
     *         is assumed to contain the mapped Pipe-ID for the given
     *         query -- including the possibility of a zero hash
     *         to signal an \em unconnected mapping. */
    HashVal
    resolveQuery (Query<asset::Pipe> const& query4pipe)
    {
      PPipe res;
      QueryHandler<asset::Pipe>& typeHandler = ConfigResolver::instance();  
      typeHandler.resolve (res, query4pipe);
      HashVal resulting_targetPipeID (res? (HashVal)res->getID() : 0 );
      return resulting_targetPipeID;
    }
    
    Symbol SEQNR_PREDICATE = "ord";
    
    uint
    is_defaults_query_with_channel (Query<asset::Pipe> const& query4pipe)
    {
      string seqNr = query4pipe.extractID (SEQNR_PREDICATE);
      return uNum (seqNr);  // defaults to 0 in case of an invalid number
    }
    
    Query<asset::Pipe>
    build_corresponding_sourceQuery (Query<asset::Pipe> const& query4pipe)
    {
      return query4pipe.rebuild()
                       .removeTerm (SEQNR_PREDICATE);
    }
  }
  
  
  
}} // namespace mobject
