/*
  ASSET-DIAGNOSTICS.hpp  -  collection of test and debug helpers

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

/** @file asset-diagnostics.hpp
 ** Small helper and diagnostic functions related to Asset and AssetManager
 ** 
 ** @see assetmanager.hpp
 ** @see CreateAsset_test
 ** @see IdentityOfAssets_test
 */


#ifndef ASSET_ASSET_DIAGNOSTICS_H
#define ASSET_ASSET_DIAGNOSTICS_H


#include "proc/assetmanager.hpp"
#include "lib/util-foreach.hpp"
#include "lib/util.hpp"

#include <boost/format.hpp>
#include <tr1/functional>
#include <iostream>

using util::contains;
using util::for_each;
using std::tr1::placeholders::_1;
using std::tr1::bind;
using boost::format;
using std::string;
using std::cout;


namespace asset {
  
  inline void
  dump (PcAsset& aa)
  {
    if (!aa)
      cout << "Asset(NULL)\n";
    else
      {
        format fmt("%s %|50T.| id=%s  adr=%p smart-ptr=%p use-count=%u");
        cout << fmt % str(aa) % aa->getID() % aa.get() % &aa % (aa.use_count() - 1) << "\n";
  }   }
  
  
  inline void
  dumpAssetManager ()
  {
    list<PcAsset> assets (AssetManager::instance().listContent());
    cout << "----all-registered-Assets----\n";
    for_each (assets, bind (&dump, _1));
  }
  
  
  template<class CHI, class PAR>
  inline bool
  dependencyCheck (P<CHI> child, P<PAR> parent)
  {
    return (child == parent)
        || (0 < child->getParents().size()
           && (parent == child->getParents()[0])
           && (contains (parent->getDependant(), child)))
        ;
  }
  
  
  
  
}} // namespace proc::asset
#endif
