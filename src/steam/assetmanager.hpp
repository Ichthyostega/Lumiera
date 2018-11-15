/*
  ASSETMANAGER.hpp  -  Facade for the Asset subsystem

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

*/

/** @file assetmanager.hpp
 ** Steam-Layer Interface: Asset Lookup and Organisation.
 ** Declares the AssetManager interface used to access individual 
 ** Asset instances.
 ** 
 ** These classes are placed into namespace asset and proc_interface.
 **
 ** @todo 10/10 meanwhile I'm unhappy with some aspects of this implementation //////////////TICKET #691
 ** @todo 12/12 and meanwhile I'm way more unhappy with the whole design. Looks like I wasn't proficient
 **             enough with OO design in C++ when writing that original design. Especially, this design
 **             couples the object creation, the memory management and the registration way too tightly.
 **             Having a registration magic in the asset baseclass ctor is just too clever and a half.
 **             Rather, there should be a responsibility chain, using completely passive Asset objects:
 **             Factory -> AssetManaager -> Asset object
 ** 
 ** @see asset.hpp
 ** @see mobject.hpp
 */


#ifndef PROC_INTERFACE_ASSETMANAGER_H
#define PROC_INTERFACE_ASSETMANAGER_H


#include "steam/asset.hpp"
#include "lib/error.hpp"
#include "lib/depend.hpp"


#include <cstddef>
#include <string>
#include <list>
#include <boost/utility.hpp>

using std::string;
using std::list;


namespace proc {
namespace asset {
  
  class DB;
  
  
  /**
   * Facade for the Asset subsystem
   */
  class AssetManager 
    : util::NonCopyable
    {
      asset::DB & registry;
    
    
    public:
      static lib::Depend<AssetManager> instance;
      
      /** provide the unique ID for given Asset::Ident tuple */
      static ID<Asset> getID (const Asset::Ident&);
      
      /** retrieve the registered smart-ptr for any asset */
      template<class KIND>
      static lib::P<KIND> wrap (const KIND& asset);
      
      /** find and return corresponding object */
      template<class KIND>
      lib::P<KIND>  getAsset (const ID<KIND>& id);
      
      
      /** @return true if the given id is registered in the internal asset DB  */
      bool known (IDA id) ;
      
      /** @return true if the given id is registered with the given Category  */
      bool known (IDA id, const Category& cat) ;
      
      /** remove the given asset from the internal DB.
       * <i>together with all its dependents</i> */
      void remove (IDA id) ;
      
      /** deregister and evict all known Assets.
       * @note the actual object instances are managed by reference count,
       *       i.e. typically the Assets will be kept alive by MObjects from the session
       * @warning unsure if this design is sane. Asset subsystem needs a rework   ////////////////////////////TICKET #691
       * @todo verify this actually works, especially with session shutdown       ////////////////////////////TICKET #154
       */
      void clear() ;
      
      /** extract a sorted list of all registered Assets */
      list<PcAsset> listContent() const;
      
      
      
    protected:
      /** registers an asset object in the internal DB, providing its unique key.
       *  @internal used by the Asset base class ctor to create Asset::id.
       */
      template<class KIND>
      static ID<KIND>
      reg (KIND* obj, const Asset::Ident& idi);
      
      /** deleter function used by the Asset smart pointers to delete Asset objects */
      static void destroy (Asset* aa) { delete aa; }
      
      friend Asset::Asset (Asset::Ident const& idi);
      
      AssetManager();
      
      friend class lib::DependencyFactory<AssetManager>;
      
    };
    
    
    LUMIERA_ERROR_DECLARE (UNKNOWN_ASSET_ID);  ///< use of a non-registered Asset ID.
    LUMIERA_ERROR_DECLARE (WRONG_ASSET_KIND);  ///< Asset ID of wrong Asset kind, unable to cast.

}} // namespace proc::asset



namespace proc_interface
  {
  using proc::asset::AssetManager;
}

#endif
