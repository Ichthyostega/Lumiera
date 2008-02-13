/*
  STRUCTFACTORYIMPL.hpp  -  crating structural assets (impl details)
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "common/configrules.hpp"
#include "common/error.hpp"

#include <boost/format.hpp>

using boost::format;

using asset::Query;
using cinelerra::query::CINELERRA_ERROR_CAPABILITY_QUERY;


namespace asset
  {
  
  
  template<class STRU>
  struct Traits
    {
      static Symbol namePrefix;
      static Symbol catFolder;
    };
  
  template<> Symbol Traits<Track>::namePrefix = "track-";
  template<> Symbol Traits<Track>::catFolder  = "tracks";
  
  template<> Symbol Traits<Port>::namePrefix = "port-";
  template<> Symbol Traits<Port>::catFolder  = "ports";
  
  template<> Symbol Traits<const ProcPatt>::namePrefix = "patt-";
  template<> Symbol Traits<const ProcPatt>::catFolder  = "build-templates";
  
  
  
  
  /** 
   * Implementation deatils, esp. concerning how configuration
   * queries are resolved and when to create new objects automatically.
   * @todo better use a general struct traits class, esp.for creating the Ident
   */ 
  class StructFactoryImpl
    {

      /** @internal derive a sensible asset ident tuple when creating 
       *  structural asset instances  based on a capability query
       *  @todo define the actual naming scheme of struct assets
       */
      template<class STRU>
      const Asset::Ident
      createIdent (const Query<STRU>& query)
        {
          string name (Traits<STRU>::namePrefix + query);  // TODO something sensible here; append number, sanitize etc.
          TODO ("struct asset naming scheme??");
          Category cat (STRUCT, Traits<STRU>::catFolder);
          return Asset::Ident (name, cat );
        }
      
      typedef std::pair<string,string> PortIDs;
    
      PortIDs
      createPortIdent (const Query<Port>& query)
        {
          string name (Traits<Port>::namePrefix + query);  // TODO get some more sensible dummy values
          TODO ("port naming scheme??");
          TODO ("actually extract the port stream type from the query...");
          return PortIDs (name, "data");                // dummy stream type
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
        throw cinelerra::error::Config ( str(format("The following Query could not be resolved: %s.") % caps.asKey())
                                       , CINELERRA_ERROR_CAPABILITY_QUERY );
        }
      
    };
    
    
    /* ============= specialisations =========================== */

    template<>
    Track* 
    StructFactoryImpl::fabricate (const Query<Track>& caps)
      {
        TODO ("actually extract properties/capabilities from the query...");
        TODO ("make sure AssetManager detects dublicates (it doesn't currently)");
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
    Port* 
    StructFactoryImpl::fabricate (const Query<Port>& caps)
      {
        PortIDs ids (createPortIdent (caps));
        return recursive_create_ (ids.first, ids.second).get();
      }
    
    
    
    
} // namespace asset
#endif
