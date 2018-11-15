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

/** @file ui-location-solver-test.cpp
 ** unit test \ref UILocationSolver_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "stage/interact/ui-coord.hpp"
#include "stage/interact/ui-location-solver.hpp"
#include "gen-node-location-query.hpp"
#include "lib/format-cout.hpp"

#include <string>


using std::string;
using lib::diff::MakeRec;
using lib::diff::Rec;

using util::isnil;


namespace gui  {
namespace interact {
namespace test {
  
  
  
  /******************************************************************************//**
   * @test verify a mechanism to resolve the desired location of an UI-element.
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
      
      
      /** @test demonstrate the typical invocation and usage */
      void
      simple_usage_example()
        {
           //-------------------------------------------------------------Test-Fixture
          // a Test dummy placeholder for the real UI structure
          Rec dummyUiStructure = MakeRec()
                                   .set("window-1"
                                       , MakeRec()
                                           .type("perspective")
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
          CHECK ("UI:window-1[perspective]-exclusivePanel.worldview" == string(location));
          
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
          
          /* === query on elided perspective ("just any existing") === */
          LocationRule r42{UICoord().persp(UIC_ELIDED)};
          CHECK ("UI:win[A]"   == string{solver.solve (r42, UIC_PERSP, "x")});
          CHECK ("UI:win[A]-x" == string{solver.solve (r42, UIC_PANEL, "x")});
          
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
//        location.append      (UICoord().tab("assetType()"));                         //////////////////////TICKET #1130 : do we want to support match based on invocation context (here: the type of the asset to be displayed)
          location.append      (UICoord().persp("asset").view("asset"));
          location.append      (UICoord().panel("asset").view("asset").create());
          location.append      (UICoord::currentWindow().panel("viewer").create());                          //Note: especially for this kind of rule, .persp(UIC_ELIDED) is injected automatically
          location.append      (UICoord::window("meta").persp("config").panel("infobox").view("inspect").create());
          
          cout << location << endl;
          
          
          /* === match by perspective + panel === */
          uiTree = MakeRec()
                     .set("win"
                         , MakeRec()
                             .type("edit")
                             .set ("viewer", MakeRec()));
          CHECK ("UI:win[edit]-viewer.video" == string{solver.solve (location, UIC_VIEW, "video")});
          
          /* === match by generic window + panel === */
          uiTree = MakeRec()
                     .set("win"
                         , MakeRec()
                             .type("murky")
                             .set ("viewer", MakeRec()))
                     .set("woe"
                         , MakeRec()
                             .type("gloomy")
                             .set ("viewer", MakeRec()));
          CHECK ("UI:woe[gloomy]-viewer.video" == string{solver.solve (location, UIC_VIEW, "video")});       //Note: first rule does not match due to perspective
          
          /* === match by panel alone === */
          uiTree = MakeRec()
                     .set("win"
                         , MakeRec()
                             .type("murky")
                             .set ("viewer", MakeRec()))
                     .set("woe"
                         , MakeRec()
                             .type("gloomy")
                             .set ("timeline", MakeRec()));
          CHECK ("UI:win[murky]-viewer.video" == string{solver.solve (location, UIC_VIEW, "video")});        //Note: current window (==last one) has no "viewer"-panel
          
          
          
          /* === wildcard match on explicit existing view === */
          uiTree = MakeRec()
                     .set("win"
                         , MakeRec()
                             .type("shady")
                             .set("timeline", MakeRec()))
                     .set("woe"
                         , MakeRec()
                             .type("asset")
                             .set ("panel"
                                  , MakeRec()
                                      .set ("asset", MakeRec())
                                  ));
          CHECK ("UI:woe[asset]-panel.asset" == string{solver.solve (location, UIC_VIEW, "video")});         //Note: the 4th Rule matches on existing view "asset",
                                                                                                             //      in spite of our query demanding a view "video"
          /* === wildcard match based on the type of entity to be displaced === */
#if false ///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1130 : not yet possible. Match based on placeholder substituted from context
//        uiTree = MakeRec()
//                   .set("win"
//                       , MakeRec()
//                           .type("shady")
//                           .set ("special"
//                                , MakeRec()
//                                    .set ("asset",
//                                          MakeRec()
//                                            .set ("specialAsset", MakeRec())
//                                         )
//                                ))
//                   .set("woe"
//                       , MakeRec()
//                           .type("asset")
//                           .set ("panel"
//                                , MakeRec()
//                                    .set ("asset", MakeRec())
//                                ));
//        CHECK ("UI:win[shady]-special.asset.specialAsset" == string{solver.solve (location, UIC_TAB, "specialAsset")});
//                                                                                                         //Note: the next rule would match on the general asset panel
//                                                                                                         //      but this special rule allows to re-use a tab dedicated to specialAsset
#endif    ///////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1130 : not yet possible. Match based on placeholder substituted from context
          
          
          
          /* === create clause to build on a specific anchor point === */
          uiTree = MakeRec()
                     .set("win"
                         , MakeRec()
                             .type("shady")
                             .set ("asset", MakeRec())
                                  );
          auto solution = solver.solve (location, UIC_TAB, "video");                                         //Note: here the first "create"-rule is triggered: UI:?-asset.asset
          CHECK ("UI:win[shady]-asset.asset.video" == string{solution});                                     //      It requires a panel("asset") to exist, but creates the rest;
          CHECK ( 3 == UICoordResolver(solution, *query)                                                     //      indeed only the part up to the panel is detected as covered.
                                      .coverDepth());
                                                                                                             //Note: the following test cases can not trigger this rule, since it
          /* === match on create clause with generic window spec and panel === */                            //      contains leading wildcards and thus requires panel("asset")
          uiTree = MakeRec()
                     .set("win"
                         , MakeRec()
                             .type("shady")
                             .set("timeline", MakeRec()))
                     .set("woe"
                         , MakeRec()
                             .type("shoddy"));
          solution = solver.solve (location, UIC_VIEW, "video");
          CHECK ("UI:woe[shoddy]-viewer.video" == string{solution});
          CHECK ( 2 == UICoordResolver(solution, *query)                                                     //Note: only window and perspective are covered, the rest is to be created
                                      .coverDepth());
          
          /* === completely uncovered create-from-scratch === */
          solution = solver.solve (location, UIC_TAB, "engine");                                             //Note: same UI-tree, but this time we ask for a tab, so the previous rule
          CHECK ("UI:meta[config]-infobox.inspect.engine" == string{solution});                              //      is too short and thus the last catch-all rule gets triggered;
          CHECK ( 0 == UICoordResolver(solution, *query)                                                     //Note: result is indeed entirely uncovered (-> create from scratch)
                                      .coverDepth());
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (UILocationSolver_test, "unit gui");
  
  
}}} // namespace gui::interact::test
