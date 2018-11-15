/*
  CHANNEL-DESCRIPTOR.hpp  -  Channel / Buffer type representation for the engine

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

/** @file channel-descriptor.hpp
 ** Representation of the Media type of a data channel used within the engine.
 ** 
 ** @todo as it stands (11/2011) this file is obsoleted and needs to be refactored,
 **       alongside with adapting the node invocation to the new BufferProvider interface.
 ** 
 ** @see nodewiring-def.hpp
 ** @see nodeoperation.hpp
 ** @see bufftable-obsolete.hpp       storage for the buffer table
 ** @see engine::RenderInvocation
 */

#ifndef ENGINE_CHANNEL_DESCRIPTOR_H
#define ENGINE_CHANNEL_DESCRIPTOR_H


#include "lib/error.hpp"
#include "lib/hash-value.h"
#include "steam/streamtype.hpp"


namespace proc {
namespace engine {
  
  namespace error = lumiera::error;
  using error::LERR_(LIFECYCLE);
  
  using lib::HashVal;
  
  class BuffHandle;
  class BufferProvider;
  class BufferDescriptor;
  
  
  
  class ProcNode;
  typedef ProcNode* PNode;
  
  
  struct ChannelDescriptor  ///////TODO really need to define that here? it is needed for node wiring only
    {
      const lumiera::StreamType * bufferType;                /////////////////////////////////////////TICKET #828
    };
  
  struct InChanDescriptor : ChannelDescriptor
    {
      PNode dataSrc;    ///< the ProcNode to pull this input from
      uint srcChannel; ///<  output channel to use on the predecessor node
    };
  
  
  
}} // namespace proc::engine
#endif
