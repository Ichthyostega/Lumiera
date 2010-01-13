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
 ** External MObject/Placement reference. 
 ** This smart-handle referres to an MObject, attached (placed) into the session.
 ** It is a copyable value object, implemented by an LUID (hash) and an shared_ptr.
 ** Holding an MObject ref keeps the referred MObject alive, but gives no guarantees
 ** regarding the validity of the referred \em Placement within the session. On each
 ** access, the placement is re-fetched using the PlacementIndex, which may fail.
 ** 
 ** MObjectRef allows to access the pointee (MObject subclass) by using the dereferentiation
 ** operator, and it allows to access the Placement within the session. Moreover, as an
 ** convenience shortcut, some of Placement's query operations are directly exposed.
 ** 
 ** !Lifecycle
 ** An MObjectRef is always created inactive. It needs to be activated explicitly,
 ** providing either a direct (language) ref to an Placement within the session,
 ** or an PlacementRef tag, or another MObjecRef. It can be closed (detached).
 ** 
 ** !Type handling
 ** Like any smart-ptr MObjectRef is templated on the actual type of the pointee.
 ** It can be built or re-assigned from a variety of sources, given the runtime type
 ** of the referred pointee is compatible to this template parameter type. This
 ** allows flexibly to re-gain a specifically typed context, even based just 
 ** on a plain LUID. This functionality is implemented by accessing the
 ** PlacementIndex within the session, and then by using the RTTI of
 ** the fetched Placement's pointee. 
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
#include "lib/handle.hpp"
#include "lib/lumitime.hpp"
#include "proc/mobject/placement.hpp"
#include "proc/mobject/placement-ref.hpp"


///////////////////////////////////////////TODO: define an C-API representation here, make the header multilingual!

namespace mobject {
  
  
  
  class MObject;
  
  
  LUMIERA_ERROR_DECLARE (BOTTOM_MOBJECTREF);  ///<  NIL MObjectRef encountered
  
  /**
   * An active (smart-ptr like) external reference
   * to a specifically placed MObject "instance" within the session.
   * Implemented as a smart-ptr sharing ownership of the pointee MObject
   * with the corresponding placement within the session. Additionally,
   * a PlacementRef is incorporated, allowing to re-access this placement
   * with the help of PlacementIndex within the current session.
   */
  template<class MO =MObject>
  class MORef
    : public lib::Handle<MO>
    {
      typedef lib::Handle<MO> _Handle;
      
      
      PlacementRef<MO> pRef_;
      using _Handle::smPtr_;
      
      
      
    public:
      
      MO*
      operator-> ()  const
        {
          if (!smPtr_)
            throw lumiera::error::State("Lifecycle error: MObject ref not activated"
                                       ,LUMIERA_ERROR_BOTTOM_MOBJECTREF);
          
          ENSURE (INSTANCEOF (MO, smPtr_.get()));
          return smPtr_.operator-> ();
        }
      
      Placement<MO>& getPlacement()  const
        {
          ENSURE (INSTANCEOF (MO, smPtr_.get()));
          return *pRef_;
        }
      
      /** resolves the referred placement to an 
       *  ExplicitPlacement and returns the found start time
       */
      lumiera::Time const&
      getStartTime()
        {
          return pRef_.resolve().time;
        }
      
      
      
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
          ASSERT (placement.isValid());
          pRef_ = placement;                       // STRONG exception safe
          placement.extendOwnershipTo(smPtr_);     // never throws
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
          PlacementRef<MO> newRef (pRefID);
          if (newRef.isValid()
             && pRef_ != newRef )
            {
              return activate (*newRef); // STRONG exception safe
            }
          else return *this;
        }
      
      /** build and activate an MObject reference based on
       *  an existing reference of the same pointee type
       *  @note STRONG exception safety guarantee
       *  @throws error::Invalid when the referred placement 
       *          isn't known to the current session's PlacementIndex
       */
      MORef&
      activate (MORef const& oRef)
        {
          return activate (oRef.getPlacement());
        }
      
      template<typename MOX>
      MORef&
      activate (MORef<MOX> const& oRef)
        {
          return activate (oRef.getPlacement().getID());
        }
      
      /** cross assignment.
       *  @note besides that, we use the default generated copy operations.
       *  @note STRONG exception safety guarantee
       *  @throws error::Invalid when the referred placement isn't registered
       *       within the current session, or if the runtime type of the pointees
       *       aren't assignment compatible 
       */
      template<typename MOX>
      MORef&
      operator= (MORef<MOX> const& oRef)
        {
          return activate (oRef);
        }
      
      
      
      
      /* == diagnostics == */
      bool
      isValid()  const
        {
          return pRef_.isValid();
        }
      
      size_t
      use_count()  const
        {
          return pRef_.use_count();
        }
      
      template<class MOX>
      bool
      isCompatible()  const
        {
          return pRef_
              && (*pRef_).isCompatible<MOX>();
        }
      
      
      
      
      /* == equality comparisons == */
      
      template<class MOX>
      bool
      operator== (MORef<MOX> const& oRef)  const
        {
          return oRef == this->pRef_;
        }
      
      template<class MOX>
      bool
      operator!= (MORef<MOX> const& oRef)  const
        {
          return oRef != this->pRef_;
        }
      
      template<class MOX>
      friend bool
      operator== (MORef const& oRef, PlacementRef<MOX> const& pRef)
        {
          return oRef.pRef_ == pRef;
        }
      
      template<class MOX>
      friend bool
      operator!= (MORef const& oRef, PlacementRef<MOX> const& pRef)
        {
          return oRef.pRef_ != pRef;
        }
      
      template<class MOX>
      friend bool
      operator== (PlacementRef<MOX> const& pRef, MORef const& oRef)
        {
          return pRef == oRef.pRef_;
        }
      
      template<class MOX>
      friend bool
      operator!= (PlacementRef<MOX> const& pRef, MORef const& oRef)
        {
          return pRef != oRef.pRef_;
        }
      
      bool
      operator== (PlacementMO::ID const& pID)  const
        {
          return PlacementMO::ID (pRef_) == pID;
        }
      
      bool
      operator!= (PlacementMO::ID const& pID)  const
        {
          return PlacementMO::ID (pRef_) != pID;
        }
      
    };
  
  
  typedef MORef<MObject> MObjectRef;
  
  
} // namespace mobject
#endif
