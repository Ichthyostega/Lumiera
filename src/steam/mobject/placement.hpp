/*
  PLACEMENT.hpp  -  Key Abstraction: a way to place and locate a Media Object

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file placement.hpp
 ** Core abstraction: placement of a media object into session context.
 ** Placements are at the very core of all editing operations, because they act
 ** as handles to access the media objects to be manipulated.
 ** Moreover, Placements are the actual "content" stored within the the Session and Fixture
 ** data structure and thus are small handle like objects, which can be processed efficiently.
 ** Many editing tasks include locating some Placement within the Session or directly take
 ** a reference to a Placement. A Placement represents a _specific way to attach content._
 ** They may refer to contextual information and relate themselves to other placements.
 ** 
 ** On the implementation level, placements are *refcounting smart pointers*: By acting
 ** on the Placement object, we can change parameters of the way the media object is placed
 ** (e.g. adjust an offset), while by dereferencing the Placement object, we access the media
 ** object itself. Usually, any MObject is created by a factory and immediately wrapped into a Placement,
 ** which takes ownership of the MObject.
 ** 
 ** Besides being a handle, Placements define the logical position where some MObject is
 ** supposed to be located within the Session or Fixture. The way in which this placing happens
 ** is controlled and parametrised by a collection (chain) of LocatingPin objects. By adding
 ** to this chain, the position of the MObject is increasingly constrained. The simplest
 ** case of such constraining is to add a FixedLocation, thus placing the MObject at one
 ** absolute position (time, output).
 ** 
 ** Together, this yields semantics somewhere in between value semantics and reference semantics.
 ** As any smart-ptr, placements are copyable, but each such copy takes on a _distinct identity._
 ** Moreover, when added to the Session, a placement acts as if it was an _instance_ of the object
 ** it points at, with the purpose to bind this instance into the Session with specific placement
 ** properties. Thus, such a placement-within-session _is_ a distinguishable entity, because
 ** the settings on the contained LocatingPin chain _do constitute_ the relation properties
 ** of the MObject "placed" by this placement. To support this rather ref-like semantics, any
 ** placement has an embedded ID (identity). Building on this ID, it is possible in turn to
 ** create a smart-ptr like PlacementRef to denote a specific placement found within the Session.
 ** 
 ** Placements are templated on the type of the actual MObject they refer to, so, sometimes
 ** e.g. we rather use a Placement<Clip> to be able to use the more specific methods of the
 ** session::Clip interface. But _please note the following detail:_ this type labelling
 ** and downcasting is the _only_ difference between these subclasses, besides that,
 ** they can be replaced literally by one another (slicing acceptable).
 ** 
 ** @see ExplicitPlacement
 ** @see LocatingPin interface for controlling the positioning parameters
 **
 */



#ifndef MOBJECT_PLACEMENT_H
#define MOBJECT_PLACEMENT_H

#include "lib/error.hpp"
#include "lib/hash-indexed.hpp"
#include "lib/time/timevalue.hpp"
#include "steam/mobject/session/locatingpin.hpp"

#include "steam/asset/pipe.hpp"   //////////////TICKET #109 : get rid of this

#include <memory>


namespace steam {
namespace mobject {
  
  namespace session{ class MObjectFactory; }
  
  class MObject;
  class ExplicitPlacement;
  
  using std::shared_ptr;
  using std::static_pointer_cast;
  using lib::HashIndexed;
  
  
  
