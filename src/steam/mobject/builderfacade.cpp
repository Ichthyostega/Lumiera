/*
  BuilderFacade  -  Facade and service access point for the Builder Subsystem

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file builderfacade.cpp
 ** Implementation of top-level access to builder functionality
 */


#include "steam/mobject/builder/common.hpp"
#include "steam/mobject/builderfacade.hpp"
#include "steam/fixture/fixture.hpp"

namespace steam {
namespace mobject {
  
  using session::Fixture;
  
  LUMIERA_ERROR_DEFINE (BUILDER_LIFECYCLE, "Builder activated while in non operational state");
  
  
  
  /**
   * Main Operation of the Builder:
   * rebuild the low-level model based on the current contents of the Session.
   */
  Fixture&
  BuilderFacade:: rebuildFixture ()
  {
    UNIMPLEMENTED ("build the builder...");
  }
  
  
  
}} // namespace mobject
