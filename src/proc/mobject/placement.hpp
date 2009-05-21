/*
  PLACEMENT.hpp  -  Key Abstraction: a way to place and locate a Media Object
 
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
 
*/


/** @file placement.hpp 
 ** Placements are at the very core of all editing operations,
 ** because they act as handles to access the media objects to be manipulated. 
 ** Moreover, Placements are the actual content of the EDL(s) and Fixture and thus
 ** are small objects with value semantics. Many editing tasks include locating some
 ** Placement in the EDL or directly take a ref to a Placement.
 ** 
 ** Placements are <b>refcounting smart pointers</b>: By acting on the Placement object,
 ** we can change parameters of the way the media object is placed (e.g. adjust an offset), 
 ** while by dereferencing the Placement object, we access the "real" media object.
 ** Usually, any MObject is created by a factory and immediately wrapped into a Placement,
 ** which takes ownership of the MObject.
 **
 ** Besides being a handle, Placements define the logical position where some MObject is
 ** supposed to be located within the EDL or Fixture. The way in which this placing happens
 ** is controlled and parametrised by a collection (chain) of LocatingPin objects. By adding
 ** to this chain, the position of the MObject is increasingly constrained. The simplest
 ** case of such constraining is to add a FixedLocation, thus placing the MObject at one
 ** absolute position (time, track).
 ** 
 ** Placements are templated on the type of the actual MObject they refer to, so, sometimes
 ** we rather use a Placement<Clip> to be able to use the more specific methods of the
 ** session::Clip interface. But <i>please note the following detail:</i> this type
 ** labelling and downcasting is the <i>only</i> difference between these subclasses, 
 ** besides that, they can be replaced literally by one another (slicing is accepted).
 **
 ** @see ExplicitPlacement
 ** @see LocatingPin interface for controlling the positioning parameters
 **
 */



#ifndef MOBJECT_PLACEMENT_H
#define MOBJECT_PLACEMENT_H

#include "pre.hpp"
#include "proc/mobject/session/locatingpin.hpp"
#include "proc/asset/pipe.hpp"

#include <tr1/memory>


namespace mobject {
  
  namespace session{ class MObjectFactory; }
  
  class ExplicitPlacement;
  
  using std::tr1::shared_ptr;
  
  
  
  /**
   * A refcounting Handle to an MObject of type MO,
   * used to constrain or explicitly specify the
   * location where the MObject is supposed to
   * be within the Session/EDL
   */
  template<class MO>
  class Placement : protected shared_ptr<MO>
    {
    protected:
      typedef shared_ptr<MO> Base;
      typedef lumiera::Time Time;
      typedef asset::shared_ptr<asset::Pipe> Pipe;
      
      
      
    public:
      /** smart pointer: accessing the MObject, 
       *  which is subject to placement.
       *  @note we don't provide operator*
       */
      virtual MO * 
      operator-> ()  const 
        { 
          ENSURE (*this); 
          return Base::operator-> (); 
        }      
      
      operator string()   const ;
      size_t use_count()  const { return Base::use_count(); }
      
      virtual ~Placement() {};
      
      
      
      /** interface for defining the kind of placement
       *  to employ, and for controlling any additional
       *  constraints and properties. 
       */
      session::LocatingPin chain;
      
      /** combine and resolve all constraints defined
       *  by the various LocatingPin (\see #chain) and
       *  provide the resulting (explicit) placement.
       */
      virtual ExplicitPlacement resolve ()  const; 
      
      
    protected:
      Placement (MO & subject, void (*moKiller)(MO*)) 
        : shared_ptr<MO> (&subject, moKiller) {};
        
      friend class session::MObjectFactory;
    };
  
  
  
  
  
  /* === defining specialisations to be subclasses === */

#define DEFINE_SPECIALIZED_PLACEMENT(SUBCLASS, BASE)  \
  template<>                                           \
  class Placement<SUBCLASS> : public Placement<BASE>    \
    {                                                    \
    protected:                                            \
      Placement (SUBCLASS & m, void (*moKiller)(MObject*)) \
        : Placement<BASE>::Placement (m, moKiller)         \
        { };                                               \
      friend class session::MObjectFactory;                \
                                                           \
    public:                                                \
      virtual SUBCLASS*                                    \
      operator-> ()  const                                 \
        {                                                  \
          ENSURE (INSTANCEOF (SUBCLASS, this->get()));     \
          return static_cast<SUBCLASS*>                    \
            (shared_ptr<MObject>::operator-> ());          \
        }                                                  \
    };
  
  /* a note to the maintainer: please don't add any fields or methods to
   * these subclasses which aren't also present in Placement<MObject>!
   * Placements are frequently treated like values and thus slicing
   * will happen, which in this special case is acceptable.
   */
  


} // namespace mobject
#endif
