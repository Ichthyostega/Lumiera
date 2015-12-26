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
 ** gui::model::Tangible, meaning that any generic effect of interface interactions
 ** can be expressed in terms of this interface contract. As far as the UI participates
 ** in interactions with the lower layers, like e.g. command invocation, structure updates
 ** and state notifications, these processes can be modelled and verified with the help
 ** of a specially prepared Tangible instance. This gui::test::MockElm provides the
 ** necessary instrumentation to observe what has been invoked and received.
 ** 
 ** @todo initial draft and WIP-WIP-WIP as of 11/2015
 ** 
 ** @see abstract-tangible-test.cpp
 ** 
 */


#ifndef GUI_TEST_MOCK_ELM_H
#define GUI_TEST_MOCK_ELM_H


#include "lib/error.hpp"
//#include "lib/idi/entry-id.hpp"
//#include "lib/util.hpp"
#include "lib/test/event-log.hpp"
#include "gui/model/tangible.hpp"
#include "lib/diff/record.hpp"
#include "lib/idi/genfunc.hpp"
#include "test/test-nexus.hpp"
#include "lib/symbol.hpp"

#include <iostream>
#include <string>


namespace gui {
  namespace error = lumiera::error;
  using error::LUMIERA_ERROR_ASSERTION;
  using lib::test::EventLog;
  using lib::test::EventMatch;
  
namespace test{
  
  
//  using lib::HashVal;
//  using util::isnil;
//  using lib::idi::EntryID;
//  using lib::diff::Record;
  using lib::Symbol;
  using std::string;
  using std::cout;
  using std::cerr;
  using std::endl;
  
  
  /**
   * Mock UI element or controller.
   * Within Lumiera, all interface components of relevance are based
   * on the [Tangible] interface, which we mock here for unit testing.
   * This special implementation is instrumented to [log][lib::test::EventLog]
   * any invocation and any messages sent or received through the UI Backbone,
   * which is formed by the [UiBus].
   * 
   * @todo some usage details
   * @see abstract-tangible-test.cpp
   */
  class MockElm
    : public gui::model::Tangible
    {
      using _Par = gui::model::Tangible;
      
      EventLog log_{this->identify()};
      
      bool virgin_{true};
      bool expanded_{false};
      
      
      /* ==== Tangible interface ==== */
      
      virtual void
      doReset()  override
        {
          log_.call(this->identify(), "reset");
          expanded_ = false;
          virgin_ = true;
          log_.event("reset");
        }
      
      virtual bool
      doExpand (bool yes)  override
        {
          log_.call(this->identify(), "expand", yes);
          virgin_ = false;
          expanded_ = yes;
          log_.event (expanded_? "expanded" : "collapsed");
          return true;
        }
      
      virtual void
      doReveal (ID child)  override
        {
          UNIMPLEMENTED ("mock doReveal");
        }
      
      virtual void
      doRevealYourself()  override
        {
          UNIMPLEMENTED ("mock doRevealYourself");
        }
      
      virtual void
      doMsg (string text)  override
        {
          log_.call (this->identify(), "doMsg", text);
          cout << this->identify() << " <-- Message(\""<<text<<"\")" <<endl;
          log_.note ("type=mark", "ID=Message", text);
        }
      
      virtual void
      doErr (string text)  override
        {
          log_.call (this->identify(), "doErr", text);
          cerr << this->identify() << " <-- Error(\""<<text<<"\")" <<endl;
          log_.note ("type=mark", "ID=Error", text);
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
          cout << this->identify() << " <-- state-mark = "<< string(mark) <<endl;
          log_.note ("type=mark", "ID="+mark.idi.getSym(), mark);
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
        : gui::model::Tangible(identity, nexus)
        {
          log_.call (this->identify(), "ctor", identity, string(nexus));
          log_.create (getID().getSym());
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
          log_.event ("successfully connected to zombie bus");
        }
      
      
      
      
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
      verifyMark (string msgContentMatch)  const
        {
          return getLog().verifyEvent("mark", msgContentMatch);
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
    };
  
  
  
}} // namespace gui::test
#endif /*GUI_TEST_MOCK_ELM_H*/
