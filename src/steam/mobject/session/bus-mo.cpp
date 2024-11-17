/*
  BusMO  -  attachment point to form a global pipe

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file bus-mo.cpp
 ** Implementation details for a _processing pipe_ representation in the Session model
 ** @todo stalled effort towards a session implementation from 2010
 ** @todo 2016 likely to stay, but expect some extensive rework
 */


#include "steam/mobject/session/bus-mo.hpp"

namespace steam {
namespace mobject {
namespace session {
  
  /** */
  BusMO::BusMO (PPipe const& pipe_to_represent)
    : pipe_(pipe_to_represent)
    {
      throwIfInvalid();
      TODO ("what additionally to do when rooting a global pipe??");
    }
  
  
  bool
  BusMO::isValid()  const
  {
    TODO ("self-check of a global pipe within the model"); ///////////////////////////////TICKET #584
    return true;
    // Ideas: - maybe re-access the pipe "from outward"
    //        - and then verify matching WiringClaim in the corresponding placement
  }
  
  
  
  
}}} // namespace steam::mobject::session
