/*
  Struct(Asset)  -  key abstraction: structural asset

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


/** @file struct.cpp
 ** Implementation functions to support the "structural asset" abstraction.
 ** This translation unit defines some functions of high importance for querying
 ** and self-discovery of model parts. Moreover, it drives explicit template specialisations
 ** for the core functions of the StructFacotry.
 ** 
 ** # Vision
 ** The vision is to express knowledge, configuration and some flexible parts of the model construction
 ** through predicate rules. To support this vision, at various places within the translation from
 ** Session model to render node network, a _query_ is used instead of hard wired functionality.
 ** @todo while most code here is a very preliminary draft (let's call it a hack), as of 2016
 **       we acknowledge that this design is here to stay and represents one of the most
 **       distinctive features of Lumiera
 */


#include "proc/asset/struct.hpp"
#include "proc/assetmanager.hpp"
#include "proc/config-resolver.hpp"

#include "proc/asset/struct-factory-impl.hpp"

#include "lib/util.hpp"
#include "lib/symbol.hpp"
#include "lib/query-util.hpp"
#include "lib/format-string.hpp"
#include "include/logging.h"
#include "common/query.hpp"

using util::_Fmt;
using lib::Symbol;
using lib::query::normaliseID;
using lumiera::query::QueryHandler;
using proc::ConfigResolver;

using util::contains;


namespace proc {
namespace asset {
  
  /****** NOTE: not fully implemented yet. What follows is partially a hack to build simple tests *******/
  
  
  
  
  
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
   * @todo using the AssetManager this way for picking up the previously stored
   *        asset is a code smell ////////////////////////////TICKET #691
   */
  template<class STRU>
  lib::P<STRU>
  StructFactory::newInstance (Symbol nameID)
  {
    Query<STRU> desired_name (isnil(nameID)? "" : "id("+nameID+")");
    STRU* pS = impl_->fabricate (desired_name);
    return AssetManager::instance().wrap (*pS);
  }
  
  
  
  /** Retrieve a suitable Structural Asset instance, possibly create one.
   *  First tries to resolve the asset by issuing an capability query.
   *  If unsuccessful, use some internally specialised ctor call.
   *  @todo work out the struct asset naming scheme! /////////////////////////////////TICKET #565
   *  @todo for now we're using a faked config query, just pulling preconfigured
   *        hardwired answers from a table. Should be replaced by a real resolution engine.
   *  @note the exact calling sequence implemented here can be considered a compromise,
   *        due to having neither a working resolution, nor a generic interface for
   *        issuing queries. Thus, directly calling this factory acts as a replacement
   *        for both. The intended solution would be to have a dedicated QueryResolver,
   *        which is fully integrated into a generic rules driven query subsystem, but
   *        has the additional ability to "translate" capabilities directly into the
   *        respective properties of of asset::Struct subclasses.
   *  @return a Struct smart ptr linked to the internally registered smart ptr
   *        created as a side effect of calling the concrete Struct subclass ctor.
   */
  template<class STRU>
  lib::P<STRU>
  StructFactory::operator() (Query<STRU> const& capabilities)
  {
    lib::P<STRU> res;
    QueryHandler<STRU>& typeHandler = ConfigResolver::instance();  
    typeHandler.resolve (res, capabilities);
    
    if (res)
      return res;
    
    // create new one, since the 
    // ConfigQuery didn't yield any result
    STRU* pS = impl_->fabricate(capabilities);
    return AssetManager::instance().wrap (*pS);
  }
  
  
  ////////////////////////////////////////////////////////////////////////////////////TICKET #710 : backdoor for fake-configrules
  /** special backdoor for fake-configrules.hpp
   *  This allows to simulate creation of objects triggered by rules.
   *  Actually we use just a fake implementation based on a table lookup
   *  plus some hard wired special cases, which need to call in here to
   *  fabricate new objects, which can then be used as "solutions".
   * @param query a prolog like query string
   * @note  works quite similar like the #operator(), but without
   *        re-invoking the ConfigRules....
   */
  template<class STRU>
  lib::P<STRU>
  StructFactory::made4fake (Query<STRU> const& query)
  {
    STRU* pS = impl_->fabricate(query);
    return AssetManager::instance().wrap (*pS);
  }
  ////////////////////////////////////////////////////////////////////////////////////TICKET #710 : backdoor for fake-configrules
  
  
  /** Factory method for creating Pipes explicitly.
   *  Normalises pipe- and streamID, then retrieves the
   *  default processing pattern (ProcPatt) for this streamID.
   *  The Pipe ctor will fill out the shortDesc and longDesc
   *  automatically, based on pipeID and streamID (and they
   *  are editable anyways)
   * @see ProcPatt
   * @see DefaultsManager 
   */ 
  lib::P<Pipe> 
  StructFactory::newPipe (string pipeID, string streamID)
  {
    normaliseID (pipeID);
    normaliseID (streamID);
    _Fmt descriptor("pipe(%s), stream(%s).");
    Pipe* pP = impl_->fabricate (Query<Pipe> (descriptor % pipeID % streamID));
    return AssetManager::instance().wrap (*pP);
  }
  
  
  
}} // namespace asset




   /**************************************************/
   /* explicit instantiations of the factory methods */
   /**************************************************/

#include "proc/asset/pipe.hpp"
#include "proc/asset/procpatt.hpp"
#include "proc/asset/timeline.hpp"
#include "proc/asset/sequence.hpp"


namespace proc {
namespace asset {
  using PPipe = lib::P<Pipe>;
  
  template PPipe       StructFactory::operator() (Query<Pipe> const&);
  template PProcPatt   StructFactory::operator() (Query<const ProcPatt> const&);
  template PTimeline   StructFactory::operator() (Query<Timeline> const&);
  template PSequence   StructFactory::operator() (Query<Sequence>const&);
  
  template PPipe       StructFactory::newInstance (Symbol);
  template PProcPatt   StructFactory::newInstance (Symbol);
  template PTimeline   StructFactory::newInstance (Symbol);
  template PSequence   StructFactory::newInstance (Symbol);
  
  template PPipe       StructFactory::made4fake   (Query<Pipe> const&);          
  template PProcPatt   StructFactory::made4fake   (Query<const ProcPatt> const&);
  template PTimeline   StructFactory::made4fake   (Query<Timeline> const&);      
  template PSequence   StructFactory::made4fake   (Query<Sequence>const&);       
  
}} // namespace asset
