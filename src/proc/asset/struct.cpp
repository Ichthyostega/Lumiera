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
#include "proc/asset/pipe.hpp"
#include "proc/mobject/session.hpp"
#include "common/configrules.hpp"

#include "proc/asset/structfactoryimpl.hpp"

#include "common/util.hpp"
#include "nobugcfg.h"

using mobject::Session;
using cinelerra::query::normalizeID;

using cinelerra::ConfigRules;
using cinelerra::query::QueryHandler;



namespace asset
  {
  
  /****** NOTE: not really implemented yet. What follows is partially a hack to build simple tests *******/


  
  
  /** storage for the static StructFactory instance */
  StructFactory Struct::create;
  
  
  /** using private implementation detail class */
  StructFactory::StructFactory ()
    : impl_(new StructFactoryImpl(*this)) 
  { }
  
  
  
  /** Factory method for Structural Asset instances.
   *  First tries to relove the asset by issuing an capability query.
   *  If unsuccessfull, use some internally specialized ctor call.
   *  @todo work out the struct asset naming scheme!
   *  @return an Struct smart ptr linked to the internally registered smart ptr
   *          created as a side effect of calling the concrete Struct subclass ctor.
   */
  template<class STRU>
  shared_ptr<STRU> 
  StructFactory::operator() (const Query<STRU>& capabilities)
  {
    QueryHandler<STRU>& typeHandler = ConfigRules::instance();  
    shared_ptr<STRU> res = typeHandler.resolve (capabilities);
    
    if (res)
      return res;
    
    // create new one, since the 
    // ConfigQuery didn't yield any result
    STRU* pS = impl_->fabricate(capabilities);
    return AssetManager::instance().wrap (*pS);
  }
  
  
  
  
  /** Factory method for creating Pipes explicitly.
   *  Normalizes pipe- and streamID, then retrieves the
   *  default processing pattern (ProcPatt) for this streamID.
   *  The Pipe ctor will fill out the shortDesc and longDesc
   *  automatically, based on pipeID and streamID (and they
   *  are editable anyways)
   * @see ProcPatt
   * @see DefaultsManager 
   */ 
  shared_ptr<Pipe> 
  StructFactory::operator() (string pipeID, string streamID)
  {
    normalizeID (pipeID);
    normalizeID (streamID);
    PProcPatt processingPattern = Session::current->defaults (Query<const ProcPatt>("stream("+streamID+")"));
    Pipe* pP = new Pipe (processingPattern, pipeID);
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
  
  template shared_ptr<Pipe>     StructFactory::operator() (const Query<Pipe>& query);
  template shared_ptr<Track>    StructFactory::operator() (const Query<Track>& query);
  template PProcPatt            StructFactory::operator() (const Query<const ProcPatt>& query);

  
} // namespace asset
