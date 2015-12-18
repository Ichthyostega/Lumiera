/*
  TEST-NEXUS.hpp  -  fake user interface backbone for test support

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


/** @file test/test-nexus.hpp
 ** A fake UI backbone for investigations and unit testing.
 ** Any relevant element within the Lumiera GTK UI is connected to the [UI-Bus][ui-bus.hpp]
 ** So for testing and investigation we need a white room setup to provide an instrumented
 ** backbone to run any test probes against. The test::Nexus allows to [hook up][::testUI]
 ** a generic interface element, to participate in a simulated interface interaction.
 ** 
 ** This class test::Nexus acts as front-end for unit tests, while the actual implementation
 ** of a test rigged mock interface backbone remains an implementation detail.
 ** 
 ** @todo initial draft and WIP-WIP-WIP as of 12/2015
 ** 
 ** @see abstract-tangible-test.cpp
 ** 
 */


#ifndef GUI_TEST_TEST_NEXUS_H
#define GUI_TEST_TEST_NEXUS_H


#include "lib/error.hpp"
//#include "lib/idi/entry-id.hpp"
#include "gui/ctrl/bus-term.hpp"
//#include "lib/util.hpp"
//#include "gui/model/tangible.hpp"
//#include "lib/diff/record.hpp"

#include <boost/noncopyable.hpp>
//#include <string>


namespace gui {
namespace test{
  
//  using lib::HashVal;
//  using util::isnil;
//  using lib::idi::EntryID;
//  using lib::diff::Record;
//  using std::string;
  
  
  /**
   * Mock UI backbone for unit testing.
   * In the absence of a real UI, this simulated [UI-Bus][ui-bus.hpp]
   * can be used to wire a [test probe][MockElm] and address it in unit testing.
   * 
   * @todo some usage details
   * @see abstract-tangible-test.cpp
   */
  class Nexus
    : boost::noncopyable
    {
      
    public:
      /** get a connection point to a UI backbone faked for test */
      static ctrl::BusTerm& testUI();
    };
  
  
  
}} // namespace gui::test
#endif /*GUI_TEST_TEST_NEXUS_H*/
