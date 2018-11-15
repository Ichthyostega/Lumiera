/*
  PlacementScope(Test)  -  accessing and navigating placement scope

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file placement-scope-test.cpp
 ** unit test \ref PlacementScope_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "steam/mobject/mobject.hpp"
#include "steam/mobject/session/scope.hpp"
#include "steam/mobject/session/test-scopes.hpp"
#include "steam/mobject/session/scope-locator.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"


using util::isSameObject;


namespace proc    {
namespace mobject {
namespace session {
namespace test    {
  
  namespace { // Helper to enumerate Contents
             //  of the test-dummy session
    typedef _ScopeIterMO _Iter;
    
    _Iter
    contents_of_testSession (PPIdx testSession)
    {
      return ScopeLocator::instance().query<MObject> (testSession->getRoot());
    }
    
    _Iter
    pathToRoot (PlacementMO& elm)
    {
      Scope startScope(elm);
      return ScopeLocator::instance().getRawPath (startScope);
    }
  }
  
  
  
  
  
  /***********************************************************************//**
   * @test basic behaviour of the nested placement search scopes.
   *       Using a pseudo-session (actually just a PlacementIndex),
   *       this test creates some nested scopes and then...
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
          // Prepare an (test)Session
          // with some dummy contents
          PPIdx index = build_testScopes();
          
          verifyEquality();
          verifyLookup (index);
          verifyNavigation (index);
        }
      
      
      
      
      /** @test for each Placement in our test "session",
       *        find the scope and verify it's in line with the index
       */
      void
      verifyLookup (PPIdx sess)
        {
          for (_Iter ii = contents_of_testSession(sess); ii; ++ii)
            {
              PlacementMO& elm = *ii;
              CHECK (elm.isValid());
              Scope const& scope1 = Scope::containing(elm);
              cout << "Scope: " << scope1 << endl;
              cout << elm << endl;
              
              RefPlacement ref (elm);
              Scope const& scope2 = Scope::containing(ref);
              
              // verify this with the scope registered within the index...
              PlacementMO& scopeTop = sess->getScope(elm);
              CHECK (scope1 == scopeTop);
              CHECK (scope2 == scopeTop);
              CHECK (scope1 == scope2);
              
              CHECK (!isSameObject (scope1,scope2));
            }
        }
      
      
      
      /** @test equality of scopes is based on the ID of the scope top (Placement) */
      void
      verifyEquality ()
        {
          PlacementMO& aPlac = retrieve_startElm();
          Scope scope1(aPlac);
          Scope scope2(aPlac);
          Scope nil;
          
          CHECK (scope1 == scope2); CHECK (scope2 == scope1);
          CHECK (scope1 != nil);    CHECK (nil != scope1);
          CHECK (scope2 != nil);    CHECK (nil != scope2);
          
          CHECK (aPlac == scope1);  CHECK (scope1 == aPlac);
          CHECK (aPlac == scope2);  CHECK (scope2 == aPlac);
          CHECK (aPlac != nil);     CHECK (nil != aPlac);
          
          Scope par (scope1.getParent());
          CHECK (scope1 != par);    CHECK (par != scope1);
          CHECK (scope2 != par);    CHECK (par != scope2);
          
          PlacementMO& placm2 (scope2.getTop());
          CHECK (aPlac.getID() == placm2.getID());
          
          PlacementMO& parPlac (par.getTop());
          CHECK (aPlac.getID() != parPlac.getID());
        }
      
      
      
      /** @test for each element in our test session,
       *        establish the scope and retrieve the path to root,
       *        verifying the parent relationships as we go up.
       *  @note this is the "raw" path, i.e as stored in the
       *        PlacementIndex, as opposed to the effective
       *        path, which might digress for meta-clips
       */
      void
      verifyNavigation (PPIdx sess)
        {
          for (_Iter elm = contents_of_testSession(sess); elm; ++elm)
            {
              _Iter pathIter = pathToRoot(*elm);
              Scope const& enclosing = Scope::containing(*elm);
              CHECK (enclosing == Scope(*elm).getParent());
              CHECK (*pathIter == Scope(*elm));
              
              for ( ; pathIter; ++pathIter)
                {
                  Scope sco(*pathIter);
                  if (sco.isRoot())
                    {
                      VERIFY_ERROR (NO_PARENT_SCOPE, sco.getParent() );
                      PlacementMO& top = sco.getTop();
                      PlacementMO& root = sess->getRoot();
                      
                      CHECK (isSameObject (top,root));
                    }
                  else
                    {
                      Scope parent = sco.getParent();
                      PlacementMO& top = sco.getTop();
                      Scope parentsScope = Scope::containing(top);
                      PlacementMO& topsTop = sess->getScope(top);   ///////////////////TODO impact of Binding a Sequence? see Ticket #311
                      CHECK (topsTop == parentsScope);
                      CHECK (isSameObject (topsTop, parentsScope.getTop()));
                    }}}
        }
      
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (PlacementScope_test, "function session");
  
  
}}}} // namespace proc::mobject::session::test
