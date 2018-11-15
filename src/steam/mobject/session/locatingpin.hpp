/*
  LOCATINGPIN.hpp  -  Chaining and constraining the Placement of a Media Object

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file locatingpin.hpp 
 ** Implementing the Placement mechanics. The various specifications how
 ** to place some MObject (logically) within the Session are given by small
 ** LocatingPin objects forming a chain. For resolving the actual position, at the 
 ** moment (10/07) we use a preliminary implementation to support the most common
 ** Placement types (fixed and relative). It is comprised of the nested LocatingSolution
 ** and the functions FixedLocation#resolve(LocatingSolution&) and
 ** RelativeLocation#resolve(LocatingSolution&) etc. If this is to be extended,
 ** we'll need a real spatial discrete constraint solver (and this probably will be
 ** some library implementation, because the problem is anything but trivial).
 ** 
 ** @todo this can be considered a preliminary sketch without being backed
 **       by actual functionality. Just enough to be able to drive the design of
 **       other parts ahead. See esp. TICKET #100, which contains an idea for a
 **       refactoring. 
 **
 */





#ifndef STEAM_MOBJECT_SESSION_LOCATINGPIN_H
#define STEAM_MOBJECT_SESSION_LOCATINGPIN_H


#include "steam/common.hpp"
#include "lib/time/timevalue.hpp"

#include <utility>
#include <memory>




namespace proc {
namespace asset { class Pipe; }

namespace mobject {
  
  class MObject;
  
///////////////////////////////////////////TODO: all those dependencies are just a plain mess right now.  
//  
//template<class MO, class B=MObject> 
//class Placement ;
//typedef Placement<MObject> PMO;
  
  template<class MO>
  class PlacementRef;    ///TODO: as of 5/09 the idea is to phase out direct dependency on the placement class and recast those dependencies in terms of PlacementRef
  
  typedef PlacementRef<MObject> PlaRef;
  
  
  namespace session {
    
    class FixedLocation;
    class RelativeLocation;
    
    
    
    /**
     * Positioning specification, possibly chained
     * to further specifications. The base class LocatingPin
     * is a "no-op" specification which doesn't constrain the
     * possible locations and thus can be embedded into pristine
     * Placement by default. The Functor operators provide a
     * way to add concrete positioning specifications, thereby
     * defining the position of the MObject to be placed.
     * 
     * @note to the implementer of subclasses: LocatingPins are
     *       copyable and need to handle cloning (incl the chain)
     */
    class LocatingPin
      {
      protected:
        typedef lib::time::Time Time;
        typedef lib::time::TimeVar TimeVar;
        typedef lib::time::Offset Offset;
        typedef Time* Fork; //TODO dummy declaration; we don't use Tracks as first-class entity any longer. This role should be taken by the "Output Designation"
        typedef std::shared_ptr<asset::Pipe> Pipe;
        typedef std::pair<Time,Pipe> SolutionData;  //TICKET #100 (ichthyo considers better passing of solution by subclass)
        struct LocatingSolution;
      
        /** next additional Pin, if any */
        std::unique_ptr<LocatingPin> next_;
        
        /** order to consider when resolving. 0=highest */
        virtual int getPrioLevel ()  const { return 0; }
        
        LocatingPin& addChain (LocatingPin*);
        void resolve (LocatingSolution&)  const;
        virtual void intersect (LocatingSolution&)  const;
        
      public:
        const SolutionData resolve ()  const;
        bool isOverdetermined () const;
        
        /* Factory functions for adding LocatingPins */
        
        FixedLocation&    operator() (Time start, Fork track=0);                                                 //////////TODO: Tracks are gone, long live the Forks!
        RelativeLocation& operator() (PlacementRef<MObject>& refObj, Offset const& offset=Offset(Time::ZERO));   //////////TODO: warning, just a dummy placeholder for now!!
        
        LocatingPin (const LocatingPin&);
        LocatingPin& operator= (const LocatingPin&);
        virtual LocatingPin* clone ()  const;
        
        virtual ~LocatingPin() {};
        
//      protected:
        LocatingPin () {};
        
//TODO (for working out the buildable interface; ctor should be protected)
protected:
  
//        friend class Placement<MObject>;
        
        /** 
         * @internal helper for the (preliminary)
         * position resolve() implementation.
         * @todo we can't sensibly reason about tracks, 
         * because at the moment (10/07) we lack a track implementation...
         * @todo shouldn't we use a range-restriction LocatingPin (subclass)
         * to represent the to-be-found solution? (ichthyo: see Trac #100)
         */  
        struct LocatingSolution
          {
            TimeVar minTime;
            TimeVar maxTime;
            Fork minTrack; // TODO don't use Tracks
            Fork maxTrack;
            bool impo;
            
            LocatingSolution () 
              : minTime(Time::MIN),  // TODO: better implementation of "unspecified..."
                maxTime(Time::MAX),
                minTrack(0),     // TODO
                maxTrack(0),
                impo(false)
              { }
            
            Time getTime ();
            Pipe getPipe ();
            
            bool is_definite ();
            bool is_impossible ();
            bool still_to_solve (); 
            
          };
        
        friend bool operator== (LocatingPin const&, LocatingPin const&);
        
      };
      
    
    /** check for equivalent definition of a complete locating chain */
    
    inline bool
    operator== (LocatingPin const& pin1, LocatingPin const& pin2)
    {
//////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #511  really implement equivalence relation!          
      return pin1.resolve().first == pin2.resolve().first;
    }
      
      

}}} // namespace proc::mobject::session
#endif
