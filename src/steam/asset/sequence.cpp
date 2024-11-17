/*
  Sequence  -  Compound of MObjects placed within a tree of tracks. Building block of the Session

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file asset/sequence.cpp
 ** Implementation parts of the Sequence entity within the Session model
 */


#include "steam/asset/sequence.hpp"
//#include "steam/mobject/session/fork.hpp"
#include "steam/assetmanager.hpp"
//#include "steam/mobject/placement.hpp"
//#include "steam/mobject/session/mobjectfactory.hpp"

namespace steam {
namespace asset {
  
  using lib::AutoRegistered;
  
  
  /** create an empty default configured Sequence */
  Sequence::Sequence (const Asset::Ident& idi)
    : Struct (idi)
  { }
  
  
  PSequence
  Sequence::create (Asset::Ident const& idi)
  {
    REQUIRE (getRegistry, "can't create a Sequence prior to session initialisation");
    
    PSequence newSeq (AssetManager::instance().wrap (*new Sequence (idi)));
    getRegistry().append (newSeq);
    
    ENSURE (newSeq);
    ENSURE (getRegistry().isRegistered (*newSeq));
    return newSeq;
  }
  
  
  void
  Sequence::unlink ()
  {
    AutoRegistered<Sequence>::detach();
    TODO ("purge attached fork");             //////////////////////////////////////////////TICKET #692
    Struct::unlink();
  }
  
  
  
}} // namespace asset
