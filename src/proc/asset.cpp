/*
  Asset}  -  Superinterface: bookeeping view of "things" present in the session
 
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


#include "proc/asset.hpp"
#include "proc/assetmanager.hpp"
#include "common/util.hpp"

#include <boost/function.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>


using boost::bind;
using boost::format;
using boost::function;
using util::contains;
using util::removeall;
using util::for_each;
using util::cStr;


namespace asset
  {

  Asset::Ident::Ident(const string& n, const Category& cat, const string& o, const uint ver) 
    :   name(util::sanitize (n)), 
        category(cat), org(o), version(ver)
  { }

  
  /** Asset is a Interface class; usually, objects of 
   *  concrete subclasses are created via specialized Factories
   */
  Asset::Asset (const Ident& idi) 
    : ident(idi), id(AssetManager::reg (this, idi)) 
  {
    TRACE (assetmem, "ctor Asset(id=%lu) :  adr=%x %s", size_t(id), this, cStr(this->ident) );
  }
  
  Asset::~Asset ()
  { 
    TRACE (assetmem, "dtor Asset(id=%lu) :  adr=%x", size_t(id), this );
  }
  

  Asset::Ident::operator string ()  const
  {
    format id_tuple("(%2%:%3%.%1% v%4%)");
    return str (id_tuple % name % category % org % version);
  }
  

  Asset::operator string ()  const
  {
    format id_tuple("Asset(%2%:%3%.%1% v%4%)");
    return str (id_tuple % ident.name % ident.category % ident.org % ident.version);
  }

  
  /**
   * whether this asset is swithced on and consequently included
   * in the fixture and participates in rendering.
   */
  bool
  Asset::isActive ()  const
  {
    UNIMPLEMENTED ("enable/disable Assets.");
  }


  /**
   * change the enablement status of this asset. 
   * @note the corresponding #isActive predicate may depend 
   *       on the enablement status of parent assets as well.
   */
  void
  Asset::enable ()  throw(cinelerra::error::State)
  {
    UNIMPLEMENTED ("enable/disable Assets.");
  }
  
  
  
  
  void
  Asset::unregister (PAsset& other)  ///< @internal
  {
    other->unlink (this->id);  
  }
    
  /** release all links to other Asset objects held internally. */
  void 
  Asset::unlink ()
  {
    function<void(PAsset&)> unregister_me = bind(&Asset::unregister, this,_1);
    
    for_each (parents, unregister_me);
    for_each (dependants, unregister_me);
    dependants.clear();
    parents.clear();
  }
      
  /** variant dropping only the links to the given Asset */
  void 
  Asset::unlink (IDA target)
  {
    PAsset asset (AssetManager::instance().getAsset (target));
    removeall (dependants,asset);
    removeall (parents,asset);
  }
  
  
  void 
  Asset::defineDependency (PAsset parent)
  {
    PAsset p_this (AssetManager::getPtr(*this));
    REQUIRE (!contains (parent->dependants, p_this));
    REQUIRE (!contains (this->parents, parent));
    parents.push_back (parent);
    parent->dependants.push_back(p_this);
  }

  
} // namespace asset
