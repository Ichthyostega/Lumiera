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


#include "proc/asset/struct.hpp"
#include "proc/assetmanager.hpp"
#include "common/configrules.hpp"

#include "proc/asset/struct-factory-impl.hpp"

#include "lib/util.hpp"
#include "lib/symbol.hpp"
#include "include/logging.h"

#include <boost/format.hpp>

using boost::format;

using lib::Symbol;
using lumiera::query::normaliseID;
using lumiera::query::QueryHandler;
using lumiera::ConfigRules;

using util::contains;


namespace asset {
  
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
  StructFactory Struct::retrieve;
  
  
  /** using private implementation detail class */
  StructFactory::StructFactory ()
    : impl_(new StructFactoryImpl(*this)) 
  { }
  
  
  /** invoke the factory to create new Structural Asset.
   *  This function skips the query and retrieval of existing
   *  instances and immediately creates a new one.
   * @param nameID (optional) an ID to use; if omitted an ID
   *        will be default created, based on the kind of Asset.
   * @throw error::Invalid in case of ID clash with an existing Asset        
   * @return  an Struct smart ptr linked to the internally registered smart ptr
   *        created as a side effect of calling the concrete Struct subclass ctor.
   */
  template<class STRU>
  P<STRU>
  StructFactory::newInstance (Symbol nameID)
  {
    Query<STRU> desired_name (isnil(nameID)? "" : "id("+nameID+")");
    STRU* pS = impl_->fabricate (desired_name);
    return AssetManager::instance().wrap (*pS);
  }
  
  
  
  /** Retrieve a suitable Structural Asset instance, possibly create.
   *  First tries to resolve the asset by issuing an capability query.
   *  If unsuccessful, use some internally specialised ctor call.
   *  @todo work out the struct asset naming scheme! /////////////////////////////////TICKET #565
   *  @todo for now we're using a faked config query, just pulling preconfigured
   *        hardwired answers from a table. Should be replaced by a real resolution engine.
   *  @note the exact calling sequence implemented here can be considered a compromise,
   *        due to not having neither a working resolution, nor a generic interface for
   *        issuing queries. Thus, directly calling this factory acts as a replacement
   *        for both. The final algorithm to be implemented later should fabricate
   *        \em first, and then then query as a second step to define the capabilities.
   *  @return an Struct smart ptr linked to the internally registered smart ptr
   *        created as a side effect of calling the concrete Struct subclass ctor.
   */
  template<class STRU>
  P<STRU> 
  StructFactory::operator() (Query<STRU> const& capabilities)
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
    normaliseID (pipeID);
    normaliseID (streamID);
    static format descriptor("pipe(%s), stream(%s).");
    Pipe* pP = impl_->fabricate (Query<Pipe> (descriptor % pipeID % streamID));
    return AssetManager::instance().wrap (*pP);
  }
  
  
  
} // namespace asset




   /**************************************************/
   /* explicit instantiations of the factory methods */
   /**************************************************/

#include "proc/asset/struct.hpp"
#include "proc/asset/struct-scheme.hpp"
#include "proc/asset/procpatt.hpp"
#include "proc/asset/pipe.hpp"
#include "proc/asset/timeline.hpp"
#include "proc/asset/sequence.hpp"


namespace asset {
  
  template P<Pipe>     StructFactory::operator() (const Query<Pipe>& query);
  template PProcPatt   StructFactory::operator() (const Query<const ProcPatt>& query);
  template PTimeline   StructFactory::operator() (const Query<Timeline>& query);
  template PSequence   StructFactory::operator() (const Query<Sequence>& query);
  
  template P<Pipe>     StructFactory::newInstance (Symbol);
  template PProcPatt   StructFactory::newInstance (Symbol);
  template PTimeline   StructFactory::newInstance (Symbol);
  template PSequence   StructFactory::newInstance (Symbol);
  
  
} // namespace asset
