/*
  PlacementHierarchy(Test)  -  cooperating hierarchical placement types

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

/** @file placement-hierarchy-test.cpp
 ** unit test \ref PlacementHierarchy_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "lib/depend-inject.hpp"
#include "vault/media-access-mock.hpp"
#include "steam/mobject/test-dummy-mobject.hpp"
#include "steam/mobject/session/clip.hpp"
#include "steam/mobject/placement.hpp"
#include "steam/asset/media.hpp"
#include "lib/format-cout.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"



namespace steam    {
namespace mobject {
namespace session {
namespace test    {
  
  using session::Clip;
  using lib::test::showSizeof;
  using namespace mobject::test;
  using lumiera::error::LUMIERA_ERROR_ASSERTION;
  
  using MediaAccessMock = lib::DependInject<vault::MediaAccessFacade>
                                ::Local<vault::test::MediaAccessMock>;
  
  
  /***********************************************************************************//**
   * @test creating placements specifically typed, forming an hierarchy of placement types
   *       which loosely mirrors the hierarchy of the pointee types. Assignments between
   *       placement of differing type, while still sharing ownership.
   * @see  mobject::Placement
   * @see  mobject::MObject#create
   * @see  lib::hash::HashIndexed
   */
  class PlacementHierarchy_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          MediaAccessMock useMockMedia;
          
          
          typedef Placement<MObject>                    PMObj;
          typedef TestPlacement<>                       PDummy;
          typedef TestPlacement<TestSubMO1>             PSub1;
          typedef TestPlacement<TestSubMO2>             PSub2;
          typedef TestPlacement<TestSubMO21,TestSubMO2> PSub21;
          
          PSub1  pSub1(*new TestSubMO1);
          PSub2  pSub2(*new TestSubMO2);
          PSub21 pSub3(*new TestSubMO21);
          
          PDummy pSubM (pSub3);
          
          PMObj  pClip = asset::Media::create("test-1", asset::VIDEO)->createClip();
          
          CHECK (INSTANCEOF (Placement<MObject>, &pSub1));
          CHECK (INSTANCEOF (Placement<MObject>, &pSub2));
          CHECK (INSTANCEOF (Placement<MObject>, &pSub3));
          CHECK (INSTANCEOF (Placement<MObject>, &pSubM));
          
          CHECK (INSTANCEOF (Placement<DummyMO>, &pSub1));
          CHECK (INSTANCEOF (Placement<DummyMO>, &pSub2));
          CHECK (INSTANCEOF (Placement<DummyMO>, &pSub3));
          
          CHECK (INSTANCEOF (TestPlacement<DummyMO>, &pSub1));
          CHECK (INSTANCEOF (TestPlacement<DummyMO>, &pSub2));
          CHECK (INSTANCEOF (TestPlacement<DummyMO>, &pSub3));
          
          CHECK ( INSTANCEOF (TestPlacement<TestSubMO2>,  &pSub3));
          
          // the following won't succeed...
//        CHECK (INSTANCEOF (TestPlacement<TestSubMO21>, &pSub2)); // parent not instance of subclass
//        CHECK (INSTANCEOF (TestPlacement<TestSubMO2>,  &pSub1)); // separate branch in the hierarchy
          
          cout << showSizeof(pSub1) << endl;
          cout << showSizeof(pSub2) << endl;
          cout << showSizeof(pSub3) << endl;
          cout << showSizeof(pSubM) << endl;
          cout << showSizeof(pClip) << endl;
          
          CHECK (sizeof(pSub1) == sizeof(pSub3));
          CHECK (sizeof(pClip) == sizeof(pSub3));
          
          cout << pSub1 << endl;
          cout << pSub2 << endl;
          cout << pSub3 << endl;
          cout << pSubM << endl;
          cout << pClip << endl;
          
          pSub3->specialAPI();
          
          CHECK (2 == pSubM.use_count());
          CHECK (2 == pSub3.use_count());
          pClip = pSubM;                      // slicing and shared ownership
          CHECK (3 == pSubM.use_count());
          CHECK (3 == pSub3.use_count());
          CHECK (3 == pClip.use_count());
          
          
          // now do a brute-force re-interpretation
          // note this is still protected by an ASSERT on the operator->()
          PSub21& hijacked = reinterpret_cast<PSub21&> (pClip);
          
          hijacked->specialAPI();
          CHECK (3 == hijacked.use_count());
          CHECK (hijacked.getID() == pClip.getID());
          
          cout << format_PlacementID(pSub1) << endl;
          cout << format_PlacementID(pSub2) << endl;
          cout << format_PlacementID(pSub3) << endl;
          cout << format_PlacementID(pSubM) << endl;
          cout << format_PlacementID(pClip) << endl;
          
          pClip = pSub1;
          CHECK (2 == pSubM.use_count());
          CHECK (2 == pSub3.use_count());
          
          CHECK (2 == pClip.use_count());
          CHECK (2 == pSub1.use_count());
          
#if false
///////////////////////////////////////////////////////////////////////////////TODO: find a way to configure NoBug to throw in case of assertion
///////////////////////////////////////////////////////////////////////////////TODO: configure NoBug specifically for the testsuite
          VERIFY_ERROR (ASSERTION, hijacked->specialAPI() );
#endif
          
          // runtime type diagnostics based on pointee RTTI
          CHECK ( pSub2.isCompatible<MObject>());
          CHECK ( pSub2.isCompatible<DummyMO>());
          CHECK ( pSub2.isCompatible<TestSubMO2>());
          CHECK (!pSub2.isCompatible<TestSubMO21>());
          CHECK (!pSub2.isCompatible<TestSubMO1>());
          CHECK (!pSub2.isCompatible<Clip>());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (PlacementHierarchy_test, "unit session");
  
  
}}}} // namespace steam::mobject::session::test
