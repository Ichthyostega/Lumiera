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
  using lumiera::error::LUMIERA_ERROR_INDEX_BOUNDS;
  using lumiera::error::LUMIERA_ERROR_LOGIC;
  
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
          Rec dummyUiStrcture = MakeRec().scope(
                                    MakeRec().type("perspective-A")
                                    .genNode("window-1"),
                                    MakeRec().type("perspective-B")
                                    .scope(
                                        MakeRec().genNode("panelX")
                                    ).genNode("window-2")
                                );

        }
      
      
      void
      verify_backingQuery()
        {
          UNIMPLEMENTED ("verify the command-and-query interface backing the resolver");
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
