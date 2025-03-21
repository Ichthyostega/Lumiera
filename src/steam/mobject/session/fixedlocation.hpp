/*
  FIXEDLOCATION.hpp  -  directly positioning a MObject to a fixed location   

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file fixedlocation.hpp
 ** Specialised LocatingPin for use in Placement, especially for globally fixed positions
 ** The FixedLocation is assumed to play a central role within the Build process, which
 ** ultimately aims ad resolving any part of the session into either a wiring directive
 ** or a piece of media or processing to happen at a location fixed in time.
 ** 
 ** @todo stalled effort towards a session implementation from 2008
 ** @todo 2016 likely to stay, but expect some extensive rework
 */


#ifndef STEAM_MOBJECT_SESSION_FIXEDLOCATION_H
#define STEAM_MOBJECT_SESSION_FIXEDLOCATION_H

#include "steam/mobject/session/locatingpin.hpp"



namespace steam {
namespace mobject {
  
  class ExplicitPlacement; //TODO trac #100
  
  namespace session {
    
    /** 
     * The most common case of positioning a MObject
     * in the Session: directly specifying a constant position.
     * @todo use a subclass to represent the LocatingSolution?
     *       would solve the construction of a ExplicitPlacement
     *       much more natural. //////////TICKET #100
     */
    class FixedLocation : public LocatingPin
      {
        Time time_;
        Fork fork_;
        
        friend class ExplicitPlacement; //////////////////////TICKET #100
        
      protected:
        FixedLocation (Time ti, Fork fo) : time_(ti), fork_(fo) {};
        friend class LocatingPin;
        
        virtual void intersect (LocatingSolution&)  const;
        
      public:
        virtual FixedLocation* clone ()  const;
        
      };
    
    
    
}}} // namespace steam::mobject::session
#endif
