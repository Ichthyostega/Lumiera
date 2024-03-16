/*
  PlacementIndex(Test)  -  facility keeping track of Placements within the Session

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

/** @file placement-index-test.cpp
 ** unit test \ref PlacementIndex_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "steam/mobject/session/placement-index.hpp"
#include "steam/mobject/session/scope.hpp"
#include "steam/mobject/placement.hpp"
#include "steam/asset/media.hpp"
#include "lib/format-string.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include "steam/mobject/session/testclip.hpp"
#include "steam/mobject/session/testroot.hpp"


using util::_Fmt;
using util::isSameObject;
using std::string;


namespace steam   {
namespace mobject {
namespace session {
namespace test    {
  
  using session::test::TestClip;
  using LERR_(BOTTOM_PLACEMENTREF);
  using LERR_(NONEMPTY_SCOPE);
  using LERR_(INVALID_SCOPE);
  
  typedef PlacementIndex& Idx;
  
  
  /***********************************************************************//**
   * @test basic behaviour of the index mechanism used to keep track
   *       of individual Placements as added to the current Session.
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
          CHECK (index.isValid());
          
          checkSimpleInsertRemove (index);
          has_size (0, index);
          
          PMO::ID elmID = checkSimpleAccess (index);
          CHECK (index.isValid());
          has_size (2, index);
          
          checkTypedAccess (index, elmID);
          has_size (3, index);
          
          checkScopeHandling (index);
          has_size (9, index);
          
          checkContentsEnumeration (index);
          
          has_size (9, index);
          CHECK (index.isValid());
          
          index.clear();
          has_size (0, index);
          CHECK (index.isValid());
        }
      
      void
      has_size(uint siz, Idx index)
        {
          CHECK (siz == index.size());
        }
      
      
      void
      checkSimpleInsertRemove (Idx index)
        {
          PMO clip = TestClip::create();
          PMO& root = index.getRoot();
          
          CHECK (0 == index.size());
          
          PMO::ID elmID = index.insert (clip, root);
          CHECK (1 == index.size());
          CHECK ( index.contains (elmID));
          CHECK (!index.contains (clip));  // index stores copies
          
          index.remove(clip); // has no effect
          CHECK (1 == index.size());
          index.remove(elmID);
          CHECK (0 == index.size());
          CHECK (!index.contains (elmID));
          CHECK ( index.contains (root));
        }
      
      
      PMO::ID
      checkSimpleAccess (Idx index)
        {
          PMO testObj = TestClip::create();
          PMO& root = index.getRoot();
          PMO::ID elmID = index.insert (testObj, root);
          
          PMO& elm = index.find(elmID);
          CHECK (elmID == elm.getID());
          CHECK (!isSameObject (elm,testObj));   // note: placements are registered as copy
          CHECK (isSameDef(elm,testObj));       //        they are semantically equivalent    ////////TICKET #511
          CHECK (elmID != testObj.getID());    //         but have a distinct identity
          
          PMO::ID elmID2 = index.insert(testObj, root);
          CHECK (elmID2 != elmID);          //            ...and each insert creates a new instance
          CHECK (testObj != index.find (elmID));
          CHECK (testObj != index.find (elmID2));
          CHECK (isSameDef(testObj, index.find(elmID)));
          CHECK (isSameDef(testObj, index.find(elmID2)));
          CHECK (!isSameObject (testObj, index.find(elmID2)));
          CHECK (!isSameObject (elm, index.find(elmID2)));
          
          // can repeatedly retrieve a reference to the same object
          CHECK ( isSameObject (elm, index.find(elmID )));
          CHECK ( isSameObject (elm, index.find(elmID )));
          
          // can also re-access objects by previous ref
          CHECK ( isSameObject (elm, index.find(elm)));
          return elmID;
        }
      
      
      
      void
      checkTypedAccess (Idx index, PMO::ID elmID)
        {
          PMO& elm = index.find(elmID);
          CHECK (elmID == elm.getID());
          
          typedef Placement<Clip> PClip;
          PClip anotherTestClip = TestClip::create();
          
          typedef PlacementMO::Id<Clip> IDClip;
          IDClip clipID = index.insert(anotherTestClip, elmID);
          // retaining the more specific type info
          
          // access as MObject...
          PMO::ID mID = clipID;
          PMO& asMO = index.find(mID);
          
          // access as Clip
          PClip& asClip = index.find(clipID);
          CHECK (LENGTH_TestClip == asClip->getMedia()->getLength());  // using the Clip API
          
          CHECK ( isSameObject(asMO,asClip));
          CHECK (!isSameObject(asClip, anotherTestClip)); // always inserting a copy into the PlacementIndex
        }
      
      
      void
      checkInvalidRef (Idx index)
        {
          RefPlacement invalid;
          PlacementMO::ID invalidID (invalid);
          CHECK (!bool(invalidID));
          CHECK (!bool(invalid));
          
          VERIFY_ERROR(BOTTOM_PLACEMENTREF, index.find(invalid) );
          VERIFY_ERROR(BOTTOM_PLACEMENTREF, index.find(invalidID) );
          VERIFY_ERROR(BOTTOM_PLACEMENTREF, index.getScope(invalidID) );
          
          CHECK (!index.contains(invalidID));
          
          PMO testObj = TestClip::create();
          VERIFY_ERROR(INVALID_SCOPE, index.insert(testObj, invalidID) );
          
          CHECK (false == index.remove(invalidID));
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
          
          CHECK (index.isValid());
          CHECK (root == index.getScope(e1));
          CHECK (e1   == index.getScope(e11).getID());
          CHECK (e1   == index.getScope(e12).getID());
          CHECK (e1   == index.getScope(e13).getID());
          CHECK (e13  == index.getScope(e131).getID());
          CHECK (e13  == index.getScope(e132).getID());
          CHECK (e13  == index.getScope(e133).getID());
          CHECK (e133 == index.getScope(e1331).getID());
          CHECK (e1 != e13);
          CHECK (e13 != e133);
          
          CHECK (index.getScope(e11) == index.getScope(index.find(e11)));
          CHECK (index.getScope(e131) == index.getScope(index.find(e131)));
          
          VERIFY_ERROR(NONEMPTY_SCOPE, index.remove(e13) ); // can't remove a scope-constituting element
          VERIFY_ERROR(NONEMPTY_SCOPE, index.remove(e133) );
          
          CHECK (index.contains(e1331));
          CHECK (index.remove(e1331));
          CHECK (!index.contains(e1331));
          CHECK (!index.remove(e1331));
          
          CHECK (index.remove(e133));      // but can remove an scope, after emptying it
          CHECK (!index.contains(e133));
          CHECK (index.isValid());
          
          // build a complete new subtree
          uint siz   = index.size();
          ID e1321   = index.insert (testObj, e132);
          ID e13211  = index.insert (testObj, e1321);
          ID e13212  = index.insert (testObj, e1321);
          ID e13213  = index.insert (testObj, e1321);
          ID e13214  = index.insert (testObj, e1321);
          ID e132131 = index.insert (testObj, e13213);
          ID e132132 = index.insert (testObj, e13213);
          ID e132133 = index.insert (testObj, e13213);
          ID e132134 = index.insert (testObj, e13213);
          ID e132141 = index.insert (testObj, e13214);
          ID e132142 = index.insert (testObj, e13214);
          ID e132143 = index.insert (testObj, e13214);
          ID e132144 = index.insert (testObj, e13214);
          
          // ...and kill it recursively in one sway
          index.clear (e1321);
          CHECK (!index.contains (e1321));
          CHECK (!index.contains (e13211));
          CHECK (!index.contains (e13212));
          CHECK (!index.contains (e13213));
          CHECK (!index.contains (e132131));
          CHECK (!index.contains (e132132));
          CHECK (!index.contains (e132133));
          CHECK (!index.contains (e132134));
          CHECK (!index.contains (e132141));
          CHECK (!index.contains (e132142));
          CHECK (!index.contains (e132143));
          CHECK (!index.contains (e132144));
          CHECK (siz == index.size());
          CHECK (index.isValid());
        }
      
      
      
      typedef PlacementIndex::iterator Iter;
      
      /** @test drill down into the tree-like structure
       *        and enumerate the contents of each element, if any
       *  @note at this point, our test index holds 9 Placements,
       *        which actually refer to 3 distinct TestClip objects
       *        - two where installed into root scope in `checkSimpleAccess()`
       *        - one was installed below one of the above in `checkTypedAccess()`
       *        - `checkScopeHandling()` left back 6 instances, all pointing
       *          to the same TestClip.
       *        This can be verified in the test output (look at the
       *        `use-cnt` and the actual address of the pointee
       */
      void
      checkContentsEnumeration (Idx index)
        {
          PMO& root = index.getRoot();
          
          Iter rootContents = index.getReferrers (root.getID());
          CHECK (rootContents);
          
          discover (index, rootContents, 0);
        }
      
      
      void
      discover (Idx index, Iter iter, uint level)
        {
          uint count (0);
          for ( ; iter; ++iter )
            {
              cout << indent(level) << "::" << *iter << endl;
              
              ++count;
              Iter scopeContents = index.getReferrers (iter->getID());
              if (scopeContents)
                discover (index, scopeContents, level+1);
            }
          
          static _Fmt summary{"...%i elements at Level %i"};
          cout << indent(level) << summary % count % level << endl;
          
          CHECK (!iter);
          CHECK (0 < count);
        }
      
      static string
      indent (uint level)
        {
          return string (level, ' ');
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (PlacementIndex_test, "unit session");
  
  
}}}} // namespace steam::mobject::session::test
