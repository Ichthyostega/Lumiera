/*
  PLACEMENT-REF.hpp  -  generic reference to an individual placement added to the session
 
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


/** @file placement-ref.hpp 
 **
 ** @see Placement
 ** @see PlacementRef_test
 **
 */



#ifndef MOBJECT_PLACEMENT_REF_H
#define MOBJECT_PLACEMENT_REF_H

//#include "pre.hpp"
//#include "proc/mobject/session/locatingpin.hpp"
//#include "proc/asset/pipe.hpp"
#include "proc/mobject/placement.hpp"

//#include <tr1/memory>


namespace mobject {

//  using std::tr1::shared_ptr;
  
  
  class MObject;
  
  // see placement-index.cpp 
  Placement<MObject> &
  fetch_PlachementIndex(Placement<MObject>::ID const&) ;
  
  
  /**
   */
  template<class MO =MObject>
  class PlacementRef
    {
      typedef Placement<MO>      PlacementMO;
      typedef Placement<MObject>::ID     _ID;         ////TODO: superfluous...
      typedef Placement<MObject>::Id<MO> _Id;
      
      _Id id_;
      
    public:
      /**
       * Creating a PlacementRef from a compatible reference source.
       * Any source allowing to infer a \em compatible mobject::Placement
       * is accepted. Here, compatibility is decided based on the run time
       * type of the pointee, in comparison to the template parameter Y.
       * In any case, for this ctor to succeed, the provided ref or ID 
       * needs to be resolvable to a placement by the implicit PlacementIndex
       * facility used by all PlacementRef instances (typically the Session).
       * @note there is no default ctor, a reference source is mandatory.
       * @param refID reference resolvable to a placement via Index, especially
       *              - an existing Placement
       *              - just an Placement::ID
       *              - a plain LUID
       * @throw error::Invalid on incompatible run time type of the resolved ID  
       */
      template<class Y>
      explicit
      PlacementRef (Y const& refID)
        : id_(recast (refID))
        { 
          validate(id_); 
        }
      
      PlacementRef (PlacementRef const& r)    ///< copy ctor
        : id_(r.id_)
        { 
          validate(id_); 
        }
      
      template<class X>
      PlacementRef (PlacementRef<X> const& r) ///< extended copy ctor, when type X is assignable to MO
        : id_(r.id_)
        {
          validate(id_); 
        }
      
      
      PlacementRef&
      operator= (PlacementRef const& r)
        {
          validate(r.id_);
          id_ = r.id_;
          return *this;
        }
      
      template<class X>
      PlacementRef&
      operator= (PlacementRef<X> const& r)
        {
          validate(r.id_);
          id_ = r.id_;
          return *this;
        }
      
      template<class Y>
      PlacementRef&
      operator= (Y const& refID)
        {
          validate (recast (refID));
          id_ = recast (refID);
          return *this;
        }
      
      
      /* == forwarding smart-ptr operations == */
      
      PlacementMO& operator*()  const { return access(id_); } ///< dereferencing fetches referred Placement from Index
      
      MO* operator->()          const { return access(id_); } ///< provide access to pointee API by smart-ptr chaining 
      
      operator string()         const { return access(id_).operator string(); }
      size_t use_count()        const { return access(id_).use_count(); }
      
      ////////////////TODO more operations to come....
      
    private:
      static void
      validate (_Id rId)
        {
          if (!access(rId).isCompatible<MO>())
            throw error::Invalid(LUMIERA_ERROR_INVALID_PLACEMENTREF);
        }
      
      static _Id const&
      recast (_ID const& someID)
        {
          return static_cast<_Id const&> (someID);
        }
      
      static _Id const&
      recast (const LumieraUid luid)
        {
          REQUIRE (luid);
          return reinterpret_cast<_Id const&> (*luid);
        }
      
      static PlacementMO&
      access (_Id placementID)
        {
          Placement<MObject> & pla (fetch_PlachementIndex (placementID));  // may throw
          REQUIRE (pla);
          ASSERT (pla.isCompatible<MO>());
          return static_cast<PlacementMO&> (pla);
        }
    };
  
  
  
  
} // namespace mobject
#endif
