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
#include "lib/test/test-helper.hpp"
#include "proc/mobject/session/test-scopes.hpp"
#include "proc/mobject/session/placement-index.hpp"
#include "proc/mobject/session/scope-path.hpp"
#include "lib/util.hpp"



namespace mobject {
namespace session {
namespace test    {
  
  using util::isnil;
  using util::isSameObject;
  
  using lumiera::error::LUMIERA_ERROR_LOGIC;
  using lumiera::error::LUMIERA_ERROR_INVALID;
  
  
  namespace { // subversive test helper...
    
    Scope const&
    fabricate_invalidScope()
    {        /** 
              * assumed to have identical memory layout
              * to a Scope object, as the latter is implemented
              * by a PlacementRef, which in turn is just an
              * encapsulated Placement-ID
              */
             struct Ambush
               {
                 /** random ID assumed to be
                  *  nowhere in the model */
                 PlacementMO::ID derailed_;
               };
      
      static Ambush _kinky_;
      return *reinterpret_cast<Scope*> (&_kinky_);
    }
  }
  
  
  
  
  
  /***************************************************************************
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
          ASSERT (startPlacement.isValid());
          
          checkInvalidScopeDetection();
          ScopePath testPath = buildPath (startPlacement);
          checkRelations (testPath,startPlacement);
          invalidPath (testPath,startPlacement);
          rootPath (testPath);
          check_Identity_and_Copy (startPlacement);
          check_RefcountProtection (startPlacement);
          navigate (testPath, index);
          clear (testPath, index);
                                  ////////////////////////////////////////TICKET #429 : verify diagnostic output (to be added later)
        }
      
      
      ScopePath
      buildPath (PMO& startPla)
        {
          Scope startScope (startPla);
          ScopePath path  (startScope);
          ScopePath path2 (startScope);
          ScopePath path3 (path2);
          
          ASSERT (path);
          ASSERT (path.contains (startScope));
          ASSERT ( path.getLeaf() == path2.getLeaf());
          ASSERT (path2.getLeaf() == path3.getLeaf());
          
          return path;
        }
      
      
      void
      checkInvalidScopeDetection()
        {
          // verify detection of illegal scopes and paths...
          TestPlacement<> notRelated2anything (*new DummyMO);
          VERIFY_ERROR (NOT_IN_SESSION, Scope invalid (notRelated2anything) );
          
          Scope const& scopeOfEvil = fabricate_invalidScope();
          REQUIRE (!scopeOfEvil.isValid());
          
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
          ASSERT (ii);
          while (++ii)
            {
              ASSERT (*ii == refScope.getParent());
              refScope = *ii;
            }
        }
      
      
      void
      checkRelations (ScopePath path1, PMO& refPlacement)
        {
          ASSERT (path1.contains (refPlacement));
          
          Scope refScope (refPlacement);
          ASSERT (path1.contains (refScope));
          ASSERT (path1.endsAt (refScope));
          
          ScopePath path2 (refScope);
          ASSERT (path2.contains (refScope));
          ASSERT (path2.endsAt (refScope));
          
          ASSERT (path1 == path2);
          ASSERT (!isSameObject (path1,path2));
          
          Scope parent = path2.moveUp();
          ASSERT (path2.endsAt (parent));
          ASSERT (path1.endsAt (refScope));
          ASSERT (parent == refScope.getParent());
          ASSERT (path1 != path2);
          ASSERT (path2 != path1);
          ASSERT (path1.contains (path2)); ////////////////////TODO: not clear if we really need to implement those relations
          ASSERT (!disjoint(path1,path2));
          ASSERT (path2 == commonPrefix(path1,path2));
          ASSERT (path2 == commonPrefix(path2,path1));
          ASSERT (path1 != commonPrefix(path1,path2));
          ASSERT (path1 != commonPrefix(path2,path1));
        }
      
      
      void
      rootPath (ScopePath refPath)
        {
          ASSERT ( refPath);
          refPath.goRoot();
          ASSERT (!refPath);
          ASSERT (!refPath.empty());
          ASSERT (!refPath.isValid());
          ASSERT (1 == refPath.length());
          
          ScopePath defaultPath;
          ASSERT (!defaultPath);
          ASSERT (refPath == defaultPath);
        }
      
      
      void
      invalidPath (ScopePath refPath, PMO& refPlacement)
        {
          ASSERT (refPath);
          ASSERT (!ScopePath::INVALID);
          ASSERT (isnil (ScopePath::INVALID));
          
          ScopePath invalidP (ScopePath::INVALID);
          ASSERT (isnil (invalidP));
          ASSERT (invalidP == ScopePath::INVALID);
          ASSERT (!isSameObject (invalidP, ScopePath::INVALID));
          
          ASSERT (refPath.contains (refPlacement));
          ASSERT (!invalidP.contains (refPlacement));
          
          Scope refScope (refPlacement);
          ASSERT (!invalidP.contains (refScope));
          VERIFY_ERROR (EMPTY_SCOPE_PATH, invalidP.endsAt (refScope) ); // Logic: can't inspect the end of nothing
          
          ASSERT (refPath.contains (invalidP));            // If the moon is made of green cheese, I'll eat my hat!
          ASSERT (!invalidP.contains (refPath));
          ASSERT (invalidP == commonPrefix(refPath,invalidP));
          ASSERT (invalidP == commonPrefix(invalidP,refPath));
          
          VERIFY_ERROR (EMPTY_SCOPE_PATH, invalidP.moveUp() );
          Scope root = refPath.goRoot();
          ASSERT (1 == refPath.length());
          
          Scope const& nil = refPath.moveUp();
          ASSERT (refPath.empty());
          ASSERT (!nil.isValid());
          ASSERT (refPath == invalidP);
          ASSERT (invalidP.contains (nil));
          ASSERT (invalidP.contains (refPath));
          ASSERT (!invalidP.contains (refScope));
          
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
          
          ASSERT (path1.contains (startScope));
          ASSERT (path2.contains (startScope));
          ASSERT (path3.contains (startScope));
          
          ASSERT (path1 == path2);
          ASSERT (path2 == path3);
          ASSERT (path1 == path3);
          ASSERT (!isSameObject (path1,path2));
          ASSERT (!isSameObject (path2,path3));
          ASSERT (!isSameObject (path1,path3));
          
          Scope parent = path3.moveUp();
          ASSERT (path1 == path2);
          ASSERT (path2 != path3);
          ASSERT (path1 != path3);
          
          path2 = path3;
          ASSERT (path1 != path2);
          ASSERT (path2 == path3);
          ASSERT (path1 != path3);
          
          path2 = ScopePath::INVALID;
          ASSERT (path1 != path2);
          ASSERT (path2 != path3);
          ASSERT (path1 != path3);
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
          ScopePath path (refPath);
          ASSERT (path == refPath);
          
          Scope leaf = path.getLeaf();
          Scope parent = path.moveUp();
          ASSERT (path != refPath);
          ASSERT (refPath.contains (path));
          ASSERT (refPath.endsAt (leaf));
          ASSERT (path.endsAt (parent));
          ASSERT (parent == leaf.getParent());
          ASSERT (parent == path.getLeaf());
          
          Scope root = path.goRoot();
          ASSERT (path != refPath);
          ASSERT (path.endsAt (root));
          ASSERT (refPath.contains (path));
          ASSERT (!path.endsAt (parent));
          ASSERT (!path.endsAt (leaf));
          
          path.navigate (parent);
          ASSERT (path.endsAt (parent));
          ASSERT (!path.endsAt (root));
          ASSERT (!path.endsAt (leaf));
          
          TestPlacement<> newNode (*new DummyMO);
          PMO& parentRefPoint = parent.getTop();
          Scope newLocation = 
              index->find(      // place newNode as sibling of "leaf"
                  index->insert (newNode, parentRefPoint));
          path.navigate (newLocation);
          Scope sibling = path.getLeaf();
          ASSERT (sibling == newLocation);
          ASSERT (parent == sibling.getParent());
          ASSERT (path.endsAt (sibling));
          ASSERT (path.contains (parent));
          ASSERT (path.contains (root));
          ASSERT (!refPath.contains (path));
          ASSERT (!path.contains (refPath));
          ASSERT (!disjoint (path,refPath));
          ASSERT (!disjoint (refPath,path));
          
          ScopePath prefix = commonPrefix (path,refPath);
          ASSERT (prefix == commonPrefix (refPath,path));
          ASSERT (prefix.endsAt (parent));
          ASSERT (!prefix.contains (leaf));
          ASSERT (!prefix.contains (sibling));
          path.navigate (prefix.getLeaf());
          ASSERT (path == prefix);
          
          // try to navigate to an unconnected location...
          ScopePath beforeInvalidNavigation = path;
          Scope const& unrelatedScope (fabricate_invalidScope());
          VERIFY_ERROR (INVALID_SCOPE, path.navigate (unrelatedScope) );
          ASSERT (path == beforeInvalidNavigation); // not messed up by the incident
          
          // now explore a completely separate branch....
          PMO& separatePlacement = *explore_testScope (
                                     *explore_testScope (
                                       *explore_testScope (
                                         root.getTop())));
          path.navigate (separatePlacement);
          ASSERT (path);
          ASSERT (disjoint (path,refPath));
          ASSERT (path.contains(separatePlacement));
          Scope other = path.getLeaf();
          ASSERT (isSameObject (other.getTop(), separatePlacement));
          ScopePath rootPrefix = commonPrefix (path,refPath);
          ASSERT (rootPrefix.endsAt (root));
          
        }
      
      
      
      void
      clear (ScopePath& path, PPIdx index)
        {
          ASSERT (path);
          PMO& rootNode = index->getRoot();
          ASSERT (path.getLeaf() != rootNode);
          
          path.clear();
          ASSERT (!path);
          ASSERT (!isnil (path));
          ASSERT (path.getLeaf() == rootNode);
        }
    };
  
  
  
  /** Register this test class... */
  LAUNCHER (ScopePath_test, "unit session");
  
  
}}} // namespace mobject::session::test
