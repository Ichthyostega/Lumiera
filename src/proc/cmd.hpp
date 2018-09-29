/*
  CMD.hpp  -  Proc Command definition keys

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

*/


/** @file cmd.hpp
 ** Common ID definitions for Proc-Layer commands.
 ** This header is included when defining the actual command scripts, but also
 ** from all those UI elements to use and invoke the defined Proc-Layer commands.
 ** @todo WIP 3/2017 early draft
 ** 
 ** @see command.hpp
 ** @see command-def.hpp
 ** @see TODO_CommandGuiUsage_test
 **
 */



#ifndef PROC_CMD_H
#define PROC_CMD_H

#include "proc/control/command-setup.hpp"
//#include "lib/symbol.hpp"
//#include "proc/common.hpp"

//#include <string>




namespace proc {
namespace cmd {
  
//  using std::string;
//  using lib::Symbol;
  using control::Command;
  using control::CommandDef;
  using control::CommandSetup;
  //using std::shared_ptr;
  
  
  /* ========= global actions ========= */
  
  extern CommandSetup session_saveSnapshot;
  extern CommandSetup session_newSequence;
  
  
  
  /* ========= sequence actions ======= */
  
  extern CommandSetup sequence_newTrack;
  
  
  
  /* ========= meta actions ======= */
  
  extern CommandSetup test_meta_activateContentDiff;               ////////////////////////////////TICKET #211 prefix test_ bypasses logging and immediately executes for now (2018)
  extern CommandSetup meta_deactivateContentDiff;
  
  //--Demo-and-Development----
  extern CommandSetup test_meta_displayInfo;
  extern CommandSetup test_meta_markError;
  extern CommandSetup test_meta_markNote;
  extern CommandSetup test_meta_markAction;
  
  
}} // namespace proc::cmd
#endif /*PROC_CMD_H*/
