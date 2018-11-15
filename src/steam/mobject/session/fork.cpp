/*
  Fork  -  A grouping device within the Session.

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


/** @file fork.cpp
 ** Implementation of the basic grouping device within the session ("Track" / "Media Bin")
 ** @todo WIP implementation of session core from 2010
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#include "steam/mobject/session/fork.hpp"


namespace proc {
namespace mobject {
namespace session {
  
  /** create a new fork-MObject based on the given unique ID.
   *  While the fork is an MObject attached locally within the
   *  given Sequence, it referres to a globally known forkID,
   *  which is an Asset. These fork-IDs might be shared among
   *  several Sequences and can be used to refer to several
   *  scopes simultaneously.
   */
  Fork::Fork (ForkID const& forkID)
    : start_(Time::ZERO)                     //////////////////////////////////TODO something more inspired please
    , id_(forkID)
  {
    throwIfInvalid();
  }
  
  
  bool 
  Fork::isValid()  const
  {
    return bool(id_.isValid());  ////////TODO anything more to check 'for real'?
  }
  
  
  
}}} // namespace proc::mobject::session
