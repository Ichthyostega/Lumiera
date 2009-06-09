/*
  MOBJECT-REF.hpp  -  active external reference to an MObject within the Session
 
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


/** @file mobject-ref.hpp
 ** A reference tag for accessing an object within the session externally,
 ** from a separate Layer or from plugin code.
 **
 ** @see MObject
 ** @see Session
 ** @see PlacementRef
 ** @see PlacementIndex
 **
 */



#ifndef MOBJECT_MOBJECT_REF_H
#define MOBJECT_MOBJECT_REF_H

//#include "pre.hpp"
//#include "proc/mobject/session/locatingpin.hpp"
//#include "proc/asset/pipe.hpp"
#include "lib/handle.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/mobject/placement-ref.hpp"

//#include <tr1/memory>

///////////////////////////////////////////TODO: define an C-API representation here, make the header multilingual!

namespace mobject {
  
//  using std::tr1::shared_ptr;
  
  
  class MObject;
  
  
  /**
   * An active (smart-ptr like) external reference
   * to a specifically placed MObject "instance" within the session.
   */
  template<class MO =MObject>
  class MORef
    : public lib::Handle<MO>
    {
      typedef lib::Handle<MO> _Par;
      
      PlacementRef<MO> pRef_; ////////////////////////////////////////////////////////////////////TODO: how to create an "inactive" PlacementRef???
      
      using _Par::smPtr_;
      
    public:
      
      MO*
      operator-> ()  const
        {
          REQUIRE (smPtr_.get(), "Lifecycle-Error");
          ENSURE (INSTANCEOF (MO, smPtr_.get()));
          return smPtr_.operator-> ();
        }
      
      Placement<MO>& getPlacement();
      
      
      /* === Lifecycle === */
      
      /** activate an MObject reference, based on an existing placement,
       *  which needs to be contained (added to) the session. After checking
       *  the validity of the placement, this MObjectRef shares ownership
       *  of the referred MObject with the denoted placement.
       *  @note STRONG exception safety guarantee
       *  @see #close() for detaching this MObjectRef
       */
      MORef&
      activate (Placement<MO> const& placement)
        {
          ASSERT (placement);
          pRef_ = placement;           // STRONG exception safe
          smPtr_.swap (placement);     // never throws
          return *this;
        }
      
      /** build and activate an MObject reference, based on anything
       *  which might be assigned to an PlarementRef<MO> :
       *  - any Placement
       *  - any Placement-ID
       *  - any Placement-Ref
       *  - a plain LUID
       * @throws error::Invalid when the (directly or indirectly 
       *         referred placement isn't known to the session PlacementIndex,
       *         or when the placement actually found has an incompatible dynamic type 
       */
      template<typename REF>
      MORef&
      activate (REF const& pRefID)
        {
          if (pRefID != pRef_)
            {
              PlacementRef<MO> newRef (pRefID);
              ASSERT (newRef);
              return activate (*newRef); // STRONG exception safe
            }
          else return *this;
        }
      
    };
  ////////////////TODO currently just fleshing  out the API....
  
  
  typedef MORef<MObject> MObjectRef;
  
  
} // namespace mobject
#endif
