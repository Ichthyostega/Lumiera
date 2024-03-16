/*
  AssetManager  -  Facade for the Asset subsystem

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


/** @file assetmanager.cpp
 ** Implementation of Asset lookup and organisation functions
 */


#include "steam/assetmanager.hpp"
#include "steam/asset/db.hpp"

#include "lib/sync.hpp"
#include "lib/util-foreach.hpp"
#include "lib/format-string.hpp"

#include <functional>

using std::static_pointer_cast;
using std::function;
using std::placeholders::_1;
using util::for_each;
using util::_Fmt;

using lib::Depend;
using lib::Sync;


namespace lumiera {
namespace error {
   // ------pre-defined-common-error-cases---------------
  LUMIERA_ERROR_DEFINE (UNKNOWN_ASSET_ID, "non-registered Asset ID");
  LUMIERA_ERROR_DEFINE (WRONG_ASSET_KIND, "wrong Asset kind, unable to cast");
}}

namespace steam {
namespace asset {
  namespace error = lumiera::error;
  
  /**
   * AssetManager error responses, caused by querying
   * invalid Asset IDs from the internal DB.
   */
  struct IDErr
    : error::Invalid
    {
      using error::Invalid::Invalid;
    };
  
  
  
  struct UnknownID : IDErr
    {
      UnknownID (ID<Asset> aID)
        : IDErr(_Fmt("Query for Asset with ID=%d, which up to now "
                     "hasn't been created or encountered.") % aID
               ,LERR_(UNKNOWN_ASSET_ID))
        { }
    };
  
  struct WrongKind : IDErr
    {
      WrongKind (Asset::Ident idi)
        : IDErr (_Fmt("Request for Asset(%s), specifying an Asset kind, "
                      "that doesn't match the actual type (and can't be "
                      "casted either).") % idi
                ,LERR_(WRONG_ASSET_KIND))
        { }
    };
  
  
  
  
  
  /** get at the system-wide asset manager instance.
   *  Implemented as singleton.
   */
  Depend<AssetManager> AssetManager::instance;
  
  AssetManager::AssetManager ()
    : registry (Depend<asset::DB>() ())
  { }
  
  
  
  /** provide the unique ID for given Asset::Ident tuple */
  ID<Asset>
  AssetManager::getID (const Asset::Ident& idi)
  {
    return asset::hash_value (idi);
  }
  
  
  
  /**
   * registers an asset object in the internal DB, providing its unique key.
   * This includes creating the smart ptr in charge of the asset's lifecycle
   * @throw error::Invalid in case of invalid identity spec
   */
  template<class KIND>
  ID<KIND>
  AssetManager::reg (KIND* obj, const Asset::Ident& idi)
  {
    AssetManager& _aMang (AssetManager::instance());
    DB& registry (_aMang.registry);
    //////////////////////////////////////////////////////////TICKET #840 check validity of Ident Category
    ID<KIND> asset_id (getID (idi));
    
    DB::Lock guard{&registry};
    //////////////////////////////////////////////////////////TICKET #840 handle duplicate Registrations
    lib::P<KIND> smart_ptr (obj, &destroy);
    
    registry.put (asset_id, smart_ptr);
    return asset_id;
  }
  
  
  /** @note the KIND of asset needs to be assignable by the actual stored asset
   *  @throw error::Invalid if nothing is found or if the actual KIND
   *         of the stored object differs and can't be casted.
   */
  template<class KIND>
  lib::P<KIND>
  AssetManager::getAsset (const ID<KIND>& id)
  {
    if (lib::P<KIND> obj = registry.get (id))
      return obj;
    else
      if (known (id))    // provide Ident tuple of existing Asset
        throw WrongKind (registry.get(ID<Asset>(id))->ident);
      else
        throw UnknownID (id);
  }
  
