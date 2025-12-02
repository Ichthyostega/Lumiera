/*
  Binding  -  link to use a sequence within the session

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file session/binding.cpp
 ** Implementation details of the Binding MObject to tie a sequence into a timeline or virtual clip
 ** @todo stalled effort towards a session implementation from 2010
 ** @todo 2016 likely to stay, but expect some extensive rework
 */


#include "steam/mobject/session/binding.hpp"
#include "steam/asset/sequence.hpp"

namespace steam {
namespace mobject {
namespace session {
  
  /** */
  Binding::Binding (PSequence const& sequence_to_bind)
    : boundSequence_(sequence_to_bind)
    {
      throwIfInvalid();
      TODO ("what additionally to do when binding a sequence??");
    }
  
  
  bool
  Binding::isValid()  const
  {
    TODO ("self-check of a binding within the model"); ///////////////////////////////TICKET #584
    return true;
    // Ideas: - compare the bound sequence's channels with this binding's channel configuration
    //        - verify the virtual media, unless this biding is creating a top-level timeline
    //        - verify the bound sequence is based on a fork within the model
  }

  


}}} // namespace steam::mobject::session
