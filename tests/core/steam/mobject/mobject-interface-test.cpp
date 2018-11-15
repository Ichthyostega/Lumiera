/*
  MObjectInterface(Test)  -  covers behaviour common to all MObjects

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

/** @file mobject-interface-test.cpp
 ** unit test \ref MObjectInterface_test
 */


#include "lib/test/run.hpp"
#include "lib/symbol.hpp"

#include "steam/asset/media.hpp"
#include "steam/mobject/mobject.hpp"
#include "steam/mobject/session/mobjectfactory.hpp"
//#include "steam/mobject/mobject-ref.hpp"
#include "steam/mobject/placement.hpp"
//#include "steam/mobject/placement-ref.hpp"
//#include "steam/mobject/session/placement-index.hpp"
//#include "steam/mobject/session/session-service-mock-index.hpp"
//#include "steam/mobject/session/clip.hpp"
//#include "steam/mobject/explicitplacement.hpp"
#include "steam/mobject/test-dummy-mobject.hpp"
#include "vault/media-access-mock.hpp"
#include "lib/depend-inject.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-cout.hpp"





namespace steam    {
namespace mobject {
namespace test    {
  
//  using lib::test::showSizeof;
  
  using lib::Symbol;
  using lib::time::Duration;
  using lib::time::FSecs;
  using lib::time::Time;
  
  
//  using session::Clip;
//  using session::PMedia;
  
  using MediaAccessMock = lib::DependInject<vault::MediaAccessFacade>
                                ::Local<vault::test::MediaAccessMock>;
  
  
  using namespace mobject::test;
  typedef TestPlacement<DummyMO> PDummy;
  
  
  /*****************************************************************************//**
   * @test cover the common behaviour of all MObjects. 
   * @note the MObject interface is still very preliminary (as of 10/10).
   *       It is expected to support some kind of metadata and object serialisation
   * 
   * @see  mobject::MObject
   * @see  mobject::Placement
   */
  class MObjectInterface_test : public Test
    {
      
      
      
      virtual void
      run (Arg) 
        {
          MediaAccessMock useMockMedia;
          
          
          PMO  testClip1 = asset::Media::create("test-1", asset::VIDEO)->createClip();
          PMO  testClip2 = asset::Media::create("test-2", asset::VIDEO)->createClip();
          
          // set up a tie to fixed start positions (i.e. "properties of placement")
          testClip1.chain(Time(FSecs(10)));
          testClip2.chain(Time(FSecs(20)));
          
          Symbol labelType ("dummyLabel");
          PMO testLabel1 = MObject::create (labelType);
          
          testLabel1.chain(Time(FSecs(30)));
          
          PDummy testDummy1(*new DummyMO);
          PDummy testDummy2(*new TestSubMO1);
          
          CHECK (testClip1->isValid());
          CHECK (testClip2->isValid());
          CHECK (testLabel1->isValid());
          CHECK (testDummy1->isValid());
          CHECK (testDummy2->isValid());
          
          Duration lenC1 = testClip1->getLength();
          Duration lenC2 = testClip2->getLength();
          Duration lenL1 = testLabel1->getLength();
          CHECK (lenC1 > Time::ZERO);
          CHECK (lenC2 > Time::ZERO);
          CHECK (lenL1 ==Time::ZERO);
          
          cout << testClip1->shortID() << endl;
          cout << testClip2->shortID() << endl;
          cout << testLabel1->shortID() << endl;
          cout << testDummy1->shortID() << endl;
          cout << testDummy2->shortID() << endl;
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (MObjectInterface_test, "unit session");
  
  
}}} // namespace steam::mobject::test
