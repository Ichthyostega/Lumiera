/*
  SessionElementQuery(Test)  -  querying and retrieving elements from the session 

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file session-element-query-test.cpp
 ** unit test \ref SessionElementQuery_test
 */


#include "lib/test/run.hpp"
#include "steam/mobject/session/test-scopes.hpp"
#include "steam/mobject/session/element-query.hpp"
#include "steam/mobject/session/scope-path.hpp"
#include "steam/mobject/session/scope.hpp"
#include "steam/mobject/mobject-ref.hpp"
#include "lib/util.hpp"

#include <functional>
#include <string>



namespace steam    {
namespace mobject {
namespace session {
namespace test    {
  
  using std::placeholders::_1;
  using std::function;
  using std::bind;
  using std::string;
  
  using util::cStr;
  using util::contains;
  
  
  namespace { // helpers and shortcuts....
    
    typedef Placement<DummyMO> const& PDummy;  // note const& required by ElementQuery filter definition
    
    
    /** a filter predicate to pick some objects from a resultset,
     *  based on string match with the element's self-display string.
     *  @note the query system allows us to use the specific API of DummyMO,
     *        without the need for any cast. It is sufficient to declare
     *        a suitable signature on the query predicate! */
    inline function<bool(PDummy)>
    elementID_contains (string expectedText)
    {
      return [=] (PDummy candidate)
        {
          REQUIRE (candidate.isValid());
          string desc(candidate->operator string());
          return contains(desc, expectedText);
        };
    }
    
  }
  
  
  
  /******************************************************************************************//**
   * @test cover the part of the session API allowing to retrieve specific elements by query.
   *       - This test first picks an object from the test session, where the filter predicate
   *         utilises the specific MObject subclass (here DummyMO).
   *       - Then re-fetches the same object using a different filter
   *         (based on the specific random int-ID).
   *       - Next the element is removed from the test session to verify the "not found" result
   *       - finally we re-attach another placement of the same underlying MObject instance
   *         at a different location in the test session and verify we can again pick this
   *         element with the specific query.
   *       
   * @see  mobject::session::ElementQuery
   * @see  mobject::session::ContentsQuery
   * @see  scope-query-test.cpp
   */
  class SessionElementQuery_test : public Test
    {
      virtual void
      run (Arg) 
        {
          // Prepare an (test)Index (dummy "session")
          PPIdx testSession (build_testScopes());
          
          ElementQuery queryAPI;
          
          MORef<DummyMO> dummy1 = queryAPI.pick (elementID_contains("MO2"));
          CHECK (dummy1);
          CHECK (dummy1->isValid());
          INFO  (test, "Location in Tree: %s", cStr(ScopePath(dummy1.getPlacement())));
          
          string elementID = dummy1->operator string();
          CHECK (contains (elementID, "MO2"));
          
          string specificID = elementID.substr(10);   // should contain the random int-ID
          MORef<DummyMO> dummy2;
          CHECK (!dummy2);
          dummy2 = queryAPI.pick (elementID_contains(specificID));
          CHECK (dummy2);                         // found the same object again
          CHECK (dummy2->isValid());
          CHECK (dummy2 == dummy1);
          
          
          // put aside a new handle holding onto the MObject
          PDum newPlacement(dummy1.getPlacement());
          CHECK (testSession->contains(dummy1));
          CHECK (!testSession->contains(newPlacement));
          
          // and now remove the placement and all contained elements
          testSession->clear (dummy1);
          CHECK (!testSession->contains(dummy1));
          
          MORef<DummyMO> findAgain = queryAPI.pick (elementID_contains(specificID));
          CHECK (!findAgain);     // empty result because searched element was removed from session...
          
          MORef<DummyMO> otherElm = queryAPI.pick (elementID_contains("MO21"));
          CHECK (otherElm);    // now pick just some other arbitrary element 
          
          testSession->insert(newPlacement, otherElm);
          dummy2 = queryAPI.pick (elementID_contains(specificID));
          CHECK (dummy2);
          CHECK (dummy2 != dummy1);
          CHECK (dummy2 != newPlacement);
          CHECK (isSharedPointee(newPlacement, dummy2.getPlacement()));
          CHECK (Scope::containing (dummy2.getRef()) == Scope (otherElm));
          INFO  (test, "New treelocation: %s", cStr(ScopePath(dummy2.getPlacement())));
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (SessionElementQuery_test, "function session");
  
  
}}}} // namespace steam::mobject::session::test
