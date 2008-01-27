/*
  Struct(Asset)  -  key abstraction: structural asset
 
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
 
* *****************************************************/


#include "proc/assetmanager.hpp"
#include "proc/asset/struct.hpp"
#include "proc/asset/procpatt.hpp"
#include "proc/asset/track.hpp"
#include "proc/asset/port.hpp"
#include "proc/mobject/session.hpp"

#include "common/query.hpp"
#include "common/util.hpp"
#include "nobugcfg.h"

using mobject::Session;
using cinelerra::query::normalizeID;

namespace asset
  {
  
  /****** NOTE: not implemented yet. What follows is a hack to build simple tests *******/
  
  
  namespace // common Struct Asset implementation details
    {
      /** @internal derive a sensible asset ident tuple when creating 
       *  a track asset based on a query
       *  @todo define the actual naming scheme of struct assets
       */
      const Asset::Ident
      createTrackIdent (const Query<Track>& query)
        {
          string name ("track-" + query);  // TODO something sensible here; append number, sanitize etc.
          TODO ("track naming scheme??");
          Category category (STRUCT,"tracks");
          return Asset::Ident (name, category );
        }
      
      typedef std::pair<string,string> PortIDs;
      
      PortIDs
      createPortIdent (const Query<Port>& query)
        {
          string name ("port-" + query);   // TODO get some more sensible dummy values
          TODO ("port naming scheme??");
          return PortIDs (name, "mpeg"); // dummy stream type
        }
    } 


  
  StructFactory Struct::create;  ///< storage for the static StructFactory instance
  
  
  
  /** Factory method for Structural Asset instances. ....
   *  @todo work out the struct asset naming scheme!
   *  @return an Struct smart ptr linked to the internally registered smart ptr
   *          created as a side effect of calling the concrete Struct subclass ctor.
   */
  template<>
  shared_ptr<Track> 
  StructFactory::operator() (const Query<Track>& query)
  {
    TODO ("actually evaluate the query...");
    Track* pT = new Track (createTrackIdent (query));
    return AssetManager::instance().wrap (*pT);
  }
  
  
  /** Similar instance for Port Query....
   *  @todo better a generic implementation utilizing ConfigQuery....
   */
  template<>
  shared_ptr<Port> 
  StructFactory::operator() (const Query<Port>& query)
  {
    TODO ("actually evaluate the query...");
    PortIDs ids (createPortIdent (query));
    return operator() (ids.first, ids.second);
  }
  
  
  /** Factory method for creating Ports explicitly.
   *  Normalizes port- and streamID, then retrieves the
   *  default processing pattern (ProcPatt) for this streamID.
   *  The Port ctor will fill out the shortDesc and longDesc
   *  automatically, based on portID and streamID (and they
   *  are editable anyways)
   * @see ProcPatt
   * @see DefaultsManager 
   */ 
  shared_ptr<Port> 
  StructFactory::operator() (string portID, string streamID)
  {
    normalizeID (portID);
    normalizeID (streamID);
    PProcPatt processingPattern = Session::current->defaults (Query<ProcPatt>("stream("+streamID+")"));
    Port* pP = new Port (processingPattern, portID);
    return AssetManager::instance().wrap (*pP);
  }



} // namespace asset
