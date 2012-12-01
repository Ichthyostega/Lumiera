/*
  STRUCT-FACTORY-IMPL.hpp  -  crating structural assets (impl details)

  Copyright (C)         Lumiera.org
    2008,2010,          Hermann Vosseler <Ichthyostega@web.de>

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


/** @file struct-factory-impl.hpp
 ** Private implementation details of creating various structural assets.
 ** Details how to fabricate specific kinds of structural assets, based
 ** on capability information encoded into a query (goal).
 ** @internal to be used by struct.cpp
 **
 ** @see ConfigQuery
 **
 */


#ifndef ASSET_STRUCT_FACTORY_IMPL_H
#define ASSET_STRUCT_FACTORY_IMPL_H


#include "proc/mobject/session.hpp"
#include "proc/mobject/session/binding.hpp"
#include "proc/mobject/session/mobjectfactory.hpp"
#include "proc/mobject/session/element-query.hpp"
#include "proc/mobject/session/session-query.hpp"
#include "proc/mobject/session/scope.hpp"
#include "common/config-rules.hpp"
#include "common/query.hpp"
#include "proc/asset/timeline.hpp"
#include "proc/asset/sequence.hpp"
#include "proc/asset/procpatt.hpp"
#include "proc/asset/pipe.hpp"

#include "proc/asset/struct-scheme.hpp"

#include "lib/symbol.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"

#include <boost/format.hpp>
#include <cstdlib>



namespace proc {
namespace asset {
  
  using boost::format;
  
  using lib::Symbol;
  using util::isnil;
  using util::contains;
  using lumiera::Query;
  using lumiera::query::LUMIERA_ERROR_CAPABILITY_QUERY;
  using lumiera::query::extractID;
  
  using proc::mobject::Session;
  using proc::mobject::MObject;
  using proc::mobject::session::Scope;
  using proc::mobject::session::match_specificTrack;
  using proc::mobject::session::RBinding;
  using proc::mobject::session::RTrack;
  
  using idi::StructTraits;
  
  
  namespace {
    
    Symbol genericIdSymbol ("id");
    Symbol seqNrPredicate  ("ord");
    
    inline uint
    asNumber (string const& spec)
    {
      return abs(std::atoi (spec.c_str()));
    }        // returns 0 in case of unparseable number
  }
  
  
  
  
  /** 
   * Implementation details, especially concerning how configuration
   * queries are resolved and when to create new objects automatically.
   * @todo better use a general struct traits class, esp.for creating the Ident
   * @todo shouldn't some of the generic factory logic be moved over into the struct baseclass? ////////////////TICKET #565
   */ 
  class StructFactoryImpl
    {
      
      /** @internal derive a sensible asset ident tuple when creating 
       *  structural asset instances  based on a capability query
       */
      template<class STRU>
      const Asset::Ident
      createIdent (Query<STRU> const& query)
        {
          // does the query somehow specify the desired name-ID?
          string nameID = "TODO";//extractID (genericIdSymbol, query);////////////////////////////////////////////////////////////////////////////////////////////TODO
          UNIMPLEMENTED("Query rebuilding");////////////////////////////////////////////////////////////////////////////////////////////TODO
          if (isnil (nameID))
            nameID = "TODO";//extractID (StructTraits<STRU>::idSymbol(), query);////////////////////////////////////////////////////////////////////////////////////////////TODO
          UNIMPLEMENTED("Query rebuilding");////////////////////////////////////////////////////////////////////////////////////////////TODO
          if (isnil (nameID))
            {
               // no name-ID contained in the query...
              //  so we'll create a new one
              static int i=0;
              static format namePattern ("%s.%d");
              nameID = str(namePattern % StructTraits<STRU>::namePrefix() % (++i) );
            }
          ENSURE (!isnil (nameID));
          
          // does the query actually demand the Nth instance/element?
          string seqID = "TODO";//extractID (seqNrPredicate, query);////////////////////////////////////////////////////////////////////////////////////////////TODO
        UNIMPLEMENTED("Query rebuilding");////////////////////////////////////////////////////////////////////////////////////////////TODO)
          if (!isnil (seqID) && 1 < asNumber(seqID))
            nameID += "."+seqID;
          
          Category cat (STRUCT, StructTraits<STRU>::catFolder());
          return Asset::Ident (nameID, cat );
        }
      
      
      /** either fetch or build a suitable root track for a new sequence */
      RTrack
      getTrack_forSequence (string const& desiredID)
        {
          RTrack track;
          if (!isnil (desiredID))
            track = Session::current->elements.pick (match_specificTrack (desiredID));
          
          if (track && !Scope::containing (track.getRef()).isRoot())
            {
              UNIMPLEMENTED ("how to deal with 'stealing' a sub-track tree to a new sequence??");
            }
          if (!track)
            track = Session::current->getRoot().attach (MObject::create (TrackID (desiredID)));
          
          return track;
        }
      
      
      
      
      
      
      
      
      /** used for issuing recursive create calls to top level */
      StructFactory& recursive_create_;
      
