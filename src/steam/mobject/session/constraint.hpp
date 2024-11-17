/*
  CONSTRAINT.hpp  -  LocatingPin representing an mandatory directive

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file constraint.hpp
 ** Specialised LocatingPin for use in placements
 ** @todo result of the very first code generation from UML in 2008. Relevance not clear yet... 
 */


#ifndef STEAM_MOBJECT_SESSION_CONSTRAINT_H
#define STEAM_MOBJECT_SESSION_CONSTRAINT_H

#include "steam/mobject/session/allocation.hpp"



namespace steam {
namespace mobject {
namespace session {
  
  
  /**
   * LocatingPin representing an directive by the user 
   * that must not be violated
   */
  class Constraint : public Allocation
    {};
  
}}} // namespace steam::mobject::session
#endif
