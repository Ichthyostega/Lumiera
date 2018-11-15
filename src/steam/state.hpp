/*
  STATE.hpp  -  Key Interface representing a render process and encapsulating state

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


/** @file state.hpp
 ** Access point to an ongoing render's processing state.
 */


#ifndef STEAM_INTERFACE_STATE_H
#define STEAM_INTERFACE_STATE_H


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
  
  
  /**
   * Abstraction to access the state of a currently ongoing render/calculation process,
   * as it is tied to the supporting facilities of the vault layer. An State (subclass) instance
   * is the sole connection for the render node to invoke services of the vault needed
   * to carry out the calculations.
   * 
   * @see engine::RenderInvocation top-level entrance point
   * @see nodeinvocation.hpp impl. used from \em within the nodes
   */
  class State
    {
                                   ////////////////////////////////////////////////TICKET #826  expected to be reworked to quite some extent (9/2011)
    public:
      /** allocate a new writable buffer with type and size according to
       *  the BufferDescriptor. The actual provider of this buffer depends
       *  on the State implementation; it could be a temporary, located in
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
      virtual ~State() {};
      
      /** resolves to the State object currently "in charge".
       *  Intended as a performance shortcut to avoid calling
       *  up through a chain of virtual functions when deep down
       *  in chained ProcNode::pull() calls. This allows derived
       *  classes to proxy the state interface.
       */ 
      virtual State& getCurrentImplementation () =0;
      
      friend class engine::StateAdapter;
    };
  
}} // namespace steam::engine



namespace proc_interface {

  using steam::engine::State;
    
    
} // namespace proc_interface
#endif
