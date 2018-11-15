/*
  ScopeCmd  -  actual steam command scripts for actions within model scope

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
  
  
  
  
  /** @todo placeholder 4/2017 -- need to build a timeline display first...
   */
//COMMAND_DEFINITION (session_newSequence)
//  {
//    def.operation ([](LuidH context, LuidH newID)
//                      {
//                        UNIMPLEMENTED ("add a new Sequence into the given context scope within the Session");
//                      })
//       .captureUndo ([](LuidH, LuidH addedSeq) -> LuidH
//                      {
//                        return addedSeq;
//                      })
//       .undoOperation ([](LuidH context, LuidH addedSeq, LuidH newID)
//                      {
//                        REQUIRE (addedSeq == newID);
//                        UNIMPLEMENTED ("how to remove a sub-sequence from the given context");
//                      });
//  };
  
  
  
  
  /** more to come here...*/
  
  
  
}} // namespace steam::cmd
