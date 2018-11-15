/*
  ScopePath(Test)  -  verify handling of logical access path down from Session root

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

/** @file scope-path-test.cpp
 ** unit test \ref ScopePath_test
 */


#include "lib/test/run.hpp"
#include "lib/test/test-helper.hpp"
#include "proc/mobject/session/test-scopes.hpp"
#include "proc/mobject/session/placement-index.hpp"
#include "proc/mobject/session/scope-path.hpp"
#include "proc/mobject/session/test-scope-invalid.hpp"
#include "lib/format-cout.hpp"
#include "lib/util.hpp"

#include <string>


namespace proc    {
namespace mobject {
namespace session {
namespace test    {
  
  using std::string;
  using util::isnil;
  using util::isSameObject;
  
  using lumiera::error::LERR_(LOGIC);
  using lumiera::error::LERR_(INVALID);
  
  
  
  
  /***********************************************************************//**
   * @test properties and behaviour of the path of nested scopes.
   *       Using a pseudo-session (actually just a PlacementIndex),
   *       this test  creates some nested scopes, builds scope paths
   *       and executes various comparisons navigation moves on them.
   *       Especially detection of invalid scopes and paths and the
   *       special handling of empty and root paths is covered.
   * @see  mobject::Placement
   * @see  mobject::session::ScopePath
   * @see  mobject::session::QueryFocus
   */
  class ScopePath_test : public Test
    {
      
      virtual void
      run (Arg) 
        {
          // Prepare an (test)Index backing the PlacementRefs
          PPIdx index = build_testScopes();
          PMO& startPlacement = retrieve_startElm();
          CHECK (startPlacement.isValid());
          
          checkInvalidScopeDetection();
          ScopePath testPath = buildPath (startPlacement);
          checkRelations (testPath,startPlacement);
          invalidPath (testPath,startPlacement);
          rootPath (testPath);
          check_Identity_and_Copy (startPlacement);
          check_RefcountProtection (startPlacement);
          navigate (testPath, index);
          clear (testPath, index);
        }
      
      
      ScopePath
      buildPath (PMO& startPla)
        {
          Scope startScope (startPla);
          ScopePath path  (startScope);
          ScopePath path2 (startScope);
          ScopePath path3 (path2);
          
          CHECK (path);
          CHECK (path.contains (startScope));
          CHECK ( path.getLeaf() == path2.getLeaf());
          CHECK (path2.getLeaf() == path3.getLeaf());
          
          return path;
        }
      
      
      void
      checkInvalidScopeDetection()
        {
          // verify detection of illegal scopes and paths...
          TestPlacement<> notRelated2anything (*new DummyMO);
          VERIFY_ERROR (NOT_IN_SESSION, Scope invalid (notRelated2anything) );
          
          Scope const& scopeOfEvil = fabricate_invalidScope();
          CHECK (!scopeOfEvil.isValid());
          
          VERIFY_ERROR (INVALID_SCOPE, ScopePath outsideCurrentModel (scopeOfEvil) );
          
          // but there is one exception to this rule...
          ScopePath theInvalidToken (Scope::INVALID);
          CHECK (!theInvalidToken.isValid());
          CHECK (theInvalidToken.empty());
        }
      
      
      
