/*
  Sequence  -  Compound of MObjects placed on a track tree. Building block of the Session

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


#include "proc/asset/sequence.hpp"
//#include "proc/mobject/session/fork.hpp"
#include "proc/assetmanager.hpp"
//#include "proc/mobject/placement.hpp"
//#include "proc/mobject/session/mobjectfactory.hpp"

namespace proc {
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
    TODO ("purge attached track");             //////////////////////////////////////////////TICKET #692
    Struct::unlink();
  }
  
  
  
}} // namespace asset
