/*
  BusTerm(Test)  -  cover the building block of the UI-Bus

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file bus-term-test.cpp
 ** unit test \ref BusTerm_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/sync.hpp"
#include "lib/sync-classlock.hpp"
#include "backend/thread-wrapper.hpp"
#include "gui/ctrl/bus-term.hpp"
#include "gui/ctrl/state-manager.hpp"
#include "proc/control/command.hpp"
#include "test/test-nexus.hpp"
#include "test/mock-elm.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/diff/mutation-message.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/iter-stack.hpp"
#include "lib/call-queue.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/luid.h"
#include "lib/util.hpp"

#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

using lib::Sync;
using lib::ClassLock;
using backend::ThreadJoinable;
using lib::iter_stl::dischargeToSnapshot;
using lib::IterQueue;
using lib::IterStack;
using std::function;
using util::contains;
using util::isnil;
using util::_Fmt;


namespace gui  {
namespace model{
namespace test {
  
  using lib::idi::EntryID;
  using lib::idi::BareEntryID;
  using proc::control::Command;
  using gui::ctrl::StateManager;
  using gui::ctrl::BusTerm;
  using gui::test::MockElm;
  using lib::diff::MutationMessage;
  using lib::diff::TreeDiffLanguage;
  using lib::diff::DiffSource;
  using lib::diff::DiffStep;
  using lib::diff::GenNode;
  using lib::diff::MakeRec;
  using lib::diff::Rec;
  using lib::diff::Ref;
  using lib::time::Time;
  using lib::time::TimeSpan;
  using lib::hash::LuidH;
  using lib::test::EventLog;
  using lib::CallQueue;
  
  using proc::control::LUMIERA_ERROR_UNBOUND_ARGUMENTS;
  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
  
  using ID = lib::idi::BareEntryID const&;
  
  
  namespace {// test data...
    
    // --------random-diff-test------
    uint const MAX_RAND_BORGS = 100;        // stay below 400, verification export grows quadratic
    uint const MAX_RAND_NUMBS = 500;
    uint const MAX_RAND_DELAY = 5000;       // throttle generation, since diff application is slower
    // --------random-diff-test------
    
    int generator_instances = 0;
  }
  
  
  
  
  
  
  /**************************************************************************//**
   * @test cover the standard node element (terminal element) within the UI-Bus,
   * with the help of an attached mock UI element. Contrary to the related
   * [ui-element test](AbstractTangible_test), here we focus on the bus side
   * of the standard interactions.
   * 
   * This test enacts the fundamental generic communication patterns
   * to verify the messaging behaviour
   * - attaching a \ref BusTerm
   * - detaching on element destruction
   * - generate a command invocation
   * - argument passing
   * - capture a _state mark_
   * - replay a _state mark_
   * - cast messages and error states downstream
   * - generic operating of interface states
   * - multithreaded integration test of diff mutation
   * 
   * @see AbstractTangible_test
   * @see gui::model::Tangible
   * @see gui::ctrl::BusTerm
   */
  class BusTerm_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          attachNewBusTerm();
          commandInvocation();
          captureStateMark();
          replayStateMark();
          verifyNotifications();
          clearStates();
          pushDiff();
        }
      
      
      /** @test build a new BusTerm and verify connectivity.
       * Every [tangible UI-element](\ref Tangible) bears an embedded BusTerm
       * member. Since the latter _requires another, up-link BusTerm_ on construction,
       * connection to the [UI-Bus](ui-bus.hpp) is structurally ensured. Moreover,
       * when hooking up a new UI-element, the initialisation of the embedded BusTerm
       * will cause a down-link connection to be installed into the central routing
       * table within the \ref Nexus, the hub of the UI-Bus. Routing and addressing
       * is based on the UI-element's unique EntryID, destruction of the element,
       * through invocation of BusTerm's dtor, will ensure deregistration
       * from the Hub.
       */
      void
      attachNewBusTerm()
        {
          MARK_TEST_FUN;
          // our dummy will be linked with this identity
          BareEntryID elmID = EntryID<MockElm>{"zeitgeist"};
          
          // Access the log on the Test-Nexus hub
          EventLog nexusLog = gui::test::Nexus::startNewLog();
          CHECK (nexusLog.ensureNot("zeitgeist"));
          
          MockElm mock(elmID);
          CHECK (nexusLog.verifyCall("routeAdd").on("TestNexus").arg(elmID,"Tangible")       // Note: invoked from ctor, so it is just a tangible at the moment
                         .beforeEvent("TestNexus", "added route to bID-zeitgeist"));
          
          EventLog elmLog = mock.getLog();
          CHECK (elmLog.verifyCall("ctor").on(&mock)
                       .beforeEvent("create", "zeitgeist"));
          
          
          // now verify there is indeed bidirectional connectivity...
          CHECK (elmLog.ensureNot("expanded"));
          CHECK (elmLog.ensureNot("doFlash"));
          CHECK (nexusLog.ensureNot("zeitgeist").arg("expand"));
          CHECK (nexusLog.ensureNot("zeitgeist").arg("Flash"));
          
          // invoke action on element to cause upstream message (with a "state mark")
          mock.slotExpand();
          CHECK (elmLog.verifyEvent("expanded"));
          CHECK (nexusLog.verifyCall("note").on("TestNexus").arg(elmID, "GenNode-ID(\"expand\")-DataCap|«bool»|true"));
          
          // send a state mark down to the mock element
          gui::test::Nexus::testUI().mark (elmID, GenNode("Flash", 23));
          CHECK (nexusLog.verifyCall("mark").on("TestNexus").arg(elmID, "Flash")
                         .beforeEvent("TestNexus", "mark to bID-zeitgeist"));
          CHECK (elmLog.verifyCall("doFlash").on("zeitgeist"));
          
          
          // kill the zeitgeist and verify disconnection
          mock.kill();
          CHECK (elmLog.verifyEvent("destroy","zeitgeist"));
          CHECK (nexusLog.verifyCall("routeDetach").on("TestNexus").arg(elmID)
                         .beforeEvent("TestNexus", "removed route to bID-zeitgeist"));
          
          gui::test::Nexus::testUI().mark (elmID, GenNode("Flash", 88));
          CHECK (nexusLog.verify("removed route to bID-zeitgeist")
                         .beforeCall("mark").on("TestNexus").arg(elmID, "Flash")
                         .beforeEvent("warn","discarding mark to unknown bID-zeitgeist"));
          CHECK (elmLog.ensureNot("Flash")
                       .afterEvent("destroy","zeitgeist"));
          
          
          cout << "____Probe-Log_________________\n"
               << util::join(elmLog, "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
          
          cout << "____Nexus-Log_________________\n"
               << util::join(gui::test::Nexus::getLog(), "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
        }
      
      
      /** @test perform the full command binding and invocation protocol */
      void
      commandInvocation()
        {
          MARK_TEST_FUN
          gui::test::Nexus::startNewLog();
          Symbol cmd = gui::test::Nexus::prepareMockCmd<string, TimeSpan, LuidH>();
          
          MockElm mock("uiElm");
          
          // random command arguments...
          string text {lib::test::randStr(12)};
          TimeSpan clip (Time(1,2,3), lib::test::randTime());
          LuidH luid;
          
          // we cannot invoke commands without binding required arguments
          VERIFY_ERROR (WRONG_TYPE, mock.invoke(cmd) );
          
          // proper argument typing is ensured while dispatching the bind message. 
          VERIFY_ERROR (WRONG_TYPE, mock.invoke(cmd, Rec({"lalala"})) );
          
          // command can't be issued, since it's still unbound
          CHECK (not Command::canExec(cmd));
          
          
          mock.invoke (cmd, text, clip, luid);
          
          CHECK (Command::canExec(cmd));
          CHECK (gui::test::Nexus::wasBound(cmd, text, clip, luid));
          CHECK (not gui::test::Nexus::wasBound(cmd, "lololo"));
          CHECK (gui::test::Nexus::wasInvoked(cmd));
          CHECK (gui::test::Nexus::wasInvoked(cmd, text, clip, luid));
          CHECK (not gui::test::Nexus::wasInvoked(cmd, " huh ", clip, luid));
          CHECK (not gui::test::Nexus::wasInvoked(cmd, text, clip));
          
          // Mock commands are automatically unique
          auto cmdX = gui::test::Nexus::prepareMockCmd<>();
          auto cmdY = gui::test::Nexus::prepareMockCmd<>();
          CHECK (cmd != cmdX);
          CHECK (cmd != cmdY);
          
          CHECK (not gui::test::Nexus::wasInvoked(cmdX));
          CHECK (not gui::test::Nexus::wasInvoked(cmdY));
          
          cout << "____Nexus-Log_________________\n"
               << util::join(gui::test::Nexus::getLog(), "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
          
          gui::test::Nexus::setCommandHandler(); // deinstall custom command handler
        }
      
      
      /** @test collect state mark notifications from bus
       * We use a test version of the PresentationStateManager,
       * based on the same building blocks as _the real thing_ */
      void
      captureStateMark()
        {
          MARK_TEST_FUN
          gui::test::Nexus::startNewLog();
          StateManager& stateManager = gui::test::Nexus::useMockStateManager();
          
          MockElm mockA("alpha");   BareEntryID alpha = mockA.getID();
          MockElm mockB("bravo");   BareEntryID bravo = mockB.getID();
          MockElm mockC("charly");  BareEntryID charly = mockC.getID();
          
          mockA.slotExpand();
          
          mockB.slotExpand();
          mockB.slotCollapse();
          
          CHECK (stateManager.currentState(alpha, "expand") == GenNode("expand", true ));
          CHECK (stateManager.currentState(bravo, "expand") == GenNode("expand", false ));
          
          // handling of missing information
          CHECK (stateManager.currentState(charly, "expand") == Ref::NO); // no data recorded yet
          CHECK (stateManager.currentState(bravo, "extinct") == Ref::NO); // unknown property
          
          EntryID<MockElm> bruno("bruno");
          CHECK (stateManager.currentState(bruno, "expand")  == Ref::NO); // who knows bruno?
          
          mockC.slotExpand();
          CHECK (stateManager.currentState(charly, "expand") == GenNode("expand", true ));
          
          // error states can be sticky
          mockC.markErr("overinflated");
          CHECK (stateManager.currentState(charly, "Error")  == GenNode("Error", "overinflated"));
          
          mockC.reset();
          CHECK (stateManager.currentState(charly, "expand") == Ref::NO); // back to void
          
          
          cout << "____Nexus-Log_________________\n"
               << util::join(gui::test::Nexus::getLog(), "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
        }
      
      
      /** @test replay previously captured state information" */
      void
      replayStateMark()
        {
          MARK_TEST_FUN
          StateManager& stateManager = gui::test::Nexus::getMockStateManager();
          
          MockElm mockA("alpha");
          // no "bravo" this time
          MockElm mockC("charly");
          
          CHECK (not mockA.isExpanded());
          CHECK (not mockC.isTouched());
          
          BareEntryID alpha = mockA.getID();
          stateManager.replayState (alpha, "expand");
          CHECK (mockA.isExpanded());
          
          auto& uiBus = gui::test::Nexus::testUI();
          uiBus.mark (mockA.getID(), GenNode{"expand", false});
          
          CHECK (not mockA.isExpanded());
          CHECK (mockA.isTouched());
          
          stateManager.replayAllState ("expand");
          
          CHECK (mockA.isExpanded());
          CHECK (not mockC.isExpanded());
          CHECK (not mockC.isTouched());
        }
      
      
      /** @test send notifications to a distinct element, or as broadcast. */
      void
      verifyNotifications()
        {
          MARK_TEST_FUN
          EventLog nexusLog = gui::test::Nexus::startNewLog();
          
          MockElm mockA("alpha");   BareEntryID alpha = mockA.getID();   mockA.joinLog (nexusLog);
          MockElm mockB("bravo");   BareEntryID bravo = mockB.getID();   mockB.joinLog (nexusLog);
          MockElm mockC("charly");  BareEntryID charly = mockC.getID();  mockC.joinLog (nexusLog);
          
          auto& uiBus = gui::test::Nexus::testUI();
          
          CHECK (not mockA.isTouched());
          CHECK (not mockB.isTouched());
          CHECK (not mockC.isTouched());
          
          uiBus.mark (alpha, GenNode{"Message", "Centauri"});
          uiBus.mark (bravo, GenNode{"Flash", true});
          uiBus.mark (charly, GenNode{"Message", "Delta"});
          uiBus.mark (charly, GenNode{"Error", "Echo"});
          
          CHECK (    mockA.isTouched());
          CHECK (not mockB.isTouched());
          CHECK (    mockC.isTouched());
          
          CHECK (not mockA.isError());
          CHECK (not mockB.isError());
          CHECK (    mockC.isError());
          
          CHECK ("Centauri" == mockA.getMessage());
          CHECK ("Delta"    == mockC.getMessage());
          
          CHECK ("Echo"     == mockC.getError());
          
          // verify the message passing in the combined log...
          CHECK (nexusLog.verifyEvent("create", "alpha")
                         .beforeCall("mark").on("TestNexus").arg("alpha", "Centauri")  // bus API invoked
                         .beforeCall("doMsg").on("alpha").arg("Centauri")              // handler on target invoked
                         .beforeEvent("mark", "Centauri")                              // target action activated 
                         .beforeEvent("TestNexus","delivered mark to bID-alpha"));     // dispatch done within UI-Bus
          
          CHECK (nexusLog.verifyEvent("TestNexus","delivered mark to bID-alpha")
                         .beforeCall("mark").on("TestNexus").arg("bravo", "GenNode-ID(\"Flash\")-DataCap|«bool»|true")
                         .beforeCall("doFlash").on("bravo")
                         .beforeEvent("TestNexus","delivered mark to bID-bravo"));
          
          // NOTE: calls are passed down synchronously, in one hop, and in sequence
          CHECK (nexusLog.verifyEvent("TestNexus","delivered mark to bID-bravo")
                         .beforeCall("mark").on("TestNexus").arg("charly", "GenNode-ID(\"Message\")-DataCap|«string»|Delta")
                         .beforeCall("doMsg").on("charly").arg("Delta")
                         .beforeEvent("mark", "Delta").id("Message")
                         .beforeEvent("TestNexus","delivered mark to bID-charly")
                         .beforeCall("mark").on("TestNexus").arg("charly", "GenNode-ID(\"Error\")-DataCap|«string»|Echo")
                         .beforeCall("doErr").on("charly").arg("Echo")
                         .beforeEvent("mark", "Echo").id("Error")
                         .beforeEvent("TestNexus","delivered mark to bID-charly"));
          
          
          // broadcast message
          uiBus.markAll (GenNode{"Message", "Foxtrot"});
          CHECK (not mockA.isError());
          CHECK (not mockB.isError());
          CHECK (    mockC.isError());
          CHECK (    mockA.isTouched());
          CHECK (    mockB.isTouched());
          CHECK (    mockC.isTouched());
          CHECK ("Foxtrot" == mockA.getMessage());
          CHECK ("Foxtrot" == mockB.getMessage());
          CHECK ("Foxtrot" == mockC.getMessage());
          CHECK (       "" == mockA.getError());
          CHECK (       "" == mockB.getError());
          CHECK (   "Echo" == mockC.getError());
          
          CHECK (nexusLog.verifyEvent("mark", "Echo").id("Error")
                         .beforeCall("markAll").on("TestNexus").arg("Foxtrot")
                         .beforeEvent("Broadcast", "Foxtrot")
                         .beforeCall("mark").on("TestNexus").arg("bravo", "GenNode-ID(\"Message\")-DataCap|«string»|Foxtrot")
                         .beforeCall("doMsg").on("bravo").arg("Foxtrot")
                         .beforeEvent("TestNexus", "broadcasted mark to 3 terminals"));
          
          // the order of dispatch is unspecified,
          // but we know a regular mark call sequence happens for each connected terminal
          CHECK (nexusLog.verifyCall("markAll").on("TestNexus").arg("Foxtrot")
                         .beforeCall("mark").on("TestNexus").arg("alpha", "Foxtrot")
                         .beforeCall("doMsg").on("alpha").arg("Foxtrot")
                         .beforeEvent("TestNexus", "successfully broadcasted"));
          
          CHECK (nexusLog.verifyCall("markAll").on("TestNexus").arg("Foxtrot")
                         .beforeCall("mark").on("TestNexus").arg("bravo", "Foxtrot")
                         .beforeCall("doMsg").on("bravo").arg("Foxtrot")
                         .beforeEvent("TestNexus", "successfully broadcasted"));
          
          CHECK (nexusLog.verifyCall("markAll").on("TestNexus").arg("Foxtrot")
                         .beforeCall("mark").on("TestNexus").arg("charly", "Foxtrot")
                         .beforeCall("doMsg").on("charly").arg("Foxtrot")
                         .beforeEvent("TestNexus", "successfully broadcasted"));
          
          
          cout << "____Nexus-Log_________________\n"
               << util::join(nexusLog, "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
        }
      
      
      /** @test broadcast various degrees of state reset */
      void
      clearStates()
        {
          MARK_TEST_FUN
          EventLog nexusLog = gui::test::Nexus::startNewLog();
          
          MockElm mockA("alpha");   BareEntryID alpha = mockA.getID();   mockA.joinLog (nexusLog);
          MockElm mockB("bravo");   BareEntryID bravo = mockB.getID();   mockB.joinLog (nexusLog);
          MockElm mockC("charly");  BareEntryID charly = mockC.getID();  mockC.joinLog (nexusLog);
          
          auto& uiBus = gui::test::Nexus::testUI();
          
          CHECK (not mockA.isTouched());
          CHECK (not mockB.isTouched());
          CHECK (not mockC.isTouched());
          
          mockB.slotExpand();
          uiBus.mark (alpha,  GenNode{"Message", "Centauri"});
          uiBus.mark (charly, GenNode{"Message", "Delta"});
          uiBus.mark (charly, GenNode{"Error",   "Echo"});
          
          CHECK (mockB.isExpanded());
          CHECK (mockC.isError());
          CHECK ("Delta"    == mockC.getMessage());
          CHECK ("Centauri" == mockA.getMessage());
          
          // reset all notification messages
          uiBus.markAll (GenNode{"clearMsg", true});
          CHECK (mockB.isExpanded());
          CHECK (mockC.isError());
          CHECK (isnil (mockA.getMessage()));
          CHECK (isnil (mockC.getMessage()));
          CHECK ("Echo" == mockC.getError());
          
          uiBus.mark (bravo, GenNode{"Message", "miss"});
          mockA.slotExpand();
          mockA.slotCollapse();
          
          auto& stateManager = gui::test::Nexus::getMockStateManager();
          CHECK (stateManager.currentState(alpha, "expand") == GenNode("expand", false ));
          CHECK (stateManager.currentState(bravo, "expand") == GenNode("expand", true ));
          CHECK (stateManager.currentState(charly, "expand") == Ref::NO);
          CHECK (stateManager.currentState(charly, "Error")  == GenNode("Error", "Echo"));  // sticky error state was recorded
          
          // reset error state(s)
          uiBus.markAll (GenNode{"clearErr", true});
          CHECK (not mockA.isExpanded());
          CHECK (mockB.isExpanded());
          CHECK ("miss" == mockB.getMessage());
          CHECK (not mockC.isError());
          
          CHECK (stateManager.currentState(alpha, "expand") == GenNode("expand", false ));
          CHECK (stateManager.currentState(bravo, "expand") == GenNode("expand", true ));
          CHECK (stateManager.currentState(charly, "expand") == Ref::NO);
          CHECK (stateManager.currentState(charly, "Error")  == Ref::NO); // sticky error state was cleared,
                                                                         //  because charly sent a clearErr state mark notification back
          
          // send global sweeping reset
          uiBus.markAll (GenNode{"reset", true});
          
          CHECK (not mockA.isTouched());
          CHECK (not mockB.isTouched());
          CHECK (not mockC.isTouched());
          
          CHECK (not mockA.isExpanded());
          CHECK (not mockB.isExpanded());
          
          CHECK (isnil (mockA.getMessage()));
          CHECK (isnil (mockB.getMessage()));
          CHECK (isnil (mockC.getMessage()));
          
          CHECK (stateManager.currentState(alpha,  "expand") == Ref::NO);
          CHECK (stateManager.currentState(bravo,  "expand") == Ref::NO);
          CHECK (stateManager.currentState(charly, "expand") == Ref::NO);
          CHECK (stateManager.currentState(charly, "Error" ) == Ref::NO);
          
          
          cout << "____Nexus-Log_________________\n"
               << util::join(nexusLog, "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
          
          gui::test::Nexus::setStateMarkHandler(); // deinstall custom state mark handler
        }
      
      
      
      
      
      /**
       * @test integration test of mutation by diff message
       *  Since this test focuses on the bus side of standard interactions,
       *  it seems indicated to emulate the complete invocation situation,
       *  which involves passing thread boundraries. The main thread running
       *  this test shall enact the role of the UI event thread (since the
       *  UI-Bus in the real application is confined to this UI thread).
       *  Thus we'll start another thread to enact the role of the Session,
       *  to produce diff messages and "cast" them towards the UI.
       * @note a defining property of this whole interaction is the fact that
       *       the diff is _pulled asynchronously,_ which means the actual diff
       *       generation happens on callback from the UI. Access to any "session"
       *       data needs to be protected by lock in such a situation.
       */
      void
      pushDiff()
        {
          MARK_TEST_FUN
          
          struct SessionThread
            : Sync<>
            , ThreadJoinable
            {
              // shared data
              uint64_t borgChecksum_ = 0;
              IterStack<uint> sessionBorgs_;
              
              // access to shared session data
              void
              scheduleBorg (uint id)
                {
                  Lock sync(this);
                  borgChecksum_ += id;
                  sessionBorgs_.push(id);
                }
              
              auto
              dispatchBorgs()
                {
                  Lock sync(this);
                  return dischargeToSnapshot (sessionBorgs_);
                }
              
              
              
              /**
               * Independent heap allocated diff generator.
               * Implements the IterSource<DiffStep> interface
               * and will be pulled from the GUI-Thread for actually
               * generating the diff. At this point, it needs to access
               * the shared session data with proper locking, and derive
               * a representation of the "changes" in diff format
               */
              struct BorgGenerator
                : util::NonCopyable
                , TreeDiffLanguage
                , DiffSource
                {
                  uint generatorID_;
                  SessionThread& theCube_;
                  IterQueue<DiffStep> steps_;
                  
                  BorgGenerator (SessionThread& motherShip, uint id)
                    : generatorID_{id}
                    , theCube_{motherShip}
                    {
                      ClassLock<BorgGenerator> sync;
                      ++generator_instances;
                    }
                 ~BorgGenerator()
                    {
                      ClassLock<BorgGenerator> sync;
                      --generator_instances;
                    }
                    
                  
                  /* == Interface IterSource<DiffStep> == */
                  
                  virtual DiffStep*
                  firstResult ()  override
                    {
                      REQUIRE (not steps_);
                      auto plannedBorgs = theCube_.dispatchBorgs();
                      uint max = plannedBorgs.size();
                      uint cur = 0;
                      
                      _Fmt borgName{"%d of %d ≺%03d.gen%03d≻"};
                      
                      
                      steps_.feed (after(Ref::ATTRIBS));  // important: retain all existing attributes
                      for (uint id : plannedBorgs)        // Generate diff to inject a flock of Borg
                        {
                          GenNode borg = MakeRec().genNode(borgName % ++cur % max % id % generatorID_);
                          steps_.feed (ins(borg));
                          steps_.feed (mut(borg));        // open nested scope for this Borg
                          steps_.feed (  ins(GenNode{"borgID", int(id)}));
                          steps_.feed (emu(borg));        // close nested scope
                        }
                      steps_.feed (after(Ref::END));      // important: fast-forward and accept already existing Borgs
                      
                      return & *steps_;  // the IterSource protocol requires us to return a ptr to current element
                    }
                  
                  
                  virtual void
                  nextResult (DiffStep*& pos)  override
                    {
                      if (!pos) return;
                      if (steps_) ++steps_;
                      if (steps_)
                        pos = & *steps_; // pointer to current element
                      else
                        pos = NULL;      // signal iteration end
                    }
                };
              
                
              /**
               * launch the Session Thread and start injecting Borg
               */
              SessionThread(function<void(DiffSource*)> notifyGUI)
                : ThreadJoinable{"BusTerm_test: asynchronous diff mutation"
                                , [=]() {
                                    uint cnt       = rand() % MAX_RAND_BORGS;
                                    for (uint i=0; i<cnt; ++i)
                                      {
                                        uint delay = rand() % MAX_RAND_DELAY;
                                        uint id    = rand() % MAX_RAND_NUMBS;
                                        usleep (delay);
                                        scheduleBorg (id);
                                        notifyGUI (new BorgGenerator{*this, i});
                                      }
                                }}
                { }
            };
          
          
          
          EventLog nexusLog = gui::test::Nexus::startNewLog();
          
          MockElm rootMock("alpha zero");
          ID rootID = rootMock.getID();
          
          rootMock.attrib["α"] = "Quadrant";
          CHECK ("Quadrant" == rootMock.attrib["α"]);
          CHECK (isnil (rootMock.scope));
          
          CHECK (0 == generator_instances);
          
          
          // The final part in the puzzle is to dispatch the diff messages into the UI
          // In the real application, this operation is provided by the NotificationService.
          // It has access to the UI-Bus, but has to ensure all bus operations are actually
          // performed on the UI event thread.
          auto& uiBus = gui::test::Nexus::testUI();
          CallQueue uiDispatcher;
          auto notifyGUI = [&](DiffSource* diffGenerator)
                              {
                                uiDispatcher.feed ([&, diffGenerator]()
                                                    {
                                                      // apply and consume diff message stored within closure
                                                      uiBus.change (rootID, MutationMessage{diffGenerator});
                                                    });
                              };
          
          
          
          //----start-multithreaded-mutation---
          SessionThread session{notifyGUI};
          usleep (2 * MAX_RAND_DELAY);
          while (not isnil(uiDispatcher))
            {
              usleep (100);
              uiDispatcher.invoke();
            }
          session.join();
          //------end-multithreaded-mutation---
          
          // on rare occasions we (consumer thread)
          // prematurely empty the queue...
          while (not isnil(uiDispatcher))
            {
            uiDispatcher.invoke();
            }
          
          
          // now verify rootMock has been properly assimilated...
          uint generatedBorgs = rootMock.scope.size();
          
          // root and all Borg child nodes are connected to the UI-Bus
          CHECK (1 + generatedBorgs == gui::test::Nexus::size());
          
          uint64_t borgChecksum = 0;
          for (uint i=0; i<generatedBorgs; ++i)
            {
              MockElm& borg = *rootMock.scope[i];
              CHECK (contains (borg.attrib, "borgID"));
              string borgID = borg.attrib["borgID"];
              borgChecksum += lexical_cast<int> (borgID);
              string childID = borg.getID().getSym();
              CHECK (contains (childID, borgID));
              CHECK (contains (childID, " of ")); // e.g. "3 of 5"
              
              CHECK (nexusLog.verifyCall("routeAdd").arg(rootMock.getID(), memLocation(rootMock))      // rootMock was attached to Nexus
                             .beforeCall("change")  .argMatch(rootMock.getID(),                        // diff message sent via UI-Bus
                                                              "after.+_ATTRIBS_.+"                     // verify diff pattern generated for each Borg
                                                              "ins.+"+childID+".+"
                                                              "mut.+"+childID+".+"
                                                              "ins.+borgID.+"+borgID+".+"
                                                              "emu.+"+childID)
                             .beforeCall("routeAdd").arg(borg.getID(), memLocation(borg))              // Borg was inserted as child and attached to Nexus
                             .beforeEvent("applied diff to "+string(rootMock.getID()))
                             );
            }
          
          CHECK (rootMock.attrib["α"] == "Quadrant");     // attribute alpha was preserved while injecting all those Borg
          
          
          // sanity checks
          CHECK (borgChecksum == session.borgChecksum_);  // no Borgs got lost
          CHECK (0 == generator_instances);               // no generator instance leaks
          
          
          
          cout << "____Event-Log_________________\n"
               << util::join(rootMock.getLog(), "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
          
          cout << "____Nexus-Log_________________\n"
               << util::join(nexusLog, "\n")
               << "\n───╼━━━━━━━━━╾────────────────"<<endl;
        }
      
      
      static string
      memLocation (Tangible& uiElm)
        {
          return lib::idi::instanceTypeID (&uiElm);
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (BusTerm_test, "unit gui");
  
  
}}} // namespace gui::model::test
