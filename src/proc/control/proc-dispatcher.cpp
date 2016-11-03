/*
  ProcDispatcher  -  Proc-Layer command dispatch and execution

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


/** @file proc-dispatcher.cpp
 ** TODO proc-dispatcher.cpp
 */



#include "proc/control/proc-dispatcher.hpp"
#include "proc/mobject/session.hpp"
#include "include/logging.h"
//#include "proc/mobject/mobject-ref.hpp"
//#include "proc/mobject/mobject.hpp"
//#include "proc/mobject/placement.hpp"

//#include <boost/format.hpp>
//using boost::str;

namespace proc {
namespace control {
  
  
  namespace {
  
    /** when session gets ready,
     *  open the command processing interface.
     */
    void
    activateCommandProcessing()
    {
      ProcDispatcher::instance().activate();
    }
    
    
    lumiera::LifecycleHook _schedule (mobject::ON_SESSION_INIT, &activateCommandProcessing);
  
  }
  
  /** storage for Singleton access */
  lib::Depend<ProcDispatcher> ProcDispatcher::instance;
  
  
  
  /** */
  void
  ProcDispatcher::activate()
  {
    INFO (command, "Session command processing activated.");
    TODO ("implement command processing queue");
  }
  
  
  void
  ProcDispatcher::deactivate()
  {
    INFO (command, "Session command interface closed.");
    TODO ("implement command processing queue");
  }

  
  void
  ProcDispatcher::clear()
  {
    if (!empty())
      WARN (command, "DISCARDING pending Session commands.");
    TODO ("implement command processing queue");
  }
  
  
  bool
  ProcDispatcher::empty()  const
  {
    TODO ("implement command processing queue");
    return true;
  }

  
  
  
  
  
}} // namespace proc::control
