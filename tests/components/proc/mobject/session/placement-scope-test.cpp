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
#include "lib/test/test-helper.hpp"
#include "proc/mobject/mobject.hpp"
#include "proc/mobject/session/scope.hpp"
#include "proc/mobject/session/test-scopes.hpp"
#include "proc/mobject/session/scope-locator.hpp"
#include "lib/util.hpp"

#include <iostream>

using util::isSameObject;


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
  
  
  
  
  
  /***************************************************************************
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
              ASSERT (elm.isValid());
              Scope const& scope1 = Scope::containing(elm);
              std::cout << "Scope: " << string(scope1) << std::endl;
              std::cout << string(elm) << std::endl;
              
              RefPlacement ref (elm);
              Scope const& scope2 = Scope::containing(ref);
              
              // verify this with the scope registered within the index...
              PlacementMO& scopeTop = sess->getScope(elm);
              ASSERT (scope1 == scopeTop);
              ASSERT (scope2 == scopeTop);
              ASSERT (scope1 == scope2);
              
              ASSERT (!isSameObject (scope1,scope2));
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
          
          ASSERT (scope1 == scope2); ASSERT (scope2 == scope1);
          ASSERT (scope1 != nil);    ASSERT (nil != scope1);
          ASSERT (scope2 != nil);    ASSERT (nil != scope2);
          
          ASSERT (aPlac == scope1);  ASSERT (scope1 == aPlac);
          ASSERT (aPlac == scope2);  ASSERT (scope2 == aPlac);
          ASSERT (aPlac != nil);     ASSERT (nil != aPlac);
          
          Scope par (scope1.getParent());
          ASSERT (scope1 != par);    ASSERT (par != scope1);
          ASSERT (scope2 != par);    ASSERT (par != scope2);
          
          PlacementMO& placm2 (scope2.getTop());
          ASSERT (aPlac.getID() == placm2.getID());
          
          PlacementMO& parPlac (par.getTop());
          ASSERT (aPlac.getID() != parPlac.getID());
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
              ASSERT (enclosing == Scope(*elm).getParent());
              ASSERT (*pathIter == Scope(*elm));
              
              for ( ; pathIter; ++pathIter)
                {
                  Scope sco(*pathIter);
                  if (sco.isRoot())
                    {
                      VERIFY_ERROR (NO_PARENT_SCOPE, sco.getParent() );
                      PlacementMO& top = sco.getTop();
                      PlacementMO& root = sess->getRoot();
                      
                      ASSERT (isSameObject (top,root));
                    }
                  else
                    {
                      Scope parent = sco.getParent();
                      PlacementMO& top = sco.getTop();
                      Scope parentsScope = Scope::containing(top);
                      PlacementMO& topsTop = sess->getScope(top);   ///////////////////TODO impact of Binding a Sequence? see Ticket #311
                      ASSERT (topsTop == parentsScope);
                      ASSERT (isSameObject (topsTop, parentsScope.getTop()));
                    }}}
        }
      
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (PlacementScope_test, "function session");
  
  
}}} // namespace mobject::session::test
