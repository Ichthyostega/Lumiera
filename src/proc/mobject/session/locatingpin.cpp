/*
  LocatingPin  -  Chaining and constraining the Placement of a Media Object
 
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
 
* *****************************************************/


#include "proc/mobject/placement.hpp"
#include "proc/mobject/session/locatingpin.hpp"
#include "proc/mobject/session/fixedlocation.hpp"
#include "proc/mobject/session/relativelocation.hpp"

namespace mobject
  {
  namespace session
    {
    
    /** it's OK to copy a LocainngPin,
     *  causing duplication of any chained lPins
     */
    LocatingPin::LocatingPin (const LocatingPin& other)
      : next_(other.next_)
    { }
    
    
    LocatingPin& 
    LocatingPin::operator= (const LocatingPin& other)
    {
      if (this!=other) 
        this.next_.reset (other.next_);
      return *this;
    }
    
    
    LocatingPin&
    LocatingPin::addChain (LocatingPin newLp)
    {
      REQUIRE (newLp);
      REQUIRE (!newLp->next_, "can insert only single LocatingPins");
      
      if (next_ && newLp->getPrioLevel() > next_->getPrioLevel())
        return next_->addChain (newLp);
      else
        {
          scoped_ptr<LocatingPin> tmp_next (newLp);
          tmp_next->next_.reset(this.next_);
          this.next_.swap(tmp_next);
          return *newLp;
        }
    }

    
    /** implementing the core Placement functionality.
     *  By combining all the chained locating pins, try
     *  to get at a definite position (for this chain and
     *  consequently for the MObject handled by the enclosing
     *  Placement object).
     *  @todo this could/should be replaced by a full-blown
     *        constraint solver at some point in the future
     */
    const FixedLocation
    LocatingPin::resolve ()  const
    {
      LocatingSolution solution;
      resolve (solution);
      return FixedLocation (solution.getTime(), solution.getTrack());
    }
    
    
    void 
    LocatingPin::resolve (LocatingSolution& solution)  const
    {
      if (solution.is_definite() || solution.is_impossible()) 
        return;
      this.intersect (solution);
      if (this.next_ && 
          !(solution.is_definite() || solution.is_impossible())
         )
        next_->resolve(solution);
    }
    
    
    void 
    LocatingPin::intersect (LocatingSolution& solution)  const
    {
      REQUIRE (!solution.is_definite() && !solution.is_impossible());
      // base class Implementation is NOP...
    }
    
    
    /** get some time value which could stand in for this
     *  solution. This doesn't imply this value is a solution,
     *  It's just a value we can use. At the moment (10/07),
     *  LocatingSolution is implemented as interval, and
     *  we return the lower bound here. 
     */
    LocatingPin::Time 
    LocatingPin::LocatingSolution::getTime()
    {
      return minTime;   
    }
    
    LocatingPin::Track
    LocatingPin::LocatingSolution::getTrack()
    {
      UNIMPLEMENTED ("get effective Track number of Solution");
    }
    
    
    bool 
    LocatingPin::LocatingSolution::is_definite()  ///< found a solution?
    {
      return (minTime == maxTime && minTrack == maxTrack);
    }
    
    bool 
    LocatingPin::LocatingSolution::is_impossible()
    {
      if (minTime > maxTime)  impo = true;
      TODO ("track???");
      return impo;
    }
    
    
    

    
    
    /* === Factory functions for adding LocatingPins === */
    
    FixedLocation&    
    LocatingPin::operator() (Time, Track)
    {
      return static_cast<FixedLocation&> 
                (this.addChain (*new FixedLocation (Time, Track)));
    }
    
    
    RelativeLocation& 
    LocatingPin::operator() (PMO refObj, Time offset=0)
    {
      return static_cast<RelativeLocation&> 
                (this.addChain (*new RelativeLocation (Time, Track)));
    }
    
    
    
    
  } // namespace mobject::session

} // namespace mobject
