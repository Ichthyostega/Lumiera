/*
  UICoord(Test)  -  properties of topological UI coordinate specifications

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

/** @file ui-coord-test.cpp
 ** unit test \ref UICoord_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "gui/interact/ui-coord.hpp"
#include "lib/format-cout.hpp"/////////////////////////TODO RLY?
#include "lib/format-util.hpp"
//#include "lib/idi/entry-id.hpp"
//#include "lib/diff/gen-node.hpp"
#include "lib/util.hpp"

#include <string>


using std::string;
//using lib::idi::EntryID;
//using lib::diff::GenNode;
//using util::isSameObject;
using lib::Symbol;
using util::isnil;
using util::join;



namespace gui  {
namespace interact {
namespace test {
  
//  using lumiera::error::LUMIERA_ERROR_WRONG_TYPE;
  using lumiera::error::LUMIERA_ERROR_INDEX_BOUNDS;
  using lumiera::error::LUMIERA_ERROR_LOGIC;
  
  namespace { //Test fixture...
    
  }//(End)Test fixture
  
  
  /******************************************************************************//**
   * @test verify the mechanics of a functor based internal DSL
   *       to configure access and allocation patters for component views.
   * 
   * @see id-scheme.hpp
   * @see ViewLocator
   * @see UICoord_test
   */
  class UICoord_test : public Test
    {
      
      virtual void
      run (Arg)
        {
          verify_basics();
          verify_builder();
          verify_stringRepr();
          verify_comparisons();
          verify_queryAnchor();
          verify_queryCoverage();
          verify_mutateAnchor();
          verify_mutateCover();
          verify_mutateExtend();
          verify_mutateCreate();
        }
      
      
      void
      verify_basics()
        {
          UICoord undef;
          CHECK (isnil (undef));
          
          UICoord uic{"Γ","Δ","Θ","Ξ","Σ","Ψ","Φ","Ω"};
          CHECK (not isnil (uic));
          CHECK (8 == uic.size());
          // path is iterable
          CHECK ("ΓΔΘΞΣΨΦΩ" == join(uic,""));
          
          // indexed access
          CHECK ("Γ" == uic[UIC_WINDOW]);
          CHECK ("Δ" == uic[UIC_PERSP]);
          CHECK ("Θ" == uic[UIC_PANEL]);
          CHECK ("Ξ" == uic[UIC_VIEW]);
          CHECK ("Σ" == uic[UIC_TAB]);
          CHECK ("Ψ" == uic[UIC_PATH]);
          CHECK ("Φ" == uic[UIC_PATH+1]);
          CHECK ("Ω" == uic[UIC_PATH+2]);
          
          // iteration matches index order
          uint i=0;
          for (UICoord::iterator ii = uic.begin(); ii; ++ii, ++i)
            CHECK (uic[i] == *ii);
          CHECK (8 == i);
          
          VERIFY_ERROR (INDEX_BOUNDS, uic[8]);
          
          // string representation
          CHECK ("UI:Γ[Δ]-Θ.Ξ.Σ/Ψ/Φ/Ω" == string(uic));
          CHECK ("Γ[Δ]-Θ.Ξ.Σ" == uic.getComp());
          CHECK ("Ψ/Φ/Ω" == uic.getPath());
          
          // new value can be assigned, but not altered in place
          uic = UICoord{nullptr,nullptr,"Θ",nullptr,"Σ",nullptr,"Φ"};
          CHECK (7 == uic.size());
          
          // representation is trimmed and filled
          CHECK ("UI:?-Θ.*.Σ/*/Φ" == string(uic));
          CHECK (Symbol::EMPTY == uic[UIC_WINDOW]);
          CHECK (Symbol::EMPTY == uic[UIC_PERSP]);
          CHECK ("Θ" == uic[UIC_PANEL]);
          CHECK ("*" == uic[UIC_VIEW]);
          CHECK ("Σ" == uic[UIC_TAB]);
          CHECK ("*" == uic[UIC_PATH]);
          CHECK ("Φ" == uic[UIC_PATH+1]);
          VERIFY_ERROR (INDEX_BOUNDS, uic[UIC_PATH+2]);
        }
      
      
      void
      verify_builder()
        {
          UICoord uic1 = UICoord::window("window");
          UICoord uic2 = uic1.view("view");
          CHECK ("UI:window" == string(uic1));
          CHECK ("UI:window[*]-*.view" == string(uic2));
          CHECK (1 == uic1.size());
          CHECK (4 == uic2.size());
          
          // fault-tolerant accessors for the generic part
          CHECK ("window" == uic1.getWindow());
          CHECK ("window" == uic2.getWindow());
          CHECK (""       == uic1.getPersp());
          CHECK ("*"      == uic2.getPersp());
          CHECK (""       == uic1.getPanel());
          CHECK ("*"      == uic2.getPanel());
          CHECK (""       == uic1.getView());
          CHECK ("view"   == uic2.getView());
          CHECK (""       == uic1.getTab());
          CHECK (""       == uic2.getTab());
          CHECK (""       == uic1.getPath());
          CHECK (""       == uic2.getPath());
          CHECK ("window"           == uic1.getComp());
          CHECK ("window[*]-*.view" == uic2.getComp());
          
          VERIFY_ERROR (INDEX_BOUNDS, uic1[UIC_PERSP]);
          VERIFY_ERROR (INDEX_BOUNDS, uic2[UIC_TAB]);
          
          UICoord uic3 = UICoord().view("view");
          CHECK (4 == uic3.size());
          CHECK ("UI:?.view" == string(uic3));
          CHECK (""     == uic3.getWindow());
          CHECK (""     == uic3.getPersp());
          CHECK (""     == uic3.getPanel());
          CHECK ("view" == uic3.getView());
          
          UICoord uic4 = uic3.persp("persp");
          CHECK (4 == uic4.size());
          CHECK ("UI:?[persp]-*.view" == string(uic4));

          uic4 = uic3.append("tab");
          CHECK (5 == uic4.size());
          CHECK ("UI:?.view.tab" == string(uic4));
          uic4 = uic3.prepend("panel");
          CHECK (4 == uic4.size());
          CHECK ("UI:?-panel.view" == string(uic4));
          uic4 = uic4.tab(555);
          CHECK (5 == uic4.size());
          CHECK ("UI:?-panel.view.#555" == string(uic4));
          VERIFY_ERROR(LOGIC, uic1.prepend("root"));
        }
      
      
      void
      verify_stringRepr()
        {
          UICoord uic;
          CHECK ("UI:?" == string(uic));
          CHECK ("" == uic.getComp());
          CHECK ("" == uic.getPath());
          
          uic = uic.path("ἁρχή");
          CHECK ("UI:?/ἁρχή" == string(uic));
          CHECK ("" == uic.getComp());
          CHECK ("ἁρχή" == uic.getPath());
          
          uic = uic.path("α/β/γ");
          CHECK ("UI:?/α/β/γ" == string(uic));
          CHECK ("" == uic.getComp());
          CHECK ("α/β/γ" == uic.getPath());
          
          uic = uic.append("δ");
          CHECK ("UI:?/α/β/γ/δ" == string(uic));
          CHECK ("" == uic.getComp());
          CHECK ("α/β/γ/δ" == uic.getPath());
          
          uic = uic.append("");
          CHECK ("UI:?/α/β/γ/δ" == string(uic));
          CHECK ("" == uic.getComp());
          CHECK ("α/β/γ/δ" == uic.getPath());
          
          uic = uic.append("ε/λ/ον");
          CHECK ("UI:?/α/β/γ/δ/ε/λ/ον" == string(uic));
          CHECK ("" == uic.getComp());
          CHECK ("α/β/γ/δ/ε/λ/ον" == uic.getPath());

          // note: we built a partially empty path array...
          CHECK (12 == uic.size());
          CHECK (Symbol::EMPTY == uic.getView());
          CHECK (Symbol::EMPTY == uic.getTab());
          CHECK (Symbol::EMPTY == uic[UIC_WINDOW]);
          CHECK (Symbol::EMPTY == uic[UIC_PERSP]);
          CHECK (Symbol::EMPTY == uic[UIC_PANEL]);
          CHECK (Symbol::EMPTY == uic[UIC_VIEW]);
          CHECK (Symbol::EMPTY == uic[UIC_TAB]);
          CHECK ("α"  == uic[UIC_PATH]);
          CHECK ("β"  == uic[UIC_PATH+1]);
          CHECK ("γ"  == uic[UIC_PATH+2]);
          CHECK ("δ"  == uic[UIC_PATH+3]);
          CHECK ("ε"  == uic[UIC_PATH+4]);
          CHECK ("λ"  == uic[UIC_PATH+5]);
          CHECK ("ον" == uic[UIC_PATH+6]);
          
          uic = uic.prepend("ειδος");
          CHECK ("UI:?.ειδος/α/β/γ/δ/ε/λ/ον" == string(uic));
          CHECK ("?.ειδος" == uic.getComp());
          CHECK ("α/β/γ/δ/ε/λ/ον" == uic.getPath());
          CHECK (12 == uic.size());
          
          uic = uic.tab("");
          CHECK ("UI:?/α/β/γ/δ/ε/λ/ον" == string(uic));
          CHECK ("" == uic.getComp());
          CHECK ("α/β/γ/δ/ε/λ/ον" == uic.getPath());
          
          uic = uic.view("ειδος");
          CHECK ("UI:?.ειδος.*/α/β/γ/δ/ε/λ/ον" == string(uic));
          CHECK ("?.ειδος.*" == uic.getComp());
          CHECK ("α/β/γ/δ/ε/λ/ον" == uic.getPath());
          
          uic = uic.prepend("panel");
          CHECK ("UI:?-panel.ειδος.*/α/β/γ/δ/ε/λ/ον" == string(uic));
          CHECK ("?-panel.ειδος.*" == uic.getComp());
          CHECK ("α/β/γ/δ/ε/λ/ον" == uic.getPath());
          
          uic = uic.view(nullptr);
          CHECK ("UI:?-panel.*.*/α/β/γ/δ/ε/λ/ον" == string(uic));
          CHECK ("?-panel.*.*" == uic.getComp());
          CHECK ("α/β/γ/δ/ε/λ/ον" == uic.getPath());
          
          uic = uic.tab(" ");
          CHECK ("UI:?-panel.*. /α/β/γ/δ/ε/λ/ον" == string(uic));
          CHECK ("?-panel.*. " == uic.getComp());
          CHECK ("α/β/γ/δ/ε/λ/ον" == uic.getPath());
          
          uic = uic.prepend("perspective");
          CHECK ("UI:?[perspective]-panel.*. /α/β/γ/δ/ε/λ/ον" == string(uic));
          CHECK ("?[perspective]-panel.*. " == uic.getComp());
          CHECK ("α/β/γ/δ/ε/λ/ον" == uic.getPath());
          
          uic = uic.prepend("win");
          CHECK ("UI:win[perspective]-panel.*. /α/β/γ/δ/ε/λ/ον" == string(uic));
          CHECK ("win[perspective]-panel.*. " == uic.getComp());
          CHECK ("α/β/γ/δ/ε/λ/ον" == uic.getPath());
          
          uic = uic.persp("");
          CHECK ("UI:win[*]-panel.*. /α/β/γ/δ/ε/λ/ον" == string(uic));
          CHECK ("win[*]-panel.*. " == uic.getComp());
          CHECK ("α/β/γ/δ/ε/λ/ον" == uic.getPath());
          CHECK (12 == uic.size());
          
          uic = uic.path(" ");
          CHECK ("UI:win[*]-panel.*. / " == string(uic));
          CHECK ("win[*]-panel.*. " == uic.getComp());
          CHECK (" " == uic.getPath());
          CHECK (6 == uic.size());
          CHECK (" " == uic[UIC_PATH]);
          VERIFY_ERROR (INDEX_BOUNDS, uic[UIC_PATH+1]);
          
          uic = uic.path(nullptr);
          CHECK ("UI:win[*]-panel.*. " == string(uic));
          CHECK ("win[*]-panel.*. " == uic.getComp());
          CHECK ("" == uic.getPath());
          CHECK (5 == uic.size());
          VERIFY_ERROR (INDEX_BOUNDS, uic[UIC_PATH]);
          
          uic = uic.append(nullptr);
          CHECK ("UI:win[*]-panel.*. " == string(uic));
          CHECK ("win[*]-panel.*. " == uic.getComp());
          CHECK ("" == uic.getPath());
          CHECK (5 == uic.size());
          
          uic = uic.append("*");
          CHECK ("UI:win[*]-panel.*. " == string(uic));
          CHECK ("win[*]-panel.*. " == uic.getComp());
          CHECK ("" == uic.getPath());
          CHECK (5 == uic.size());
          
          uic = uic.append("**");
          CHECK ("UI:win[*]-panel.*. /**" == string(uic));
          CHECK ("win[*]-panel.*. " == uic.getComp());
          CHECK ("**" == uic.getPath());
          CHECK ("**" == uic[UIC_PATH]);
          CHECK (6 == uic.size());
          
          uic = uic.tab("");
          CHECK ("UI:win[*]-panel.*.*/**" == string(uic));
          CHECK ("win[*]-panel.*.*" == uic.getComp());
          CHECK ("**" == uic.getPath());
          CHECK (6 == uic.size());
          
          uic = uic.path("");
          CHECK ("UI:win[*]-panel" == string(uic));
          CHECK ("win[*]-panel" == uic.getComp());
          CHECK ("" == uic.getPath());
          CHECK (3 == uic.size());
        }
      
      
      void
      verify_comparisons()
        {
          UNIMPLEMENTED ("verify comparison of UI coordinates");
        }
      
      
      void
      verify_queryAnchor()
        {
          UNIMPLEMENTED ("query anchorage of given UI coordinates");
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
      verify_mutateCover()
        {
          UNIMPLEMENTED ("mutate given UI coordinates by reducing to covered part");
        }
      
      
      void
      verify_mutateExtend()
        {
          UNIMPLEMENTED ("mutate given UI coordinates by uncovered extension");
        }
      
      
      void
      verify_mutateCreate()
        {
          UNIMPLEMENTED ("mutate given UI coordinates by creating new components");
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (UICoord_test, "unit gui");
  
  
}}} // namespace gui::interact::test
