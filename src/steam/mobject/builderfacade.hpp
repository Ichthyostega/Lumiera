/*
  BUILDERFACADE.hpp  -  Facade and service access point for the Builder Subsystem

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file builderfacade.hpp
 ** Steam-Layer Facade: access to the Builder.
 ** The Builder walks the Session model to compile a corresponding render node network.
 ** 
 ** @todo as of 2016 the builder, as the most crucial component within the architecture,
 **       still remains to be addressed. While we'd hoped to get ahead faster, it is considered
 **       a good idea not to decide upon the implementation based on assumptions.
 **       We will start building the builder "when it is ready"
 */


#ifndef STEAM_MOBJECT_BUILDERFACADE_H
#define STEAM_MOBJECT_BUILDERFACADE_H

#include "lib/error.hpp"
#include "steam/mobject/builder/common.hpp"


namespace steam {
namespace mobject {
  namespace session {
    class Fixture;
  }
  
  LUMIERA_ERROR_DECLARE (BUILDER_LIFECYCLE);  ///< Builder activated while in non operational state
  
  /**
   * Provides unified access to the builder functionality.
   * While individual components of the builder subsystem may be called
   * if necessary or suitable, it is usually better to do all external
   * invocations via the high level methods of this Facade.
   */
  class BuilderFacade
    {
    public:
      session::Fixture & rebuildFixture ();
    };
  
  
  
}} // namespace steam::mobject
#endif /*STEAM_MOBJECT_BUILDERFACADE_H*/
