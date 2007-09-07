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
#include "common/singleton.hpp"

//#include <boost/functional/hash.hpp>

using cinelerra::Singleton;

namespace asset
  {
  
  /** get at the system-wide asset manager instance.
   *  Implemented as singleton.
   */
  AssetManager& 
  AssetManager::instance ()
  {
    return Singleton<AssetManager>::instance();
  }
  
  AssetManager::AssetManager ()
    : registry (Singleton<asset::DB>::instance())
  { }


  
  /** provide the unique ID for given Asset::Ident tuple */
  ID<Asset> 
  AssetManager::getID (const Asset::Ident& idi)
  {
    return asset::hash_value (idi);
  }
  
  
  
  /**
   * registers an asset object in the internal DB, providing its unique key
   */
  template<class KIND>
  ID<KIND>  
  AssetManager::reg (KIND& obj, const Asset::Ident& idi)
    //throw(cinelerra::error::Invalid)
  {
  }
  
  
  /**
   * find and return corresponging object
   */
  template<class KIND>
  shared_ptr<KIND>
  AssetManager::getAsset (const ID<KIND>& id)  ////throw(cinelerra::Invalid)
  {
  }


  /**
   * @return true if the given id is registered in the internal asset DB
   */
  bool
  AssetManager::known (IDA id)
  {
  }


  /**
   * remove the given asset <i>together with all its dependants</i> from the internal DB
   */
  void
  AssetManager::remove (IDA id)  /////throw(cinelerra::Invalid, cinelerra::State)
  {
  }



} // namespace asset
