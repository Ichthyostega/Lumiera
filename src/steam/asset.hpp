/*
  ASSET.hpp  -  Superinterface: bookkeeping view of "things" present in the session

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

/** @file asset.hpp
 ** @ingroup asset
 ** Steam-Layer Interface: Assets.
 ** Declares the Asset top level Interface, the Asset::Ident identification tuple
 ** and the asset::ID primary key wrapper. Normally, Assets should be handled
 ** using asset::PAsset, a ref counting smart pointer.
 ** 
 ** These classes are placed into namespace asset and proc_interface.
 **
 ** Assets are handled by a hierarchy of interfaces. Below the top level Asset interface
 ** there are interfaces for various different <i>Kinds</i> of Assets, like asset::Media,
 ** asset::Proc, etc. Code utilising the specific properties of e.g. Media assets, will
 ** be implemented directly against the asset::Media interface. To make this feasible 
 ** while at the same time being able to handle all asset Kinds in a uniform manner, 
 ** we use a hierarchy of ID classes. These IDs are actually just thin wrappers around 
 ** a hash value, but they carry a template parameter specifying the Asset Kind and the
 ** Asset Kind subinterfaces provide a overloaded getID method with a covariant return
 ** value. For example the asset::Media#getID returns an ID<Media>. By using the
 ** templated query function AssetManager#getAsset, we can get at references to the more 
 ** specific subinterface asset::media just by using the ID value in a typesafe manner.
 ** This helps avoiding dynamic typing and switch-on-type, leading to more robust,
 ** extensible and clear code.
 **
 ** (Implementation detail: as g++ is not able to handle member function template
 ** instantiations completely automatic, we need to trigger some template instantiations
 ** at the end of assetmanager.cpp )
 ** 
 ** @see assetmanager.hpp
 ** @see media.hpp
 */


#ifndef PROC_INTERFACE_ASSET_H
#define PROC_INTERFACE_ASSET_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "include/logging.h"
#include "steam/asset/category.hpp"
#include "lib/hash-value.h"
#include "lib/p.hpp"

#include <boost/type_traits/is_base_of.hpp>
#include <boost/operators.hpp>

#include <cstddef>
#include <string>
#include <vector>
#include <set>


using std::string;
using std::vector;
using std::set;


namespace proc {
namespace asset {
  
  namespace error = lumiera::error;
  
  using std::size_t;
  using std::shared_ptr;
  using std::static_pointer_cast;
  
  using lib::HashVal;
  
  
  
  /** 
   * thin wrapper around a size_t hash ID 
   * used as primary key for all Asset objects.
   * The Template parameter is intended for tagging
   * the ID with type information, causing the
   * compiler to select specialised behaviour 
   * for the different kinds of Assets.
   * @see Asset
   * @see AssetManager#getID generating ID values
   * @see asset::Media
   * @see ID<asset::Media>
   * 
   * @todo it would be desirable to use EntryID instead ///////////////////////////////////////////TICKET #739 : make identification schemes compatible 
   */
  template<class KIND>
  class ID
    {
      HashVal hash_;
    public:
      ID (HashVal id)        : hash_(id)         {}
      ID (const KIND& asset) : hash_(asset.getID()) {}
      operator HashVal() const { return hash_; }
      
      static ID INVALID;
    };
    
  class DB;
  class Asset;
  class AssetManager;
  typedef const ID<Asset>& IDA;
  typedef lib::P<Asset>       PAsset;
  typedef lib::P<const Asset> PcAsset;

  
  
  
  
