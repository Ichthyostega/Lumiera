/*
  STATE-CLOSURE.hpp  -  Key Interface representing a render process and encapsulating state

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file state-closure.hpp
 ** Access point to an ongoing render's processing state.
 ** 
 ** @deprecated 2024 Node-Invocation is reworked from ground up for the »Playback Vertical Slice«
 */


#ifndef STEAM_ENGINE_STATE_CLOSURE_OBSOLETE_H
#define STEAM_ENGINE_STATE_CLOSURE_OBSOLETE_H


#include "lib/error.hpp"
#include "lib/frameid.hpp"
#include "steam/engine/buffhandle.hpp"

#include <cstddef>


namespace steam {
namespace engine { 
  
  using lumiera::FrameID;
  using lumiera::NodeID;

  class StateAdapter;
  class BuffTableStorage;
  
  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
  /**
   * Abstraction to access the state of a currently ongoing render/calculation process,
   * as it is tied to the supporting facilities of the vault layer. An StateClosure (subclass) instance
   * is the sole connection for the render node to invoke services of the vault needed
   * to carry out the calculations.
   * 
   * @see engine::RenderInvocation top-level entrance point
   * @see nodeinvocation.hpp impl. used from \em within the nodes
   */
  class StateClosure_OBSOLETE
    {
                                   ////////////////////////////////////////////////TICKET #826  expected to be reworked to quite some extent (9/2011)
    public:
      /** allocate a new writable buffer with type and size according to
       *  the BuffDescr. The actual provider of this buffer depends
       *  on the StateClosure implementation; it could be a temporary, located in
       *  the cache, used for feeding calculated frames over a network, etc.
       *  @return a BuffHandle encapsulating the information necessary to get
       *          at the actual buffer address and for releasing the buffer.
       */
      virtual BuffHandle allocateBuffer (const lumiera::StreamType*)  =0;
      
      /** resign control of the buffer denoted by the handle */
      virtual void releaseBuffer (BuffHandle&)  =0;
      
      /** declare the data contained in the Buffer to be ready.
       *  The caller is required to restrain itself from modifying the data
       *  afterwards, as this buffer now can be used (readonly) by other
       *  calculation processes in parallel.
       */
      virtual void is_calculated (BuffHandle const&)  =0;
      
      /** generate (or calculate) an ID denoting a media data frame
       *  appearing at the given position in the render network,
       *  for the time point this rendering process is currently 
       *  calculating data for.
       *  @param NodeID the unique identification of a specific node
       *  @param chanNo the number of the output channel of this node
       *  @return a complete FrameID which unambiguously denotes this
       *        specific frame and can be used for caching 
       */
      virtual FrameID const& genFrameID (NodeID const&, uint chanNo)  =0;
      
      /** try to fetch an existing buffer containing the denoted frame
       *  from a cache or similar backing system (e.g. network peer).
       *  @return either a handle to a readonly buffer, or a null handle
       *  @note the client is responsible for not modifying the provided data
       */
      virtual BuffHandle fetch (FrameID const&)  =0;
      
      /** necessary for creating a local BuffTableChunk */
      virtual BuffTableStorage& getBuffTableStorage()  =0; 
      
      
    protected:
      virtual ~StateClosure_OBSOLETE() {};
      
      /** resolves to the StateClosure object currently "in charge".
       *  Intended as a performance shortcut to avoid calling
       *  up through a chain of virtual functions when deep down
       *  in chained ProcNode::pull() calls. This allows derived
       *  classes to proxy the state interface.
       */ 
      virtual StateClosure_OBSOLETE& getCurrentImplementation () =0;
      
      friend class engine::StateAdapter;
    };
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_STATE_CLOSURE_OBSOLETE_H*/
