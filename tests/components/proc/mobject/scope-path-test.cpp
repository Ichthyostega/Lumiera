/*
  ScopePath(Test)  -  verify handling of logical access path down from Session root
 
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
   * @test properties and behaviour of the path of nested scopes.
   *       Using a pseudo-session (actually just a PlacementIndex), this test
   *       creates some nested scopes and executes navigation moves on them.
   * @see  mobject::Placement
   * @see  mobject::session::ScopePath
   * @see  mobject::session::QueryFocus
   */
  class ScopePath_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          UNIMPLEMENTED ("unit test regarding placement scope handling");
        }
          
    };
  
  
  /** Register this test class... */
  LAUNCHER (ScopePath_test, "unit session");
  
  
}}} // namespace mobject::session::test
