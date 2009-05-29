/*
  PlacementHierarchy(Test)  -  concept draft how to deal with the MObject hierarchy in Placements
 
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
//#include "proc/asset/media.hpp"
//#include "proc/mobject/session.hpp"
//#include "proc/mobject/session/edl.hpp"
//#include "proc/mobject/session/testclip.hpp"
//#include "proc/mobject/placement.hpp"
//#include "proc/mobject/placement-ref.hpp"
//#include "proc/mobject/explicitplacement.hpp"
#include "proc/mobject/test-dummy-mobject.hpp"
#include "lib/hash-indexed.hpp"
#include "lib/util.hpp"

#include <boost/format.hpp>
#include <tr1/memory>
#include <iostream>
#include <cstdlib>

using std::tr1::shared_ptr;
using boost::format;
using boost::str;
//using lumiera::Time;
//using util::contains;
using lib::HashIndexed;
using std::string;
using std::cout;
using std::rand;


namespace mobject {
namespace session {
namespace test    {
  
  using namespace mobject::test;
  
  
  /*******************************************************************************
   * @test Currently (5/09) this is a concept draft separate of any existing types.
   *       The intention is to rework the Placement implementation based on the
   *       outcomes of this experiment. When doing so, this test could later on
   *       serve to document and cover the corresponding Placement properties.
   * @see  mobject::Placement
   * @see  mobject::MObject#create
   */
  class PlacementHierarchy_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          TestPlacement<TestSubMO1>  pSub1(*new TestSubMO1);
          TestPlacement<TestSubMO2>  pSub2(*new TestSubMO2);
          TestPlacement<TestSubMO21> pSub3(*new TestSubMO21);
          
          TestPlacement<DummyMO>     pSubM (pSub3);
          
          /////////////////////////////////TODO
          format fmt ("sizeof( %s ) = %d\n");
          cout << fmt % "Pla<Sub1>"   % sizeof(pSub1);
          cout << fmt % "Pla<Sub2>"   % sizeof(pSub2);
          cout << fmt % "Pla<Sub3>"   % sizeof(pSub3);
          
          cout << string(pSub1) << "\n";
          cout << string(pSub2) << "\n";
          cout << string(pSubM) << "\n";
          cout << pSub1->operator string() << "\n";
          cout << pSub2->operator string() << "\n";
          cout << pSubM->operator string() << "\n";
          pSub3->specialAPI();
          
          cout << format_PlacementID(pSub1) << "\n";
          cout << format_PlacementID(pSub2) << "\n";
          cout << format_PlacementID(pSub3) << "\n";
          cout << format_PlacementID(pSubM) << "\n";
          
          cout << "Hurgha!\n";
        }
    };
  
  
  /** Register this test class... */
  LAUNCHER (PlacementHierarchy_test, "unit session");
  
  
}}} // namespace mobject::session::test
