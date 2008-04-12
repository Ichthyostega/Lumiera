/*
  LOCATINGPIN.hpp  -  Chaining and constraining the Placement of a Media Object
 
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


/** @file locatingpin.hpp 
 ** Implementing the Placement mechanics. The various specifications how
 ** some MObject is to be placed (logically) within the EDL are given by small
 ** LocatingPin objects forming a chain. For resolving the actual position, at the 
 ** moment (10/07) we use a preliminary implementation to support the most common
 ** Placement types (fixed and relative). It is comprised of the nested LocatingSolution
 ** and the functions FixedLocation#resolve(LocatingSolution&) and
 ** RelativeLocation#resolve(LocatingSolution&) etc. If this is to be extended,
 ** we'll need a real spatial discrete constraint solver (and this probably will be
 ** some library implementation, because the problem is anything but trivial).
 **
 */





#ifndef MOBJECT_SESSION_LOCATINGPIN_H
#define MOBJECT_SESSION_LOCATINGPIN_H


#include "lumiera.h"

#include <utility>
#include <tr1/memory>
#include <boost/scoped_ptr.hpp>
using boost::scoped_ptr;



namespace asset { class Pipe; }

namespace mobject
  {
  class MObject;
  template<class MO> class Placement;
  typedef Placement<MObject> PMO;

  namespace session
    {
    class FixedLocation;
    class RelativeLocation;
    
    
    
    /**
     * Positioning specification, possibliy chained
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
        typedef lumiera::Time Time;
        typedef Time* Track; //TODO dummy declaration; we don't use Tracks as first-class entity any longer
        typedef std::tr1::shared_ptr<asset::Pipe> Pipe;
        typedef std::pair<Time,Pipe> SolutionData;  //TODO (ichthyo consideres better passing of solution by subclass)
        struct LocatingSolution;
      
        /** next additional Pin, if any */
        scoped_ptr<LocatingPin> next_;
        
        /** order to consider when resolving. 0=highest */
        virtual int getPrioLevel ()  const { return 0; }
        
        LocatingPin& addChain (LocatingPin*);
        void resolve (LocatingSolution&)  const;
        virtual void intersect (LocatingSolution&)  const;
        
      public:
        const SolutionData resolve ()  const;
        bool isOverdetermined () const;
        
        /* Factory functions for adding LocatingPins */
        
        FixedLocation&    operator() (Time start, Track track=0);
        RelativeLocation& operator() (PMO refObj, Time offset=0);
        
        LocatingPin (const LocatingPin&);
        LocatingPin& operator= (const LocatingPin&);
        virtual LocatingPin* clone ()  const;
        virtual ~LocatingPin() {};
        
      protected:
        LocatingPin () {};
        
        friend class Placement<MObject>;
        
        /** 
         * @internal helper for the (preliminary)
         * position resolve() implementation.
         * @todo we can't sensibly reason about tracks, 
         * because at the moment (10/07) we lack a track implementation...
         * @todo shouldn't we use a range-restriction LocatingPin (subclass)
         * to represent the to-be-found solution? (ichthyo: siehe Trac #100)
         */  
        struct LocatingSolution
          {
            Time minTime;
            Time maxTime;
            Track minTrack; // TODO don't use Tracks
            Track maxTrack;
            bool impo;
            
            LocatingSolution () 
              : minTime(Time::MAX),  // TODO: better implementation of "unspecified..."
                maxTime(Time::MIN),
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
      };
      
      
      

  } // namespace mobject::session

} // namespace mobject
#endif
