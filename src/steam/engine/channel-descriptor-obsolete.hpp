/*
  CHANNEL-DESCRIPTOR.hpp  -  Channel / Buffer type representation for the engine

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file channel-descriptor-obsolete.hpp
 ** Representation of the Media type of a data channel used within the engine.
 ** 
 ** @todo as it stands (11/2011) this file is obsoleted and needs to be refactored,
 **       alongside with adapting the node invocation to the new BufferProvider interface.
 ** @todo as of 5/2024, this will be retained as part of the `FeedManifold`,
 **       which in turn takes on some aspects of the old buffer table draft.
 ** @deprecated 12/2024 it is now clear that the low-level-Model will be really low-level,
 **       implying that any connection to the usage domain will be completely resolved in the builder
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


namespace steam {
namespace engine {
  namespace error = lumiera::error;
  
  using lib::HashVal;
  
  class BuffDescr;
  class BuffHandle;
  class BufferProvider;
  
  
  
  class ProcNode_Obsolete;
  typedef ProcNode_Obsolete* PNode;
  
  
  struct ChannelDescriptor  ///////TODO really need to define that here? it is needed for node wiring only
    {
      const lumiera::StreamType * bufferType;                /////////////////////////////////////////TICKET #828
    };
  
  struct InChanDescriptor : ChannelDescriptor
    {
      PNode dataSrc;    ///< the ProcNode to pull this input from
      uint srcChannel; ///<  output channel to use on the predecessor node
    };
  
  
  
}} // namespace steam::engine
#endif
