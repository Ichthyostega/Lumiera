/*
  Placement  -  Key Abstraction: a way to place and locate a Media Object
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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


  /** factory for creating the corretct Placement subclass */
  PlacementFactory Placement::create;


  /** create an actual (explicit) placement while trying to
   * satisfy the network of adjacent objects and placements.
   * TODO: need GC!!!!!!!!!!!
   */
  ExplicitPlacement&
  Placement::resolve ()
  {
  }
  
  
  
  /** implements the logic for selecting the correct 
   *  Placement subclass.
   *  @return smart ptr owning the created placement object
   */
  PlacementFactory::PType 
  PlacementFactory::operator() (Placement::Style, Time, PMO subject)
  {
    UNIMPLEMENTED ("create correct Placement subclass");
  }



} // namespace mobject
