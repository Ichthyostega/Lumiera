/*
  ScopePath  -  logical access path down from Session root
 
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


#include "proc/mobject/session/scope-path.hpp"
#include "lib/error.hpp"
#include "lib/util.hpp"
//#include "proc/mobject/session/track.hpp"

//#include "proc/mobject/placement.hpp"
//#include "proc/mobject/session/mobjectfactory.hpp"
//#include "proc/asset/track.hpp"
#include <tr1/functional>

namespace mobject {
namespace session {
  
  using std::tr1::bind;
  using std::tr1::function;
  using std::tr1::placeholders::_1;
  using util::and_all;
  
  using namespace lumiera;
  
  
  /** by default, a scope path just contains 
   *  the root scope of the current session (PlacementIndex).
   *  @note invoking this function accesses the session and thus
   *        may cause an empty default session to be created.
   */
  ScopePath::ScopePath ()
  {
    UNIMPLEMENTED ("default path just containing root");
  }
  
  
  /** When creating a path to a given (leaf) scope,
   *  the complete sequence of nested scopes leading to 
   *  this special scope is discovered, using the query service
   *  exposed by the session (through ScopeLocator).
   *  @note when locating the default (invalid) scope,
   *         a special empty ScopePath is created
   *  @throw error::Invalid if the given target scope
   *         can't be connected to the (implicit) root
   */
  ScopePath::ScopePath (Scope const& leaf)
  {
    UNIMPLEMENTED ("initialise by discovering complete scope sequence");
  }
  
  
  /** constant \em invalid path token. Created by locating an invalid scope */
  const ScopePath ScopePath::INVALID = ScopePath(Scope());
  
  
  /* == Diagnostics == */
  
  /** a \em valid path consists of more than just the root element.
   *  @note contrary to this, an \em empty path doesn't even contain a root element
   */
  bool
  ScopePath::isValid()  const
  {
    UNIMPLEMENTED ("validity self check: more than just root");
  }
  
  /** an empty path doesn't even contain a root element.
   *  Many operations throw when invoked on such a path.
   *  Navigating up from an root path creates an empty path.
   */
  bool
  ScopePath::empty()  const
  {
    UNIMPLEMENTED ("empty == no elements, even no root!");
  }
  
  
  
  /* == Relations == */
  
  Scope&
  ScopePath::getLeaf()  const
  {
    UNIMPLEMENTED ("access end node of current path");
  }
  
  
  /** verify the scope in question is equivalent
   *  to our leaf scope. Equivalence of scopes means
   *  they are defined by the same scope top placement,
   *  i.e. registered with the same Placement-ID. 
   */
  bool
  ScopePath::endsAt(Scope const& aScope)  const
  {
    UNIMPLEMENTED ("verify the scope in question is identical (same ID) to our leaf scope");
  }
  
  
  bool
  ScopePath::contains (Scope const& aScope)  const
  {
    UNIMPLEMENTED ("containment check");
  }
  
  
  bool
  ScopePath::contains (ScopePath const& otherPath)  const
  {
    if ( empty())             return false;
    if (!otherPath.isValid()) return true;
    if (!isValid())           return false;
    
    ASSERT (1 < length());
    ASSERT (1 < otherPath.length());
    
    for (iterator ii = otherPath.begin(); ii; ++ii)
      if (!this->contains (*ii))
        return false;
    
    return true;
  }
  
  
  ScopePath
  commonPrefix (ScopePath const& path1, ScopePath const& path2)
  {
    UNIMPLEMENTED ("determine the common prefix, if any");
  }
  
  bool
  disjoint (ScopePath const& path1, ScopePath const& path2)
  {
    if (path1.empty() || path2.empty()) return false;

    return (path1.isValid() && path2.isValid())
        && (path1.path_[1]  != path2.path_[1]) // no common prefix
         ; 
  }


  

  /* == Mutations == */
  
  void
  ScopePath::clear()
  {
    UNIMPLEMENTED ("reset the current path to default state (just root)");
  }
  
  
  Scope&
  ScopePath::moveUp()
  {
    UNIMPLEMENTED ("navigate one level up, then return leaf");
  }
  
  
  Scope&
  ScopePath::goRoot()
  {
    UNIMPLEMENTED ("navigate up to the root scope");
  }
  
  
  void
  ScopePath::navigate (Scope const& target)
  {
    *this = ScopePath(target);             //////////////////////////////TICKET #424
  }
  
  
  
  
}} // namespace mobject::session
