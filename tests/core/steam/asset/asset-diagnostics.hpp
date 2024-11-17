/*
  ASSET-DIAGNOSTICS.hpp  -  collection of test and debug helpers

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

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


#include "steam/assetmanager.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/util-foreach.hpp"
#include "lib/util.hpp"

#include <functional>

using util::contains;
using util::for_each;
using util::_Fmt;
using std::placeholders::_1;
using std::bind;
using std::string;
using std::cout;


namespace steam {
namespace asset {
  
  inline void
  dump (PcAsset const& aa)
  {
    if (!aa)
      cout << "Asset(NULL)\n";
    else
      {
        _Fmt fmt("%s %|50T.| id=%s  adr=%p smart-ptr=%p use-count=%u");
        cout << fmt % aa % aa->getID() % (void*)aa.get() % &aa % (aa.use_count() - 1) << "\n";
  }   }
  
  
  inline void
  dumpAssetManager ()
  {
    list<PcAsset> assets (AssetManager::instance().listContent());
    cout << "----all-registered-Assets----\n";
    for (auto const& pA : assets)
      dump (pA);
  }
  
  
  template<class CHI, class PAR>
  inline bool
  dependencyCheck (lib::P<CHI> child, lib::P<PAR> parent)
  {
    return (child == parent)
        || (0 < child->getParents().size()
           && (parent == child->getParents()[0])
           && (contains (parent->getDependant(), child)))
        ;
  }
  
  
  
  
}} // namespace steam::asset
#endif
