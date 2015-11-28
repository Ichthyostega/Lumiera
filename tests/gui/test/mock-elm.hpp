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
#include "lib/idi/entry-id.hpp"
//#include "lib/util.hpp"
#include "gui/model/tangible.hpp"
#include "lib/diff/record.hpp"
#include "test/nexus.hpp"

//#include <boost/noncopyable.hpp>
#include <string>


namespace gui {
  namespace error = lumiera::error;
  using error::LUMIERA_ERROR_ASSERTION;
  
namespace test{
  
  
//  using lib::HashVal;
//  using util::isnil;
  using lib::idi::EntryID;
  using lib::diff::Record;
  using std::string;
  
  
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
      
      /* ==== Tangible interface ==== */
      
      virtual void doReset()
        {
          UNIMPLEMENTED ("mock doReset");
        }
      
      virtual void doExpand()
        {
          UNIMPLEMENTED ("mock doExpand");
        }
      
      virtual void doReveal()
        {
          UNIMPLEMENTED ("mock doReveal");
        }
      
      virtual void doMsg()
        {
          UNIMPLEMENTED ("mock doMsg");
        }
      
      virtual void doErr()
        {
          UNIMPLEMENTED ("mock doErr");
        }
      
      virtual void doFlash()
        {
          UNIMPLEMENTED ("mock doFlash");
        }
      
      virtual void doMark()
        {
          UNIMPLEMENTED ("mock doMark");
        }
      
      
    protected:
    public:
      explicit
      MockElm(string id)
        : MockElm(EntryID<MockElm>(id))
        { }
      
      explicit
      MockElm(ID identity, ctrl::BusTerm& nexus  =Nexus::testUI())
        : gui::model::Tangible(identity, nexus)
        { }
    };
  
  
  
}} // namespace gui::test
#endif /*GUI_TEST_MOCK_ELM_H*/
