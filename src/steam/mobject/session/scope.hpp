/*
  SCOPE.hpp  -  nested search scope for properties of placement

   Copyright (C)
     2009,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file scope.hpp
 ** @todo WIP implementation of session core from 2010
 ** @todo as of 2016, this effort is considered stalled but basically valid
 */


#ifndef STEAM_MOBJECT_SESSION_SCOPE_H
#define STEAM_MOBJECT_SESSION_SCOPE_H

#include "steam/mobject/placement.hpp"
#include "steam/mobject/placement-ref.hpp"
#include "lib/error.hpp"

#include <string>


namespace lumiera {
namespace error {
  LUMIERA_ERROR_DECLARE (EMPTY_SCOPE_PATH); ///< Placement scope not locatable (empty model path)
  LUMIERA_ERROR_DECLARE (NO_PARENT_SCOPE); ///<  Parent scope of root not accessible
  LUMIERA_ERROR_DECLARE (INVALID_SCOPE);  ///<   Placement scope invalid and not locatable within model
}}

namespace steam {
namespace mobject {
namespace session {
  
  
  
  /**
   * A Placement scope within the high-level-model.
   * Within the Session/Model, Placements are used to attach
   * MObjects; but beyond that, each Placement can \em contain
   * other Placements, effectively forming a scope. Thus Scope
   * is basically another view on Placements <i>which are attached
   * to the session.</i> This (hidden) link to the session is utilised
   * to establish the nesting of scopes and allow querying and navigating.
   * 
   * Actually, Scope is implemented through a PlacementRef pointing to
   * the Placement which \em constitutes this Scope. We call this Placement
   * the "scope top". A fork e.g. can \em contain several clips, but also
   * nested sub forks, all of which would be within the scope of this fork.
   * This scoping relation plays an important role when it comes to \em resolving
   * properties of placement, like e.g. the output designation, overlay mode,
   * sound pan position etc -- properties from enclosing scopes will be 
   * inherited unless \em shaded by local definitions, similar to the
   * behaviour known from most programming languages when referring
   * to local variables. 
   * @note Scope is a passive entity,
   *       basically just wrapping up a Scope-top Placement.
   *       Contrast this to QueryFocus, which actively
   *       maintains the current focus location
   *       and exposes query facilities.
   */
  class Scope
    {
      RefPlacement anchor_;
      
    public:
      Scope (PlacementMO const& constitutingPlacement);
      Scope (); ///< unlocated NIL scope
      
      explicit
      Scope (PlacementMO::ID const& constitutingPlacement);
      
      Scope (Scope const&);
      Scope& operator= (Scope const&);
      
      static const Scope INVALID;
      
      static Scope containing (PlacementMO const& aPlacement);
      static Scope containing (RefPlacement const& refPlacement);
      
      operator std::string() const;
      Scope getParent()      const;
      PlacementMO& getTop()  const;
      bool isValid()         const;
      bool isRoot()          const;
      
      friend bool operator== (Scope const&, Scope const&);
      friend bool operator!= (Scope const&, Scope const&);
    };
  
  
  
  
  
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
  
  
  
  
}}} // namespace steam::mobject::session
#endif
