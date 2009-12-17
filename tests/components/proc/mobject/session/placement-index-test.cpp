/*
  PlacementIndex(Test)  -  facility keeping track of Placements within the Session
 
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
//#include "proc/asset/media.hpp"
//#include "proc/mobject/session.hpp"
//#include "proc/mobject/session/edl.hpp"
#include "proc/mobject/session/placement-index.hpp"
#include "proc/mobject/session/scope.hpp"
#include "proc/mobject/placement.hpp"
#include "lib/util.hpp"
#include "proc/mobject/session/testclip.hpp"
#include "proc/mobject/session/testroot.hpp"

//#include <boost/format.hpp>
//#include <iostream>

//using boost::format;
//using lumiera::Time;
//using util::contains;
using util::isSameObject;
using std::string;
//using std::cout;


namespace mobject {
namespace session {
namespace test    {
      
  using asset::VIDEO;
  using session::test::TestClip;
  
  typedef PlacementIndex& Idx;
  
  
  /***************************************************************************
   * @test basic behaviour of the index mechanism used to keep track of
   *       individual Placements as added to the current Session..
   * @see  mobject::Placement
   * @see  mobject::MObject#create
   * @see  mobject::Placement#addPlacement
   * @see  mobject::Placement#resolve
   */
  class PlacementIndex_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          PlacementIndex index (make_dummyRoot());
          ASSERT (index.isValid());
          
          checkSimpleInsertRemove (index);
          has_size (0, index);
          
          checkSimpleAccess (index);
          has_size (1, index);
          
          checkScopeHandling (index);
          has_size (7, index);
          
          ////////////////////////////TODO
          
          index.clear();
          has_size (0, index);
        }
      
      void
      has_size(uint siz, Idx index)
        {
          ASSERT (siz == index.size());
        }
      
      void
      checkSimpleInsertRemove (Idx index)
        {
          PMO clip = TestClip::create();
          PMO& root = index.getRoot();
          
          ASSERT (0 == index.size());
          ASSERT (!index.contains (clip));
          
          index.insert (clip, root);
          ASSERT (1 == index.size());
          ASSERT ( index.contains (clip));
          
          index.remove(clip);
          ASSERT (0 == index.size());
          ASSERT (!index.contains (clip));
          ASSERT ( index.contains (root));
        }
      
      
      void
      checkSimpleAccess (Idx index)
        {
          PMO testObj = TestClip::create();
          PMO& root = index.getRoot();
          PMO::ID elmID = index.insert (testObj, root);
          
          PMO& elm = index.find(elmID);
          ASSERT (elmID == elm.getID());
          ASSERT (!isSameObject (elm,testObj));  // note: placements are registered as copy
//        ///////////////////////////////////////////////////////////////////////////////TICKET #119
//        ASSERT (elm == testObj);              //        they are semantically equivalent
          ASSERT (elmID != testObj.getID());   //         but have a distinct identity
          
          PMO::ID elmID2 = index.insert(testObj, root);
          ASSERT (elmID != elmID);          //            ...and each insert creates a new instance
//        ///////////////////////////////////////////////////////////////////////////////TICKET #119
//        ASSERT (testObj == index.find(elmID2));
          ASSERT (!isSameObject (elm, index.find(elmID2)));
          ASSERT ( isSameObject (elm, index.find(elmID )));
          
          // can also re-access objects by previous ref
          ASSERT ( isSameObject (elm, index.find(elm)));
        }
      
      
      void
      checkInvalidRef (Idx index)
        {
          RefPlacement invalid;
          PlacementMO::ID invalidID (invalid);
          ASSERT (!bool(invalidID));
          ASSERT (!bool(invalid));
          
          VERIFY_ERROR(BOTTOM_PLACEMENTREF, index.find(invalid) );
          VERIFY_ERROR(BOTTOM_PLACEMENTREF, index.find(invalidID) );
          VERIFY_ERROR(BOTTOM_PLACEMENTREF, index.getScope(invalidID) );
          
          ASSERT (!index.contains(invalidID));
          
          PMO testObj = TestClip::create();
          VERIFY_ERROR(INVALID_SCOPE, index.insert(testObj, invalidID) );
          
          ASSERT (false == index.remove(invalidID));
        }
      
      
      void
      checkScopeHandling (Idx index)
        {
          PMO testObj = TestClip::create();
          PMO& root = index.getRoot();
          
          typedef PMO::ID ID;
          ID e1 = index.insert (testObj, root);
          ID e11 = index.insert (testObj, e1);
          ID e12 = index.insert (testObj, e1);
          ID e13 = index.insert (testObj, e1);
          ID e131 = index.insert (testObj, e13);
          ID e132 = index.insert (testObj, e13);
          ID e133 = index.insert (testObj, e13);
          ID e1331 = index.insert (testObj, e133);
          
//        ///////////////////////////////////////////////////////////////////////////////TICKET #119
//        ASSERT (root == index.getScope(e1));
          ASSERT (e1   == index.getScope(e11).getID());
          ASSERT (e1   == index.getScope(e12).getID());
          ASSERT (e1   == index.getScope(e13).getID());
          ASSERT (e13  == index.getScope(e131).getID());
          ASSERT (e13  == index.getScope(e132).getID());
          ASSERT (e13  == index.getScope(e133).getID());
          ASSERT (e133 == index.getScope(e1331).getID());
          ASSERT (e1 != e13);
          ASSERT (e13 != e133);
          
//        ///////////////////////////////////////////////////////////////////////////////TICKET #119
//        ASSERT (index.getScope(e11) == index.getScope(index.find(e11)));
//        ASSERT (index.getScope(e131) == index.getScope(index.find(e131)));
          
          VERIFY_ERROR(NONEMPTY_SCOPE, index.remove(e13) ); // can't remove a scope-constituting element
          VERIFY_ERROR(NONEMPTY_SCOPE, index.remove(e133) );
          
          ASSERT (index.contains(e1331));
          ASSERT (index.remove(e1331));
          ASSERT (!index.contains(e1331));
          ASSERT (!index.remove(e1331));
          
          ASSERT (index.remove(e133));     // but can remove an scope, after emptying it 
          ASSERT (!index.contains(e133));
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (PlacementIndex_test, "unit session");
      
      
}}} // namespace mobject::session::test
