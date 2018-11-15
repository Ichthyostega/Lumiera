/*
  BUILDERFACADE.hpp  -  Facade and service access point for the Builder Subsystem

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


#ifndef PROC_MOBJECT_BUILDERFACADE_H
#define PROC_MOBJECT_BUILDERFACADE_H

#include "lib/error.hpp"
#include "proc/mobject/builder/common.hpp"


namespace proc {
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
  
  
  
}} // namespace proc::mobject
#endif /*PROC_MOBJECT_BUILDERFACADE_H*/
