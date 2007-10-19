/*
  AssetManager  -  Facade for the Asset subsystem
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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

#include "common/multithread.hpp"
#include "common/util.hpp"

//#include <boost/lambda/lambda.hpp>
#include <boost/function.hpp>

#include <boost/format.hpp>
#include <boost/bind.hpp>

using boost::format;
using boost::bind;
//using boost::lambda::_1;
using util::for_each;

using cinelerra::Singleton;
using cinelerra::Thread;


namespace asset
  {
  
  /** 
   * AssetManager error responses, caused by querying
   * invalid Asset IDs from the internal DB.
   */ 
  class IDErr : public cinelerra::error::Invalid 
    {
    public:
      IDErr (const char* eID, format fmt) 
        : cinelerra::error::Invalid(fmt.str(),eID) {}
    };

  
   // ------pre-defined-common-error-cases---------------
  //
  CINELERRA_ERROR_DEFINE (UNKNOWN_ASSET_ID, "non-registered Asset ID");
  CINELERRA_ERROR_DEFINE (WRONG_ASSET_KIND, "wrong Asset kind, unable to cast");
  
  class UnknownID : public IDErr
    {
    public:
      UnknownID (ID<Asset> aID) : IDErr (CINELERRA_ERROR_UNKNOWN_ASSET_ID, 
                                         format("Query for Asset with ID=%d, which up to now "
                                                "hasn't been created or encountered.") % aID) {}
    };
  
  class WrongKind : public IDErr
    {
    public:
      WrongKind (Asset::Ident idi) : IDErr (CINELERRA_ERROR_WRONG_ASSET_KIND,
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
      throw(cinelerra::error::Invalid)
  {
    typedef shared_ptr<KIND> PType;
    AssetManager& _aMang (AssetManager::instance());
    TODO ("check validity of Ident Category");
    ID<KIND> asset_id (getID (idi));
    
    Thread::Lock<DB> guard   SIDEEFFECT;
    TODO ("handle duplicate Registrations");
    PType smart_ptr (obj, &destroy);

    _aMang.registry.put (asset_id, smart_ptr);
    return asset_id;
  }
  
  
  /**
   * find and return the object registered with the given ID.
   * @throws Invalid if nothing is found or if the actual KIND
   *         of the stored object differs and can't be casted.  
   */
  template<class KIND>
  shared_ptr<KIND>
  AssetManager::getAsset (const ID<KIND>& id)  
      throw(cinelerra::error::Invalid)
  {
    if (shared_ptr<KIND> obj = registry.get (id))
      return obj;
    else
      if (known (id))    // provide Ident tuple of existing Asset 
        throw WrongKind (registry.get(ID<Asset>(id))->ident);
      else
        throw UnknownID (id);
  }


  /**
   * @return true if the given id is registered in the internal asset DB
   */
  bool
  AssetManager::known (IDA id)
  {
    return ( registry.get (ID<Asset>(id)) );
  }       // query most general Asset ID-kind and use implicit 
         //  conversion from shared_ptr to bool (test if empty)


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
  AssetManager::detach_child (PAsset& pA, IDA id)
  {
    pA->unlink(id);
  }

  /**
   * remove the given asset <i>together with all its dependants</i> from the internal DB
   */
  void
  AssetManager::remove (IDA id)  
      throw(cinelerra::error::Invalid, 
            cinelerra::error::State)
  {
    UNIMPLEMENTED ("remove Asset with all dependecies");
    
    PAsset pA = getAsset (id);
    vector<PAsset> par = pA->getParents();
    boost::function<void(PAsset&)> func = bind(&detach_child, _1,id ); 
    for_each (par, func); //   ,boost::lambda::var(id)));
  }

  
  
  list<PAsset> 
  AssetManager::listContent() const
  {
    list<PAsset> res;
    registry.asList (res);
    res.sort();
    return res;
  }

  
} // namespace asset




   /************************************************************/
   /* explicit template instantiations for various Asset Kinds */
   /************************************************************/

#include "proc/asset/media.hpp"  
#include "proc/asset/proc.hpp"  
#include "proc/asset/struct.hpp"  
#include "proc/asset/meta.hpp"  


namespace asset
  {
  
  template ID<Asset> AssetManager::reg (Asset* obj, const Asset::Ident& idi);
  
  
  template shared_ptr<Asset>  AssetManager::getAsset (const ID<Asset>&  id)  throw(cinelerra::error::Invalid);
  template shared_ptr<Media>  AssetManager::getAsset (const ID<Media>&  id)  throw(cinelerra::error::Invalid);
  template shared_ptr<Proc>   AssetManager::getAsset (const ID<Proc>&   id)  throw(cinelerra::error::Invalid);
  template shared_ptr<Struct> AssetManager::getAsset (const ID<Struct>& id)  throw(cinelerra::error::Invalid);
  template shared_ptr<Meta>   AssetManager::getAsset (const ID<Meta>&   id)  throw(cinelerra::error::Invalid);
  
} // namespace asset
