/*
  DummySessionConnection  -  scaffolding placeholder to drive the GUI-Session connection

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file dummy-session-connection.cpp
 ** Implementation details to build some scaffolding for UI<-->Session integration.
 ** In this translation unit, actually a singleton will be instantiated, whenever
 ** some other part of the application (or some unit test) needs backing by a faked
 ** session with...
 ** - some session content
 ** - commands to be invoked
 ** 
 ** @todo WIP as of 10/2018                         ///////////////////////TICKET #1042
 ** 
 ** @see DummySessionConnection_test
 ** 
 */

#include "lib/util.hpp"
//#include "lib/symbol.hpp"
//#include "include/logging.h"
#include "steam/mobject/session/dummy-session-connection.hpp"
#include "steam/mobject/session/root.hpp"
#include "steam/control/command-def.hpp"
#include "include/ui-protocol.hpp"
#include "include/gui-notification-facade.h"
#include "lib/diff/tree-diff-application.hpp"
#include "lib/diff/mutation-message.hpp"
#include "lib/diff/gen-node.hpp"
//#include "lib/idi/entry-id.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
//#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <string>
//#include <map>

using lib::diff::MutationMessage;
using lib::diff::GenNode;
using lib::diff::MakeRec;
using lib::diff::Ref;
using lib::idi::RandID;
//using util::cStr;
using util::_Fmt;
using std::string;

#include <string>

//using std::map;
using std::string;

using util::contains;
using util::isnil;

namespace steam {
namespace mobject {
namespace session {
  
  namespace { //Implementation details....
  
    GenNode
    emptyTimeline (string baseID, RandID const& forkRootID)
    {
      return MakeRec()
               .set(MakeRec()
                      .type (string{stage::TYPE_Fork})
                    .genNode(forkRootID)
                   )
             .genNode(baseID);
    }
  } //(End)Implementation details....
  
  
  DummySessionConnection::~DummySessionConnection() { }
  
  DummySessionConnection::DummySessionConnection()
    {
    }
  
  
  /** storage for the Singleton accessor */
  lib::Depend<DummySessionConnection> DummySessionConnection::instance;
  
  
  /**
   * Build a population diff message to describe a specific session structure to add
   */
  MutationMessage
  DummySessionConnection::fabricateSeq1 (string baseID)
  {
    const RandID forkRootID{stage::ATTR_fork};
    const GenNode timeline      = emptyTimeline (baseID, forkRootID)
                , rootTrackName = GenNode{string{stage::ATTR_name}, "Track-"+baseID}
                , forkRoot      = MakeRec().genNode(forkRootID)
                ;
    
    return MutationMessage{ ins (timeline)
                          , mut (timeline)
                            , mut (forkRoot)
                              , set (rootTrackName)
                            , emu (forkRoot)
                          , emu (timeline)
                          };
  }
  
  
  void
  DummySessionConnection::applyCopy (MutationMessage const& diff)
  {
    TODO ("build internal diagnostic data structure, apply a copy of the message");
  }
  
  
}}// namespace steam::mobject::session

namespace cmd {
  
  using lib::hash::LuidH;
  using stage::ID;
  using stage::NOTE_INFO;
  using stage::NOTE_WARN;
  using stage::NOTE_ERROR;
//  using stage::NotifyLevel;
//  using stage::MARK_expand;
  using stage::GuiNotification;
//  using util::isYes;

  namespace session = steam::mobject::session;
  
  using DummySess = session::DummySessionConnection;
  
  
  /* ============ dedicated Fake-Commands ============ */
  
  /** Populate the Timeline in the UI with a typical simple Dummy sequence.
   *  This Steam-Layer command script fabricates a faked "population diff", which not
   *  corresponds to any existing session data structure, but looks as if emanated while
   *  loading current session state.
   *  - one single Timeline
   *  - just the root track
   *  - two clips placed on that track
   * @todo use this to establish basic Timeline display in the UI                          //////////////////TICKET #1014 : produce Dummy content to populate timeline
   */
COMMAND_DEFINITION (test_fake_injectSequence_1)
  {
    def.operation ([](string dummyID)
                      {
                        string message{_Fmt{"fabricate Sequence_1 (dummyID='%s')"} % dummyID};
                        GuiNotification::facade().displayInfo (NOTE_INFO, message);
                        auto popuDiff = DummySess::instance().fabricateSeq1 (dummyID);
                        DummySess::instance().applyCopy (popuDiff);
                        auto rootID = session::Root::getID();
                        GuiNotification::facade().mutate (rootID, move(popuDiff));
                      })
       .captureUndo ([](string dummyID) -> string
                      {
                        return _Fmt{"fabricateSequence_1('%s')"} % dummyID;
                      })
       .undoOperation ([](string, string memento)
                      {
                        GuiNotification::facade().displayInfo (NOTE_WARN, "can not UNDO Dummy-Action: "+memento);
                      });
  };
  
  
  /** Populate the Timeline in the UI with a rather complex Dummy sequence.
   *  This command script fabricates a faked convoluted "population diff"...
   * @todo use this to enact a complex layout structure in the Timeline-UI                 //////////////////TICKET #1014 : produce Dummy content to populate timeline
   */
COMMAND_DEFINITION (test_fake_injectSequence_2)
  {
    def.operation ([](string dummyID)
                      {
                        string message{_Fmt{"fabricate Sequence_2 (dummyID='%s')"} % dummyID};
                        GuiNotification::facade().displayInfo (NOTE_INFO, message);
                      })
       .captureUndo ([](string dummyID) -> string
                      {
                        return _Fmt{"fabricateSequence_2('%s')"} % dummyID;
                      })
       .undoOperation ([](string, string memento)
                      {
                        GuiNotification::facade().displayInfo (NOTE_WARN, "can not UNDO Dummy-Action: "+memento);
                      });
  };
  
  
  /** Template for dummy-code....
   * @todo use this to enact a complex layout structure in the Timeline-UI                 //////////////////TICKET #1042 : further the DummySessionConnection
   */
COMMAND_DEFINITION (test_fake_blubb)
  {
    def.operation ([](string dummyID)
                      {
                        string message{_Fmt{"fabricate gulp (dummyID='%s')"} % dummyID};
                        GuiNotification::facade().displayInfo (NOTE_INFO, message);
                      })
       .captureUndo ([](string dummyID) -> string
                      {
                        return _Fmt{"fabricateGulp('%s')"} % dummyID;
                      })
       .undoOperation ([](string, string memento)
                      {
                        GuiNotification::facade().displayInfo (NOTE_WARN, "can not UNDO Dummy-Action: "+memento);
                      });
  };
  
  
}} // namespace steam::mobject::session
