/*
  Allocation  -  objective to place a MObject in a specific way  

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file allocation.cpp
 ** @todo result of the very first code generation from UML in 2008. Relevance not clear yet... 
 */


#include "steam/mobject/session/allocation.hpp"

namespace steam {
namespace mobject {
namespace session {
  
  /** @todo probably a placeholder and to be pushed down....*/
  void 
  Allocation::intersect (LocatingSolution& solution)  const
  {
    LocatingPin::intersect (solution);
    
    TODO ("work out how the Allocation types solve for the position...");
  }
  
  
  
}}} // namespace steam::mobject::session
