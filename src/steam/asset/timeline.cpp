/*
  Timeline  -  independent top-level element of the Session

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file timeline.cpp
 ** Implementation functions of the Timeline entity within the Session model
 */


#include "steam/asset/timeline.hpp"
#include "steam/mobject/session/binding.hpp"
#include "steam/assetmanager.hpp"


namespace steam {
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
