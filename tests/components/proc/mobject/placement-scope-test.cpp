/*
  PlacementScope(Test)  -  accessing and navigating placement scope
 
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
#include "proc/mobject/session/test-scopes.hpp"
//#include "lib/lumitime.hpp"
//#include "proc/mobject/placement-ref.hpp"
//#include "proc/mobject/placement-index.hpp"
//#include "proc/mobject/test-dummy-mobject.hpp"
//#include "lib/util.hpp"

//#include <iostream>
//#include <string>

//using util::isSameObject;
//using lumiera::Time;
//using std::string;
//using std::cout;
//using std::endl;


namespace mobject {
namespace session {
namespace test    {
  
//  using namespace mobject::test;
//  typedef TestPlacement<TestSubMO21> PSub;
  
  
  /***************************************************************************
   * @test basic behaviour of the nested placement search scopes.
   *       Using a pseudo-session (actually just a PlacementIndex), this test
   *       creates some nested scopes and then...
   *       - discovers the scope of a placement
   *       - finds the parent scope
   *       - enumerates a scope path up to root
   * 
   * @see  mobject::Placement
   * @see  mobject::session::ScopePath
   * @see  mobject::session::QueryFocus
   */
  class PlacementScope_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          // Prepare an (test)Index backing the PlacementRefs
          PIdx index = build_testScopes();
          
          UNIMPLEMENTED ("function test of placement scope interface");
        }
      
    };
  
  
  /** Register this test class... */
  LAUNCHER (PlacementScope_test, "function session");
  
  
}}} // namespace mobject::session::test