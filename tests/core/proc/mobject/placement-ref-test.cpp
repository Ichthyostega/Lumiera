/*
  PlacementRef(Test)  -  generic reference to a Placement within the Session

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file placement-ref-test.cpp
 ** unit test \ref PlacementRef_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/mobject/placement-ref.hpp"
#include "proc/mobject/session/placement-index.hpp"
#include "proc/mobject/session/session-service-mock-index.hpp"
#include "proc/mobject/explicitplacement.hpp"
#include "proc/mobject/test-dummy-mobject.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"


using util::isSameObject;


namespace proc    {
namespace mobject {
namespace session {
namespace test    {
  
  using namespace mobject::test;
  typedef TestPlacement<TestSubMO21> PSub;
  
  typedef PlacementMO::ID P_ID;
  
  
  /***********************************************************************//**
   * @test properties and behaviour of the reference-mechanism for Placements.
   *       We create an mock placement index and install it to be used
   *       by all PlacementRef instances while conducting this test.
   *       Then we add two dummy placements and create some
   *       references to conduct placement operations
   *       through these references.
   * @see  mobject::Placement
   * @see  mobject::MObject#create
   * @see  mobject::Placement#addPlacement
   * @see  mobject::Placement#resolve
   */
  class PlacementRef_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          PSub testPlacement1(*new TestSubMO21);
          PSub testPlacement2(*new TestSubMO21);
          
          lib::time::Time twoSec(0,2);
          testPlacement2.chain (twoSec);     // define start time of Placement-2 to be at t=2sec
          
          // Prepare an (test)Index backing the PlacementRefs
          PPIdx index = SessionServiceMockIndex::install();
          PMO& root = index->getRoot();
          
          P_ID id1   = index->insert (testPlacement1, root);
          P_ID tmpID = index->insert (testPlacement2, root);
          CHECK (2 == index->size());
          
          // References to the "live" placements within our test index
          PMO&  p1 = index->find(id1);
          PMO&  p2 = index->find(tmpID);
          
          PlacementMO::Id<TestSubMO21> id2 = p2.recastID<TestSubMO21>();
          CHECK (id2);
          CHECK (id2 != p1.getID());
          
          // create placement refs
          PlacementRef<TestSubMO21> ref1 (p1);
          PlacementRef<TestSubMO21> ref2 (id2);
          
          PlacementRef<MObject> refX (ref2);
          
          CHECK (ref1);
          CHECK (ref2);
          CHECK (refX);
          CHECK (ref1 != ref2);
          CHECK (ref2 == refX);
          
          // indeed a "reference": resolves to the same memory location
          CHECK (isSameObject (p1, *ref1));
          CHECK (isSameObject (p2, *ref2));
          CHECK (isSameObject (p2, *refX));
          
          cout << *ref1 << endl;
          cout << *ref2 << endl;
          cout << *refX << endl;
          
          // PlacementRef mimics placement behaviour
          ref1->specialAPI();
          CHECK (2 == ref1.use_count());
          CHECK (2 == ref2.use_count());
          ExplicitPlacement exPla = refX.resolve();
          CHECK (exPla.time == twoSec);            // indeed get back the time we set on p2 above
          CHECK (3 == ref2.use_count());           // exPla shares ownership with p2
          
          CHECK (index->contains(ref1));           // ref can stand-in for a placement-ID
          CHECK (sizeof(id2) == sizeof(ref2));     // (and is actually implemented based on an ID)
          
          // assignment on placement refs
          refX = ref1;
          CHECK (ref1 != ref2);
          CHECK (ref1 == refX);
          CHECK (ref2 != refX);
          
          // re-assignment with a new placement
          refX = p2;
          CHECK (refX == ref2);
          CHECK (isSameObject (*refX, p2));
          refX = p1.getID();
          CHECK (refX == ref1);
          CHECK (refX != ref2);
          CHECK (isSameObject (*refX, p1));
          
          LumieraUid luid2 (p2.getID().get());
          refX = luid2;                            // assignment works even based on a plain LUID
          ref2 = ref1;
          ref1 = refX;                             // dynamic type check when downcasting
          CHECK (isSameObject (p1, *ref2));
          CHECK (isSameObject (p2, *ref1));
          refX = ref2;
          ref2 = ref1;
          ref1 = refX;
          CHECK (isSameObject (p1, *ref1));
          CHECK (isSameObject (p1, *refX));
          CHECK (isSameObject (p2, *ref2));
          CHECK (ref1 != ref2);
          CHECK (ref1 == refX);
          CHECK (ref2 != refX);
          
          // resolution is indeed "live", we see changes to the referred placement
          CHECK (refX.resolve().time == lib::time::Time::MIN);
          p1.chain = p2.chain;                     // do a change on the placement within index....
          CHECK (refX.resolve().time == twoSec);   // now we get the time tie we originally set on p2
          
          CHECK (p1.getID() != p2.getID());        // but the instance identities are still unaltered
          CHECK (2 == ref1.use_count());
          CHECK (3 == ref2.use_count());           // one more because of shared ownership with exPla
          
          
          // actively removing p2 invalidates the other refs to
          index->remove (ref1);
          CHECK (!ref1);                           // checks invalidity without throwing
          CHECK (!refX);
          VERIFY_ERROR(NOT_IN_SESSION, *ref1 );
          
          // deliberately create an invalid PlacementRef
          PlacementRef<TestSubMO21> bottom;
          CHECK (!bottom);
          VERIFY_ERROR(BOTTOM_PLACEMENTREF, *bottom );
          VERIFY_ERROR(BOTTOM_PLACEMENTREF, bottom->specialAPI() );
          VERIFY_ERROR(BOTTOM_PLACEMENTREF, bottom.resolve() );
          
          //consistency check; then reset PlacementRef index to default
          CHECK (1 == index->size());
          CHECK (index->isValid());
          index.reset();
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (PlacementRef_test, "unit session");
  
  
}}}} // namespace proc::mobject::session::test
