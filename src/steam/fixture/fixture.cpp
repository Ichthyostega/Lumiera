/*
  Fixture  -  the (low level) representation of the Session with explicit placement data

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


/** @file fixture.cpp
 ** Implementation of the Fixture datastructure
 ** @todo WIP implementation of session core from 2010
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#include "steam/fixture/fixture.hpp"
#include "include/logging.h"

namespace steam {
namespace fixture {
  
  
  
  list<ExplicitPlacement> &
  Fixture::getPlaylistForRender ()
  {
    UNIMPLEMENTED ("get Playlist For Render");
  }
  
  
  /** TODO: a placeholder for the Operation needed for
   *  wiring the Automation providers in the Build process
   */
  Auto<double>*
  Fixture::getAutomation ()
  {
    UNIMPLEMENTED ("getAutomation from Fixture");
    return 0;
  }
  
  
  /** @todo self-verification of the fixture? necessary? */
  bool
  Fixture::isValid()  const
  {
    TODO ("actually do a self-verification of the Fixture");
    return true;
  }
  
  
  
  
}} // namespace steam::fixture
