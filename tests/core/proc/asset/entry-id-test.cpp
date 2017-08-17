/*
  EntryID(Test)  -  proof-of-concept test for a combined hash+symbolic ID

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file entry-id-test.cpp
 ** unit test \ref EntryID_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"

#include "lib/idi/entry-id.hpp"
#include "proc/asset/struct-scheme.hpp"
#include "proc/mobject/session/clip.hpp"
#include "proc/mobject/session/fork.hpp"
#include "lib/meta/trait-special.hpp"
#include "lib/util-foreach.hpp"
#include "lib/format-cout.hpp"
#include "lib/symbol.hpp"

#include <unordered_map>
#include <string>

using lib::test::showSizeof;
using lib::test::randStr;
using util::isSameObject;
using util::contains;
using util::and_all;
using lib::Literal;
using std::string;



namespace lib {
namespace idi {
namespace test{
  
  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
  
  namespace { // Test definitions...
    
    struct Dummy { };
    
  }
  
  using DummyID = EntryID<Dummy>;
  using ForkID = EntryID<proc::mobject::session::Fork>;
  using ClipID = EntryID<proc::mobject::session::Clip>;
  
  
  
  
  /***********************************************************************//**
   * @test proof-of-concept test for a combined symbolic and hash based ID.
   *       - create some symbolic IDs 
   *       - check default assignment works properly
   *       - check comparisons
   *       - check hashing
   *       - use the embedded hash ID (LUID) as hashtable key
   * 
   * @see lib::HashIndexed::Id
   * @see mobject::Placement real world usage example 
   */
  class EntryID_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          checkCreation();
          checkBasicProperties();
          checkComparisions();
          checkErasure();
          buildHashtable();
        }
      
      
      
      void
      checkCreation ()
        {
          DummyID dID1;
          DummyID dID2("strange");
          DummyID dID3;
          CHECK (dID1.isValid());
          CHECK (dID2.isValid());
          CHECK (dID3.isValid());
          CHECK (dID1 != dID2); CHECK (dID2 != dID1);
          CHECK (dID2 != dID3); CHECK (dID3 != dID2);
          CHECK (dID1 != dID3); CHECK (dID3 != dID1);
          
          ForkID tID1;
          ForkID tID2;
          ForkID tID3("special");
          CHECK (tID1.isValid());
          CHECK (tID2.isValid());
          CHECK (tID3.isValid());
          CHECK (tID1 != tID2); CHECK (tID2 != tID1);
          CHECK (tID2 != tID3); CHECK (tID3 != tID2);
          CHECK (tID1 != tID3); CHECK (tID3 != tID1);
          
          cout << dID1 << endl;
          cout << dID2 << endl;
          cout << dID3 << endl;
          
          cout << tID1 << endl;
          cout << tID2 << endl;
          cout << tID3 << endl;
          
          DummyID x (dID2);  // copy ctor
          CHECK (x == dID2);
          CHECK (!isSameObject (x, dID2));
        }
      
      
      void
      checkBasicProperties ()
        {
          using proc::asset::Asset;
          using proc::asset::STRUCT;
          using proc::asset::Category;
          using proc::asset::idi::getAssetIdent;
          
          ForkID tID(" test  ⚡ ☠ ☭ ⚡  track  ");
          
          // Symbol-ID will be "sanitised"
          CHECK ("test_track" == tID.getSym());
          CHECK (tID == ForkID("☢ test ☢ track ☢"));
          CHECK (tID == ForkID(string{"☢ test ☢ track ☢"}));
          
          // but: there is a pass-through for internal symbols
          CHECK (tID != ForkID(Symbol{"☢ test ☢ track ☢"}));
          
          CHECK (tID.getHash() == ForkID("☢ test ☢ track ☢").getHash());
          
          CHECK (getAssetIdent(tID) == Asset::Ident("test_track", Category(STRUCT,"forks"), "lumi", 0));
          
          CHECK (tID.getSym() == getAssetIdent(tID).name);
          CHECK (getAssetIdent(ForkID()).category == Category (STRUCT,"forks"));
          CHECK (getAssetIdent(ClipID()).category == Category (STRUCT,"clips"));
          
          ClipID cID2,cID3;
          CHECK (cID2.getSym() < cID3.getSym());
          CHECK (ClipID("x").getSym() == ClipID(" x ").getSym());
          
          for (uint i=0; i<10000; ++i)
            {
              ForkID arbitrary(randStr(30));
              CHECK (0 < arbitrary.getHash());
              CHECK (tID.getHash() != arbitrary.getHash());
              tID = arbitrary;
              CHECK (tID.getHash() == arbitrary.getHash());
              CHECK (tID.getSym()  == arbitrary.getSym());
              CHECK (getAssetIdent(tID)== getAssetIdent(arbitrary));
            }
          
          cout << showSizeof<ForkID>() << endl;
          cout << showSizeof<BareEntryID>() << endl;
          cout << showSizeof<lumiera_uid>() << endl;
          cout << showSizeof<string>() << endl;
          cout << showSizeof<void*>() << endl;
          CHECK (sizeof(ForkID) == sizeof(BareEntryID));
          CHECK (sizeof(ForkID) == sizeof(lumiera_uid) + sizeof(string));
        }
      
      
      void
      checkComparisions ()
        {
          ForkID tID1("a1");
          ForkID tID2("a1");
          ForkID tID3("a2");
          ForkID tID4("b");
          CHECK (tID1 == tID2);
          
          CHECK (tID2 < tID3);
          CHECK (tID2 <= tID3);
          CHECK (tID3 >= tID2);
          CHECK (tID3 > tID2);
          
          CHECK (tID3 < tID4);
          CHECK (tID3 <= tID4);
          CHECK (tID4 >= tID3);
          CHECK (tID4 > tID3);
          
          ForkID trackID1, trackID2;
          CHECK (trackID1 < trackID2); // auto generated IDs are prefix + running counter
        }
      
      
      
      /** @test handling of EntryIDs through their common base class,
       *        which means erasing the specific type information.
       *        While this type information can't be recovered 
       *        after erasure, we can try to upcast back 
       *        to a known type; this upcast is safe,
       *        because the embedded hash-ID
       *        is based on the type info. 
       */
      void
      checkErasure ()
        {
          ForkID fID("suspicious");
          ClipID cID("suspicious");
          
          CHECK (fID.getHash() != cID.getHash());
          CHECK (fID.getSym()  == cID.getSym());
          
          BareEntryID bIDf (fID);
          BareEntryID bIDc (cID);
          
          CHECK (bIDf != bIDc);
          CHECK (bIDf.getHash() != bIDc.getHash());
          CHECK (bIDf.getSym()  == bIDc.getSym());
          CHECK ("suspicious"  == bIDc.getSym());
          
          using proc::mobject::session::Fork;
          using proc::mobject::session::Clip;
          ForkID tIDnew = bIDf.recast<Fork>();
          ClipID cIDnew = bIDc.recast<Clip>();
          CHECK (tIDnew == fID);
          CHECK (cIDnew == cID);
          
          VERIFY_ERROR (WRONG_TYPE, bIDf.recast<Clip>());
          VERIFY_ERROR (WRONG_TYPE, bIDc.recast<Fork>());
          VERIFY_ERROR (WRONG_TYPE, bIDc.recast<Dummy>());
          VERIFY_ERROR (WRONG_TYPE, bIDf.recast<Dummy>());
          
          CHECK (fID == ForkID::recast (bIDf));          // equivalent static API on typed subclass
          VERIFY_ERROR (WRONG_TYPE,  ForkID::recast(bIDc));
          VERIFY_ERROR (WRONG_TYPE,  ClipID::recast(bIDf));
          VERIFY_ERROR (WRONG_TYPE, DummyID::recast(bIDc));
          VERIFY_ERROR (WRONG_TYPE, DummyID::recast(bIDf));
          
          // mixed equality comparisons (based on the hash)
          BareEntryID bIDt_copy (bIDf);
          CHECK (bIDf == bIDt_copy);
          CHECK (!isSameObject (bIDf, bIDt_copy));
          
          CHECK (fID != bIDc);
          CHECK (cID != bIDt_copy);
          CHECK (fID == bIDt_copy);
          
          CHECK (bIDf == ForkID ("suspicious"));
          CHECK (bIDf != ClipID ("suspicious"));
          CHECK (bIDc == ClipID ("suspicious"));
          CHECK (ForkID ("suspicious") != ClipID ("suspicious"));
        }
      
      
      
                              //---key--+-value-+-hash-function--- 
      typedef std::unordered_map<DummyID, string, DummyID::UseEmbeddedHash> Hashtable;
      
      /** @test build a hashtable, using EntryID as key,
       *        thereby using the embedded hash-ID
       *  @note there is a known weakness of the boost::hash
       *        when used on IDs with a running number suffix. /////TICKET #587
       *        We use a trick to spread the numbers better.
       *  @see  HashGenerator_test#verify_Knuth_workaround
       */
      void
      buildHashtable ()
        {
          Hashtable tab;
          
          for (uint i=0; i<100000; ++i)
            {
              DummyID dummy;
              tab[dummy] = string(dummy);
            }
          
          CHECK (and_all (tab, verifyEntry));
          CHECK (100000 == tab.size());
        }
      
      
      static bool
      verifyEntry (Hashtable::value_type entry)
        {
          return checkForHashCollision(string(entry.first), entry.second);
        }
      
      static bool
      checkForHashCollision(string const& key, string const& val)
        {
          if (key != val) cout << "Hash collision: "<<key<<"  !=  "<<val<< endl;
          return key == val;
        }
      
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (EntryID_test, "unit common");
  
  
}}} // namespace lib::idi::test
