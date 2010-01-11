/*
  STRUCTFACTORYIMPL.hpp  -  crating structural assets (impl details)
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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


/** @file structfactoryimpl.hpp
 ** Private implementation details of creating various structural assets.
 ** @internal to be used by struct.cpp
 **
 ** @see ConfigQuery
 **
 */


#ifndef ASSET_STRUCTFACTORYIMPL_H
#define ASSET_STRUCTFACTORYIMPL_H


#include "proc/mobject/session.hpp"
#include "common/configrules.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"

#include <boost/format.hpp>

using boost::format;

using mobject::Session;

using util::isnil;
using util::contains;
using asset::Query;
using lumiera::query::LUMIERA_ERROR_CAPABILITY_QUERY;
using lumiera::query::extractID;

namespace asset
  {
  
  
  template<class STRU>
  struct Traits
    {
      static Symbol namePrefix;
      static Symbol catFolder;
      static Symbol idSymbol;
    };
  
  template<> Symbol Traits<Track>::namePrefix = "track";
  template<> Symbol Traits<Track>::catFolder  = "tracks";
  template<> Symbol Traits<Track>::idSymbol   = "track";
  
  template<> Symbol Traits<Pipe>::namePrefix = "pipe";
  template<> Symbol Traits<Pipe>::catFolder  = "pipes";
  template<> Symbol Traits<Pipe>::idSymbol   = "pipe";
  
  template<> Symbol Traits<const ProcPatt>::namePrefix = "patt";
  template<> Symbol Traits<const ProcPatt>::catFolder  = "build-templates";
  template<> Symbol Traits<const ProcPatt>::idSymbol   = "procPatt";
  
  
  
  
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
          string nameID = extractID (Traits<STRU>::idSymbol, query);
          if (isnil (nameID))
            {
               // no name-ID contained in the query...
              //  so we'll create a new one
              static int i=0;
              static format namePattern ("%s.%d");
              static format predPattern ("%s(%s), ");
              nameID = str(namePattern % Traits<STRU>::namePrefix % (++i) );
              name.insert(0, 
                       str(predPattern % Traits<STRU>::idSymbol % nameID ));
            }
          ENSURE (!isnil (name));
          ENSURE (!isnil (nameID));
          ENSURE (contains (name, nameID));
          
          Category cat (STRUCT, Traits<STRU>::catFolder);
          return Asset::Ident (name, cat );
        }
      
  
    
      
      
      
      /** used for issuing recursive create calls to top level */
      StructFactory& recursive_create_;
      
    public:
      StructFactoryImpl (StructFactory& interface)
        : recursive_create_(interface)
        { }
      
      
      
      /** make a new structural asset instance.
       *  default/fallback impl. throws.
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
    Track* 
    StructFactoryImpl::fabricate (const Query<Track>& caps)
      {
        TODO ("actually extract properties/capabilities from the query...");
        TODO ("make sure AssetManager detects duplicates (currently 4/08 it doesn't)");
        return new Track (createIdent (caps));
      }
    
    template<>
    const ProcPatt* 
    StructFactoryImpl::fabricate (const Query<const ProcPatt>& caps)
      {
        TODO ("actually extract properties/capabilities from the query...");
        return new ProcPatt (createIdent (caps));
      }
    
    template<>
    Pipe* 
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
    
    
    
    
} // namespace asset
#endif
