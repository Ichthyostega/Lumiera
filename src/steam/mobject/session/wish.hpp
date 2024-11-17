/*
  WISH.hpp  -  LocatingPin representing a low-priority directive

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file wish.hpp
 ** @todo result of the very first code generation from UML in 2008. Relevance not clear yet...
 ** @deprecated WTF?
 */


#ifndef MOBJECT_SESSION_WISH_H
#define MOBJECT_SESSION_WISH_H

#include "steam/mobject/session/allocation.hpp"



namespace steam {
namespace mobject {
namespace session {
  
  
  /**
   * LocatingPin representing a low-priority directive by the user,
   * to be fulfilled only if possible (and after satisfying the
   * more important LocatingPins)
   */
  class Wish : public Allocation
    {
      
    };
  
  
  
}}} // namespace steam::mobject::session
#endif