      void
      checkIteration (ScopePath path, PMO& refPlacement)
        {
          Scope refScope(refPlacement);
          ScopePath::iterator ii = path.begin();
          CHECK (ii);
          while (++ii)
            {
              CHECK (*ii == refScope.getParent());
              refScope = *ii;
            }
        }
      
      
      void
      checkRelations (ScopePath path1, PMO& refPlacement)
        {
          CHECK (path1.contains (refPlacement));
          
          Scope refScope (refPlacement);
          CHECK (path1.contains (refScope));
          CHECK (path1.endsAt (refScope));
          
          ScopePath path2 (refScope);
          CHECK (path2.contains (refScope));
          CHECK (path2.endsAt (refScope));
          
          CHECK (path1 == path2);
          CHECK (!isSameObject (path1,path2));
          
          Scope parent = path2.moveUp();
          CHECK (path2.endsAt (parent));
          CHECK (path1.endsAt (refScope));
          CHECK (parent == refScope.getParent());
          CHECK (path1 != path2);
          CHECK (path2 != path1);
          CHECK (path1.contains (path2));
          CHECK (!disjoint(path1,path2));
          CHECK (path2 == commonPrefix(path1,path2));
          CHECK (path2 == commonPrefix(path2,path1));
          CHECK (path1 != commonPrefix(path1,path2));
          CHECK (path1 != commonPrefix(path2,path1));
        }
      
      
      void
      rootPath (ScopePath refPath)
        {
          CHECK ( refPath);
          refPath.goRoot();
          CHECK (!refPath);
          CHECK (!refPath.empty());
          CHECK (!refPath.isValid());
          CHECK (1 == refPath.length());
          
          ScopePath defaultPath;
          CHECK (!defaultPath);
          CHECK (refPath == defaultPath);
        }
      
      
      void
      invalidPath (ScopePath refPath, PMO& refPlacement)
        {
          CHECK (refPath);
          CHECK (!ScopePath::INVALID);
          CHECK (isnil (ScopePath::INVALID));
          CHECK ("!" == string(ScopePath::INVALID));
          
          ScopePath invalidP (ScopePath::INVALID);
          CHECK (isnil (invalidP));
          CHECK (invalidP == ScopePath::INVALID);
          CHECK (!isSameObject (invalidP, ScopePath::INVALID));
          
          CHECK (refPath.contains (refPlacement));
          CHECK (!invalidP.contains (refPlacement));
          
          Scope refScope (refPlacement);
          CHECK (!invalidP.contains (refScope));
          VERIFY_ERROR (EMPTY_SCOPE_PATH, invalidP.endsAt (refScope) ); // Logic: can't inspect the end of nothing
          
          CHECK (refPath.contains (invalidP));            // If the moon is made of green cheese, I'll eat my hat!
          CHECK (!invalidP.contains (refPath));
          CHECK (invalidP == commonPrefix(refPath,invalidP));
          CHECK (invalidP == commonPrefix(invalidP,refPath));
          
          VERIFY_ERROR (EMPTY_SCOPE_PATH, invalidP.moveUp() );
          Scope root = refPath.goRoot();
          CHECK (1 == refPath.length());
          
          Scope const& nil = refPath.moveUp();
          CHECK (refPath.empty());
          CHECK (!nil.isValid());
          CHECK (refPath == invalidP);
          CHECK (invalidP.contains (nil));
          CHECK (invalidP.contains (refPath));
          CHECK (!invalidP.contains (refScope));
          
          VERIFY_ERROR (EMPTY_SCOPE_PATH, refPath.navigate(root) );
          
        //ScopePath::INVALID.navigate(root);  // doesn't compile: INVALID is immutable
        }
      
      
      void
      check_Identity_and_Copy (PMO& refPlacement)
        {
          Scope startScope (refPlacement);
          ScopePath path1 (startScope);
          ScopePath path2 (startScope);
          ScopePath path3 (path2);
          
          CHECK (path1.contains (startScope));
          CHECK (path2.contains (startScope));
          CHECK (path3.contains (startScope));
          
          CHECK (path1 == path2);
          CHECK (path2 == path3);
          CHECK (path1 == path3);
          CHECK (!isSameObject (path1,path2));
          CHECK (!isSameObject (path2,path3));
          CHECK (!isSameObject (path1,path3));
          
          Scope parent = path3.moveUp(); // mutation
          CHECK (parent == path2.getLeaf().getParent());
          
          CHECK (path1 == path2); // the others are not affected
          CHECK (path2 != path3);
          CHECK (path1 != path3);
          
          path2 = path3;
          CHECK (path1 != path2);
          CHECK (path2 == path3);
          CHECK (path1 != path3);
          
          path2 = ScopePath::INVALID;
          CHECK (path1 != path2);
          CHECK (path2 != path3);
          CHECK (path1 != path3);
        }
      
      
      /** @test the embedded refcount is handled sensibly
       *        when it comes to copying. (This refcount
       *        is used by QueryFocusStack) */
      void
      check_RefcountProtection (PMO& refPlacement)
        {
          Scope startScope (refPlacement);
          ScopePath path1 (startScope);
          ScopePath path2 (path1);
          
          path1 = path2;
          CHECK (!isSameObject (path1,path2));
          CHECK (0 == path1.ref_count());
          CHECK (0 == path2.ref_count());
          
          intrusive_ptr_add_ref (&path2);
          CHECK (0 == path1.ref_count());
          CHECK (0 <  path2.ref_count());
          
          ScopePath path3 (path2);
          CHECK (0 == path3.ref_count());  // refcount not copied
          
          path3.moveUp();
          
          VERIFY_ERROR (LOGIC, path2 = path3 ); // overwriting of path with refcount is prohibited
          CHECK (path1 != path3);
          path1 = path2;                        // but path without refcount may be overwritten
          path1 = path3;
          CHECK (path1 == path3);
                    
          intrusive_ptr_release (&path2);       // refcount drops to zero...
          CHECK (0 == path1.ref_count());
          CHECK (0 == path2.ref_count());
        }
      
      
      
