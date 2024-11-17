/*
  SequenceCmd  -  actual steam command scripts for actions operating within a sequence

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file sequence-cmd.cpp
 ** Actual definition of Steam-Layer command scripts for actions within a sequence.
 ** Especially all those actions working on and reconfiguring the track tree.
 ** @todo WIP 4/2017 early draft
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

//#include <string>

using lib::hash::LuidH;
//using std::string;
//using util::cStr;
//using util::_Fmt;


namespace steam {
namespace cmd {
  namespace error = lumiera::error;
  
  //using mobject::Session;
  
  namespace { // implementation helper...
  }//(End) implementation helper
  
  
  
  
  /** add a new track within the fork, anchored at the given context.
   * @param context an object to use as anchor to relate the new track container to
   * @param newID identity of the new track to create
   */
  COMMAND_DEFINITION (sequence_newTrack)
    {
      def.operation ([](LuidH context, LuidH newID)
                        {
                          UNIMPLEMENTED ("add a new Track into the given context scope within the Session");
                        })
         .captureUndo ([](LuidH, LuidH addedTrack) -> LuidH
                        {
                          return addedTrack;
                        })
         .undoOperation ([](LuidH context, LuidH newID, LuidH addedTrack)
                        {
                          REQUIRE (addedTrack == newID);
                          UNIMPLEMENTED ("how to remove a sub-track from the given context");
                        });
    };
  
  
  
  
  /** more to come here...*/
  
  
  
}} // namespace steam::cmd
