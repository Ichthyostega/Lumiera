/*
  MOBJECT-REF.hpp  -  active external reference to an MObject within the Session

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

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
 ** \par Lifecycle
 ** An MObjectRef is always created inactive. It needs to be activated explicitly,
 ** providing either a direct (language) ref to an Placement within the session,
 ** or an PlacementRef tag, or another MObjecRef. It can be closed (detached).
 ** 
 ** \par Type handling
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

#include "lib/handle.hpp"
#include "steam/mobject/placement.hpp"
#include "steam/mobject/placement-ref.hpp"
#include "steam/mobject/session/session-service-mutate.hpp"

#include <string>


///////////////////////////////////////////TODO: define an C-API representation here, make the header multilingual!

namespace steam {
namespace mobject {
  
  namespace error = lumiera::error;
  
  
  
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
      typedef PlacementMO::Id<MO> _Id;

      
      PlacementRef<MO> pRef_;
      using _Handle::smPtr_;
      
      
      
    public:
      
      MO*
      operator-> ()  const
        {
          if (!smPtr_)
            throw error::State("Lifecycle error: MObject ref not activated"
                              , LERR_(BOTTOM_MOBJECTREF));
          
          ENSURE (INSTANCEOF (MO, smPtr_.get()));
          return smPtr_.operator-> ();
        }
      
      
      Placement<MO>& getPlacement()  const
        {
          if (!isValid())
            throw error::State("Accessing inactive MObject ref"
                              , LERR_(BOTTOM_MOBJECTREF));
          
          ENSURE (INSTANCEOF (MO, smPtr_.get()));
          return *pRef_;
        }
      
      
      /** allow to use a MObjectRef like a (bare) PlacementRef
       *  @note no test if this MObjectRef is NIL  */
      PlacementRef<MO> const&
      getRef() const
        {
          return pRef_;
        }
      
      /** allow to use MObjectRef instead of a Placement-ID (hash)
       *  @todo not sure if that leads to unexpected conversions,
       *        because the underlying implementation can be 
       *        converted to size_t */                      //////////////////////////TICKET #682  revisit that decision later
      operator _Id const&()  const
        {
          return pRef_;
        }
      
      
      
      /** resolves the referred placement to an 
       *  ExplicitPlacement and returns the found start time
       */
      lib::time::Time
      getStartTime()
        {
          return pRef_.resolve().time;
        }
      
      
      /** attach a child element to the model
       *  @param newPlacement to be copied into the model, placed 
       *         into the scope of the object denoted by this MORef
       *  @return MORef designing the newly created and attached object instance
       */
      template<class MOX>
      MORef<MOX>
      attach (Placement<MOX> const& newPlacement)
        {
          if (!isValid())
            throw error::State("Attempt to attach a child to an inactive MObject ref"
                              , LERR_(BOTTOM_MOBJECTREF));
          MORef<MOX> newInstance;
          PlacementMO::ID thisScope = pRef_;
          return newInstance.activate (
                    session::SessionServiceMutate::attach_toModel (newPlacement, thisScope));
        }
      
      
      /** detach this object instance from model,
       *  including all child elements.
       *  @note the object itself remains accessible
       */
      void
      purge ()
        { 
          if (isValid())
            session::SessionServiceMutate::detach_and_clear (pRef_);
          
          ENSURE (!isValid());
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
          
          if (isValid() && pRef_ == newRef )
            return *this;                // self assignment detected
          else
            return activate (*newRef); // STRONG exception safe
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
          return _Handle::isValid() 
              && pRef_.isValid();
        }
      
      size_t
      use_count()  const
        {
          return isValid()? pRef_.use_count() : 0;
        }
      
      template<class MOX>
      bool
      isCompatible()  const
        {
          return pRef_
              && (*pRef_).template isCompatible<MOX>();
        }
      
      operator string()   const                      ///////////////////////TICKET #527 #528 should be better integrated with the other object types
        {
          return isValid()? string(getPlacement())
                          : "MRef-NIL";
        }
      
      
      
      
      /* == equality comparisons == */
      
      template<class MOX>
      bool
      operator== (MORef<MOX> const& oRef)  const
        {
          return isValid()
              && oRef == this->pRef_;
        }
      
      template<class MOX>
      bool
      operator!= (MORef<MOX> const& oRef)  const
        {
          return not isValid()
              or oRef != this->pRef_;
        }
      
      template<class MOX>
      friend bool
      operator== (MORef const& oRef, PlacementRef<MOX> const& pRef)
        {
          return oRef.isValid()
             and oRef.pRef_ == pRef;
        }
      
      template<class MOX>
      friend bool
      operator!= (MORef const& oRef, PlacementRef<MOX> const& pRef)
        {
          return not oRef.isValid()
              or oRef.pRef_ != pRef;
        }
      
      template<class MOX>
      friend bool
      operator== (PlacementRef<MOX> const& pRef, MORef const& oRef)
        {
          return oRef.isValid()
             and pRef == oRef.pRef_;
        }
      
      template<class MOX>
      friend bool
      operator!= (PlacementRef<MOX> const& pRef, MORef const& oRef)
        {
          return not oRef.isValid()
              or pRef != oRef.pRef_;
        }
      
      bool
      operator== (PlacementMO::ID const& pID)  const
        {
          return isValid()
             and PlacementMO::ID (pRef_) == pID;
        }
      
      bool
      operator!= (PlacementMO::ID const& pID)  const
        {
          return not isValid()
              or PlacementMO::ID (pRef_) != pID;
        }
      
    };
  
  
  typedef MORef<MObject> MObjectRef;
  
  
  /* === convenience shortcuts === */
  
  /** check if the two references actually share ownership
   *  on the same underlying \em MObject (as opposed to referring
   *  to the same \em Placement, which is tested by \c operator== )
   */
  template<class MOX, class MOY>
  inline bool
  isSharedPointee (MORef<MOX> const& ref1, MORef<MOY> const& ref2)
  {
    return ref1.isValid() && ref2.isValid()
        && isSharedPointee (ref1.getPlacement(), ref2.getPlacement());
  }
  
  /** check if the two references actually denote an equivalent placement */
  template<class MOX, class MOY>
  inline bool
  isEquivalentPlacement (MORef<MOX> const& ref1, MORef<MOY> const& ref2)
  {
    return ref1.isValid() && ref2.isValid()
        && isSameDef (ref1.getPlacement(), ref2.getPlacement());
  }
  
  
}} // namespace steam::mobject
#endif
