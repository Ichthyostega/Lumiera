/*
  MOCK-ELM.hpp  -  generic mock UI element for unit testing

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

*/


/** @file mock-elm.hpp
 ** A generic interface element instrumented for unit testing.
 ** All relevant building blocks within the Lumiera GTK UI are based on
 ** stage::model::Tangible, meaning that any generic effect of interface interactions
 ** can be expressed in terms of this interface contract. As far as the UI participates
 ** in interactions with the lower layers, like e.g. command invocation, structure updates
 ** and state notifications, these processes can be modelled and verified with the help
 ** of a specially prepared Tangible instance. This stage::test::MockElm provides the
 ** necessary instrumentation to observe what has been invoked and received.
 ** 
 ** Since the purpose of a mock interface element is to test interactions and responses
 ** targeted at a generic interface element, the MockElm incorporates an implementation
 ** independent from the real stage::model::Widget or stage::model::Controller. This
 ** mock implementation is basically NOP, while logging any invocation. Matters get
 ** a bit fuzzy, when it comes to the distinction between _widget_ and _controller_.
 ** Yet we should note that the purpose of this setup is to cover the connectivity
 ** and integration with the UI, not the tangible "mechanics" of the UI itself.
 ** It can be argued that covering the latter with unit tests is pretty much
 ** moot and will result just in a huge pile of code duplication and
 ** maintenance burden.
 ** 
 ** People typically start to look into unit testing of user interfaces
 ** when faced with a largely dysfunctional architecture, where core functionality
 ** is littered and tangled into the presentation code. While in a system knowingly
 ** built with a distinct core, the UI should not contain anything not tangible enough
 ** as just to be verified by watching it in action. The push of a button should just
 ** invoke an action, and the action itself should be self contained enough to be
 ** tested in isolation. The UI-Bus and the [generic widget base](\ref stage::model::Tangible)
 ** was built to serve as a foundation to achieve that goal.
 ** 
 ** @see abstract-tangible-test.cpp
 ** 
 */


#ifndef STAGE_TEST_MOCK_ELM_H
#define STAGE_TEST_MOCK_ELM_H


#include "lib/error.hpp"
#include "include/ui-protocol.hpp"
#include "lib/test/event-log.hpp"
#include "stage/model/tangible.hpp"
#include "lib/diff/record.hpp"
#include "lib/idi/genfunc.hpp"
#include "test/test-nexus.hpp"
#include "lib/diff/test-mutation-target.hpp"   ///////////TICKET #1009 -- extract the render(DataCap) function?
#include "lib/format-cout.hpp"
#include "lib/symbol.hpp"
#include "lib/util.hpp"

#include <string>
#include <memory>
#include <vector>
#include <map>


namespace stage {
  using lib::test::EventLog;
  using lib::test::EventMatch;
  
namespace test{
  
  
  using lib::diff::TreeMutator;
  using util::isnil;
  using lib::Symbol;
  using std::string;
  
