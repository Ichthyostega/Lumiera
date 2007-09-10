/*
  ASSETMANAGER.hpp  -  Facade for the Asset subsystem
 
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
 
*/

/** @file assetmanager.hpp
 ** Proc-Layer Interface: Asset Lookup and Organization.
 ** Declares the AssetManager interface used to access individual 
 ** Asset instances.
 ** 
 ** These classes are placed into namespace asset and proc_interface. 
 **
 ** @see asset.hpp
 ** @see mobject.hpp
 */


#ifndef PROC_INTERFACE_ASSETMANAGER_H
#define PROC_INTERFACE_ASSETMANAGER_H


#include "proc/asset.hpp"
#include "common/error.hpp"

#include <cstddef>
#include <string>
#include <boost/utility.hpp>

using std::string;



namespace asset
  {
  
  class DB;
  
  
  /**
   * Facade for the Asset subsystem
   */
  class AssetManager : private boost::noncopyable
    {
      asset::DB & registry;
    
    
    public:
      static AssetManager& instance();
      
      /** provide the unique ID for given Asset::Ident tuple */
      static ID<Asset> getID (const Asset::Ident&);
      
      
      /** find and return corresponging object */
      template<class KIND>
      shared_ptr<KIND>  getAsset (const ID<KIND>& id)  throw(cinelerra::error::Invalid);
      
      
      /** @return true if the given id is registered in the internal asset DB  */
      bool known (IDA id) ;
      
      /** @return true if the given id is registered with the given Category  */
      bool known (IDA id, const Category& cat) ;
      
      /**remove the given asset from the internal DB.
       * <i>together with all its dependants</i> */
      void remove (IDA id)  throw(cinelerra::error::Invalid, 
                                  cinelerra::error::State);
      
      
    protected:
      /** registers an asset object in the internal DB, providing its unique key.
       *  @internal used by the Asset base class ctor to create Asset::id.
       */
      template<class KIND>
      static ID<KIND>  reg (KIND& obj, const Asset::Ident& idi)
          throw(cinelerra::error::Invalid);
      
      friend Asset::Asset (const Asset::Ident& idi);
      
      AssetManager ();
    
    };

} // namespace asset



namespace proc_interface
  {
  using asset::AssetManager;
}

#endif
