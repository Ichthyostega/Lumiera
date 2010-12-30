/*
  META.hpp  -  key abstraction: metadata and organisational asset

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


/** @file meta.hpp
 ** Internal and organisational metadata. Some internally created data elements,
 ** like automation data sets, inventory of session contents, can be exposed and 
 ** treated as specific Kind of Asset.
 ** 
 ** For the different <i>Kinds</i> of Assets, we use sub-interfaces inheriting
 ** from the general Asset interface. To be able to get asset::Meta instances
 ** directly from the AssetManager, we define a specialisation of the Asset ID.
 ** 
 ** @see asset.hpp explanation of assets in general
 ** @see MetaFactory creating concrete asset::Meta instances
 **
 */

#ifndef ASSET_META_H
#define ASSET_META_H

#include "pre_a.hpp"

#include "proc/asset.hpp"
#include "proc/asset/entry-id.hpp"
#include "lib/factory.hpp"



namespace asset {
  
  class Meta;
  class MetaFactory;
  
  
  template<>
  class ID<Meta> : public ID<Asset>
    {
    public:
      ID (HashVal id);
      ID (const Meta&);
    };
  
  namespace meta {
    
    /**
     * Interface: the unspecific, reflective base of meta assets.
     * Based on descriptors, meta assets form a self referential structure.
     */
    class Descriptor
      {
      public:
        virtual ~Descriptor();  ///< this is an ABC
      };
  }
  
  
  /**
   * key abstraction: metadata, parametrisation, customisation and similar organisational traits.
   * @todo just a stub, still have to figure out the distinctive properties of asset::Meta
   */
  class Meta
    : public Asset
    , public meta::Descriptor
    {
    public:
      static MetaFactory create;
      
      virtual const ID<Meta>& getID()  const    ///< @return ID of kind Meta 
        { 
          return static_cast<const ID<Meta>& > (Asset::getID()); 
        }
      
    protected:
      Meta (Asset::Ident const& idi) : Asset(idi) {}  //////////////TODO
      
//    friend class MetaFactory;  ///////////////////////////////////TODO still necessary?
    };
    
    
    // definition of ID<Meta> ctors is possible now,
   //  after providing full definition of class Meta
  
  inline ID<Meta>::ID(HashVal id)       : ID<Asset> (id)           {};
  inline ID<Meta>::ID(const Meta& meta) : ID<Asset> (meta.getID()) {};
  
  typedef P<Meta> PMeta;
  
  
  
  /** 
   * Factory specialised for creating Metadata Asset objects.
   */ 
  class MetaFactory : public lib::Factory<asset::Meta>
    {
    public:
      typedef P<asset::Meta> PType;
      
      template<class MA>
      P<MA> operator() (EntryID<MA> elementIdentity); 
      
      template<class MA>
      P<MA> operator() (meta::Descriptor const& prototype, EntryID<MA> elementIdentity); 
      
    };
  
  
  
  
} // namespace asset
#endif
