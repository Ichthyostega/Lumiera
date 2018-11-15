/*
  Viewer  -  asset corresponding to a viewer element in the GUI

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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

* *****************************************************/


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


namespace proc {
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
  
  
  
  
}} // namespace proc::asset
