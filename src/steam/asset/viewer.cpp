/*
  Viewer  -  asset corresponding to a viewer element in the GUI

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file viewer.cpp
 ** Implementation functions to represent a viewer or viewer attachment point.
 ** @todo half finished draft, topic was postponed as of 2014, but will become relevant again
 */


#include "steam/asset/viewer.hpp"
//#include "steam/mobject/session/fork.hpp"
//#include "steam/mobject/placement.hpp"
//#include "steam/mobject/session/mobjectfactory.hpp"
//#include "steam/mobject/session/binding.hpp"
//#include "steam/assetmanager.hpp"


namespace steam {
namespace asset {
  
//  using lib::AutoRegistered;
  
  
  
  /** @todo anything significant to do here??? */
  Viewer::Viewer (const Asset::Ident& idi)
    : Struct (idi)
  {
    UNIMPLEMENTED ("anything regarding Viewer Assets");
  }
  
  
//PViewer
//Viewer::create (Asset::Ident const& idi)
//{
//  REQUIRE (getRegistry, "can't create a Timeline prior to session initialisation");
//  
//  PTimeline newElement (AssetManager::instance().wrap (*new Viewer(idi)));
//  getRegistry().append (newElement);
//  
//  ENSURE (newElement);
//  ENSURE (getRegistry().isRegistered (*newElement));
//  return newElement;
//}
  
  
  void
  Viewer::unlink ()
  {
//  AutoRegistered<Timeline>::detach();
    Struct::unlink();
  }
  
  
  
  
}} // namespace steam::asset
