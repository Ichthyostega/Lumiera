/*
  MetaCmd  -  actual proc command scripts for internal meta and control activities

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file meta-cmd.cpp
 ** Actual definition of Proc-Layer command scripts for meta/ctrl actions.
 ** Any actions issued by the UI-Layer to control internal application state
 ** @todo WIP 8/2018 early draft
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
//#include "proc/mobject/session.hpp"
#include "include/gui-notification-facade.h"
#include "gui/interact/wizard.hpp"             //////////////////////////////////////////////////////////////TICKET #1140 : include needed temporarily
#include "lib/diff/gen-node.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/format-string.hpp"               //////////////////////////////////////////////////////////////TICKET #1140 : include needed temporarily
//#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <string>

using lib::hash::LuidH;
using gui::ID;
using gui::NOTE_INFO;
using gui::NOTE_WARN;
using gui::NOTE_ERROR;
using gui::NotifyLevel;
using gui::GuiNotification;
using lib::diff::GenNode;
//using util::cStr;
using util::_Fmt;                              //////////////////////////////////////////////////////////////TICKET #1140 : include needed temporarily
using util::isYes;
using std::string;


namespace proc {
namespace cmd {
  namespace error = lumiera::error;
  
  //using mobject::Session;
  
  namespace { // implementation helper...
  }//(End) implementation helper
  
  
  
  
  /** instruct the session to start keeping the UI up-to-date through diff messages.
   * @remarks we assume the UI is _empty_ when issuing this command. Thus the session
   *    will commence by sending a _population diff_, to reflect all the UI relevant
   *    structures currently present within the session data. Moreover, from that point
   *    on, any changes within the session structure and contents will be reflected by
   *    pushing appropriate diff messages upwards through the GuiNotification façade.
   * @note to UNDO this action means to waive the live-update state. In addition,
   *    the session will push up a status change to mark the session-root as defunct.
   * @todo used for preliminary integration testing               ///////////////////////////////////////////TICKET #211 prefix test_ bypasses logging and immediately executes for now (2018)
   */
COMMAND_DEFINITION (test_meta_activateContentDiff)
  {
    def.operation ([]()
                      {
                        TODO ("send a population diff starting from scratch");
                        TODO ("activate life-update service within the session");
                        // Temporary hack to get any meaningful UI <-> Proc communication
                        GuiNotification::facade().displayInfo(NOTE_INFO, "Request: population-Diff from Session.");
                      })
       .captureUndo ([]() -> bool
                      {
                        return false;             ///////////////////////////////////////////////////////////TICKET #1153 : use status from service for live diff updates
                      })
       .undoOperation ([](bool wasActive)
                      {
                        if (not wasActive)
                          {
                            UNIMPLEMENTED ("send a status update diff into the root node, to mark it as defunct");
                            TODO ("clear the life-update service within the session");
                          }
                      });
  };
  
  
  /** instruct the session to start keeping the UI up-to-date through diff messages.
   * @remarks we assume the UI is _empty_ when issuing this command. Thus the session
   *    will commence by sending a _population diff_, to reflect all the UI relevant
   *    structures currently present within the session data. Moreover, from that point
   *    on, any changes within the session structure and contents will be reflected by
   *    pushing appropriate diff messages upwards through the GuiNotification façade.
   * @note to UNDO this action means to waive the live-update state. In addition,
   *    the session will push up a status change to mark the session-root as defunct.
   */
COMMAND_DEFINITION (meta_deactivateContentDiff)
  {
    def.operation ([]()
                      {
                        TODO ("send a status update diff into the root node, to mark it as defunct");
                        TODO ("clear the life-update service within the session");
                      })
       .captureUndo ([]() -> bool
                      {
                        return true;              ///////////////////////////////////////////////////////////TICKET #1153 : use status from service for live diff updates
                      })
       .undoOperation ([](bool wasActive)
                      {
                        if (wasActive)
                          {
                            UNIMPLEMENTED ("send a diff update to completely clear the root node, then send a population diff with current session content");
                            TODO ("activate life-update service within the session");
                          }
                      });
  };
  
  
  
  
  /* ===== Demo and Development ===== */
  
  /** DemoGuiRoundtrip: push a notification info message back up into the UI.
   * @todo this is a demo mock setup to further development of the Proc-UI integration     //////////////////TICKET #1140 : prototypical Proc-GUI communication
   */
COMMAND_DEFINITION (test_meta_displayInfo)
  {
    def.operation ([](int level, string message)
                      {
                        GuiNotification::facade().displayInfo (static_cast<NotifyLevel>(level), message);
                      })
       .captureUndo ([](int level, string message) -> string
                      {
                        return _Fmt{"displayInfo(%d, '%s')"} % level % message;
                      })
       .undoOperation ([](int, string, string uiAction)
                      {
                        GuiNotification::facade().displayInfo (NOTE_WARN, "can not UNDO UI-Action: "+uiAction);
                      });
  };
  
  
  /** DemoGuiRoundtrip: send a `markError` message back up into the UI.
   * @todo this is a demo mock setup to further development of the Proc-UI integration     //////////////////TICKET #1140 : prototypical Proc-GUI communication
   */
COMMAND_DEFINITION (test_meta_markError)
  {
    def.operation ([](string message)
                      {
                        ID errorLogID = gui::interact::Wizard::getErrorLogID();
                        GuiNotification::facade().markError (errorLogID, message);
                      })
       .captureUndo ([](string message) -> string
                      {
                        return _Fmt{"GUI::errorLog <- markError('%s')"} % message;
                      })
       .undoOperation ([](string, string uiAction)
                      {
                        GuiNotification::facade().displayInfo (NOTE_WARN, "can not UNDO UI-Action: "+uiAction);
                      });
  };
  
  
  /** DemoGuiRoundtrip: send a `markNote` message back up into the UI.
   * @todo this is a demo mock setup to further development of the Proc-UI integration     //////////////////TICKET #1140 : prototypical Proc-GUI communication
   */
COMMAND_DEFINITION (test_meta_markNote)
  {
    def.operation ([](string message)
                      {
                        ID errorLogID = gui::interact::Wizard::getErrorLogID();
                        GuiNotification::facade().markNote (errorLogID, message);
                      })
       .captureUndo ([](string message) -> string
                      {
                        return _Fmt{"GUI::errorLog <- markNote('%s')"} % message;
                      })
       .undoOperation ([](string, string uiAction)
                      {
                        GuiNotification::facade().displayInfo (NOTE_WARN, "can not UNDO UI-Action: "+uiAction);
                      });
  };
  
  
  /** DemoGuiRoundtrip: send a generic `mark` message with given _action ID_ back up into the UI.
   * @todo this is a demo mock setup to further development of the Proc-UI integration     //////////////////TICKET #1140 : prototypical Proc-GUI communication
   */
COMMAND_DEFINITION (test_meta_markAction)
  {
    def.operation ([](string actionID, string message)
                      {
                        ID errorLogID = gui::interact::Wizard::getErrorLogID();
                        GuiNotification::facade().mark (errorLogID
                                                       ,actionID=="expand"? GenNode{actionID, isYes(message)}
                                                                          : GenNode{actionID, message});
                      })
       .captureUndo ([](string actionID, string message) -> string
                      {
                        return _Fmt{"GUI::errorLog <- mark(%s, '%s')"} % actionID % message;
                      })
       .undoOperation ([](string, string, string uiAction)
                      {
                        GuiNotification::facade().displayInfo (NOTE_WARN, "can not UNDO UI-Action: "+uiAction);
                      });
  };
  
  
  
  /** more to come here...*/
  
  
  
}} // namespace proc::cmd
