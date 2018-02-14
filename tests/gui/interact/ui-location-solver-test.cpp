/*
  UILocationSolver(Test)  -  verify mechanics of a DSL to configure view allocation

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file ui-location-resolver-test.cpp
 ** unit test \ref UILocationSolver_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "gui/interact/ui-coord.hpp"
#include "gui/interact/ui-location-solver.hpp"
#include "gui/interact/gen-node-location-query.hpp"
#include "lib/format-cout.hpp" ////////////TODO
//#include "lib/idi/entry-id.hpp"
//#include "lib/diff/gen-node.hpp"
//#include "lib/util.hpp"

//#include <string>


using std::string;
using lib::diff::MakeRec;
using lib::diff::Rec;
//using lib::Symbol;
//using util::join;
//using lib::idi::EntryID;
//using lib::diff::GenNode;
//using util::isSameObject;
using util::isnil;


namespace gui  {
namespace interact {
namespace test {
  
//  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
//  using lib::test::showSizeof;
  
  namespace { //Test fixture...
    
  }//(End)Test fixture
  
  
  /******************************************************************************//**
   * @test cover a mechanism to resolve the desired location of an UI-element.
   *       The UILocationSolver is operated by the ViewLocator service, which itself
   *       is part of the InteractionDirector. In typical usage, the location rules
   *       are drawn from the [ViewSpec-DSL](\ref view-spec-dsl.hpp), evaluated
   *       with the help of a [Coordinate Resolver](\ref UICoordResolver), based on
   *       the real UI topology existing at that moment, accessible in abstracted
   *       form through the LocationQuery interface. This test setup here mimics that
   *       invocation scheme, but replaces the real UI by an abstract tree notation
   *       embedded directly into the individual test cases.
   * 
   * @see ui-location-solver.hpp
   * @see view-spec-dsl.hpp
   * @see UICoordResolver_test
   */
  class UILocationSolver_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          simple_usage_example();
          verify_cornerCases();
          verify_standardSituations();
        }
      
      
      /** @test demonstrate the typical invocation and usage" */
      void
      simple_usage_example()
        {
           //-------------------------------------------------------------Test-Fixture
          // a Test dummy placeholder for the real UI structure
          Rec dummyUiStructure = MakeRec()
                                   .set("window-1"
                                       , MakeRec()
                                           .type("perspective-A")
                                           .set("exclusivePanel", MakeRec())
                                       );
            // helper to answer "location queries" backed by this structure
          GenNodeLocationQuery locationQuery{dummyUiStructure};
          //--------------------------------------------------------------(End)Test-Fixture
          
          
          // our test subject....
          UILocationSolver solver{locationQuery};
          
          // a rule to probe (meaning: attach it at the "shoddy" panel)
          LocationRule rule{UICoord().panel("shoddy")};
          
          // Now ask for a location to attach a view named "worldview" at the "shoddy" panel
          // No solution can be found, since there is no "shoddy" panel
          CHECK (isnil (solver.solve (rule, UIC_VIEW, "worldview")));
          
          // add second location clause to the rule
          // (meaning: accept any path leading down to an "exclusivePanel")
          rule.append(UICoord().panel("exclusivePanel"));
          
          // and now we get a solution, since the second rule can be wildcard-matched
          UICoord location = solver.solve (rule, UIC_VIEW, "worldview");
          CHECK (not isnil (location));
          
          // the full solution filled in the missing parts and added the new view on top
          CHECK ("UI:window-1[perspective-A]-exclusivePanel.worldview" == string(location));
          
          // NOTE: the new view does not (yet) exist, but the preceding part can be "covered"
          //       To verify this, we attach a coordinate resolver (likewise backed by our dummy UI)
          UICoordResolver resolver{location, locationQuery};
          CHECK (resolver.isCoveredPartially());
          CHECK (not resolver.isCoveredTotally());
          CHECK (UIC_VIEW == resolver.coverDepth());  // covered up to VIEW level
        }                                            //  (the view itself is not covered)
      
      
      
      /** @test cover theoretical corner cases regarding the process of location solving.
       * Point in question are the requirements and limits when querying against one or several 
       * location specification clauses. The actual matching of a location pattern against a UI topology
       * is beyond scope and covered [elsewhere](\ref UICoordResolver_test::verify_mutateCoverage)
       * - empty clauses act as neutral element
       * - prerequisites regarding the depth of a location clause relevant for solution
       * - the impact of the query and especially its expected depth
       * - completely explicit clauses vs clauses with wildcards
       * - relevance of partial or total coverage for the solution
       * - regular clauses vs. _create clauses_ (which mandate creating parents as needed)
       * - usage of the first applicable solution when several clauses are given
       */
      void
      verify_cornerCases()
        {
           //-------------------------------------------------------------Test-Fixture
          GenNodeLocationQuery tree{MakeRec()
                                      .set("win"
                                          , MakeRec()
                                              .type("A")
                                              .set ("thePanel"
                                                   , MakeRec()
                                                       .set ("theView"
                                                            , MakeRec()
                                                                .set ("#5"
                                                                     , MakeRec()
                                                                         .set ("up", MakeRec())
                                                                         .set ("down"
                                                                              , MakeRec()
                                                                                  .set ("the"
                                                                                       , MakeRec()
                                                                                           .set ("kitchen"
                                                                                                , MakeRec()
                                                                                                    .set ("sink", MakeRec())
                                                                                                )
                                                                                       )
                                                                              )
                                                                     )
                                                            )
                                                   )
                                          )};
          UILocationSolver solver{tree};
          //--------------------------------------------------------------(End)Test-Fixture
          
          
          /* === empty clause === */
          LocationRule r1{UICoord()};
          CHECK (isnil (solver.solve (r1, UIC_PATH, "to/salvation")));
          CHECK (isnil (solver.solve (r1, UIC_WINDOW, "redemption")));
          
          /* === empty clause is neutral === */
          r1.append (UICoord().path("down/to").create());
          auto s1 = solver.solve(r1, UIC_PATH+2, "hell");
          CHECK ("UI:win[A]-thePanel.theView.#5/down/to/hell" == string{s1});
          

          /* === clause too short === */
          LocationRule r2{UICoord().path("down/the")};
          CHECK (    isnil (solver.solve (r2, UIC_PATH+3, "sink")));
          
          /* === clause too long === */
          CHECK (    isnil (solver.solve (r2, UIC_VIEW, "theView")));
          
          CHECK (not isnil (solver.solve (r2, UIC_PATH+1, "any")));
          CHECK (not isnil (solver.solve (r2, UIC_PATH+2, "kitchen")));
          
          
          
          /* === query on existing window === */
          LocationRule r31{UICoord::window("win")};
          CHECK ("UI:win" == string{solver.solve (r31, UIC_WINDOW, "wigwam")});
          
          /* === query on generic window spec === */
          LocationRule r32{UICoord::currentWindow()};
          CHECK ("UI:win" == string{solver.solve (r32, UIC_WINDOW, "wigwam")});
          
          /* === query on non existing window === */
          LocationRule r33{UICoord::window("lindows")};
          CHECK (isnil (solver.solve (r33, UIC_WINDOW, "wigwam")));
          
          /* === query on existing window with create clause === */
          LocationRule r34{UICoord::window("win").create()};
          CHECK ("UI:win" == string{solver.solve (r34, UIC_WINDOW, "wigwam")});
          
          /* === query on non existing window with create clause === */
          LocationRule r35{UICoord::window("windux").create()};
          CHECK ("UI:windux" == string{solver.solve (r35, UIC_WINDOW, "wigwam")});
          
          
          /* === query on existing perspective === */
          LocationRule r41{UICoord().persp("A")};
          CHECK ("UI:win[A]"   == string{solver.solve (r41, UIC_PERSP, "x")});
          CHECK ("UI:win[A]-x" == string{solver.solve (r41, UIC_PANEL, "x")});
          
          /* === query on elided perspective === */
          LocationRule r42{UICoord().persp(UIC_ELIDED)};
//        CHECK ("UI:win[A]"   == string{solver.solve (r42, UIC_PERSP, "x")}); //////////////////////////////TICKET #1128 : support existential quantification
//        CHECK ("UI:win[A]-x" == string{solver.solve (r42, UIC_PANEL, "x")});
          
          /* === query on non existing perspective === */
          LocationRule r43{UICoord::firstWindow().persp("Ω")};
          CHECK (isnil (solver.solve (r43, UIC_PERSP, "x")));
          CHECK (isnil (solver.solve (r43, UIC_PANEL, "x")));
          
          /* === query on non existing perspective with create clause === */
          LocationRule r44{UICoord::firstWindow().persp("Ω").create()};
          CHECK ("UI:win[Ω]"   == string{solver.solve (r44, UIC_PERSP, "x")});
          CHECK ("UI:win[Ω]-x" == string{solver.solve (r44, UIC_PANEL, "x")});
          
          
          /* === query on deep path covered === */
          LocationRule r51{UICoord("firstWindow","A","thePanel","theView","#5","down","the","kitchen")};
          CHECK ("UI:win[A]-thePanel.theView.#5/down/the/kitchen"       == string{solver.solve (r51, UIC_PATH+2, "drain")});
          CHECK ("UI:win[A]-thePanel.theView.#5/down/the/kitchen/drain" == string{solver.solve (r51, UIC_PATH+3, "drain")});
          
          /* === query on deep path covered with create clause === */
          LocationRule r52{UICoord::firstWindow().append("A/thePanel/theView/#5/down/the/kitchen").create()};
          CHECK ("UI:win[A]-thePanel.theView.#5/down/the/kitchen"       == string{solver.solve (r52, UIC_PATH+2, "drain")});
          CHECK ("UI:win[A]-thePanel.theView.#5/down/the/kitchen/drain" == string{solver.solve (r52, UIC_PATH+3, "drain")});
          
          /* === query on deep path partially covered === */
          LocationRule r53{UICoord::firstWindow().append("A/thePanel/theView/#5/down/the/drain")};
          CHECK (isnil (solver.solve (r53, UIC_PATH+2, "drain")));
          CHECK (isnil (solver.solve (r53, UIC_PATH+3, "drain")));
          
          /* === query on deep path partially covered with create clause === */
          LocationRule r54{UICoord::firstWindow().append("A/thePanel/theView/#5/down/the/drain").create()};
          CHECK ("UI:win[A]-thePanel.theView.#5/down/the/drain"         == string{solver.solve (r54, UIC_PATH+2, "drain")});
          CHECK ("UI:win[A]-thePanel.theView.#5/down/the/drain/drain"   == string{solver.solve (r54, UIC_PATH+3, "drain")});
          
          /* === query on deep path uncovered === */
          LocationRule r55{UICoord("rearWindow","A","thePanel","theView","#5","down","the","kitchen")};
          CHECK (isnil (solver.solve (r55, UIC_PATH+2, "floor")));
          CHECK (isnil (solver.solve (r55, UIC_PATH+3, "floor")));
          
          /* === query on deep path uncovered with create clause === */
          LocationRule r56{UICoord("rearWindow","A","thePanel","theView","#5","down","the","kitchen").rebuild().create()};
          CHECK ("UI:rearWindow[A]-thePanel.theView.#5/down/the/kitchen"       == string{solver.solve (r56, UIC_PATH+2, "floor")});
          CHECK ("UI:rearWindow[A]-thePanel.theView.#5/down/the/kitchen/floor" == string{solver.solve (r56, UIC_PATH+3, "floor")});
          
          
          /* === clause with wildcard covered === */
          LocationRule r61{UICoord().path("//kitchen")};
          CHECK ("UI:win[A]-thePanel.theView.#5/down/the/kitchen"       == string{solver.solve (r61, UIC_PATH+2, "drain")});
          
          /* === clause with wildcard covered without final element === */
          CHECK ("UI:win[A]-thePanel.theView.#5/down/the/kitchen/drain" == string{solver.solve (r61, UIC_PATH+3, "drain")});
          
          /* === create clause with wildcard completely covered === */
          LocationRule r62{UICoord().path("//kitchen").create()};
          CHECK ("UI:win[A]-thePanel.theView.#5/down/the/kitchen"       == string{solver.solve (r62, UIC_PATH+2, "window")});
          
          /* === create clause with wildcard covered without final element === */
          CHECK ("UI:win[A]-thePanel.theView.#5/down/the/kitchen/window" == string{solver.solve (r62, UIC_PATH+3, "window")});
          
          /* === clause with wildcard partially covered === */
          LocationRule r63{UICoord().path("/the/road")};
          CHECK (isnil (solver.solve (r63, UIC_PATH+2, "kitchen")));   //NOTE: .../down/the/kitchen would match, but actually .../down/the/road is tested, which fails
          
          /* === create clause with wildcard partially covered === */
          LocationRule r64{UICoord().path("/the/road").create()};
          CHECK ("UI:win[A]-thePanel.theView.#5/down/the/road" == string{solver.solve (r64, UIC_PATH+2, "drain")});
          
          /* === clause with wildcard uncovered === */
          LocationRule r65{UICoord().path("//road")};
          CHECK (isnil (solver.solve (r65, UIC_PATH+2, "kitchen")));
          
          /* === create clause with wildcard uncovered === */
          LocationRule r66{UICoord().path("//road").create()};
          CHECK (isnil (solver.solve (r66, UIC_PATH+2, "kitchen")));
          
          
          /* === two clauses both satisfied === */
          LocationRule r71{UICoord().path("down")};
          r71.append      (UICoord().path("up"));
          CHECK ("UI:win[A]-thePanel.theView.#5/down/time" == string{solver.solve (r71, UIC_PATH+1, "time")});
          
          /* === two clauses first one unsatisfied === */
          LocationRule r72{UICoord().path("up/the")};
          r72.append      (UICoord().path("down/"));
          CHECK ("UI:win[A]-thePanel.theView.#5/down/time" == string{solver.solve (r72, UIC_PATH+1, "time")});
          
          /* === create clause first and satisfied === */
          LocationRule r73{UICoord().path("up/link").create()};
          r73.append      (UICoord().path("down/"));
          CHECK ("UI:win[A]-thePanel.theView.#5/up/link"   == string{solver.solve (r73, UIC_PATH+1, "time")});
          
          /* === create clause first and unsatisfied === */
          LocationRule r74{UICoord().path("cross/link").create()};
          r74.append      (UICoord().path("down/"));
          CHECK ("UI:win[A]-thePanel.theView.#5/down/time" == string{solver.solve (r74, UIC_PATH+1, "time")});
          
          /* === create clause second but first clause satisfied === */
          LocationRule r75{UICoord().path("up/")};
          r75.append      (UICoord().path("down/link").create());
          CHECK ("UI:win[A]-thePanel.theView.#5/up/time"   == string{solver.solve (r75, UIC_PATH+1, "time")});
          
          /* === create clause second and satisfied === */
          LocationRule r76{UICoord().path("up/link")};
          r76.append      (UICoord().path("down/link").create());
          CHECK ("UI:win[A]-thePanel.theView.#5/down/link" == string{solver.solve (r76, UIC_PATH+1, "time")});
          
          /* === create clause second and both unsatisfied === */
          LocationRule r77{UICoord().path("up/link")};
          r77.append      (UICoord().path("town/link").create());
          CHECK (isnil (solver.solve (r77, UIC_PATH+1, "time")));
          
          CHECK (string{r77} == "=~	.. UI:?/up/link"
                                "\n	OR UI:?/town/link create!");
        }
      
      
      /** @test emulate the relevant standard situations of view location resolution.
       * The typical location specifications to be expected in practice can be subsumed
       * under a small selection of standard situations; this test demonstrates how these
       * are triggered by specific tree configurations in a (hopefully) obvious way.
       * 
       * For this purpose, we create a single set of location clauses here, but evaluate them
       * each time against different (simulated) UI tree configurations to verify that the expected
       * resulting location is actually derived in all those cases.
       */
      void
      verify_standardSituations()
        {
          // Test Fixture: a solver which always queries the current state of a (simulated) uiTree
          Rec uiTree;
          std::unique_ptr<GenNodeLocationQuery> query;
          UILocationSolver solver{[&]() -> GenNodeLocationQuery&
                                     {
                                       query.reset (new GenNodeLocationQuery(uiTree));
                                       return *query;
                                     }};
          
          // Test Fixture: common set of location clauses
          LocationRule location{UICoord().persp("edit").panel("viewer")};
          location.append      (UICoord::currentWindow().panel("viewer"));
          location.append      (UICoord().panel("viewer"));
          location.append      (UICoord().tab("type(Asset)"));
          location.append      (UICoord().persp("asset").view("asset"));
          location.append      (UICoord().view("asset").tab("type(Asset)").create());
          location.append      (UICoord::currentWindow().panel("viewer").create());
          location.append      (UICoord::window("meta").panel("infobox").view("inspect").create());
          
          cout << location << endl;
          
          
          /* === match by perspective + panel === */
          uiTree = MakeRec()
                     .set("win"
                         , MakeRec()
                             .type("edit")
                             .set ("viewer", MakeRec()));
          cout << solver.solve (location, UIC_VIEW, "videoViewer") <<endl;
          cout << solver.solve (location, UIC_TAB, "clipAssets") <<endl;
          
          /* === match by generic window + panel === */
          
          /* === match by panel alone === */
          
          
          /* === wildcard match on view === */
          
          /* === wildcard match on panel and view appended === */
          
          
          /* === successful create clause with wildcard === */
          
          /* === unsatisfied create clause with wildcard === */
          
          /* === match on create clause with generic window spec and panel === */
          
          /* === completely uncovered create-from-scratch === */
          
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (UILocationSolver_test, "unit gui");
  
  
}}} // namespace gui::interact::test
