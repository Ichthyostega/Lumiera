/*
  Placement  -  Key Abstraction: a way to place and locate a Media Object

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file placement.cpp
 ** Implementation functions belonging to the Placement entity
 */


#include "steam/mobject/placement.hpp"
#include "steam/mobject/explicitplacement.hpp"
#include "steam/mobject/mobject.hpp"
#include "lib/format-string.hpp"

#include <typeinfo>

using util::_Fmt;
using lib::meta::typeStr;

namespace steam {
namespace mobject {


  /** @note we know we need only this single specialisation,
   *        because we define the Placements of more specific 
   *        MObject kinds to be subclasses of Placement<MObject>,
   *        so they will inherit this function.
   *                                           //////////////////////TICKET #439
   */
  ExplicitPlacement
  Placement<MObject>::resolve ()  const 
  { 
    return ExplicitPlacement (*this, chain.resolve()); 
  }
  
  
  
  Placement<MObject>::operator string ()  const 
  {
    return _Fmt{"Placement<%s> %|50T.| use-cnt=%u ID(%016x) adr=%p pointee=%p"}
              % typeStr(this->get()) % use_count()
                                     % (size_t)getID()
                                     % (void*)this
                                     % (void*)get()
                                     ;
  }
  
  
  string
  format_PlacementID (Placement<MObject> const& pla)
  {
    size_t hashVal = pla.getID();
    return _Fmt{"pID(%016x)"} % hashVal;
  }
  
  
  bool
  isSameDef (PlacementMO const& pl1, PlacementMO const& pl2)
  {
    return pl1.getID() == pl2.getID()
        || pl1.chain == pl2.chain
        ;
  }

  
  


}} // namespace steam::mobject
