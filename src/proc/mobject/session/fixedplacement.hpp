/*
  FIXEDPLACEMENT.hpp  -  TODO: didn't we plan to rename ExplicitPlacement to DirectPlacement??  
 
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
 
*/


#ifndef MOBJECT_SESSION_FIXEDPLACEMENT_H
#define MOBJECT_SESSION_FIXEDPLACEMENT_H

#include "proc/mobject/placement.hpp"
#include "proc/mobject/explicitplacement.hpp"



namespace mobject
  {
  namespace session
    {

    // TODO: didn't we plan to rename ExplicitPlacement to DirectPlacement??

    class DirectPlacement : public ExplicitPlacement
      {
        ///////////
      };



  } // namespace mobject::session

} // namespace mobject
#endif
