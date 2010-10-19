/*
  Asset  -  Superinterface: bookkeeping view of "things" present in the session
 
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


#include "proc/asset.hpp"
#include "proc/assetmanager.hpp"
#include "lib/util-foreach.hpp"
#include "lib/util.hpp"

#include <boost/format.hpp>
#include <tr1/functional>


using std::tr1::function;
using std::tr1::placeholders::_1;
using std::tr1::bind;
using boost::format;
using util::contains;
using util::removeall;
using util::for_each;
using util::and_all;
using util::cStr;


namespace asset {
  
  using ::NOBUG_FLAG(memory);
  NOBUG_CPP_DEFINE_FLAG_PARENT(assetmem,  memory);
  
  
  Asset::Ident::Ident(const string& n, const Category& cat, const string& o, const uint ver) 
    : name(util::sanitise (n))
    , category(cat)
    , org(o)
    , version(ver)
    { }
  
  
  /** Asset is a Interface class; usually, objects of 
   *  concrete subclasses are created via specialised Factories
   */
  Asset::Asset (const Ident& idi) 
    : ident(idi)
    , id(AssetManager::reg (this, idi))
    , enabled(true)
    {
      TRACE (assetmem, "ctor Asset(id=%lu) :  adr=%p %s", size_t(id), this, cStr(this->ident) );
    }
  
  Asset::~Asset ()
    { 
      TRACE (assetmem, "dtor Asset(id=%lu) :  adr=%p", size_t(id), this );
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


  
  
  function<bool(const PAsset&)> check_isActive
    = bind ( &Asset::isActive
           , bind (&PAsset::get, _1 )
           );
  
  bool
  all_parents_enabled (const vector<PAsset>& parents)
  {
    return and_all (parents, check_isActive);
  }
  
  /**
   * whether this asset is switched on and consequently included
   * in the fixture and participates in rendering.
   */
  bool
  Asset::isActive ()  const
  {
    return this->enabled
        && all_parents_enabled (parents);
  }
  
  
  void
  propagate_down (PAsset child, bool on)
  {
    child->enable(on);
  }

  /**change the enablement status of this asset. */
  bool
  Asset::enable (bool on)  throw(lumiera::error::State)
  {
    if (on == this->enabled)
      return true;
    if (on && !all_parents_enabled (parents))
      return false;
    
    // can indeed to do the toggle...
    this->enabled = on;
    for_each (dependants, &propagate_down, _1 ,on);
    return true;
  }
  
  
  
  
  void
  Asset::unregister (PAsset& other)  ///< @internal
  {
    other->unlink (this->id);  
  }
    
  /** release all links to other <i>dependent</i> 
   *  asset objects held internally and advise all parent
   *  assets to do so with the link to this asset. 
   *  @note we don't release upward links to parent assets,
   *        thus effectively keeping the parents alive, because
   *        frequently the accessibility of parent assets is part
   *        of our own contract. (e.g. media for clip assets) 
   */
  void 
  Asset::unlink ()
  {
    function<void(PAsset&)> forget_me = bind(&Asset::unregister, this, _1);
    
    for_each (parents, forget_me);
    dependants.clear();
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
    PAsset p_this (AssetManager::wrap(*this));
    REQUIRE (!contains (parent->dependants, p_this));
    REQUIRE (!contains (this->parents, parent));
    parents.push_back (parent);
    parent->dependants.push_back(p_this);
  }
  
  void 
  Asset::defineDependency (Asset& parent)
  {
    PAsset p_parent (AssetManager::wrap(parent));
    ASSERT (p_parent);
    defineDependency (p_parent);
  }

  
} // namespace asset