      /** @test modify a path by \em navigating it.
       *  - move one step above the current leaf
       *  - move up to the root element
       *  - move back to the parent and verify we're just above the leaf
       *  - attach a new sibling node and move the path down to there
       *  - extract the common prefix, which should again point to the parent
       *  - find a placement in a completely separate branch (only sharing the
       *    root node). Navigate to there and verify root is the common prefix.
       */
      void
      navigate (const ScopePath refPath, PPIdx index)
        {
          #define __SHOWPATH(N) cout << "Step("<<N<<"): "<< path << endl;
          
          ScopePath path (refPath);               __SHOWPATH(1)
          CHECK (path == refPath);
          
          Scope leaf = path.getLeaf();
          Scope parent = path.moveUp();           __SHOWPATH(2)
          CHECK (path != refPath);
          CHECK (refPath.contains (path));
          CHECK (refPath.endsAt (leaf));
          CHECK (path.endsAt (parent));
          CHECK (parent == leaf.getParent());
          CHECK (parent == path.getLeaf());
          
          Scope root = path.goRoot();             __SHOWPATH(3)
          CHECK (path != refPath);
          CHECK (path.endsAt (root));
          CHECK (refPath.contains (path));
          CHECK (!path.endsAt (parent));
          CHECK (!path.endsAt (leaf));
          
          path.navigate (parent);                 __SHOWPATH(4)
          CHECK (path.endsAt (parent));
          CHECK (!path.endsAt (root));
          CHECK (!path.endsAt (leaf));
          
          TestPlacement<> newNode (*new DummyMO);
          PMO& parentRefPoint = parent.getTop();
          Scope newLocation = 
              index->find(      // place newNode as sibling of "leaf"
                  index->insert (newNode, parentRefPoint));
          path.navigate (newLocation);            __SHOWPATH(5)
          Scope sibling = path.getLeaf();
          CHECK (sibling == newLocation);
          CHECK (parent == sibling.getParent());
          CHECK (path.endsAt (sibling));
          CHECK (path.contains (parent));
          CHECK (path.contains (root));
          CHECK (!refPath.contains (path));
          CHECK (!path.contains (refPath));
          CHECK (!disjoint (path,refPath));
          CHECK (!disjoint (refPath,path));
          
          ScopePath prefix = commonPrefix (path,refPath);
          CHECK (prefix == commonPrefix (refPath,path));
          CHECK (prefix.endsAt (parent));
          CHECK (!prefix.contains (leaf));
          CHECK (!prefix.contains (sibling));
          path.navigate (prefix.getLeaf());       __SHOWPATH(6)
          CHECK (path == prefix);
          
          // try to navigate to an unconnected location...
          ScopePath beforeInvalidNavigation = path;
          Scope const& unrelatedScope (fabricate_invalidScope());
          VERIFY_ERROR (INVALID_SCOPE, path.navigate (unrelatedScope) );
          CHECK (path == beforeInvalidNavigation); // not messed up by the incident
          
          // now explore a completely separate branch....
          PMO& separatePlacement = *explore_testScope (
                                     *explore_testScope (
                                       retrieve_firstTestSubMO21()));
          path.navigate (separatePlacement);
          CHECK (path);
          CHECK (disjoint (path,refPath));
          CHECK (path.contains(separatePlacement));
          Scope other = path.getLeaf();
          CHECK (isSameObject (other.getTop(), separatePlacement));
          ScopePath rootPrefix = commonPrefix (path,refPath);
          CHECK (rootPrefix.endsAt (root));
          
        }
      
      
      
      void
      clear (ScopePath& path, PPIdx index)
        {
          CHECK (path);
          PMO& rootNode = index->getRoot();
          CHECK (path.getLeaf() != rootNode);
          
          path.clear();
          CHECK (!path);
          CHECK (!isnil (path));
          CHECK (path.getLeaf() == rootNode);
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (ScopePath_test, "unit session");
  
  
}}}} // namespace proc::mobject::session::test