  /** Convenience shortcut for fetching the registered smart-ptr
   *  which is in charge of the given asset instance. By querying
   *  directly asset.id (of type ID<Asset>), the call to registry.get()
   *  can bypass the dynamic cast, because the type of the asset
   *  is explicitly given by type KIND.
   */
  template<class KIND>
  lib::P<KIND>
  AssetManager::wrap (const KIND& asset)
  {
    ENSURE (instance().known(asset.id),
            "unregistered asset instance encountered.");
    return static_pointer_cast<KIND,Asset>
            (instance().registry.get (asset.id));
  }
  
  
  
  /**
   * @return true if the given id is registered in the internal asset DB
   */
  bool
  AssetManager::known (IDA id)
  {
    return bool(registry.get (ID<Asset>(id)));
  }       // query most general Asset ID-kind and use implicit
         //  conversion from smart-ptr to bool (test if empty)
  
  
  /**
   * @return true if the given id is registered with the given Category
   */
  bool
  AssetManager::known (IDA id, const Category& cat)
  {
    PAsset pA = registry.get (id);
    return ( pA && pA->ident.category.isWithin(cat));
  }
  
  
  namespace { // details implementing AssetManager::remove
    
    void
    recursive_call (AssetManager* instance, PAsset& pA)
    {
      instance->remove (pA->getID());
    }
    
    function<void(PAsset&)>
    detach_child_recursively ()  ///< @return a functor recursively invoking remove(child)
    {
      return bind( &recursive_call, &AssetManager::instance(), _1 );
    }
  }
  
  /**
   * remove the given asset from the internal DB
   * _together with all its dependents_
   */
  void
  AssetManager::remove (IDA id)
  {
    PAsset asset = getAsset (id);
    for_each (asset->dependants, detach_child_recursively());
    asset->unlink();
    registry.del(id);
  }
  
  
  void
  AssetManager::clear()
  {
    INFO (progress, "Clearing the Asset registry...");
    registry.clear();
  }
  
  
  list<PcAsset>
  AssetManager::listContent() const
  {
    list<PcAsset> res;
    registry.asList (res);
    res.sort();
    return res;
  }
  
  
}} // namespace steam::asset




   /************************************************************/
   /* explicit template instantiations for various Asset Kinds */
   /************************************************************/

#include "steam/asset/media.hpp"
#include "steam/asset/clip.hpp"
#include "steam/asset/proc.hpp"
#include "steam/asset/struct.hpp"
#include "steam/asset/pipe.hpp"
#include "steam/asset/meta.hpp"
#include "steam/asset/procpatt.hpp"
#include "steam/asset/timeline.hpp"
#include "steam/asset/sequence.hpp"
#include "steam/asset/meta/time-grid.hpp"
#include "steam/asset/meta/error-log.hpp"

namespace steam {
namespace asset {
  using lib::P;
  
  template ID<Asset> AssetManager::reg (Asset* obj, const Asset::Ident& idi);
  
  
  template P<Asset>  AssetManager::getAsset (const ID<Asset>&  id);
  template P<Media>  AssetManager::getAsset (const ID<Media>&  id);
  template P<Proc>   AssetManager::getAsset (const ID<Proc>&   id);
  template P<Struct> AssetManager::getAsset (const ID<Struct>& id);
  template P<Meta>   AssetManager::getAsset (const ID<Meta>&   id);
  template P<Pipe>   AssetManager::getAsset (const ID<Pipe>&   id);
  
  template P<Asset>    AssetManager::wrap (const Asset& asset);
  template P<Media>    AssetManager::wrap (const Media& asset);
  template P<Clip>     AssetManager::wrap (const Clip&  asset);
  template P<Pipe>     AssetManager::wrap (const Pipe&  asset);
  template P<ProcPatt> AssetManager::wrap (const ProcPatt& asset);
  template P<Timeline> AssetManager::wrap (const Timeline& asset);
  template P<Sequence> AssetManager::wrap (const Sequence& asset);
  
  using meta::TimeGrid;
  using meta::ErrorLog;
  template P<TimeGrid> AssetManager::wrap (const TimeGrid& asset);
  template P<ErrorLog> AssetManager::wrap (const ErrorLog& asset);
  
  
}} // namespace steam::asset
