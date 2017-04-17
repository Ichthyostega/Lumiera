/*
  SessionCmd  -  actual proc command scripts for session-global actions

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file session-cmd.cpp
 ** Actual definition of Proc-Layer command scripts for session-global actions.
 ** @todo WIP 3/2017 early draft
 ** 
 ** @see cmd.hpp
 ** @see command.hpp
 ** @see command-def.hpp
 ** @see cmd-context.hpp
 ** @see SessionCommandFunction_test
 **
 */


#include "proc/cmd.hpp"
#include "proc/control/command-def.hpp"
#include "proc/mobject/session.hpp"
//#include "lib/symbol.hpp"
//#include "lib/format-string.hpp"

//#include <string>

//using std::string;
//using util::cStr;
//using util::_Fmt;


namespace proc {
namespace cmd {
  namespace error = lumiera::error;
  
  using mobject::Session;
  
  namespace { // implementation helper...
  }//(End) implementation helper
  
  
  
  
  /** store a snapshot of current session actions and state and UI state.
   * @param snapshotID a marker to tag the snapshot
   */
  COMMAND_DEFINITION (session_saveSnapshot)
    {
      def.operation ([](string snapshotID)
                        {
                          Session::current.save (snapshotID);
                        })
         .captureUndo ([](string snapshotID) -> string
                        {
                          return snapshotID;
                        })
         .undoOperation ([](string, string oldSnapshot)
                        {
                          UNIMPLEMENTED ("how to remove a snapshot from session history");
                        });
    };
  
  
  
  
  /** more to come here...*/
  
  
  
}} // namespace proc::cmd
