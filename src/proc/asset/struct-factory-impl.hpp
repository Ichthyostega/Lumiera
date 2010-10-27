/*
  STRUCT-FACTORY-IMPL.hpp  -  crating structural assets (impl details)
 
  Copyright (C)         Lumiera.org
    2008-2010,          Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file struct-factory-impl.hpp
 ** Private implementation details of creating various structural assets.
 ** Details how to fabricate specific kinds of structural assets, based
 ** on capability informations encoded into a query (goal).
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
#include "common/configrules.hpp"
#include "proc/asset/timeline.hpp"
#include "proc/asset/sequence.hpp"
#include "proc/asset/procpatt.hpp"
#include "proc/asset/pipe.hpp"
#include "lib/query.hpp"

#include "proc/asset/struct-scheme.hpp"

#include "lib/symbol.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"

#include <boost/format.hpp>

using boost::format;

using mobject::Session;
using mobject::MObject;
using mobject::session::Scope;
using mobject::session::match_specificTrack;
using mobject::session::RBinding;
using mobject::session::RTrack;

using lib::Symbol;
using util::isnil;
using util::contains;
using asset::Query;
using lumiera::query::LUMIERA_ERROR_CAPABILITY_QUERY;
using lumiera::query::extractID;

namespace asset {
  
  using idi::StructTraits;
  
  namespace {
    
    Symbol genericIdSymbol ("id");
    
  }
  
  
  
  
  /** 
   * Implementation details, especially concerning how configuration
   * queries are resolved and when to create new objects automatically.
   * @todo better use a general struct traits class, esp.for creating the Ident
   */ 
  class StructFactoryImpl
    {
      
      /** @internal derive a sensible asset ident tuple when creating 
       *  structural asset instances  based on a capability query
       */
      template<class STRU>
      const Asset::Ident
      createIdent (const Query<STRU>& query)
        {
          string name (query);
          
          // does the query somehow specify the desired name-ID?
          string nameID = extractID (genericIdSymbol, query);
          if (isnil (nameID))
            nameID = extractID (StructTraits<STRU>::idSymbol(), query);
          if (isnil (nameID))
            {
               // no name-ID contained in the query...
              //  so we'll create a new one
              static int i=0;
              static format namePattern ("%s.%d");
              static format predPattern ("%s(%s), ");
              nameID = str(namePattern % StructTraits<STRU>::namePrefix() % (++i) );
              name.insert(0, 
                       str(predPattern % StructTraits<STRU>::idSymbol() % nameID ));
            }
          ENSURE (!isnil (name));
          ENSURE (!isnil (nameID));
          ENSURE (contains (name, nameID));
          
          Category cat (STRUCT, StructTraits<STRU>::catFolder());
          return Asset::Ident (name, cat );                       ///////////////////////TICKET #565  the ID field should be just the ID, the query should go into a dedicated "capabilities" field.
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
      STRU* fabricate (const Query<STRU>& caps)
        {
        throw lumiera::error::Config ( str(format("The following Query could not be resolved: %s.") % caps.asKey())
                                     , LUMIERA_ERROR_CAPABILITY_QUERY );
        }
      
    };
  
  
  /* ============= specialisations =========================== */
  
  template<>
  inline const ProcPatt* 
  StructFactoryImpl::fabricate (const Query<const ProcPatt>& caps)
  {
    TODO ("actually extract properties/capabilities from the query...");
    return new ProcPatt (createIdent (caps));
  }
  
  template<>
  inline Pipe* 
  StructFactoryImpl::fabricate (const Query<Pipe>& caps)
  {
    const Asset::Ident idi (createIdent (caps));
    string pipeID = extractID ("pipe", idi.name);
    string streamID = extractID ("stream", caps);
    if (isnil (streamID)) streamID = "default"; 
    PProcPatt processingPattern = Session::current->defaults (Query<const ProcPatt>("stream("+streamID+")"));
    return new Pipe( idi
                   , processingPattern
                   , pipeID
                   );
  }
  
  template<>
  inline Timeline* 
  StructFactoryImpl::fabricate (const Query<Timeline>& caps)
  {
    TODO ("extract additional properties/capabilities from the query...");
    const Asset::Ident idi (createIdent (caps));
    string sequenceID = extractID ("sequence", caps);
    Query<Sequence> desiredSequence (isnil (sequenceID)? "" : "id("+sequenceID+")");
    PSequence sequence = recursive_create_(desiredSequence);
    ASSERT (sequence);
    RBinding newBinding = Session::current->getRoot().attach (MObject::create (sequence));
    ASSERT (newBinding);
    PTimeline newTimeline = Timeline::create (idi, newBinding);
    ENSURE (newTimeline);
    return newTimeline.get();
  }
  
  template<>
  inline Sequence* 
  StructFactoryImpl::fabricate (const Query<Sequence>& caps)
  {
    // when we reach this point it is clear a suitable sequence doesn't yet exist in the model
    TODO ("actually extract properties/capabilities from the query...");
    string trackID = extractID ("track", caps);
    Query<Track> desiredTrack (isnil (trackID)? "" : "id("+trackID+")");
//  PTrack track = Session::current->query (desiredTrack);        ///////////////////////////////////TICKET #639
    // TODO: handle the following cases
    // - track doesn't exist --> create and root attach it
    // - track exists and is root attached, but belongs already to a sequence --> throw
    // - track exists, but isn't root attached ---> what do do here? steal it??
    PSequence newSequence = Sequence::create (createIdent (caps));                      ///////////TODO fed track in here
    ENSURE (newSequence);
    return newSequence.get(); 
  }
  
  
  
  
} // namespace asset
#endif
