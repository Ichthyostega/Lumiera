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


/** @file asset/meta.hpp
 ** Internal and organisational metadata. Some internally created data elements
 ** rather serve the purpose of controlling the way the application behaves, as
 ** opposed to organising the [structure](\ref struct.hpp) of the data the user
 ** works with. Lumiera exposes this self-referential control and customisation
 ** aspects as a special kind of Asset. Examples being types, scales and quantisation
 ** grids, decision rules, control data stores (automation data), annotations attached
 ** to labels, inventory entities etc.
 ** 
 ** For the different _Kinds_ of Assets, we use sub-interfaces inheriting
 ** from the general Asset interface. To be able to get asset::Meta instances
 ** directly from the AssetManager, we define a specialisation of the Asset ID.
 ** 
 ** ## using meta assets
 ** 
 ** The usage pattern of asset::Meta entities differs from the other assets,
 ** insofar they aren't created as individual entries, rather added as part
 ** of a larger scale configuration activity, or they are derived from category.
 ** The latter fits in with a prototype-like approach; initially, the individual
 ** entry just serves to keep track of a categorisation, while at some point,
 ** such a link into a describing category may evolve into a local
 ** differentiation of some settings (copy on modification).
 ** 
 ** To cope with this special usage, the meta assets are defined to be immutable.
 ** They are created from a descriptor, which stands for a category or sub-category
 ** and can be another already existing asset::Meta (inheriting from meta::Descriptor)
 ** Generally this yields a Builder object, which can be used for outfitting the new
 ** or changed metadata entry, finally *committing* this builder to yield a new
 ** asset::Meta (which, in case of a mutation, might supersede an existing one).
 ** 
 ** @ingroup asset
 ** @see asset.hpp explanation of assets in general
 ** @see MetaFactory creating concrete asset::Meta instances
 **
 */

#ifndef ASSET_META_H
#define ASSET_META_H

#include "steam/asset.hpp"
#include "steam/asset/struct-scheme.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/nocopy.hpp"


namespace proc {
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
        virtual ~Descriptor();  ///< this is an Interface
      };
      
      /**
       * Building and configuring a meta asset.
       * The finished elements are defined to be immutable,
       * thus, on creation or when changing / superseding a
       * meta asset, the client gets a special builder instance,
       * which is a value object for configuring the specific details
       * to set. When done, the client invokes a `commit()` function,
       * which yields a smart-ptr to the new meta asset.
       * Individual meta asset subclasses are bound to define a
       * specialisation of this Builder template, which will then be
       * instantiated and provided by the MetaFactory.
       */
      template<class MA>
      struct Builder;
  }
  
  
  /**
   * key abstraction: metadata, parametrisation, customisation and similar organisational traits.
   * @todo just a stub, still have to figure out the distinctive properties of asset::Meta
   *                                                             //////////////////////////////////TICKET #1156 : do we need the distinction between STRUCT and META?
   */
  class Meta
    : public Asset
    , public meta::Descriptor
    {
    public:
      static MetaFactory create;
      
      /** @return ID of kind Meta */
      virtual const ID<Meta>& getID()  const
        {
          return static_cast<const ID<Meta>& > (Asset::getID());
        }
      
    protected:
      Meta (Asset::Ident const& idi)
        : Asset{idi}
        { }
      
      template<typename TY>
      Meta (lib::idi::EntryID<TY> const& nameID)
        : Meta{idi::getAssetIdent (nameID, META)}
        { }
      
//    friend class MetaFactory;  ///////////////////////////////////TODO still necessary?
    };
    
    
    // definition of ID<Meta> ctors is possible now,
   //  after providing full definition of class Meta
  
  inline ID<Meta>::ID(HashVal id)       : ID<Asset> (id)           {};
  inline ID<Meta>::ID(const Meta& meta) : ID<Asset> (meta.getID()) {};
  
  typedef lib::P<Meta> PMeta;
  
  
  
  /**
   * Factory specialised for creating Metadata Asset objects.
   */
  class MetaFactory
    : util::NonCopyable
    {
    public:
      typedef lib::P<asset::Meta> PType;
      
      template<class MA>
      meta::Builder<MA> operator() (lib::idi::EntryID<MA> elementIdentity);
      
      template<class MA>
      meta::Builder<MA> operator() (meta::Descriptor const& prototype, lib::idi::EntryID<MA> elementIdentity);
      
    };
  
  
  
  
}} // namespace proc::asset
#endif
