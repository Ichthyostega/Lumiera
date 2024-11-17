/*
  SessionCmd  -  actual steam command scripts for session-global actions

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file session-cmd.cpp
 ** Actual definition of Steam-Layer command scripts for session-global actions.
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
#include "steam/mobject/session.hpp"
//#include "lib/symbol.hpp"
#include "lib/idi/entry-id.hpp"
//#include "lib/format-string.hpp"

//#include <string>

using lib::hash::LuidH;
//using std::string;
//using util::cStr;
//using util::_Fmt;


namespace steam {
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
  
  
  /** add a new sequence, anchored at the given context.
   * @param context an object to use as anchor to relate the new sequence to
   * @param newID identity of the new sequence to create
   */
  COMMAND_DEFINITION (session_newSequence)
    {
      def.operation ([](LuidH context, LuidH newID)
                        {
                          UNIMPLEMENTED ("add a new Sequence into the given context scope within the Session");
                        })
         .captureUndo ([](LuidH, LuidH addedSeq) -> LuidH
                        {
                          return addedSeq;
                        })
         .undoOperation ([](LuidH context, LuidH addedSeq, LuidH newID)
                        {
                          REQUIRE (addedSeq == newID);
                          UNIMPLEMENTED ("how to remove a sub-sequence from the given context");
                        });
    };
  
  
  
  
  /** more to come here...*/
  
  
  
}} // namespace steam::cmd