  class MockElm;
  using PMockElm = std::unique_ptr<MockElm>;
  
  
  /**
   * Mock UI element or controller.
   * Within Lumiera, all interface components of relevance are based
   * on the [Tangible] interface, which we mock here for unit testing.
   * This special implementation is instrumented to [log](\ref lib::test::EventLog)
   * any invocation and any messages sent or received through the UI Backbone,
   * which is formed by the [UI-Bus](\ref ui-bus.hpp).
   * 
   * @todo some usage details
   * @see abstract-tangible-test.cpp
   */
  class MockElm
    : public stage::model::Tangible
    {
      using _Par = stage::model::Tangible;
      
      EventLog log_{this->identify()};
      
      bool virgin_{true};
      bool expanded_{false};
      
      string message_;
      string error_;
      
      
      
      /* ==== Tangible interface ==== */
      
      virtual bool
      doReset()  override
        {
          log_.call(this->identify(), string{MARK_reset});
          if (virgin_)
            return false; // there was nothing to reset
          
          error_ = "";
          message_ = "";
          expanded_ = false;
          virgin_ = true;
          log_.event(string{MARK_reset});
          return true; // we did indeed reset something
        }             //  and thus a state mark should be captured
      
      virtual bool
      doExpand (bool yes)  override
        {
          log_.call(this->identify(), string{MARK_expand}, yes);
          return Tangible::doExpand (yes);
        }
      
      virtual void
      doReveal()  override
        {
          log_.call(this->identify(), string{MARK_reveal});
          Tangible::doReveal();  // NOTE: without specific configuration this is NOP
        }
      
      virtual bool
      doMsg (string text)  override
        {
          log_.call (this->identify(), "doMsg", text);
          cout << this->identify() << " <-- Message(\""<<text<<"\")" <<endl;
          message_ = text;
          virgin_ = false;
          log_.note ("type=mark", "ID=Message", text);
          
          return false; // messages not sticky for this mock implementation
        }
      
      virtual bool
      doClearMsg ()  override
        {
          log_.call (this->identify(), "doClearMsg");
          if (isnil (message_))
            return false;
          
          message_ = "";
          log_.note ("type=mark", "ID=Message", "Message notification cleared");
          return true;
        }
      
      virtual bool
      doErr (string text)  override
        {
          log_.call (this->identify(), "doErr", text);
          cerr << this->identify() << " <-- Error(\""<<text<<"\")" <<endl;
          error_ = text;
          virgin_ = false;
          log_.note ("type=mark", "ID=Error", text);
          
          return true; // error states are sticky for this mock implementation
        }
      
      virtual bool
      doClearErr ()  override
        {
          log_.call (this->identify(), "doClearErr");
          if (not isError())
            return false;
          
          error_ = "";
          log_.note ("type=mark", "ID=Error", "Error state cleared");
          return true;
        }
      
      virtual void
      doFlash()  override
        {
          log_.call (this->identify(), "doFlash");
          cout << this->identify() << " <-- Flash!" <<endl;
          log_.note ("type=mark", "ID=Flash");
        }
      
      virtual void
      doMark (GenNode const& mark)  override
        {
          log_.call (this->identify(), "doMark", mark);
          cout << this->identify() << " <-- state-mark = "<< mark <<endl;
          log_.note ("type=mark", "ID="+mark.idi.getSym(), mark);
          
          this->virgin_ = false; // assume state change....
          
          // forward to default handler
          Tangible::doMark (mark);
        }
      
      virtual void
      buildMutator (TreeMutator::Handle buffer)  override
        {
          using Attrib = std::pair<const string,string>;
          using lib::diff::collection;
          using lib::diff::render;             ///////////TICKET #1009
          
          log_.call (this->identify(), "buildMutator");
          cout << this->identify() << " <-- DIFF" <<endl;
          
          buffer.emplace(
            TreeMutator::build()
              .attach (collection(scope)
                     .isApplicableIf ([&](GenNode const& spec) -> bool
                        {
                          return spec.data.isNested();                  // »Selector« : require object-like sub scope
                        })
                     .matchElement ([&](GenNode const& spec, PMockElm const& elm) -> bool
                        {
                          return spec.idi == elm->getID();
                        })
                     .constructFrom ([&](GenNode const& spec) -> PMockElm
                        {
                          log_.event("diff", "create child \""+spec.idi.getSym()+"\"");
                          PMockElm child = std::make_unique<MockElm>(spec.idi, this->uiBus_);
                          child->joinLog(*this);                        // create a child element wired via this Element's BusTerm
                          return child;
                        })
                     .buildChildMutator ([&](PMockElm& target, GenNode::ID const& subID, TreeMutator::Handle buff) -> bool
                        {
                          if (target->getID() != subID) return false;   //require match on already existing child object
                          target->buildMutator (buff);                  // delegate to child to build nested TreeMutator
                          log_.event("diff", ">>Scope>> "+subID.getSym());
                          return true;
                        }))
              .attach (collection(attrib)
                     .isApplicableIf ([&](GenNode const& spec) -> bool
                        {
                          return spec.isNamed()                         // »Selector« : accept attribute-like values
                             and not spec.data.isNested();              //              but no nested objects
                        })
                     .matchElement ([&](GenNode const& spec, Attrib const& elm) -> bool
                        {
                          return elm.first == spec.idi.getSym();
                        })
                     .constructFrom ([&](GenNode const& spec) -> Attrib
                        {
                          string key{spec.idi.getSym()},
                                 val{render(spec.data)};
                          log_.event("diff", "++Attrib++ "+key+" = "+val);
                          return {key, val};
                        })
                     .assignElement ([&](Attrib& target, GenNode const& spec) -> bool
                        {
                          string key{spec.idi.getSym()},
                                 newVal{render (spec.data)};
                          log_.event("diff", "set Attrib "+key+" <-"+newVal);
                          target.second = newVal;
                          return true;
                        })));
          
          log_.event ("diff", getID().getSym() +" accepts mutation...");
        }
      
      
    protected:
      string
      identify() const
        {
          return getID().getSym() +"."+ lib::idi::instanceTypeID(this);
        }
      
      
    public:
      explicit
      MockElm(string id)
        : MockElm(lib::idi::EntryID<MockElm>(id))
        { }
      
      explicit
      MockElm(ID identity, ctrl::BusTerm& nexus  =Nexus::testUI())
        : stage::model::Tangible(identity, nexus)
        {
          log_.call (this->identify(), "ctor", identity, string(nexus));
          log_.create (getID().getSym());
          installExpander ([&](){ return this->expanded_; }
                          ,[&](bool yes)
                                {
                                  virgin_ = false;
                                  expanded_ = yes;
                                  log_.event (expanded_? "expanded" : "collapsed");
                                });
        }
      
      
      /** document our death in the diagnostic log. */
     ~MockElm()
        {
          try {
            log_.call (this->identify(), "dtor");
            log_.destroy (getID().getSym());
            }
          catch(...)
            {
              const char* errID = lumiera_error();
              if (errID)
                cerr << "Error while logging shutdown of Mock-UI-Element: " << errID <<endl;
              else
                cerr << "Unknown Error while logging shutdown of Mock-UI-Element." <<endl;
            }
        }
      
      
      
      
      /* ==== special operations API ==== */
      
      /** commit suicide.
       * @warning admittedly a wonky operation
       * @remarks here the mock emulates the act of dying,
       *          by snuffing the UI-Bus connection sneakily.
       *          We leave the dead corpse hanging around,
       *          just for sake of further investigation,
       *          of course.
       */
      void
      kill()
        {
          log_.call (this->identify(), "kill");
          log_.destroy (getID().getSym());
          
          Nexus::zombificate (this->uiBus_);
          log_.event (string(getID()) + " successfully connected to zombie bus");
        }
      
      
      
      
      /* ==== Attributes and mock children ==== */
      
      std::map<string, string> attrib;
      std::vector<PMockElm>    scope;
      
      
      /* ==== Query/Verification API ==== */
      
      ID getID()  const
        {
          return uiBus_.getID();
        }
      
      bool
      isTouched()  const
        {
          return not virgin_;
        }
      
      bool
      isExpanded()  const
        {
          return expanded_;
        }
      
      bool
      isError()  const
        {
          return not isnil(error_);
        }
      
      string
      getMessage()  const
        {
          return message_;
        }
      
      string
      getError()  const
        {
          return error_;
        }
      

      
      EventMatch
      verify (string match)  const
        {
          return getLog().verify(match);
        }
      
      EventMatch
      verifyMatch (string regExp)  const
        {
          return getLog().verifyMatch(regExp);
        }
      
      EventMatch
      verifyEvent (string match)  const
        {
          return getLog().verifyEvent(match);
        }
      
      EventMatch
      verifyEvent (string classifier, string match)  const
        {
          return getLog().verifyEvent (classifier,match);
        }
      
      EventMatch
      verifyCall (string match)  const
        {
          return getLog().verifyCall(match);
        }
      
      EventMatch
      ensureNot (string match)  const
        {
          return getLog().ensureNot(match);
        }
      
      /** special verification match on a "state mark" message to this element */
      EventMatch
      verifyMark (string id)  const
        {
          return getLog().verify(id).type("mark").id(id);
        }
      
      /** verification match on a specific "state mark" message
       * @param id the ID-symbol used, identifying the kind of notification message
       * @param payloadMatch to be applied to the payload of the message solely
       */
      EventMatch
      verifyMark (string id, string payloadMatch)  const
        {
          return getLog().verifyEvent("mark", payloadMatch).type("mark").id(id);
        }
      
      template<typename X>
      EventMatch
      verifyMark (string id, X const& something)  const
        {
          return getLog().verifyEvent("mark", something).type("mark").id(id);
        }
      
      
      EventLog const&
      getLog()  const
        {
          return log_;
        }
      
      EventLog&
      joinLog (MockElm& otherMock)
        {
          log_.joinInto (otherMock.log_);
          return log_;
        }
      
      EventLog&
      joinLog (EventLog& otherLog)
        {
          log_.joinInto (otherLog);
          return log_;
        }
    };
  
  
  
}} // namespace stage::test
#endif /*STAGE_TEST_MOCK_ELM_H*/
