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


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"

#include "proc/asset/entry-id.hpp"
#include "proc/mobject/session/clip.hpp"
#include "proc/mobject/session/track.hpp"
#include "lib/meta/trait-special.hpp"
#include "lib/util-foreach.hpp"
#include "lib/symbol.hpp"

#include <tr1/unordered_map>
#include <iostream>
#include <string>

using lib::test::showSizeof;
using lib::test::randStr;
using util::isSameObject;
using util::and_all;
using lib::Literal;
using std::string;
using std::cout;
using std::endl;



namespace asset{
namespace test {
  
  namespace { // Test definitions...
    
    struct Dummy { };
    
    typedef EntryID<Dummy> DummyID;
    typedef EntryID<mobject::session::Track> TrackID;
    typedef EntryID<mobject::session::Clip>  ClipID;
  }
  
  
  
  
  
  /***************************************************************************
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
          
          TrackID tID1;
          TrackID tID2;
          TrackID tID3("special");
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
          TrackID tID(" test  ⚡ ☠ ☭ ⚡  track  ");
          CHECK (tID.getIdent() == Asset::Ident("test_track", Category(STRUCT,"tracks"), "lumi", 0));
          
          CHECK (tID.getHash() == TrackID("☢ test ☢ track ☢").getHash());
          
          CHECK (tID.getSym() == tID.getIdent().name);
          CHECK (TrackID().getIdent().category == Category (STRUCT,"tracks"));
          CHECK (ClipID().getIdent().category  == Category (STRUCT,"clips"));
          
          ClipID cID2,cID3;
          CHECK (cID2.getSym() < cID3.getSym());
          CHECK (ClipID("x").getSym() == ClipID(" x ").getSym());
          
          for (uint i=0; i<10000; ++i)
            {
              TrackID arbitrary(randStr(30));
              CHECK (0 < arbitrary.getHash());
              CHECK (tID.getHash() != arbitrary.getHash());
              tID = arbitrary;
              CHECK (tID.getHash() == arbitrary.getHash());
              CHECK (tID.getSym()  == arbitrary.getSym());
              CHECK (tID.getIdent()== arbitrary.getIdent());
            }
          
          cout << showSizeof<TrackID>() << endl;
          cout << showSizeof<BareEntryID>() << endl;
          CHECK (sizeof(TrackID) == sizeof(BareEntryID));
          CHECK (sizeof(TrackID) == sizeof(lumiera_uid) + sizeof(void*));
        }
      
      
      void
      checkComparisions ()
        {
          TrackID tID1("a1");
          TrackID tID2("a1");
          TrackID tID3("a2");
          TrackID tID4("b");
          CHECK (tID1 == tID2);
          
          CHECK (tID2 < tID3);
          CHECK (tID2 <= tID3);
          CHECK (tID3 >= tID2);
          CHECK (tID3 > tID2);
          
          CHECK (tID3 < tID4);
          CHECK (tID3 <= tID4);
          CHECK (tID4 >= tID3);
          CHECK (tID4 > tID3);
          
          TrackID trackID1, trackID2;
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
          TrackID tID("suspicious");
          ClipID  cID("suspicious");
          
          CHECK (tID.getHash() != cID.getHash());
          CHECK (tID.getSym()  == cID.getSym());
          
          BareEntryID bIDt (tID);
          BareEntryID bIDc (cID);
          
          CHECK (bIDt != bIDc);
          CHECK (bIDt.getHash() != bIDc.getHash());
          CHECK (bIDt.getSym()  == bIDc.getSym());
          CHECK ("suspicious"  == bIDc.getSym());
          
          using mobject::session::Track;
          using mobject::session::Clip;
          TrackID tIDnew = bIDt.recast<Track>();
          ClipID  cIDnew = bIDc.recast<Clip>();
          CHECK (tIDnew == tID);
          CHECK (cIDnew == cID);
          
          VERIFY_ERROR (WRONG_TYPE, bIDt.recast<Clip>());
          VERIFY_ERROR (WRONG_TYPE, bIDc.recast<Track>());
          VERIFY_ERROR (WRONG_TYPE, bIDc.recast<Dummy>());
          VERIFY_ERROR (WRONG_TYPE, bIDt.recast<Dummy>());
          
          CHECK (tID == TrackID::recast (bIDt));         // equivalent static API on typed subclass
          VERIFY_ERROR (WRONG_TYPE, TrackID::recast(bIDc));
          VERIFY_ERROR (WRONG_TYPE,  ClipID::recast(bIDt));
          VERIFY_ERROR (WRONG_TYPE, DummyID::recast(bIDc));
          VERIFY_ERROR (WRONG_TYPE, DummyID::recast(bIDt));
          
          // mixed equality comparisons (based on the hash)
          BareEntryID bIDt_copy (bIDt);
          CHECK (bIDt == bIDt_copy);
          CHECK (!isSameObject (bIDt, bIDt_copy));
          
          CHECK (tID != bIDc);
          CHECK (cID != bIDt_copy);
          CHECK (tID == bIDt_copy);
          
          CHECK (bIDt == TrackID ("suspicious"));
          CHECK (bIDt != ClipID ("suspicious"));
          CHECK (bIDc == ClipID ("suspicious"));
          CHECK (TrackID ("suspicious") != ClipID ("suspicious"));
        }
      
      
      
                                   //---key--+-value-+-hash-function--- 
      typedef std::tr1::unordered_map<DummyID, string, DummyID::UseEmbeddedHash> Hashtable;
      
      /** @test build a hashtable, using EntryID as key,
       *        thereby using the embedded hash-ID */
      void
      buildHashtable ()
        {
          Hashtable tab;
          
          for (uint i=0; i<1000; ++i)     /////////////////////////////////////TICKET #587  we get strange collisions for 10000 entries
            {
              DummyID dummy;
              tab[dummy] = string(dummy);
            }
          
          CHECK (1000 == tab.size());
          
          CHECK (and_all (tab, verifyEntry));
        }
      
      
      static bool
      verifyEntry (Hashtable::value_type entry)
        {
          return entry.second == string(entry.first);
        }
      
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (EntryID_test, "unit asset");
  
  
}} // namespace asset::test
