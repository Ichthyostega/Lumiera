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


#include "steam/mobject/session.hpp"
#include "steam/mobject/session/binding.hpp"
#include "steam/mobject/session/mobjectfactory.hpp"
#include "steam/mobject/session/element-query.hpp"
#include "steam/mobject/session/session-query.hpp"
#include "steam/mobject/session/scope.hpp"
#include "common/config-rules.hpp"
#include "common/query.hpp"
#include "steam/asset/timeline.hpp"
#include "steam/asset/sequence.hpp"
#include "steam/asset/procpatt.hpp"
#include "steam/asset/pipe.hpp"

#include "steam/asset/struct-scheme.hpp"

#include "lib/format-string.hpp"
#include "lib/query-util.hpp"
#include "lib/symbol.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"

#include <cstdlib>



namespace steam {
namespace asset {
  
  using lib::Symbol;
  using util::_Fmt;
  using util::uNum;
  using util::isnil;
  using util::contains;
  using lumiera::Query;
  using lumiera::query::LERR_(CAPABILITY_QUERY);
  using lib::query::extractID;
  
  using steam::mobject::Session;
  using steam::mobject::MObject;
  using steam::mobject::session::Scope;
  using steam::mobject::session::match_specificFork;
  using steam::mobject::session::RBinding;
  using steam::mobject::session::ForkID;
  using steam::mobject::session::RFork;
  using steam::mobject::session::Fork;
  
  using idi::StructTraits;
  
  
  namespace {
    
    Symbol genericIdSymbol ("id");
    Symbol seqNrPredicate  ("ord");
    
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
       *  structural asset instances based on a capability query
       */
      template<class STRU>
      const Asset::Ident
      createIdent (Query<STRU> const& query)
        {
          // does the query somehow specify the desired name-ID?
          string nameID = query.extractID (genericIdSymbol);
          if (isnil (nameID))
            nameID = query.extractID (StructTraits<STRU>::idSymbol());
          if (isnil (nameID))
            {
               // no name-ID contained in the query...
              //  so we'll create a new one
              static int i=0;
              nameID = _Fmt("%s.%d")
                           % StructTraits<STRU>::namePrefix()
                           % (++i);
            }
          ENSURE (!isnil (nameID));
          
          // does the query actually demand the Nth instance/element?
          string seqID = query.extractID (seqNrPredicate);
          if (!isnil (seqID) && 1 < uNum(seqID))
            nameID += "."+seqID;
          
          Category cat (STRUCT, StructTraits<STRU>::catFolder());
          return Asset::Ident (nameID, cat );
        }
      
      
      /** either fetch or build a suitable fork root for a new sequence */
      RFork
      getFork_forSequence (string const& desiredID)
        {
          RFork fork;
          if (!isnil (desiredID))
            fork = Session::current->elements.pick (match_specificFork (desiredID));
          
          if (fork && !Scope::containing (fork.getRef()).isRoot())
            {
              UNIMPLEMENTED ("how to deal with 'stealing' a fork sub-tree to a new sequence??");
            }
          if (!fork)
            fork = Session::current->getRoot().attach (MObject::create (ForkID (desiredID)));
          
          return fork;
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
          throw error::Config ("The following Query could not be resolved: " + caps.asKey()
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
    string streamID = caps.extractID ("stream");
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
    string sequenceID = caps.extractID ("sequence");
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
    string forkID = caps.extractID ("fork");
    Query<Fork> desiredFork (isnil (forkID)? "" : "id("+forkID+")");
//  PFork fork = Session::current->query (desiredFork);        ///////////////////////////////////TICKET #639
    // TODO: handle the following cases
    // - fork doesn't exist --> create and attach it as root
    // - fork exists and is root attached, but belongs already to a sequence --> throw
    // - fork exists, but isn't root attached ---> what do do here? steal it??
    PSequence newSequence = Sequence::create (createIdent (caps));                      ///////////TODO fed fork in here
    ENSURE (newSequence);                         ///////////////////////TICKET #565  maybe store the capabilities query within the Struct asset somehow?
    return newSequence.get(); 
  }
  
  
  
  
}} // namespace steam::asset
#endif
