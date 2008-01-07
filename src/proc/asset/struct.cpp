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
#include "proc/asset/track.hpp"  

#include "common/util.hpp"
#include "nobugcfg.h"

namespace asset
  {
  
  namespace // Implementation details
  {
    /** @internal derive a sensible asset ident tuple when creating 
     *  a track asset based on a query
     *  @todo define the actual naming scheme of struct assets
     */
    const Asset::Ident
    createTrackIdent (Query<Track>& query)
      {
        string name ("track-" + query);  // TODO something sensible here; append number, sanitize etc.
        TODO ("track naming scheme??");
        Category category (STRUCT,"tracks");
        return Asset::Ident (name, category );
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
  StructFactory::operator() (Query<Track> query)
  {
    TODO ("actually evaluate the query...");
    Track* pT = new Track (createTrackIdent (query));
    return AssetManager::instance().getPtr (*pT);
  }


} // namespace asset
