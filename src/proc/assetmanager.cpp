/*
  AssetManager  -  Facade for the Asset subsystem
 
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
#include "proc/asset/db.hpp"

#include "lib/concurrency.hpp"
#include "lib/util.hpp"

#include <boost/function.hpp>

#include <boost/format.hpp>
#include <boost/bind.hpp>

using std::tr1::static_pointer_cast;
using boost::function;
using boost::format;
using boost::bind;
using util::for_each;

using lumiera::Singleton;
using lib::Concurrency;


namespace asset
  {
  
  /** 
   * AssetManager error responses, caused by querying
   * invalid Asset IDs from the internal DB.
   */ 
  class IDErr : public lumiera::error::Invalid 
    {
    public:
      IDErr (const char* eID, format fmt) 
        : lumiera::error::Invalid(fmt.str(),eID) {}
    };

  
   // ------pre-defined-common-error-cases---------------
  //
  LUMIERA_ERROR_DEFINE (UNKNOWN_ASSET_ID, "non-registered Asset ID");
  LUMIERA_ERROR_DEFINE (WRONG_ASSET_KIND, "wrong Asset kind, unable to cast");
  
  class UnknownID : public IDErr
    {
    public:
      UnknownID (ID<Asset> aID) : IDErr (LUMIERA_ERROR_UNKNOWN_ASSET_ID, 
                                         format("Query for Asset with ID=%d, which up to now "
                                                "hasn't been created or encountered.") % aID) {}
    };
  
  class WrongKind : public IDErr
    {
    public:
      WrongKind (Asset::Ident idi) : IDErr (LUMIERA_ERROR_WRONG_ASSET_KIND,
                                            format("Request for Asset(%s), specifying an Asset kind, "
                                                   "that doesn't match the actual type (and can't be "
                                                   "casted either).") % string(idi)) {}
    };
  
  
  
  
  
  /** get at the system-wide asset manager instance.
   *  Implemented as singleton.
   */
  Singleton<AssetManager> AssetManager::instance;
  
  AssetManager::AssetManager ()
    : registry (Singleton<asset::DB>() ())
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
   */
  template<class KIND>
  ID<KIND>  
  AssetManager::reg (KIND* obj, const Asset::Ident& idi)
      throw(lumiera::error::Invalid)
  {
    AssetManager& _aMang (AssetManager::instance());
    TODO ("check validity of Ident Category");
    ID<KIND> asset_id (getID (idi));
    
    Concurrency::ClassLock<DB> guard();
    TODO ("handle duplicate Registrations");
    P<KIND> smart_ptr (obj, &destroy);

    _aMang.registry.put (asset_id, smart_ptr);
    return asset_id;
  }
  
  
  /** find and return the object registered with the given ID.
   *  @throws Invalid if nothing is found or if the actual KIND
   *          of the stored object differs and can't be casted.  
   */
  template<class KIND>
  P<KIND>
  AssetManager::getAsset (const ID<KIND>& id)  
      throw(lumiera::error::Invalid)
  {
    if (P<KIND> obj = registry.get (id))
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
  P<KIND>
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
    return ( registry.get (ID<Asset>(id)) );
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

  /**
   * remove the given asset from the internal DB
   * <i>together with all its dependents</i> 
   */
  void
  AssetManager::remove (IDA id)  
  {
    PAsset asset = getAsset (id);
    for_each (asset->dependants, detach_child_recursively());
    asset->unlink();
    registry.del(id);
  }

  
  
  list<PcAsset> 
  AssetManager::listContent() const
  {
    list<PcAsset> res;
    registry.asList (res);
    res.sort();
    return res;
  }

  
} // namespace asset




   /************************************************************/
   /* explicit template instantiations for various Asset Kinds */
   /************************************************************/

#include "proc/asset/media.hpp"  
#include "proc/asset/clip.hpp"  
#include "proc/asset/proc.hpp"  
#include "proc/asset/struct.hpp"  
#include "proc/asset/track.hpp"  
#include "proc/asset/pipe.hpp"  
#include "proc/asset/meta.hpp"  
#include "proc/asset/procpatt.hpp"  


namespace asset
  {
  
  template ID<Asset> AssetManager::reg (Asset* obj, const Asset::Ident& idi);
  
  
  template P<Asset>  AssetManager::getAsset (const ID<Asset>&  id)  throw(lumiera::error::Invalid);
  template P<Media>  AssetManager::getAsset (const ID<Media>&  id)  throw(lumiera::error::Invalid);
  template P<Proc>   AssetManager::getAsset (const ID<Proc>&   id)  throw(lumiera::error::Invalid);
  template P<Struct> AssetManager::getAsset (const ID<Struct>& id)  throw(lumiera::error::Invalid);
  template P<Meta>   AssetManager::getAsset (const ID<Meta>&   id)  throw(lumiera::error::Invalid);
  
  template P<Asset>    AssetManager::wrap (const Asset& asset);
  template P<Media>    AssetManager::wrap (const Media& asset);
  template P<Clip>     AssetManager::wrap (const Clip&  asset);
  template P<Track>    AssetManager::wrap (const Track& asset);
  template P<Pipe>     AssetManager::wrap (const Pipe&  asset);
  template P<ProcPatt> AssetManager::wrap (const ProcPatt& asset);

  
} // namespace asset
