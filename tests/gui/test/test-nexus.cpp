/*
  test::Nexus  -  implementation base for test user interface backbone

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


/** @file test/test-nexus.cpp
 ** Implementation of a fake UI backbone for testing.
 ** This compilation unit provides the actual setup for running a faked
 ** user interface from unit tests.
 ** 
 ** @todo initial draft and WIP-WIP-WIP as of 11/2015
 ** 
 ** @see abstract-tangible-test.cpp
 ** 
 */


//#include "lib/util.hpp"
//#include "lib/symbol.hpp"
//#include "include/logging.h"
#include "test/test-nexus.hpp"
#include "gui/ctrl/nexus.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/idi/genfunc.hpp"
#include "lib/depend.hpp"

//#include <boost/noncopyable.hpp>
#include <string>
//#include <map>

//using std::map;
using std::string;

//using lib::idi::EntryID;
using lib::diff::GenNode;
using gui::ctrl::BusTerm;
//using util::contains;
//using util::isnil;

namespace gui {
namespace test{
  
  namespace { // internal details
    
    /**
     * @internal fake interface backbone and unit test rig
     * for simulated command and presentation state handling.
     * This implementation embodies the routing functionality
     * as found in the [real nexus][gui::ctrl::Nexus], and additionally
     * also implements the handler functions of the [gui::ctrl::CoreService].
     * The latter allows us to intercept command invocations and presentation
     * state messages
     */
    class TestNexus
      : public gui::ctrl::Nexus
      {
        
        
        virtual void
        act (GenNode const& command)
          {
            UNIMPLEMENTED("receive and handle command invocation");
          }
        
        
        virtual void
        note (ID subject, GenNode const& mark)  override
          {
            UNIMPLEMENTED ("receive and handle presentation state note messages.");
          }
        
        
        virtual operator string()  const
          {
            return lib::idi::instanceTypeID(this);
          }
        
      public:
        TestNexus()
          : Nexus(*this, lib::idi::EntryID<TestNexus>("mock-UI"))
          { }
      };
    
    
    /**
     * @internal a defunct interface backbone.
     * All UI-Bus operations are implemented NOP, but warning on STDRR
     * and logging the invocation to the internal log of [TestNexus].
     * This allows to set up deceased entities within a test rigged UI.
     */
    class ZombieNexus
      : public BusTerm
      {
        
        /* ==== defunct re-implementation of the BusTerm interface ==== */
        
        virtual void
        act (GenNode const& command)
          {
            UNIMPLEMENTED("act");
          }
        
        
        virtual void
        note (ID subject, GenNode const& mark)  override
          {
            UNIMPLEMENTED ("note.");
          }
        
        
        virtual void
        mark (ID subject, GenNode const& mark)  override
          {
            UNIMPLEMENTED ("mark.");
          }
        
        
        virtual operator string()  const
          {
            return lib::idi::instanceTypeID(this);
          }
        
        
        virtual BusTerm&
        routeAdd (ID identity, Tangible& newNode)  override
          {
            UNIMPLEMENTED ("routeAdd.");
          }
        
        
        virtual void
        routeDetach (ID node)  noexcept override
          {
            UNIMPLEMENTED ("routeDetach.");
          }
        
      public:
        /** fabricate a "dead terminal", marked as deceased.
         * @note intentionally to be sliced right after generation.
         *       All operations on this object are defunct.
         */
        ZombieNexus(string formerID)
          : BusTerm(lib::idi::EntryID<ZombieNexus>("defunct-"+formerID), *this)
          { }
      };
    
    lib::Depend<TestNexus> testNexus;
    
  } // internal details
  
  
  
  //NA::~NA() { }
  
  
  
  
  /**
   * @return reference to a node of the test UI bus,
   *         which allows to hook up new nodes for test
   */
  ctrl::BusTerm&
  Nexus::testUI()
  {
    return testNexus();
  }
  
  
  /**
   * @return a defunct BusTerm with up-link to [ZombieNexus]
   * @remarks useful to create zombie mock UI-Elements for testing.
   */
  void
  Nexus::zombificate (BusTerm& doomed)
  {
    string lateName = doomed.getID().getSym();
    doomed.~BusTerm();
//  log_.destroy (lateName);
    
    new(&doomed) ZombieNexus{lateName};
  }

}} // namespace gui::test
