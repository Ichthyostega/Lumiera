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
#include "gen-node-location-query.hpp"
#include "lib/diff/gen-node.hpp"
#include "lib/format-util.hpp"
#include "lib/util.hpp"

#include <string>


using std::string;
using lib::diff::MakeRec;
using lib::diff::Rec;
using lib::Symbol;
using util::isnil;
using util::join;



namespace gui  {
namespace interact {
namespace test {
  
  using lumiera::error::LERR_(INVALID);
  using lumiera::error::LERR_(STATE);
  
  
  
  /******************************************************************************//**
   * @test verify query and mutation of UICoord in relation to actual UI topology.
   *       A UI-Coordinate resolver is a special builder, which is initialised by
   *       the given coordinate spec, and also attached to a "location query API",
   *       which allows to investigate the current actual UI structure. The builder
   *       then exposes query and mutation operations, to determine to what extent
   *       the coordinate spec is "covered" by the real UI, and to match and expand
   *       any wildcards in the coordinate spec (pattern).
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
          verify_mutateCoverage();
          verify_mutateCoverPartially();
          verify_mutateAnchor();
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
       *        - an explicit UI coordinate spec impossible to anchor within current UI tree
       *        - a UI coordinate spec with dynamic reference to first/current window
       *        - an incomplete spec, which needs to be solved (pattern matched) to determine anchor.
       */
      void
      verify_queryAnchor()
        {
          GenNodeLocationQuery tree{MakeRec()
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
          UICoord uic6 = UICoord().view("someView");

          UICoordResolver r1{uic1, tree};
          UICoordResolver r2{uic2, tree};
          UICoordResolver r3{uic3, tree};
          UICoordResolver r4{uic4, tree};
          UICoordResolver r5{uic5, tree};
          UICoordResolver r6{uic6, tree};

          CHECK (    r1.isAnchored());
          CHECK (not r2.isAnchored());
          CHECK (    r3.isAnchored());
          CHECK (    r4.isAnchored());
          CHECK (    r5.isAnchored());
          CHECK (not r6.isAnchored());

          CHECK (    r1.canAnchor());
          CHECK (not r2.canAnchor());
          CHECK (    r3.canAnchor());
          CHECK (    r4.canAnchor());
          CHECK (    r5.canAnchor());
          CHECK (    r6.canAnchor());
        }
      
      
      
      /** @test path matching algorithm to resolve UI coordinates with wildcards against the current UI structure tree.
       * Since an UI coordinate path with gaps and wildcards could match anywhere, even several times, we need to perform
       * an exhaustive search with backtracking over the whole tree. By convention, we use the first maximal solution,
       * which can be just a partial solution, leaving an additional uncovered trailing part of the UI coordinate spec.
       * Whenever a coordinate spec is _not explicit,_ has wildcards or a leading gap, we need to perform the full
       * matching algorithm, even to just answer the question if coverage _is possible_. The result, i.e. the computed
       * coverage, is cached internally, and can be used to _mutate_ the UI coordinate spec to match that coverage.
       * 
       * This test verifies various corner cases; especially there is a rule to prevent a partial match based
       * on wildcards solely, rather we require at least one explicit match to qualify as partial solution.
       * - (1) trivial cases not requiring a tree search
       *   ** total coverage
       *   ** partial coverage, leaving an uncovered suffix
       * - (2) expand dynamic anchor specifiers
       *   ** with following content
       *   ** anchor spec alone
       * - (3) wildcard interpolation
       *   ** interpolate a single gap
       *   ** interpolate several gaps
       *   ** interpolate anchor and consecutive wildcards
       *   ** discriminate by anchor and fill additional gap
       * - (4) failure detection
       *   ** trailing wildcards are stripped and ignored
       *   ** reject gap beyond existing real UI tree
       *   ** reject gap ending at perimeter of real UI tree
       *   ** reject interpolated gap on immediately following mismatch
       *   ** reject mismatch immediately behind second gap
       *   ** mismatch of tree level
       *   ** contradiction to anchorage
       * - (5) selection between several possible solutions
       *   ** the length of the covered trailing suffix decides
       *   ** when two solutions are equivalent, pick the fist one
       *   ** best solution will be picked, irrespective of discovery order
       */
      void
      verify_mutateCoverage()
        {
          GenNodeLocationQuery tree{MakeRec()
                                      .set("window-1"
                                          , MakeRec()
                                              .type("persp-A")
                                              .set("panelX"
                                                  , MakeRec()
                                                      .set("firstView", MakeRec())
                                                      .set("secondView", MakeRec())
                                                  )
                                              .set("panelZ"
                                                  , MakeRec()
                                                      .set("thirdView"
                                                          , MakeRec()
                                                              .set("#1", MakeRec())
                                                              .set("#2", MakeRec())
                                                              .set("tab", MakeRec())
                                                          )
                                                  )
                                          )
                                      .set("window-2"
                                          , MakeRec()
                                              .type("persp-B")
                                              .set("panelY", MakeRec())
                                          )
                                      .set("window-3"
                                          , MakeRec()
                                              .type("persp-C")
                                              .set("panelZ"
                                                  , MakeRec()
                                                      .set("thirdView"
                                                          , MakeRec()
                                                              .set("tab"
                                                                  , MakeRec()
                                                                      .set("sub", MakeRec())
                                                                  )
                                                              .set("#1", MakeRec())
                                                          )
                                                  )
                                              .set("panelZZ", MakeRec())
                                          )
                                   };
          
          /* === trivial cases === */
          UICoordResolver r11 {UICoord::window("window-1")
                                       .persp("persp-A")
                                       .panel("panelX"), tree};
          CHECK (r11.isCovered());
          CHECK (3 == r11.coverDepth());
          
          
          UICoordResolver r12 {UICoord::window("window-1")
                                       .persp("persp-A")
                                       .panel("panelX")
                                       .view("thirdView"), tree};
          CHECK (not r12.isCovered());
          CHECK (    r12.isCoveredPartially());
          CHECK (3 ==r12.coverDepth());
          CHECK ("UI:window-1[persp-A]-panelX.thirdView" == string(r12));
          
          r12.cover();
          CHECK (r12.isCovered());
          CHECK (r12.isCoveredPartially());
          CHECK (3 ==r12.coverDepth());
          CHECK ("UI:window-1[persp-A]-panelX" == string(r12));
          
          
          /* === expand anchor === */
          UICoordResolver r21 {UICoord::firstWindow().persp("persp-A"), tree};
          CHECK ("UI:firstWindow[persp-A]" == string(r21));
          r21.cover();
          CHECK ("UI:window-1[persp-A]"    == string(r21));
          
          /* === expand anchor alone === */
          UICoordResolver r22 {UICoord::currentWindow(), tree};
          CHECK ("UI:window-3" == string(r22.cover()));
          
          
          /* === interpolate a single gap === */
          UICoordResolver r31 {UICoord::window("window-1").view("secondView"), tree};
          CHECK ("UI:window-1[*]-*.secondView" == string(r31));
          CHECK (0 ==r31.coverDepth());
          CHECK (not r31.isCovered());
          CHECK (r31.canCover());
          r31.cover();
          CHECK (r31.isCovered());
          CHECK (4 == r31.coverDepth());
          CHECK ("UI:window-1[persp-A]-panelX.secondView" == string(r31));
          
          /* === interpolate several gaps === */
          UICoordResolver r32 {UICoord().view("thirdView").path("sub"), tree};
          CHECK ("UI:window-3[persp-C]-panelZ.thirdView.tab/sub" == string(r32.cover()));
          
          /* === interpolate anchor and consecutive wildcards === */
          UICoordResolver r33 {UICoord::firstWindow().tab(2), tree};
          CHECK ("UI:window-1[persp-A]-panelZ.thirdView.#2" == string(r33.cover()));
          
          /* === discriminate by anchor and fill second gap === */
          UICoordResolver r34 {UICoord::currentWindow().panel("panelZ").tab("tab"), tree};
          CHECK ("UI:currentWindow[*]-panelZ.*.tab"          == string(r34));
          CHECK ("UI:window-3[persp-C]-panelZ.thirdView.tab" == string(r34.cover()));              // Note: rest of the path would also match on window-1, but currentWindow == window-3
          
          UICoordResolver r35 {UICoord::currentWindow().persp(UIC_ELIDED).panel("panelZ").tab("tab"), tree};
          CHECK ("UI:currentWindow[.]-panelZ.*.tab"          == string(r35));
          CHECK ("UI:window-3[persp-C]-panelZ.thirdView.tab" == string(r35.cover()));              // elided (existentially quantified) element interpolated similar to a wildcard
          
          UICoordResolver r36 {UICoord::currentWindow().panel(UIC_ELIDED).view("nonexisting"), tree};
          CHECK ("UI:currentWindow[*]-..nonexisting"         == string(r36));
          CHECK ("UI:window-3[persp-C]-panelZ"               == string(r36.cover()));              // ...but elided counts as existing element and matches arbitrarily (-> contrast this to r44)
          
          
          /* === trailing wildcards stripped automatically === */
          UICoordResolver r41 {UICoord::window("window-2").append("*/*"), tree};
          CHECK ("UI:window-2" == string(r41));                                                    // Note: trailing wildcards are already discarded by PathArray / UICoord
          
          r41.extend("*/*");                                                                       // if we now attempt to "sneak in" trailing wildcards...
          CHECK ("UI:window-2[*]-*" == string(r41));
          CHECK (not r41.canCover());                                                              // ...then the algorithm rejects any solution
          CHECK ("UI:window-2" == string(r41.cover()));                                            // Note: but cover() will act on the previous coverage and just strip the extraneous suffix
          
          /* === reject gap beyond existing real UI tree === */
          UICoordResolver r42 {UICoord::window("window-2").append("*/*/*/some/path"), tree};
          CHECK (not r42.canCover());
          
          /* === reject gap ending at real UI tree boundary === */
          UICoordResolver r43 {UICoord::currentWindow().view("firstView").tab("nonexisting"), tree};
          CHECK (not r43.canCover());
          
          /* === reject interpolated gap on mismatch right behind === */
          UICoordResolver r44 {UICoord().view("otherView"), tree};                                 // Note: will be checked on all four existing views, but never matches
          CHECK (not r44.canCover());
          
          /* === reject mismatch immediately behind second gap === */
          UICoordResolver r45 {UICoord().panel("panelZ").tab(3), tree};                            // Note: we have two "panelZ", but none has a tab #3
          CHECK (not r45.canCover());
          
          /* === mismatch of tree level === */
          UICoordResolver r46 {UICoord::currentWindow().append("*/*/panelZ/thirdView"), tree};     // Note: one '*' too much, thus 'panelZ' is matched on view level
          CHECK (not r46.canCover());
          
          /* === impossible to anchor === */
          UICoordResolver r47 {UICoord::firstWindow().tab(3), tree};
          CHECK (not r47.canCover());
          
          
          /* === the solution with maximum covered depth wins === */
          UICoordResolver r51 {UICoord().tab("tab").path("sub"), tree};
          CHECK ("UI:window-3[persp-C]-panelZ.thirdView.tab/sub" == string(r51.cover()));          // the second solution found covers to maximum depth
          
          /* === when two solutions are equivalent, pick the fist one === */
          UICoordResolver r52 {UICoord().tab("tab"), tree};
          CHECK ("UI:window-1[persp-A]-panelZ.thirdView.tab" == string(r52.cover()));              // "UI:window-3[persp-C]-panelZ.thirdView.tab" would match too
          
          /* === best solution will be picked, irrespective of discovery order === */
          UICoordResolver r531 {UICoord().persp("persp-A").tab(1), tree};
          CHECK ("UI:window-1[persp-A]-panelZ.thirdView.#1" == string(r531.cover()));              // best solution discovered as first one
          
          UICoordResolver r532 {UICoord().view("thirdView").tab("tab"), tree};
          CHECK ("UI:window-1[persp-A]-panelZ.thirdView.tab" == string(r532.cover()));             // best solution is 3rd of five possible ones
          
          UICoordResolver r533 {UICoord().persp("persp-C").tab(1), tree};
          CHECK ("UI:window-3[persp-C]-panelZ.thirdView.#1" == string(r533.cover()));              // best solution is found as last one
        }
      
      
      
      /** @test resolve by matching, but retain an extraneous, uncovered extension.
       * This is a variation of the UICoordResolver::cover() operation, which likewise resolves
       * any wildcards; but here we tolerate _additional elements below_ the covered part, as long
       * as those are explicit. The typical use case is when we're about to create a new UI element
       * at a specific existing anchor location within the UI. The extraneous uncovered part then
       * describes those extra element yet to be created.
       */
      void
      verify_mutateCoverPartially()
        {
          GenNodeLocationQuery tree{MakeRec()
                                      .set("window-2"
                                          , MakeRec()
                                              .type("persp-B")
                                              .set("panelY"
                                                  , MakeRec()
                                                      .set("someView"
                                                          , MakeRec()
                                                              .set("#1", MakeRec())
                                                              .set("#2", MakeRec())
                                                          )
                                                  )
                                          )
                                   };
          
          /* === explicitly given spec partially covered === */
          UICoordResolver r1 {UICoord{"window-2","persp-B","panelY","otherView","tab"}, tree};
          CHECK (3 == r1.coverDepth());
          r1.coverPartially();
          CHECK (not r1.isCovered());
          CHECK (3 == r1.coverDepth());
          CHECK (r1.isCoveredPartially());                       // is covered down to the "panelY"
          CHECK ("UI:window-2[persp-B]-panelY.otherView.tab"  == string(r1));
          r1.cover();
          CHECK (r1.isCovered());                             // cover() retains the covered part only
          CHECK ("UI:window-2[persp-B]-panelY"  == string(r1));
          
          /* === fill wildcard gap but retain uncovered extension === */
          UICoordResolver r2 {UICoord::currentWindow().view("someView").tab(3).path("sub"), tree};
          CHECK (0 == r2.coverDepth());
          r2.coverPartially();
          CHECK (not r2.isCovered());
          CHECK (4 == r2.coverDepth());
          CHECK (r2.isCoveredPartially());
          CHECK ("UI:window-2[persp-B]-panelY.someView.#3/sub"  == string(r2));
          r2.cover();
          CHECK ("UI:window-2[persp-B]-panelY.someView"  == string(r2));
          
          /* === reject when gap can not be closed unambiguously === */
          UICoordResolver r3 {UICoord::currentWindow().view("someView").path("sub"), tree};
          CHECK (not r3.canCover());                                 // NOTE: second gap here, tab info missing
          r3.coverPartially();
          CHECK (isnil (r3));
          
          /* === reject when some wildcards remain after partial coverage === */
          UICoordResolver r4 {UICoord::currentWindow().tab(3).path("sub"), tree};
          r4.coverPartially();
          CHECK (isnil (r4));
          
          /* === existentially quantified (elided) element constitutes partial coverage === */
          UICoordResolver r5 {UICoord::currentWindow().persp(UIC_ELIDED).panel("fantasy").view("fantomas"), tree};
          CHECK ("UI:currentWindow[.]-fantasy.fantomas"  == string(r5));
          CHECK (1 == r5.coverDepth());
          r5.coverPartially();
          CHECK (not r5.isCovered());
          CHECK (2 == r5.coverDepth());     // Note side-effect of computing the coverage...
          CHECK (r5.isCoveredPartially());  // it is known to be covered including "the" perspective
          CHECK ("UI:window-2[persp-B]-fantasy.fantomas"  == string(r5));
          r5.cover();
          CHECK ("UI:window-2[persp-B]"  == string(r5));
          CHECK (2 == r5.coverDepth());
        }
      
      
      
      
      /** @test mutate given UI coordinates by anchoring them.
       * This operation changes only the window part of the coordinate spec;
       * it might use the result of a preceding coverage solution search or even
       * trigger such a search, but only to find out about the root window.
       * @note some fine points touched here: to anchor a path is something
       *       different than to cover it; in fact there are cases where we
       *       can determine the possible anchor point, but are unable to
       *       cover the path spec beyond that. And, on the other hand,
       *       there are cases where you _need to compute a coverage_
       *       in order to decide upon the anchor point.
       */
      void
      verify_mutateAnchor()
        {
          GenNodeLocationQuery tree{MakeRec()
                                      .set("window-1"
                                          , MakeRec()
                                              .type("persp-A")
                                              .set("panelX"
                                                  , MakeRec()
                                                      .set("firstView", MakeRec())
                                                      .set("secondView", MakeRec())
                                                  )
                                          )
                                      .set("window-2"
                                          , MakeRec()
                                              .type("persp-B")
                                              .set("panelY"
                                                  , MakeRec()
                                                      .set("thirdView"
                                                          , MakeRec()
                                                              .set("#1", MakeRec())
                                                              .set("#2", MakeRec())
                                                          )
                                                  )
                                          )
                                      .set("window-3"
                                          , MakeRec()
                                              .type("persp-C")
                                              .set("panelZ"
                                                  , MakeRec()
                                                      .set("thirdView", MakeRec())
                                                  )
                                          )
                                   };
          
          /* === explicitly given window spec remains unchanged === */
          UICoordResolver r1 {UICoord{"window-2","persp-B","panelY"}, tree};
          CHECK (3 == r1.coverDepth());
          r1.anchor();
          CHECK ("UI:window-2[persp-B]-panelY"  == string(r1));
          
          /* === `firstWindow` meta spec is resolved === */
          UICoordResolver r2 {UICoord::firstWindow().view("blah"), tree};
          CHECK (0 == r2.coverDepth());
          CHECK (r2.isAnchored());                                                                 // can obviously be anchored, since there is always a first window
          CHECK (not r2.canCover());                                                               // yet this path is impossible to cover in the current UI
          CHECK ("UI:firstWindow[*]-*.blah"  == string(r2));
          r2.anchor();
          CHECK ("UI:window-1[*]-*.blah"  == string(r2));
          CHECK (0 == r2.coverDepth());
          CHECK (not r2.canCover());
          
          /* === `currentWindow` meta spec is resolved === */
          UICoordResolver r3 {UICoord::currentWindow().view("thirdView"), tree};
          CHECK (0 == r3.coverDepth());
          CHECK (r3.isAnchored());
          CHECK (not r3.isCovered());
          CHECK (r3.canCover());
          r3.anchor();
          CHECK (not r3.isCovered());
          CHECK (r3.isCoveredPartially());
          CHECK (1 == r3.coverDepth());                                                            // anchoring also picks the second of two possible solutions
          CHECK ("UI:window-3[*]-*.thirdView" == string(r3));                                      // thereby covering the "thirdView"
          
          /* === coverage solution is calculated on demand === */
          UICoordResolver r4 {UICoord().view("thirdView").append("#2/sub"), tree};
          CHECK ("UI:?.thirdView.#2/sub" == string(r4));                                           // an incomplete path is not automatically resolved
          CHECK (not r4.isAnchored());
          CHECK (0 == r4.coverDepth());
          r4.anchor();                                                                             // but if we anchor, we force search for a coverage solution
          CHECK (1 == r4.coverDepth());                                                            // which is actually found starting from the second window,
          CHECK (r4.isCoveredPartially());                                                         // and kept in the internal cache for future use,
          CHECK ("UI:window-2[*]-*.thirdView.#2/sub" == string(r4));                               // but not made explicit, since we only requested anchorage
          
          /* === already calculated coverage solution is used === */
          UICoordResolver r5 {UICoord::currentWindow().view("thirdView"), tree};
          CHECK (not r5.isCovered());
          CHECK (not r5.isCoveredPartially());
          CHECK (0 == r5.coverDepth());
          CHECK (r5.canCover());                                                                   // this triggers search for a coverage solution
          CHECK (1 == r5.coverDepth());
          CHECK (not r5.isCovered());
          CHECK (r5.isCoveredPartially());
          CHECK ("UI:currentWindow[*]-*.thirdView" == string(r5));
          r5.anchor();                                                                             // and this (cached) solution is also used to make anchorage explicit
          CHECK ("UI:window-3[*]-*.thirdView"      == string(r5));
          CHECK (1 == r5.coverDepth());
          CHECK (not r5.isCovered());
          r5.cover();                                                                              // ...now also the coverage solution was made explicit
          CHECK (r5.isCovered());
          CHECK (4 == r5.coverDepth());
          CHECK ("UI:window-3[persp-C]-panelZ.thirdView" == string(r5));
          
          /* === impossible to cover and can not be anchored === */
          UICoordResolver r6 {UICoord::window("windows").path("to/hell"), tree};
          CHECK (not r6.isAnchored());
          CHECK (not r6.canCover());
          r6.anchor();
          CHECK (not r6.isAnchored());
          CHECK (0 == r6.coverDepth());
          CHECK ("UI:windows[*]-*.*.*/to/hell" == string(r6));
        }
      
      
      /** @test mutate given UI coordinates by uncovered extension.
       * Contrary to just appending something to the path (which is a basic path operation
       * available on the generic path builder), a _path extension_ is always rooted at the
       * end of the actually covered part of the UI coordinates. So extending a path implies
       * search for a coverage solution, followed by truncating the path to the covered part.
       * There are two flavours of extending a path:
       * - extending with a literal specification, which is just appended behind the coverage
       * - extending with an incomplete UI coordinate spec, which allows to place the extension
       *   at a specific depth (e.g. as a view). This is typically what we want in practice.
       */
      void
      verify_mutateExtend()
        {
          GenNodeLocationQuery tree{MakeRec()
                                      .set("window-2"
                                          , MakeRec()
                                              .type("persp-B")
                                              .set("panelY"
                                                  , MakeRec()
                                                      .set("thirdView"
                                                          , MakeRec()
                                                              .set("#1", MakeRec())
                                                              .set("#2", MakeRec())
                                                          )
                                                  )
                                          )
                                   };
          
          /* === extend fully covered explicit path === */
          UICoordResolver r1 {UICoord{"window-2","persp-B","panelY"}, tree};
          CHECK ("UI:window-2[persp-B]-panelY" == string(r1));
          CHECK (r1.isCovered());
          r1.extend (UICoord().path("gappy").tab(2));                                              // can extend with partially defined UI coordinates
          CHECK ("UI:window-2[persp-B]-panelY.*.#2/gappy" == string(r1));                          // ...the resulting UI path is unresolved, yet can be partially covered
          r1.extend ("seamless");                                                                  // ...and this partial coverage is used as base for further extension
          CHECK ("UI:window-2[persp-B]-panelY.thirdView.#2/seamless" ==string(r1));
          
          /* === extend partially covered path === */
          UICoordResolver r2 {UICoord().view("thirdView").append("some/where"), tree};
          CHECK ("UI:?.thirdView.some/where" ==string(r2));                                        // "thirdView" is covered, "some/where" is not
          r2.extend ("no/where");
          CHECK ("UI:window-2[persp-B]-panelY.thirdView.no/where" ==string(r2));                   // ...and thus the extension is attached behind "thirdView"
          CHECK (r2.isCoveredPartially());
          
          /* === impossible extensions rejected === */                                             // since r2 already specifies a perspective ("persp-B")....
          VERIFY_ERROR (INVALID, r2.extend(UICoord().persp("fisheye")));                           // ...overwriting with another perspective is rejected as extension
          CHECK ("UI:window-2[persp-B]-panelY.thirdView.no/where" ==string(r2));                   // ...and the existing state is unaffected from this error
          VERIFY_ERROR (INVALID, r2.extend(UICoord().view("alternative")));                        // Likewise, extending with a conflicting view spec is rejected
          r2.extend(UICoord().tab("nada"));                                                        // But a tab is not yet covered and thus acceptable as extension
          CHECK ("UI:window-2[persp-B]-panelY.thirdView.nada"     ==string(r2));
          r2.extend(UICoord());
          CHECK ("UI:window-2[persp-B]-panelY.thirdView"          ==string(r2));                   // empty coordinates implicitly attached behind the covered part
          
          /* === unsolvable: truncate, extend, recalculate coverage === */
          UICoordResolver r3 {UICoord().persp("awesome"), tree};
          CHECK (not r3.canCover());
          CHECK (0 == r3.coverDepth());
          r3.extend (UICoord::currentWindow().tab(1));                                             // Extension implies covering, which effectively truncates the path
          CHECK (1 == r3.coverDepth());                                                            // ...and "currentWindow" can even be covered, thus the coverage increases
          CHECK ("UI:currentWindow[*]-*.*.#1" ==string(r3));                                       // note coverage calculated internally, not made explicit
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (UICoordResolver_test, "unit gui");
  
  
}}} // namespace gui::interact::test
