/*
  META.hpp  -  abstract base class of all MObjects representing meta data or processing instructions

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file session/meta.hpp
 ** Intermediate Asset interface: metadata and processing instructions
 ** @todo stalled effort towards a session implementation from 2008
 ** @todo 2016 likely to stay, but expect some extensive rework
 */


#ifndef STEAM_MOBJECT_SESSION_META_H
#define STEAM_MOBJECT_SESSION_META_H

#include "steam/mobject/session/abstractmo.hpp"


namespace steam {
namespace mobject {
namespace session {
  
  
  /**
   * Meta-MObject doesn't represent real Media Content,
   * but rather all sorts of Processing Instructions
   * and other metadata, which can be placed and
   * attached within the Model/Session.
   * @todo do we need this abstract baseclass?
   */
  class Meta : public AbstractMO
    {
      ///////////
      //////////////////////////////TICKET #448   what to do with the length here??
      
      string
      initShortID()  const
        {
          return buildShortID("MetaMO");
        }
    };
  
  
  
}}} // namespace mobject::session
#endif
