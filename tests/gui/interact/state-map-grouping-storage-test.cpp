/*
  StateMapGroupingStorage(Test)  -  verify storage table for capturing UI state

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


#include "lib/test/run.hpp"
//#include "lib/test/test-helper.hpp"
//#include "gui/ctrl/bus-term.hpp"
//#include "gui/interact/presentation-state-manager.hpp"
#include "gui/interact/state-map-grouping-storage.hpp"
//#include "test/test-nexus.hpp"
//#include "test/mock-elm.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/format-cout.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/luid.h"
#include "lib/util.hpp"

#include <string>


using std::string;
using lib::idi::EntryID;
//using lib::idi::BareEntryID;
//using gui::interact::PresentationStateManager;
//using gui::ctrl::BusTerm;
//using gui::test::MockElm;
using lib::diff::GenNode; 
//using lib::diff::Rec;
//using lib::diff::Ref;
//using lib::time::Time;
//using lib::time::TimeSpan;
//using lib::hash::LuidH;
//using lib::HashVal;
using util::isSameObject;
using util::isnil;


namespace gui  {
namespace interact{
namespace test {
  
//  using proc::control::LUMIERA_ERROR_UNBOUND_ARGUMENTS;
//  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
  
  namespace { // test fixture...
    
  }//(End) test fixture
  
  
  
  
  
  
  /**************************************************************************//**
   * @test verify the storage structure for capturing UI state works as intended.
   * 
   * @see BusTerm_test::captureStateMark()
   * @see BusTerm_test::replayStateMark()
   */
  class StateMapGroupingStorage_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          EntryID<char> woof ("wau");
          EntryID<int> wooof ("wau");
          
          EntryID<int> miaow ("miau");
          EntryID<int> quack ("quack");
          
          GenNode poodle{"poodle", "Pudel"};
          GenNode pseudoPoodle {"poodle", false};
          
          GenNode mastiff{"mastiff", "Dogge"};
          
          StateMapGroupingStorage storage;
          
          CHECK (isnil (storage));
          CHECK (0 == storage.size());
          
          storage.record (woof, poodle);
          CHECK (not isnil(storage));
          CHECK (1 == storage.size());
          
          cout << poodle <<endl;
          cout << storage.retrieve(woof, "poodle") <<endl;
          CHECK (poodle == storage.retrieve(woof, "poodle"));
          CHECK (not isSameObject (poodle, storage.retrieve(woof, "poodle")));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (StateMapGroupingStorage_test, "unit gui");
  
  
}}} // namespace gui::model::test
