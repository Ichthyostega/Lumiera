/*
  Placement  -  Key Abstraction: a way to place and locate a Media Object
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "proc/mobject/placement.hpp"
#include "proc/mobject/explicitplacement.hpp"

namespace mobject
  {


  /** @note we know we need only this single
   *        specialisation, because we define 
   *        the Placements of more specific 
   *        MObject kinds to be subclasses
   *        of Placement<MObject>, so they
   *        will inherit this function.
   */
  template<>
  ExplicitPlacement
  Placement<MObject>::resolve ()  const 
    { 
      return ExplicitPlacement (*this, chain.resolve()); 
    }



} // namespace mobject