  /**
   * A refcounting Handle to an MObject of type MO,
   * used to constrain or explicitly specify the  location
   * where the MObject is supposed to be within the Session/Model.
   * Placements are copyable (like values), but may be distinguished
   * by their identity (reference semantics), which is based on an
   * \link lib::HashIndexed hash-ID \endlink.
   * 
   * Placements are defined to form a hierarchy, thereby mirroring
   * the relations between their referents to some degree. This allows
   * for building APIs targeted at specific kinds of MObjects, and
   * at the same time allows a specific placement to stand-in when
   * just a unspecific Placement<MObject> is required.
   * 
   * @param MO the (compile time) type of the referent
   * @param B  immediate base class of this placement
   */
  template<class MO, class B =MObject>
  class Placement ;
  
  
  /**
   * this specialisation forms the root of all placements
   * and defines all of Placement's actual functionality.
   */
  template<>
  class Placement<MObject,MObject>
    : protected shared_ptr<MObject>
    , public HashIndexed<Placement<MObject>, lib::hash::LuidH >
    {
    protected:
      typedef HashIndexed<Placement<MObject>, lib::hash::LuidH> HashInd;
      typedef shared_ptr<MObject> _SmartPtr;
      typedef void (*Deleter)(MObject*);
      typedef lib::time::Time Time;
      typedef asset::shared_ptr<asset::Pipe> Pipe;   ////TICKET #109 : get rid of this
      
      
      
    public:
      /** smart pointer: accessing the MObject,
       *  which is subject to placement.
       *  @note we don't provide operator*
       */
      MObject *
      operator->()  const
        {
          ENSURE (*this);
          return _SmartPtr::operator->();
        }
      
      /** run time diagnostics: is the pointee
       *  of this placement compatible to the given type?
       */
      template<class Y>
      bool
      isCompatible ()  const
        {
          return 0 != dynamic_cast<Y*> (get());
        }
      
      /** extend shared ownership to the given smart-ptr
       *  @note never throws
       */
      template<class Y>
      void
      extendOwnershipTo (shared_ptr<Y>& target)  const
        {
          REQUIRE (isCompatible<Y>());
          target = static_pointer_cast<Y>(*this);
        }
      
      /** free function to detect two placements sharing a pointee */
      friend bool
      isSharedPointee (Placement const& p1, Placement const& p2)
      {
        return static_cast<const void*> (p1.get())
            == static_cast<const void*> (p2.get());
      }
      
      
      operator string()   const ;
      size_t use_count()  const { return _SmartPtr::use_count(); }
      bool   isValid()    const { return _SmartPtr::use_count(); }
      
      
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
                               //////////////////////////TODO (1) could resolve() return a reference?  Otherwise placement-ref.hpp needs to include ExplicitPlacement
                               //////////////////////////TODO (2) does this really need to be virtual. Guess not. It's not abstract and not really polymorphic. But virtual here causes template bloat.
                                             ////////////TICKET #439
      
      
      Placement (Placement const& ref)
        : _SmartPtr (ref)
        , HashInd()    // creating a new ID!
        , chain(ref.chain)
        { }
      
    protected:
      Placement (MObject & subject, Deleter killer)
        : _SmartPtr (&subject, killer) { };
        
      friend class session::MObjectFactory;
      
      
/////////////////////////////////////////////////////////TICKET #513
//  private:
//    /** copy assignment prohibited */
//    Placement& operator= (Placement const&);
/////////////////////////////////////////////////////////TICKET #513
    };
  
  
  /**
   * any specific placements are supposed to be derived
   * from Placement<MObject>, or an intermediary interface,
   * in case the second template parameter is used.
   * @note please refrain from adding additional functionality
   *       to these subclasses. Especially, don't add any fields
   *       to the subclass, as Placements are treated like values
   *       at times, and thus slicing will happen, which in this
   *       special case is acceptable.
   */
  template<class MO, class B>
  class Placement
    : public Placement<B>
    {
    protected:
      typedef Placement<B> _Parent;
      typedef typename _Parent::template Id<MO> const& _Id;
      typedef typename _Parent::Deleter Deleter;
      typedef typename _Parent::_SmartPtr _SmartPtr;
      
      
      Placement (MO & mo, Deleter killer)
        : _Parent (mo, killer)
        { };
      friend class session::MObjectFactory;
      
    public:
      MO*
      operator-> ()  const
        {
          ENSURE (INSTANCEOF (MO, this->get()));
          return static_cast<MO*>
            (_SmartPtr::operator-> ());
        }
      
      _Id
      getID ()  const  ///< @note overrides HashIndexed::getID to pass specific type information,
        {
          return _Parent::template recastID<MO>();
        }
      
    };
  
  
  /** @todo cleanup uses of ref-to-placement. See Ticket #115 */
  typedef Placement<MObject> PlacementMO;
  typedef Placement<MObject> PMO;
  
  
  
  /* == free functions == */

  string
  format_PlacementID (PlacementMO const&) ;
  
  
  /** compare the properties of placement
   *  @return \c true if all the LocatingPin entries
   *          in both placements are semantically equivalent.
   */
  bool
  isSameDef (PlacementMO const&, PlacementMO const&);
  
  
  
}} // namespace steam::mobject
#endif
