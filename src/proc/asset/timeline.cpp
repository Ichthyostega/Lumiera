/*
  Timeline  -  independent top-level element of the Session
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "proc/asset/timeline.hpp"
//#include "proc/mobject/session/track.hpp"
//#include "proc/mobject/placement.hpp"
//#include "proc/mobject/session/mobjectfactory.hpp"
#include "proc/mobject/session/binding.hpp"

namespace asset {
  
  
  
  /** TODO??? */
  Timeline::Timeline (const Asset::Ident& idi, RBinding const& sequenceBinding)
    : Struct (idi)
    , boundSeqence_(sequenceBinding)
//  : track (makeDefaultTrack ())
//  , clips (0)
  {
    REQUIRE (boundSeqence_);
  }
  
  
  
  
  
  
} // namespace asset
