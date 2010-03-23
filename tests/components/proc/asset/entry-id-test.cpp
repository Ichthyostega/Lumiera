/*
  EntryID(Test)  -  proof-of-concept test for a combined hash+symbolic ID
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
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
#include "lib/util-foreach.hpp"
#include "lib/symbol.hpp"

#include <tr1/unordered_map>
#include <iostream>
#include <string>

using lib::test::randStr;
using util::isSameObject;
using util::for_each;
using lib::Literal;
using std::string;
using std::cout;
using std::endl;

using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;


namespace asset{
namespace test {
  
  namespace { // Test data...
    
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
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #582
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #582
          DummyID dID1;
          DummyID dID2("strange");
          DummyID dID3;
          CHECK (dID1);
          CHECK (dID2);
          CHECK (dID3);
          CHECK (dID1 != dID2); CHECK (dID2 != dID1);
          CHECK (dID2 != dID3); CHECK (dID3 != dID2);
          CHECK (dID1 != dID3); CHECK (dID3 != dID1);
          
          TrackID tID1;
          TrackID tID2;
          TrackID tID3("special");
          CHECK (tID1);
          CHECK (tID2);
          CHECK (tID3);
          CHECK (tID1 != tID2); CHECK (tID2 != tID1);
          CHECK (tID2 != tID3); CHECK (tID3 != tID2);
          CHECK (tID1 != tID3); CHECK (tID3 != tID1);
          
          cout << dID1 << dID2 << dID3 << endl;
          cout << tID1 << tID2 << tID3 << endl;
          
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
          
          CHECK (isSameObject (tID.getSym(), tID.getIdent().name));
          CHECK ("tracks" == TrackID().getIdent().category.path);
          CHECK ("clips"  == ClipID().getIdent().category.path);
          
          CHECK (ClipID().getSym() < ClipID().getSym());
          CHECK (ClipID("x").getSym() == ClipID(" x ").getSym());
          
          for (uint i=0; i<1000; ++i)
            {
              TrackID arbitrary(randStr(20));
              CHECK (0 < arbitrary.getHash());
              CHECK (tID.getHash() != arbitrary.getHash());
              tID = arbitrary;
              CHECK (tID.getHash() == arbitrary.getHash());
              CHECK (tID.getSym()  == arbitrary.getSym());
              CHECK (tID.getIdent()== arbitrary.getIdent());
            }
          
          CHECK (sizeof (tID) == sizeof(hash::LuidH) + sizeof(Literal));
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
          
          CHECK (TrackID() < TrackID());
        }
      
      
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
          
          TrackID tIDnew = bIDt.recast<TrackID>();
          ClipID  cIDnew = bIDc.recast<ClipID>();
          CHECK (tIDnew == tID);
          CHECK (cIDnew == cID);
          
          VERIFY_ERROR (WRONG_TYPE, bIDt.recast<ClipID>());
          VERIFY_ERROR (WRONG_TYPE, bIDc.recast<TrackID>());
          VERIFY_ERROR (WRONG_TYPE, bIDc.recast<Dummy>());
          
          CHECK (tID == bIDt.recast<mobject::session::Track>());   // alternatively can specify type directly
          CHECK (tID == TrackID::recast (bIDt));                   // equivalent static API on typed subclass
          
          lumiera_uid plainLUID;
          lumiera_uid_copy (&plainLUID, tID.getHash().get());
          
          CHECK (cID == ClipID::recast ("suspicious", plainLUID)); // upcast from type erased data (note: changed type)
        }
      
      
      void
      buildHashtable ()
        {                              //---key--+-value-+-hash-function--- 
          typedef std::tr1::unordered_map<DummyID, string, DummyID::UseEmbeddedHash()> Hashtable;
              
          Hashtable tab;
          
          for (uint i=0; i<1000; ++i)
            {
              DummyID dummy;
              tab[dummy] = string(dummy);
            }
          
          CHECK (1000 == tab.size());
          
          for_each (tab.getKeys(), tab[_1] == string(_1));      /////TODO use boost::lambda to make this happen....
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (EntryID_test, "unit common");
  
  
}} // namespace asset::test
