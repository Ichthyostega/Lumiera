/*
  EXPLICITPLACEMENT.hpp  -  special Placement yielding an absolute location (Time,Track)-location for a MObject
 
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


#ifndef MOBJECT_EXPLICITPLACEMENT_H
#define MOBJECT_EXPLICITPLACEMENT_H

#include "proc/mobject/placement.hpp"



namespace mobject
  {

  namespace session{ class Track; }


  // TODO: need Garbage Collector for this class!!!!!

  class ExplicitPlacement : public Placement
    {
    protected:

      Time time;
      Track* track;

    };



} // namespace mobject
#endif
