/*
  SCOPE.hpp  -  nested search scope for properties of placement
 
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
 
*/


#ifndef MOBJECT_SESSION_SCOPE_H
#define MOBJECT_SESSION_SCOPE_H

//#include "proc/mobject/mobject.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/mobject/placement-ref.hpp"
//#include "proc/mobject/session/query-resolver.hpp"  ///////////TODO: really?
#include "lib/iter-adapter.hpp"
#include "lib/error.hpp"
//#include "lib/singleton.hpp"

//#include <boost/operators.hpp>
//#include <boost/scoped_ptr.hpp>
//#include <tr1/memory>
//#include <vector>
//#include <string>

//using std::vector;
//using std::string;

namespace mobject {
namespace session {
  
  using lib::IterAdapter;
  
  LUMIERA_ERROR_DECLARE (INVALID_SCOPE);  ///< Placement scope invalid an not locatable within model
  
  

  /**
   * TODO type comment
   * @note Scope is a passive entity,
   *       basically just wrapping up a Scope-top Placement.
   *       Contrast this to QueryFocus, which actively
   *       maintains the current focus location.
   */
  class Scope
    {
      RefPlacement anchor_;
      
      typedef IterAdapter<PlacementMO*, Scope> IterType_;
      
    public:
      Scope (PlacementMO const& constitutingPlacement);
      Scope (); ///< unlocated NIL scope
      
      Scope (Scope const&);
      Scope& operator= (Scope const&);
      
      static Scope const& containing (PlacementMO const& aPlacement);              //////////////TODO really returning a const& here??
      static Scope const& containing (RefPlacement const& refPlacement);
      
      Scope const& getParent()  const;
      PlacementMO& getTop()  const;
      bool isValid() const;
      bool isRoot()  const;
      
      typedef IterType_ iterator;
      iterator ascend()  const;
      
      friend bool operator== (Scope const&, Scope const&);
      friend bool operator!= (Scope const&, Scope const&);
    };
  
  
///////////////////////////TODO currently just fleshing the API
  
    
  /** as scopes are constituted by a "scope top" element (placement)
   *  registered within the PlacementIndex of the current session,
   *  equality is defined in terms of this defining placement.
   */
  inline bool
  operator== (Scope const& scope1, Scope const& scope2)
  {
    return scope1.anchor_ == scope2.anchor_;  
  }
  
  inline bool
  operator!= (Scope const& scope1, Scope const& scope2)
  {
    return scope1.anchor_ != scope2.anchor_;  
  }
  
  
  
  
}} // namespace mobject::session
#endif
