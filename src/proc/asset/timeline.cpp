/*
  Timeline  -  independent top-level element of the Session

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file timeline.cpp
 ** Implementation functions of the Timeline entity within the Session model
 */


#include "proc/asset/timeline.hpp"
#include "proc/mobject/session/binding.hpp"
#include "proc/assetmanager.hpp"


namespace proc {
namespace asset {
  
  using lib::AutoRegistered;
  
  
  
  /** @todo anything significant to do here??? */
  Timeline::Timeline (const Asset::Ident& idi, RBinding const& sequenceBinding)
    : Struct (idi)
    , boundSequence_(sequenceBinding)
  {
    REQUIRE (boundSequence_);
  }
  
  
  PTimeline
  Timeline::create (Asset::Ident const& idi, RBinding const& sequenceBinding)
  {
    REQUIRE (getRegistry, "can't create a Timeline prior to session initialisation");
    
    PTimeline newElement (AssetManager::instance().wrap (*new Timeline(idi, sequenceBinding)));
    getRegistry().append (newElement);
    
    ENSURE (newElement);
    ENSURE (getRegistry().isRegistered (*newElement));
    return newElement;
  }
  
  
  void
  Timeline::unlink ()
  {
    AutoRegistered<Timeline>::detach();
    boundSequence_.purge();
    Struct::unlink();
  }
  
  
  
  
}} // namespace asset
