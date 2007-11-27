/*
  ASSETDIAGNOSTICS.hpp  -  collection of test and debug helpers
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
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

/** @file assetdiagnostics.hpp
 ** Small helper and diagnosic functions related to Asset and AssetManager
 ** 
 ** @see assetmanager.hpp
 ** @see CreateAsset_test
 ** @see IdentityOfAssets_test
 */


#ifndef ASSET_ASSETDIAGNOSTICS_H
#define ASSET_ASSETDIAGNOSTICS_H


#include "proc/assetmanager.hpp"
#include "common/util.hpp"

#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <iostream>

using util::contains;
using util::for_each;
using boost::format;
using boost::bind;
using std::string;
using std::cout;


namespace asset
  {
 
  inline void
  dump (PcAsset& aa)
    {
      if (!aa)
        cout << "Asset(NULL)\n";
      else
        {
          format fmt("%s %|50T.| id=%s  adr=%x smart-ptr=%x use-count=%d");
          cout << fmt % str(aa) % aa->getID() % aa.get() % &aa % (aa.use_count() - 1) << "\n";
    }   }
 
  inline void
  dumpAssetManager ()
    {
      list<PcAsset> assets (AssetManager::instance().listContent());
      cout << "----all-registered-Assets----\n";
      for_each (assets, bind (&dump, _1));
    }
  
  
  template<class C, class P>
  inline bool
  dependencyCheck (C child, P parent)
    {
      return (child == parent)
          || (0 < child->getParents().size()
             && (parent == child->getParents()[0])
             && (contains (parent->getDependant(), child)));
          ;
    }

    
    

} // namespace asset
#endif
