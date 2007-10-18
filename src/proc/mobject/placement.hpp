/*
  PLACEMENT.hpp  -  Key Abstraction: a way to place and locate a Media Object
 
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


/** @file placement.hpp 
 ** Placements are at the very core of all editing operations,
 ** because they act as handles (smart pointers) to access the media objects
 ** to be manipulated. Moreover, Placements are the actual content of the EDL(s)
 ** and Fixture and thus are small objects with value semantics. Many editing tasks
 ** include finding some Placement in the EDL or directly take a ref to some Placement.
 ** 
 ** Placements are <b>smart pointers</b>: By acting on the Placement object, we can change
 ** parameters of the way the media object is placed (e.g. adjust an offset), while by 
 ** dereferencing the Placement object, we access the "real" media object (e.g. for trimming).
 ** 
 ** Placements are templated on the type of the actual MObject they refer to, so, sometimes
 ** we rather use a Placement<Clip> to be able to use the more specific methods of the
 ** session::Clip interface. But <i>please note the following detail:</i> this type
 ** labeling is the <i>only</i> difference between this subclasses, besides that,
 ** they can be replaced by one another (no slicing).
 **
 ** @see ExplicitPlacement
 **
 */



#ifndef MOBJECT_PLACEMENT_H
#define MOBJECT_PLACEMENT_H

#include "common/time.hpp"
#include "common/factory.hpp"
#include "proc/mobject/mobject.hpp"
#include "proc/mobject/session/locatingpin.hpp"
#include "proc/mobject/session/track.hpp"


namespace mobject
  {


  class ExplicitPlacement;


  template<class MO>
  class Placement : public shared_ptr<MO>
    {
    protected:
      typedef cinelerra::Time Time;
      typedef session::Track Track;


    public:
      LocatingPin chain;
      
      /** resolve the network of placement and
       *  provide the resulting (explicit) placement.
       */
      virtual ExplicitPlacement resolve () ;
      
    protected:
      Placement ();
      virtual ~Placement() {};
    };
  

  /* === defining specialisations to be subclasses === */  
    
  namespace session
    {
    class Clip;
    class Effect;
    class Meta;
    }
  
  template<>
  class Placement<session::Clip> : public Placement<MObject>
    { 
      //TODO: overwirte shared_ptr's operator-> to return the correct subtype of MObject...
    };
  
  template<>
  class Placement<session::Effect> : public Placement<MObject>
    { };
  
  template<>
  class Placement<session::Meta> : public Placement<MObject>
    { };
  
  


} // namespace mobject
#endif
