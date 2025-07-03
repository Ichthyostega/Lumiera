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
 ** @todo 6/2025 basically everything here is unused or will likely be done
 **       in a different way — expect this and rendergraph.cpp to be obsolete
 */


#ifndef LUMIERA_FRAMEID_H
#define LUMIERA_FRAMEID_H


#include "lib/integral.hpp"
#include <compare>

namespace lumiera {
  
  
  /**
   * Identification tuple for denoting render nodes unambiguously.
   * 
   * @todo currently (10/08) this is a dummy implementation. We'll find out
   *       what is needed once the outline of the render engine is complete.
   * @todo 5/2025 meanwhile a new effort to define the processing nodes was successful,
   *       but it is still not clear if the »Render Nodes« will expose any runtime logic
   *       beyond the mere ability to be invoked.
   *       ** This is a placeholder**  — we are using ProcID instead
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
  class FrameID
    {
      long dummy;
    public:
      FrameID(long dum=0) : dummy(dum) {}
      operator long () { return dummy; }
      
      auto operator<=> (FrameID const&) const  =default;
    };
  
  
  
  
} // namespace lumiera
#endif
