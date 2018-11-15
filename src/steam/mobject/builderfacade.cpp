/*
  BuilderFacade  -  Facade and service access point for the Builder Subsystem

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

* *****************************************************/


/** @file builderfacade.cpp
 ** Implementation of top-level access to builder functionality
 */


#include "steam/mobject/builder/common.hpp"
#include "steam/mobject/builderfacade.hpp"
#include "steam/mobject/session/fixture.hpp"

namespace proc {
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