  /**
   * Superinterface describing especially bookkeeping properties.
   * As of 09/2007, there are four  <b>Kinds</b> of Assets, each
   * comprising a sub-Interface of the Asset Interface:
   * - asset::Media representing mediafiles
   * - asset::Proc representing media data processing components (e.g. Effects)
   * - asset::Struct representing structural components used in the session (e.g. Tracks)
   * - asset::Meta representing metadata, parametrisation, customisation.
   *  
   * And of course there are various concrete Asset subclasses, like asset::Clip,
   * asset::Effect, asset::Codec, asset::Dataset.
   * @note Assets objects have a strict unique identity and because of this are non-copyable.
   *       You can not create an Asset derived object without registering it with the AssetManager
   *       automatically. It is possible to copy the PAsset (smart pointer) though. 
   * 
   * @since 09/2007
   * @author Ichthyo
   */
  class Asset 
    : public boost::totally_ordered1< Asset
    , util::NonCopyable             >
    {
    public:

      /** 
       *  a POD comprised of all the information
       *  sufficiently identifying any given Asset. 
       */
      struct Ident
        : boost::totally_ordered<Ident>
        {
          /** element ID, comprehensible but sanitised.
           *  The tuple (category, name, org) is unique.
           */
          string name;
    
          /** primary tree like classification of the asset.
           *  Includes the distinction of different kinds of Assets,
           *  like Media, Audio, Video, Effects...  */
          asset::Category category;
    
          /** origin or authorship id. 
           *  Can be a project abbreviation, a package id or just the authors nickname or UID.
           *  This allows for the component name to be more generic (e.g. "blur"). 
           *  Default for all assets provided by the core Lumiera codebase is "lumi".
           */
          const string org;
    
          /** version number of the thing or concept represented by this asset.
           *  Of each unique tuple (name, category, org) there will be only one version 
           *  in the whole system. Version 0 is reserved for internal purposes. 
           *  Versions are considered to be ordered, and any higher version is 
           *  supposed to be fully backwards compatible to all previous versions.
           */
          const uint version;

          
          Ident (const string& n, 
                 const Category& cat, 
                 const string& o = "lumi", 
                 const uint ver=1);
          
          
          int compare (Ident const& other)   const;

          /** @note equality ignores version differences */
          bool operator== (Ident const& oi)  const { return compare (oi) ==0; }
          bool operator<  (Ident const& oi)  const { return compare (oi) < 0; }
    
          operator string ()  const;
          
          bool isValid()  const;
        };
      
         
      /* ===== Asset ID and Datafields ===== */
        
    public:    
      const Ident ident;     ///<  Asset identification tuple

      virtual const ID<Asset>& getID()   const { return id; }
      
      
      bool operator== (Asset const& oa)  const { return ident == oa.ident; }
      bool operator<  (Asset const& oa)  const { return ident <  oa.ident; }
          
      virtual operator string ()  const;
      
      
      
    protected:
      const ID<Asset> id;   ///<   Asset primary key.

      /** additional classification, selections or departments this asset belongs to.
       *  Groups are optional, non-exclusive and may be overlapping.
       */
      set<string> groups;

      /** user visible Name-ID. To be localised. */
      const string shortDesc;

      /** user visible qualification of the thing, unit or concept represented by this asset.
       *  preferably "in one line". To be localised.  */
      const string longDesc;
      
      vector<PAsset> parents;
      vector<PAsset> dependants;
      
      bool enabled;

      
      
    protected:
      /** Asset is a Interface class; usually, objects of 
       *  concrete subclasses are created via specialised Factories.
       *  Calling this base ctor causes registration with AssetManager.
       */
      Asset (const Ident& idi);
      virtual ~Asset()           = 0;    ///< @note Asset is abstract
      
      /** release all links to other Asset objects held internally.
       *  The lifecycle of Asset objects is managed by smart pointers
       *  and the Asset manager. Calling \c unlink() breaks interconnections
       *  to other Assets in the central Object network comprising the session.
       *  Especially, the \em downward links to dependent entities are released,
       *  while the primary (upward) smart-ptr links to our prerequisites are
       *  still retained. The rationale is, after releasing these redundant
       *  or cyclic interlinking, when the AssetManager removes its DB entry
       *  for this asset, the smart pointer goes out of scope and causes
       *  unwinding of the whole dependency chain.
       */
      virtual void unlink ();
      
      /** variant of #unlink() dropping only the links to the given specific
       *  Asset, leaving all other links intact. Usable for propagating  */
      virtual void unlink (IDA target);
      
      /** establish a connection between this and the given parent asset,
       *  denoting we are in some way dependent on the parent. */
      void defineDependency (PAsset parent);
      void defineDependency (Asset& parent); 
      
      friend class AssetManager;
      friend class DB;
      
    private:
      void unregister (PAsset& other);



    
    public:
      /** List of entities this asset depends on or requires to be functional. 
       *  May be empty. The head of this list can be considered the primary prerequisite
       */
      const vector<PAsset>& getParents ()  const { return parents; }
      
      /** All the other assets requiring this asset to be functional. 
       *  For example, all the clips depending on a given media file. 
       *  May be empty. The dependency relation is transitive.
       */
      const vector<PAsset>& getDependant ()  const { return dependants; }
      
      /** weather this asset is switched on and consequently 
       *  included in the fixture and participates in rendering
       */
      bool isActive ()  const;
      
      /** change the enabled status of this asset.
       *  Note the corresponding #isActive predicate may 
       *  depend on the enablement status of parent assets as well
       *  @return \c false if the state could not be changed
       *          due to parent objects being disabled
       */
      bool enable (bool on=true);
      
      
    };
    
    
    
    
  /* ====== ordering of Assets and Asset-Pointers ====== */
  
  /** ordering of Assets is based on the ordering
   *  of Ident tuples, which are supposed to be unique.
   *  By using our customised lumiera::P as smart ptr,
   *  comparison on P<Asset> ptrs will be automatically
   *  forwarded to the Asset comparison operators.
   *  @note version info is irrelevant */
  inline int 
  Asset::Ident::compare (Asset::Ident const& oi)  const
  { 
    int res;
    if (0 != (res=category.compare (oi.category)))  return res;
    if (0 != (res=org.compare (oi.org)))            return res;
    return name.compare (oi.name);
  }
  
  
  /** promote subtype-ptr to PAsset, e.g. for comparing */
  template<class A>
  inline const PcAsset
  pAsset (shared_ptr<A> const& subPtr)
  {
    return static_pointer_cast<const Asset,A> (subPtr);
  }
  
  
  /** type trait for detecting a shared-ptr-to-asset */
  template <class X>
  struct is_pAsset : boost::false_type {};
  
  template <class A>
  struct is_pAsset<shared_ptr<A>>
    : boost::is_base_of<Asset, A>      {};
  
  
  /** marker constant denoting a NIL asset */
  template<class KIND>
  ID<KIND> ID<KIND>::INVALID = ID{0};
  
  
}} // namespace proc::asset



namespace proc_interface {
  
  using proc::asset::Asset;
  using proc::asset::Category;
  using proc::asset::ID;
  using proc::asset::IDA;
  using proc::asset::PAsset;
}

#endif
