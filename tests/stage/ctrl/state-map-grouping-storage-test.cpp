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

/** @file state-map-grouping-storage-test.cpp
 ** unit test \ref StateMapGroupingStorage_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "gui/ctrl/state-map-grouping-storage.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/util.hpp"

#include <string>


using std::string;
using lib::idi::EntryID;
using lib::diff::GenNode; 
using util::isSameObject;
using util::isnil;


namespace gui  {
namespace ctrl {
namespace test {
  
  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
  
  
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
          EntryID<int> wooof ("wau");                // different (type) hash
          
          EntryID<int> miaow ("miau");
          EntryID<int> quack ("quack");
          
          GenNode poodle{"poodle", "Pudel"};
          GenNode toyPoodle{"poodle", "Zwergpudel"};
          GenNode labradoodle {"poodle", false };    // different payload type
          GenNode mastiff{"mastiff", "Dogge"};
          GenNode duck{"duck", "Ente"};
          
          
          StateMapGroupingStorage storage;
          
          CHECK (isnil (storage));
          CHECK (0 == storage.size());
          
          storage.record (woof, poodle);
          CHECK (not isnil(storage));
          CHECK (1 == storage.size());
          
          CHECK (poodle == storage.retrieve(woof, "poodle"));
          CHECK (not isSameObject (poodle, storage.retrieve(woof, "poodle")));
          
          CHECK (Ref::NO == storage.retrieve(wooof, "poodle"));
          CHECK (Ref::NO == storage.retrieve(woof, "pooodle"));
          
          storage.record (woof, mastiff);
          CHECK (2 == storage.size());
          CHECK (poodle == storage.retrieve(woof, "poodle"));
          CHECK (mastiff == storage.retrieve(woof, "mastiff"));
          
          // upgrade the poodle
          storage.record (woof, toyPoodle);
          CHECK (2 == storage.size());
          CHECK (poodle    != storage.retrieve(woof, "poodle"));
          CHECK (toyPoodle == storage.retrieve(woof, "poodle"));
          
          // since properties are keyed just by ID-string,
          // we might attempt sneak in a fake poodle
          // fortunately GenNode disallows cross-type abominations
          VERIFY_ERROR (WRONG_TYPE, storage.record (woof, labradoodle) );
          
          CHECK (2 == storage.size());
          CHECK (toyPoodle == storage.retrieve(woof, "poodle"));
          CHECK (mastiff == storage.retrieve(woof, "mastiff"));
          
          
          storage.record (quack, duck);
          CHECK (3 == storage.size());
          CHECK (toyPoodle == storage.retrieve(woof, "poodle"));
          CHECK (mastiff == storage.retrieve(woof, "mastiff"));
          CHECK (duck  == storage.retrieve(quack, "duck"));

          auto elm = storage.find(woof);
          CHECK (elm != storage.end());
          CHECK (woof == StateMapGroupingStorage::getID(*elm));
          CHECK (2    == StateMapGroupingStorage::getState(*elm).size());
          CHECK (Ref::NO   == StateMapGroupingStorage::getState(*elm, "doodle"));
          CHECK (toyPoodle == StateMapGroupingStorage::getState(*elm, "poodle"));
          CHECK (mastiff   == StateMapGroupingStorage::getState(*elm, "mastiff"));
          
          elm = storage.find(miaow);
          CHECK (elm == storage.end());
          
          CHECK (Ref::NO == storage.retrieve(miaow, "meow"));
          storage.record (miaow, labradoodle);
          CHECK (labradoodle == storage.retrieve(miaow, "poodle"));
          CHECK (4 == storage.size());
          
          storage.clearProperty(miaow, "meow");
          CHECK (4 == storage.size());
          CHECK (labradoodle == storage.retrieve(miaow, "poodle"));
          // clearing non existent property has no effect
          
          storage.clearProperty(miaow, "poodle");
          CHECK (3 == storage.size());
          CHECK (Ref::NO == storage.retrieve(miaow, "poodle"));
          
          // but note, an empty element record has been left back (this is harmless)
          elm = storage.find(miaow);
          CHECK (elm != storage.end());
          CHECK (miaow == StateMapGroupingStorage::getID(*elm));
          CHECK (0     == StateMapGroupingStorage::getState(*elm).size());
          
          
          storage.clear();
          CHECK (isnil (storage));
          CHECK (0 == storage.size());
          CHECK (Ref::NO == storage.retrieve(woof, "poodle"));
          CHECK (Ref::NO == storage.retrieve(woof, "mastiff"));
          CHECK (Ref::NO == storage.retrieve(quack, "duck"));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (StateMapGroupingStorage_test, "unit gui");
  
  
}}} // namespace gui::ctrl::test
