/*
  ScopeCmd  -  actual steam command scripts for actions within model scope

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file scope-cmd.cpp
 ** Actual definition of Steam-Layer command scripts for actions within scope.
 ** Here "scope" means a scope within the high-level model, which could be
 ** - a track or media bin
 ** - a clip
 ** - an effect
 ** @todo WIP 3/2017 early draft
 ** 
 ** @see cmd.hpp
 ** @see command.hpp
 ** @see command-def.hpp
 ** @see cmd-context.hpp
 ** @see SessionCommandFunction_test
 **
 */


#include "steam/cmd.hpp"
#include "steam/control/command-def.hpp"
//#include "steam/mobject/session.hpp"
//#include "lib/symbol.hpp"
#include "lib/idi/entry-id.hpp"
//#include "lib/format-string.hpp"
#include "lib/time/timevalue.hpp"

//#include <string>

using lib::hash::LuidH;
using lib::time::Time;
using lib::time::Offset;
//using std::string;
//using util::cStr;
//using util::_Fmt;


namespace steam {
namespace cmd {
  namespace error = lumiera::error;
  
  //using mobject::Session;
  
  namespace { // implementation helper...
  }//(End) implementation helper
  
  
  
  
  /** @todo WIP-WIP 3/2021 -- working on a prototype for dragging clips...
   */
  COMMAND_DEFINITION (scope_moveRelocateClip)
    {
      def.operation ([](LuidH clipID, Offset timeDelta)
                        {
                          UNIMPLEMENTED ("shift a clip by the given timeDelta offset");
                        })
         .captureUndo ([](LuidH clipID, Offset timeDelta) -> Time
                        {
                          TODO ("actually find out the previous position of the Clip");
                          return Time::ZERO;
                        })
         .undoOperation ([](LuidH clipID, Offset timeDelta, Time originalPos)
                        {
                          UNIMPLEMENTED ("how to revert shifting a clip. Shall we restore the captured Position or reverse apply the delta??");
                        })
                        ;
    };
  
  
  
  
  /** more to come here...*/
  
  
  
}} // namespace steam::cmd
