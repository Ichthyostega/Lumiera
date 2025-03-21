/*
  FRAMEID.hpp  -  distinct identification of a single data frame

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file frameid.hpp
 ** Marker tuple to identify a specific frame.
 ** @todo 2016 the intention was to use this as cache key, and to include
 **       some information regarding the processing chain which led up to
 **       the calculation of this frame, to allow for intelligent caching
 **       and to avoid throwing away most of the frames on each and every
 **       tweak of the edit. This marker was added as a preview in 2010
 **       but we didn't get to the point of actually putting that idea
 **       into practical use. Yet the basic idea remains desirable...
 ** @deprecated 10/2024 seems very likely that similar functionality moves down
 **       into the render-engine implementation and will no longer be considered
 **       a constituent of the public interface.
 */


#ifndef LUMIERA_FRAMEID_H
#define LUMIERA_FRAMEID_H


#include <boost/operators.hpp>

namespace lumiera {
  
  
  /**
   * Identification tuple for denoting render nodes unambiguously.
   * 
   * @todo currently (10/08) this is a dummy implementation. We'll find out
   *       what is needed once the outline of the render engine is complete.
   */
  struct NodeID
    {
      /////////TODO: include the segment?
      ulong id;
      ulong generation;
      ulong childSum;
      
      NodeID()
        : id (++currID),
          generation(0),
          childSum(0)
        { }
      
      static ulong currID;
    };
  
  
  
  
  /**
   * Identification tuple for addressing frames unambiguously.
   * 
   * @todo currently (7/08) this is a dummy implementation to find out
   *       what interface the Steam layer needs. Probably the vault layer will
   *       later on define what is actually needed; this header should then
   *       be replaced by a combined C/C++ header
   */
  class FrameID : boost::totally_ordered<FrameID>  ////////////TODO it seems we don't need total ordering, only comparison. Clarify this!
    {
      long dummy;
    public:
      FrameID(long dum=0) : dummy(dum) {}
      operator long () { return dummy; }
      
      bool operator<  (const FrameID& other)  const { return dummy <  other.dummy; }
      bool operator== (const FrameID& other)  const { return dummy == other.dummy; }
    };
  
  
  
  
} // namespace lumiera
#endif