    public:
      StructFactoryImpl (StructFactory& interface)
        : recursive_create_(interface)
        { }
      
      
      
      /** make a new structural asset instance.
       *  @warning default/fallback implementation just throws.
       *  @todo a real implementation using a resolution engine.
       */
      template<class STRU>
      STRU* fabricate (Query<STRU> const& caps)
        {
        throw error::Config ( str(format("The following Query could not be resolved: %s.") % caps.asKey())
                            , LUMIERA_ERROR_CAPABILITY_QUERY );
        }
      
    };
  
  
  /* ============= specialisations =========================== */
  
  template<>
  inline const ProcPatt* 
  StructFactoryImpl::fabricate (Query<const ProcPatt> const& caps)
  {
    TODO ("actually extract properties/capabilities from the query...");
    return new ProcPatt (createIdent (caps));
  }                                               ///////////////////////TICKET #565  maybe store the capabilities query within the Struct asset somehow?
  
  template<>
  inline Pipe* 
  StructFactoryImpl::fabricate (Query<Pipe> const& caps)
  {
    const Asset::Ident idi (createIdent (caps));
    string streamID = "TODO";//extractID ("stream", caps);////////////////////////////////////////////////////////////////////////////////////////////TODO
    UNIMPLEMENTED("fabricate a Pipe by query");////////////////////////////////////////////////////////////////////////////////////////////TODO
    if (isnil (streamID)) streamID = "default"; 
    PProcPatt processingPattern = Session::current->defaults (Query<const ProcPatt>("stream("+streamID+")"));
    return new Pipe( idi
                   , streamID
                   , processingPattern
                   );                             ///////////////////////TICKET #565  maybe store the capabilities query within the Struct asset somehow?
  }
  
  template<>
  inline Timeline* 
  StructFactoryImpl::fabricate (Query<Timeline> const& caps)
  {
    TODO ("extract additional properties/capabilities from the query...");
    const Asset::Ident idi (createIdent (caps));
    string sequenceID = "TODO";//extractID ("sequence", caps);////////////////////////////////////////////////////////////////////////////////////////////TODO
    UNIMPLEMENTED("fabricate a Timeline by query");////////////////////////////////////////////////////////////////////////////////////////////TODO
    Query<Sequence> desiredSequence (isnil (sequenceID)? "" : "id("+sequenceID+")");
    PSequence sequence = recursive_create_(desiredSequence);
    ASSERT (sequence);
    RBinding newBinding = Session::current->getRoot().attach (MObject::create (sequence));
    ASSERT (newBinding);
    PTimeline newTimeline = Timeline::create (idi, newBinding);
    ENSURE (newTimeline);                         ///////////////////////TICKET #565  maybe store the capabilities query within the Struct asset somehow?
    return newTimeline.get();
  }
  
  template<>
  inline Sequence* 
  StructFactoryImpl::fabricate (Query<Sequence> const& caps)
  {
    // when we reach this point it is clear a suitable sequence doesn't yet exist in the model
    TODO ("actually extract properties/capabilities from the query...");
    string trackID = "TODO";//extractID ("track", caps);////////////////////////////////////////////////////////////////////////////////////////////TODO
    UNIMPLEMENTED("fabricate a Sequence by query");////////////////////////////////////////////////////////////////////////////////////////////TODO
    Query<Track> desiredTrack (isnil (trackID)? "" : "id("+trackID+")");
//  PTrack track = Session::current->query (desiredTrack);        ///////////////////////////////////TICKET #639
    // TODO: handle the following cases
    // - track doesn't exist --> create and root attach it
    // - track exists and is root attached, but belongs already to a sequence --> throw
    // - track exists, but isn't root attached ---> what do do here? steal it??
    PSequence newSequence = Sequence::create (createIdent (caps));                      ///////////TODO fed track in here
    ENSURE (newSequence);                         ///////////////////////TICKET #565  maybe store the capabilities query within the Struct asset somehow?
    return newSequence.get(); 
  }
  
  
  
  
}} // namespace proc::asset
#endif
