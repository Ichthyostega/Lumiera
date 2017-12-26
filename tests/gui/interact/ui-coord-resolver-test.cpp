/*
  UICoordResolver(Test)  -  resolve UI coordinates against actual topology

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file ui-coord-resolver-test.cpp
 ** unit test \ref UICoordResolver_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "gui/interact/ui-coord.hpp"
#include "gui/interact/ui-coord-resolver.hpp"
#include "gui/interact/gen-node-location-query.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/format-cout.hpp"/////////////////////////TODO RLY?
#include "lib/format-util.hpp"
//#include "lib/idi/entry-id.hpp"
//#include "lib/diff/gen-node.hpp"
#include "lib/util.hpp"

#include <string>


using std::string;
//using lib::idi::EntryID;
using lib::diff::MakeRec;
using lib::diff::Rec;
//using util::isSameObject;
using lib::Symbol;
using util::isnil;
using util::join;



namespace gui  {
namespace interact {
namespace test {
  
//  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
//  using lumiera::error::LUMIERA_ERROR_INDEX_BOUNDS;
  using lumiera::error::LUMIERA_ERROR_STATE;
//  using lumiera::error::LUMIERA_ERROR_LOGIC;
  
  namespace { //Test fixture...
    
  }//(End)Test fixture
  
  
  /******************************************************************************//**
   * @test verify query and mutation of UICoord in relation to actual UI topology.
   * 
   * @see UICoordResolver
   * @see navigator.hpp
   * @see ViewLocator
   * @see UICoord_test
   */
  class UICoordResolver_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verify_simpleUsage();
          verify_backingQuery();
          verify_queryAnchor();
          verify_queryCoverage();
          verify_mutateAnchor();
          verify_mutateCovered();
          verify_mutateExtend();
        }
      
      
      /** @test introduction to UI coordinate resolution
       *        - use a backing "real" (dummy) data structure to resolve against
       *        - establish a suitable implementation of the LocationQuery interface
       *        - attach a resolver
       *        - have fun
       */
      void
      verify_simpleUsage()
        {
          // a Test dummy placeholder for the real UI structure
          Rec dummyUiStructure = MakeRec()
                                   .set("window-1"
                                       , MakeRec()
                                           .type("perspective-A")
                                       )
                                   .set("window-2"
                                       , MakeRec()
                                           .type("perspective-B")
                                           .set("panelX", MakeRec())
                                           .set("panelXX", MakeRec())
                                       );

          // helper to answer "location queries" backed by this structure
          GenNodeLocationQuery locationQuery{dummyUiStructure};

          UICoord uic{"window-2","*","panelX","someView"};
          UICoordResolver resolver{uic, locationQuery};

          CHECK (not resolver.isCovered());
          CHECK (    resolver.canCover());

          UICoord uic2 = resolver.cover()
                                 .extend("otherView");

          CHECK ("UI:window-2[perspective-B]-panelX.otherView" == string(uic2));
        }
      
      
      /** @test verify the command-and-query interface backing the resolver.
       * This test actually uses a dummy implementation of the interface, which,
       * instead of navigating an actual UI topology, just uses a `Record<GenNode>`
       * (a "GenNode tree") to emulate the hierarchical structure of UI components.
       * @remarks note some twists how the GenNode tree is used here to represent
       *          an imaginary UI structure:
       *          - we use the special _type_ attribute to represent the _perspective_
       *            within each window; deliberately, we'll use this twisted structure
       *            here to highlight the fact that the backing structure need not be
       *            homogeneous; rather, it may require explicit branching
       *          - we use the _attributes_ within the GenNode "object" representation,
       *            since these are named nested elements, and the whole notion of an
       *            UI coordinate path is based on named child components
       *          - we use the _object builder_ helper to define the whole structure
       *            as nested inline tree; named nested elements ("attributes") are
       *            added with the `set(key, val)` builder function, and for each
       *            nested scope, we start a new nested builder with `MakeRec()`.
       *          - there is a special convention _for this test setup solely_ to
       *            set the `currentWindow` to be the last one in list -- in a real
       *            UI this would of course not be a configurable property of the
       *            LocationQuery, but rather just reflect the transient window
       *            state and return the currently activated window
       * @see IterTreeExplorer_test::verify_IterSource() regarding "child exploration"...
       */
      void
      verify_backingQuery()
        {
          GenNodeLocationQuery queryAPI{MakeRec()
                                          .set("window-1"
                                              , MakeRec()
                                                  .type("perspective-A")
                                                  .set("panelX"
                                                      , MakeRec()
                                                          .set("firstView", MakeRec())
                                                          .set("secondView", MakeRec())
                                                      )
                                              )
                                          .set("window-2"
                                              , MakeRec()
                                                  .type("perspective-B")
                                                  .set("panelY", MakeRec())
                                              )
                                          .set("window-3"
                                              , MakeRec()
                                                  .type("perspective-C")
                                                  .set("panelZ"
                                                      , MakeRec()
                                                          .set("thirdView", MakeRec())
                                                      )
                                                  .set("panelZZ", MakeRec())
                                              )
                                       };

          // the LocationQuery API works by matching a UICoord spec against the "real" structure
          UICoord uic1 = UICoord::window("window-2").persp("perspective-B");
          UICoord uic2 = UICoord::window("windows");
          UICoord uic3 = UICoord::firstWindow().persp("perspective-A").panel("panelX").view("secondView");
          UICoord uic4 = UICoord::currentWindow().persp("perspective-B");
          UICoord uic5 = UICoord::currentWindow().persp("perspective-C").panel("panelZ").view("someOtherView");

          CHECK ("window-2"     == queryAPI.determineAnchor(uic1));
          CHECK (Symbol::BOTTOM == queryAPI.determineAnchor(uic2));
          CHECK ("window-1"     == queryAPI.determineAnchor(uic3));
          CHECK ("window-3"     == queryAPI.determineAnchor(uic4));
          CHECK ("window-3"     == queryAPI.determineAnchor(uic5));

          CHECK (2 == queryAPI.determineCoverage(uic1));
          CHECK (0 == queryAPI.determineCoverage(uic2));
          CHECK (4 == queryAPI.determineCoverage(uic3));
          CHECK (1 == queryAPI.determineCoverage(uic4));
          CHECK (3 == queryAPI.determineCoverage(uic5));

          LocationQuery::ChildIter cii = queryAPI.getChildren(uic3, 3);
          CHECK (not isnil(cii));
          CHECK ("firstView" == *cii);
          ++cii;
          CHECK ("secondView" == *cii);
          CHECK (not isnil(cii));
          ++cii;
          CHECK (isnil(cii));

          CHECK ("window-1, window-2, window-3" == join (queryAPI.getChildren (uic3, 0)));
          CHECK ("perspective-A"                == join (queryAPI.getChildren (uic3, 1)));
          CHECK ("panelX"                       == join (queryAPI.getChildren (uic3, 2)));
          CHECK ("firstView, secondView"        == join (queryAPI.getChildren (uic3, 3)));
          CHECK (isnil (                                 queryAPI.getChildren (uic3, 4))); // "firstView" has no children

          CHECK ("window-1, window-2, window-3" == join (queryAPI.getChildren (uic2, 0)));
          VERIFY_ERROR (STATE,                           queryAPI.getChildren (uic2, 1) ); // "windows" at pos==0 is not covered by real UI

          CHECK ("window-1, window-2, window-3" == join (queryAPI.getChildren (uic5, 0)));
          CHECK ("perspective-C"                == join (queryAPI.getChildren (uic5, 1)));
          CHECK ("panelZ, panelZZ"              == join (queryAPI.getChildren (uic5, 2)));
          CHECK ("thirdView"                    == join (queryAPI.getChildren (uic5, 3)));
          VERIFY_ERROR (STATE,                           queryAPI.getChildren (uic5, 4) ); // "someOtherView" at level 4 does not exist
          
          
          // verify "child exploration" via iterator interface
          cii = queryAPI.getChildren (uic3, 0);                               // enter at root level...
          CHECK ("window-1" == *cii);                                         // first child of root to appear is "window-1"
          CHECK (0 == cii.depth());                                           // (note depth just happens to coincide with absolute tree depth here)
          cii.expandChildren();                                               // drill down into current element's children
          CHECK (1 == cii.depth());
          CHECK ("perspective-A" == *cii);                                    // which is just one, the perspective
          cii.expandChildren();                                               // drill down into the (formal, logical) children of "perspective-A"
          CHECK (2 == cii.depth());
          CHECK ("panelX" == *cii);                                           // ..and find the "panelX" at level 2
          cii.expandChildren();                                               // drill down one level further
          CHECK (3 == cii.depth());
          CHECK ("firstView" == *cii);                                        // and then just continue iteration, which first explores that scope...
          CHECK ("firstView, secondView, window-2, window-3" == join (cii));  // ...followed by returning to the enclosing scopes, finally top level.
        }
      
      
      /** @test query anchorage of given UI coordinates.
       *        - an anchored UI coordinate spec explicitly rooted within a top level window.
       */
      void
      verify_queryAnchor()
        {
          GenNodeLocationQuery loQu{MakeRec()
                                      .set("window-1"
                                          , MakeRec()
                                              .type("perspective-A")
                                          )
                                      .set("window-2"
                                          , MakeRec()
                                              .type("perspective-B")
                                              .set("panelX"
                                                  , MakeRec()
                                                      .set("someView", MakeRec())
                                                  )
                                          )
                                   };
          UICoord uic1 = UICoord::window("window-1").persp("perspective-A");
          UICoord uic2 = UICoord::window("windows");
          UICoord uic3 = UICoord::firstWindow();
          UICoord uic4 = UICoord::currentWindow().persp("perspective-B");
          UICoord uic5 = UICoord::currentWindow().panel("panelY");
          UICoord uic6 = UICoord().view("someView").path("α/β/γ");

          UICoordResolver r1{uic1, loQu};
          UICoordResolver r2{uic2, loQu};
          UICoordResolver r3{uic3, loQu};
          UICoordResolver r4{uic4, loQu};
          UICoordResolver r5{uic5, loQu};
          UICoordResolver r6{uic6, loQu};

          CHECK (    r1.isAnchored());
          CHECK (not r2.isAnchored());
          CHECK (    r3.isAnchored());
          CHECK (    r4.isAnchored());
          CHECK (not r5.isAnchored());
          CHECK (not r6.isAnchored());

          CHECK (    r1.canAnchor());
          CHECK (not r2.canAnchor());
          CHECK (    r3.canAnchor());
          CHECK (    r4.canAnchor());
          CHECK (not r5.canAnchor());
          CHECK (    r6.canAnchor());
        }
      
      
      void
      verify_queryCoverage()
        {
          UNIMPLEMENTED ("query coverage of given UI coordinates with respect to actual UI");
        }
      
      
      void
      verify_mutateAnchor()
        {
          UNIMPLEMENTED ("mutate given UI coordinates by anchoring them");
        }
      
      
      void
      verify_mutateCovered()
        {
          UNIMPLEMENTED ("mutate given UI coordinates by reducing to covered part");
        }
      
      
      void
      verify_mutateExtend()
        {
          UNIMPLEMENTED ("mutate given UI coordinates by uncovered extension");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (UICoordResolver_test, "unit gui");
  
  
}}} // namespace gui::interact::test
