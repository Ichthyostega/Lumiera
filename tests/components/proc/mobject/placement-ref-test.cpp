/*
  PlacementRef(Test)  -  generic reference to a Placement within the Session
 
  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
 
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
#include "proc/mobject/placement.hpp"
#include "proc/mobject/placement-ref.hpp"
#include "proc/mobject/session/placement-index.hpp"
#include "proc/mobject/session/session-service-mock-index.hpp"
#include "proc/mobject/explicitplacement.hpp"
#include "proc/mobject/test-dummy-mobject.hpp"
#include "lib/lumitime.hpp"
#include "lib/util.hpp"

#include <iostream>

using util::isSameObject;
using lumiera::Time;
using std::string;
using std::cout;
using std::endl;


namespace mobject {
namespace session {
namespace test    {
  
  using namespace mobject::test;
  typedef TestPlacement<TestSubMO21> PSub;
  
  
  /***************************************************************************
   * @test properties and behaviour of the reference-mechanism for Placements.
   *       We create an mock placement index and install it to be used
   *       by all PlacementRef instances while conducting this test.
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
          PSub p1(*new TestSubMO21);
          PSub p2(*new TestSubMO21);
          p2.chain(Time(2));         // define start time of Placement-2 to be at t=2
          
          // Prepare an (test)Index backing the PlacementRefs
          PPIdx index = SessionServiceMockIndex::install();
          PMO& root = index->getRoot();
          
          index->insert (p1, root);
          index->insert (p2, root);
          ASSERT (2 == index->size());
          
          PlacementMO::Id<TestSubMO21> id2 = p2.recastID<TestSubMO21>();
          ASSERT (id2);
          ASSERT (id2 != p1.getID());
          
          // create placement refs
          PlacementRef<TestSubMO21> ref1 (p1);
          PlacementRef<TestSubMO21> ref2 (id2);
          
          PlacementRef<MObject> refX (ref2);
          
          ASSERT (ref1);
          ASSERT (ref2);
          ASSERT (refX);
          ASSERT (ref1 != ref2);
          ASSERT (ref2 == refX);
          
          // indeed a "reference": resolves to the same memory location
          ASSERT (isSameObject (p1, *ref1));
          ASSERT (isSameObject (p2, *ref2));
          ASSERT (isSameObject (p2, *refX));
          
          cout << string(*ref1) << endl;
          cout << string(*ref2) << endl;
          cout << string(*refX) << endl;
          
          // PlacementRef mimics placement behaviour
          ref1->specialAPI();
          ASSERT (1 == ref1.use_count());
          ASSERT (1 == ref2.use_count());
          ExplicitPlacement exPla = refX.resolve();
          ASSERT (exPla.time == Time(2));          // indeed get back the time we set on p2 above
          ASSERT (2 == ref2.use_count());          // exPla shares ownership with p2
          
          ASSERT (index->contains(ref1));          // ref can stand-in for a placement-ID 
          ASSERT (sizeof(id2) == sizeof(ref2));    // (and is actually implemented based on an ID)
          
          // assignment on placement refs
          refX = ref1;
          ASSERT (ref1 != ref2);
          ASSERT (ref1 == refX);
          ASSERT (ref2 != refX);
          
          // re-assignment with a new placement
          refX = p2;
          ASSERT (refX == ref2);
          ASSERT (isSameObject (*refX, p2));
          refX = p1.getID();
          ASSERT (refX == ref1);
          ASSERT (refX != ref2);
          ASSERT (isSameObject (*refX, p1));
          
          LumieraUid luid2 (p2.getID().get());
          refX = luid2;                            // assignment works even based on a plain LUID
          ref2 = ref1;                             
          ref1 = refX;                             // dynamic type check when downcasting  
          ASSERT (isSameObject (p1, *ref2));
          ASSERT (isSameObject (p2, *ref1));
          refX = ref2;
          ref2 = ref1;
          ref1 = refX;
          ASSERT (isSameObject (p1, *ref1));
          ASSERT (isSameObject (p1, *refX));
          ASSERT (isSameObject (p2, *ref2));
          ASSERT (ref1 != ref2);
          ASSERT (ref1 == refX);
          ASSERT (ref2 != refX);
          
          // resolution is indeed "live", we see changes to the referred placement
          ASSERT (refX.resolve().time == Time(0));
          p1 = p2;
          ASSERT (refX.resolve().time == Time(2)); // now we get the time tie we originally set on p2
          ASSERT (3 == ref2.use_count());          // p1, p2 and exPla share ownership
          
          // actually, the assignment has invalidated ref1, because of the changed ID
          ASSERT (p1.getID() == p2.getID());
          
          VERIFY_ERROR(INVALID_PLACEMENTREF, *ref1 );
          
          ASSERT (!index->contains(p1));           // index indeed detected the invalid ref
          ASSERT (3 == ref2.use_count());          // but ref2 is still valid
          
          // actively removing p2 invalidates the other refs to
          index->remove (ref2);
          ASSERT (!ref2);                          // checks invalidity without throwing
          ASSERT (!refX);
          VERIFY_ERROR(INVALID_PLACEMENTREF, *ref2 );
          
          // deliberately create an invalid PlacementRef
          PlacementRef<TestSubMO21> bottom;
          ASSERT (!bottom);
          VERIFY_ERROR(INVALID_PLACEMENTREF, *bottom );
          VERIFY_ERROR(INVALID_PLACEMENTREF, bottom->specialAPI() );
          VERIFY_ERROR(INVALID_PLACEMENTREF, bottom.resolve() );
          
          //consistency check; then reset PlacementRef index to default
          ASSERT (0 == index->size());
          ASSERT (1 == index.use_count());
          index.reset();
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (PlacementRef_test, "unit session");
  
  
}}} // namespace mobject::session::test
