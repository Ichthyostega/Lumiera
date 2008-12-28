/*
  Struct(Asset)  -  key abstraction: structural asset
 
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
 
* *****************************************************/


#include "proc/assetmanager.hpp"
#include "proc/asset/struct.hpp"
#include "proc/asset/procpatt.hpp"
#include "proc/asset/track.hpp"
#include "proc/asset/pipe.hpp"
#include "common/configrules.hpp"

#include "proc/asset/structfactoryimpl.hpp"

#include "lib/util.hpp"
#include "include/nobugcfg.h"

#include <boost/format.hpp>

using boost::format;

using lumiera::Symbol;
using lumiera::query::normalizeID;
using lumiera::query::QueryHandler;
using lumiera::ConfigRules;

using util::contains;


namespace asset
  {
  
  /****** NOTE: not really implemented yet. What follows is partially a hack to build simple tests *******/

  
  
  /** query the currently defined properties of this
      structural asset for a stream-ID predicate */
  const string
  Struct::queryStreamID()  const
  {
    return lumiera::query::extractID ("stream", this->ident.name);
  }
  
  /** query the currently defined properties of this
      structural asset for a stream-ID predicate */
  const string
  Struct::queryPipeID()  const
  {
    return lumiera::query::extractID ("pipe", this->ident.name);
  }

  
  
  /** storage for the static StructFactory instance */
  StructFactory Struct::create;
  
  
  /** using private implementation detail class */
  StructFactory::StructFactory ()
    : impl_(new StructFactoryImpl(*this)) 
  { }
  
  
  
  /** Factory method for Structural Asset instances.
   *  First tries to relove the asset by issuing an capability query.
   *  If unsuccessful, use some internally specialised ctor call.
   *  @todo work out the struct asset naming scheme!
   *  @return an Struct smart ptr linked to the internally registered smart ptr
   *          created as a side effect of calling the concrete Struct subclass ctor.
   */
  template<class STRU>
  P<STRU> 
  StructFactory::operator() (const Query<STRU>& capabilities)
  {
    P<STRU> res;
    QueryHandler<STRU>& typeHandler = ConfigRules::instance();  
    typeHandler.resolve (res, capabilities);
    
    if (res)
      return res;
    
    // create new one, since the 
    // ConfigQuery didn't yield any result
    STRU* pS = impl_->fabricate(capabilities);
    return AssetManager::instance().wrap (*pS);
  }
  
  
  
  
  /** Factory method for creating Pipes explicitly.
   *  Normalises pipe- and streamID, then retrieves the
   *  default processing pattern (ProcPatt) for this streamID.
   *  The Pipe ctor will fill out the shortDesc and longDesc
   *  automatically, based on pipeID and streamID (and they
   *  are editable anyways)
   * @see ProcPatt
   * @see DefaultsManager 
   */ 
  P<Pipe> 
  StructFactory::operator() (string pipeID, string streamID)
  {
    normalizeID (pipeID);
    normalizeID (streamID);
    static format descriptor("pipe(%s), stream(%s).");
    Pipe* pP = impl_->fabricate (Query<Pipe> (descriptor % pipeID % streamID));
    return AssetManager::instance().wrap (*pP);
  }



} // namespace asset




   /**************************************************/
   /* explicit instantiations of the factory methods */
   /**************************************************/

#include "proc/asset/struct.hpp"
#include "proc/asset/procpatt.hpp"
#include "proc/asset/track.hpp"
#include "proc/asset/pipe.hpp"


namespace asset
  {
  
  template P<Pipe>     StructFactory::operator() (const Query<Pipe>& query);
  template P<Track>    StructFactory::operator() (const Query<Track>& query);
  template PProcPatt   StructFactory::operator() (const Query<const ProcPatt>& query);

  
} // namespace asset
