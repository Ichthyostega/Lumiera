/*
  Root  -  root element of the high-level model, global session scope

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


/** @file root.cpp
 ** @todo WIP implementation of session core from 2010
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#include "steam/mobject/session/root.hpp"
#include "common/query/defs-manager.hpp"

using lumiera::query::DefsManager;
using lib::idi::EntryID;

namespace proc {
namespace mobject {
namespace session {
  
  /** */
  Root::Root (DefsManager& dM)
    : defaults_(dM)
    {
      throwIfInvalid();
    }
  
  
  /** get an unique ID to identify "the model root".
   *  Actually this ID is statically fixed and will be used by the UI
   *  to connect to and talk to the session model at top-level
   * @return an embedded LUID tagged with the type of the session::Root.
   *         This ID is suitable to be used in model diff and as ID on
   *         the UI-Bus to address the corresponding representations
   *         in Steam-Layer and Stage-Layer
   */
  lib::idi::EntryID<Root>
  Root::getID()
  {
    return EntryID<Root>("session");
  }
  
  
  lib::idi::EntryID<Root>
  Root::getAssetID()
  {
    return EntryID<Root>("assets");
  }
  
  
  /** @todo validity self-check of the model root
   *        should do substantial checks; the idea is
   *        to perform a complete sanity check by delegating
   *        to the parts.
   *  @note beware of performance problems here!
   */
  bool
  Root::isValid()  const
  {
    return true; //////////////////TICKET #447
  }

  /////////////////////////////////TODO more to come.....
  
  
}}} // namespace proc::mobject::session
