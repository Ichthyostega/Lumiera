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
          
          /* === empty clause is neutral === */
          r1.append (UICoord().path("down/to").create());
          auto s1 = solver.solve(r1, UIC_PATH+2, "hell");
          CHECK ("UI:win[A]-thePanel.theView.#5/down/to/hell" == string(s1));
          

          /* === clause too short === */
          
          /* === clause too long === */
          
          
          /* === query on existing window === */
          
          /* === query on generic window spec === */
          
          /* === query on non existing window === */
          
          /* === query on existing window with create clause === */
          
          /* === query on non existing window with create clause === */
          
          
          /* === query on existing perspective === */
          
          /* === query on elided perspective === */
          
          /* === query on non existing perspective === */
          
          /* === query on non existing perspective with create clause === */
          
          
          /* === query on deep path covered === */
          
          /* === query on deep path covered with create clause === */
          
          /* === query on deep path partially covered === */
          
          /* === query on deep path partially covered with create clause === */
          
          /* === query on deep path uncovered === */
          
          /* === query on deep path uncovered with create clause === */
          
          
          /* === clause with wildcard covered === */
          
          /* === clause with wildcard covered without final element === */
          
          /* === create clause with wildcard completely covered === */
          
          /* === create clause with wildcard covered without final element === */
          
          /* === clause with wildcard partially covered === */
          
          /* === create clause with wildcard partially covered === */
          
          /* === clause with wildcard uncovered === */
          
          /* === create clause with wildcard uncovered === */
          
          
          /* === two clauses both satisfied === */
          
          /* === two clauses first one unsatisfied === */
          
          /* === create clause first and satisfied === */
          
          /* === create clause first and unsatisfied === */
          
          /* === create clause second but first clause satisfied === */
          
          /* === create clause second and satisfied === */
          
          /* === create clause second and both unsatisfied === */
          
        }
      
      
      void
      verify_standardSituations()
        {
          UNIMPLEMENTED ("emulate the relevant standard situations of view location resolution");
          
          /* === match by perspective + panel === */
          
          /* === match by window + panel === */
          
          /* === match by panel alone === */
          
          /* === match on create clause with generic window spec and panel === */
          
          
          /* === wildcard match on view === */
          
          /* === wildcard match on panel and view appended === */
          
          
          /* === successful create clause with wildcard === */
          
          /* === unsatisfied create clause with wildcard === */
          
          /* === completely uncovered create-from-scratch === */
          
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (UILocationSolver_test, "unit gui");
  
  
}}} // namespace gui::interact::test
