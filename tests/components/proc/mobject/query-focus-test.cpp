/*
  QueryFocus(Test)  -  verify proper management of current scope
 
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
#include "proc/mobject/placement-index.hpp"
#include "proc/mobject/test-dummy-mobject.hpp"
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
  
  using namespace mobject::test;
  typedef TestPlacement<TestSubMO21> PSub;
  
  
  /**********************************************************************************
   * @test handling of current query focus when navigating a system of nested scopes.
   *       Using a pseudo-session (actually just a PlacementIndex), this test
   *       creates some nested scopes and then checks moving the "current scope".
   *       
   * @see  mobject::PlacementIndex
   * @see  mobject::session::ScopePath
   * @see  mobject::session::QueryFocus
   */
  class QueryFocus_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          PSub p1(*new TestSubMO21);
          PSub p2(*new TestSubMO21);
          PSub p3(*new TestSubMO21);
          PSub p4(*new TestSubMO21);
          PSub p5(*new TestSubMO21);
          
          // Prepare an (test)Index backing the PlacementRefs
          typedef shared_ptr<PlacementIndex> PIdx;
          PIdx index (PlacementIndex::create());
          PMO& root = index->getRoot();
          reset_PlacementIndex(index);

          index->insert (p1, root);
          index->insert (p2,  p1 );
          index->insert (p3,  p2 );
          index->insert (p4,  p3 );
          index->insert (p5,  p4 );
          
          UNIMPLEMENTED ("unit test to cover query focus management");
          
//??      ASSERT (0 == index->size());
          reset_PlacementIndex();
        }
          
    };
  
  
  /** Register this test class... */
  LAUNCHER (QueryFocus_test, "unit session");
  
  
}}} // namespace mobject::session::test
