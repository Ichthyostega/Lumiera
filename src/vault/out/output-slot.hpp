/*
  OUTPUT-SLOT.hpp  -  capability to transfer data to a physical output

   Copyright (C)
     2011,2026        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file output-slot.hpp
 ** An (abstract) capability to send media data to an external output.
 ** OutputSlot is the central metaphor for the organisation of actual (system level) outputs;
 ** using this concept allows to separate and abstract the data calculation and the organisation
 ** of playback and rendering from the specifics of the actual output sink. Actual output
 ** possibilities can be added and removed dynamically from various components (vault, stage),
 ** all using the same resolution and mapping mechanisms
 **
 ** Each OutputSlot is an unique and distinguishable entity. It corresponds explicitly to an
 ** external output, or a group of such outputs (e.g. left and right sound card output channels),
 ** or an output file or similar capability accepting media content. Initially, an output slot
 ** needs to be provided, configured and registered, using an implementation suitable for the
 ** kind of media data to be sent (sound, video) and also suitable for the special circumstances
 ** of the output capability (render a file, display video in a GUI widget, send video to some
 ** full screen display, establish a Jack port, just use some kind of "sound out"). An output
 ** slot is always limited to a single kind of media, and to a single connection unit, but
 ** this connection may still be comprised of multiple channels
 ** (e.g. stereoscopic video, multichannel sound).
 ** 
 ** In order to be usable as output sink, an output slot needs to be \em allocated: At any time,
 ** there may be only a single client using a given output slot this way. To stress this point:
 ** output slots don't provide any kind of inherent mixing capability; any adaptation, mixing,
 ** overlaying and sharing needs to be done within the nodes network producing the output data
 ** to be fed into the slot. (in special cases, some external output capabilities -- e.g. the
 ** Jack audio connection system -- may still provide additional mixing capabilities,
 ** but that's beyond the scope of the Lumiera application)
 ** 
 ** Once allocated, the output slot returns a set of concrete sink handles (one for each
 ** physical channel expecting data). The calculating process feeds its results into those handles.
 ** Size and other characteristics of the data frames are assumed to be suitable, which typically
 ** won't be verified at that level anymore. Besides that, the allocation of an output slot reveals
 ** detailed timing expectations. The client is required to comply to these timings when _emitting_
 ** data -- they are even required to provide a current time specification, alongside with the data.
 ** Based on this information, the output slot has the ability to handle timing failures gracefully;
 ** the concrete output slot implementation is expected to provide some kind of de-click or
 ** de-flicker facility, which kicks in automatically when a timing failure is detected.
 **
 ** @see OutputSlotProtocol_test
 ** @see diagnostic-output-slot.hpp
 */


#ifndef VAULT_OUT_OUTPUT_SLOT_H
#define VAULT_OUT_OUTPUT_SLOT_H


#include "lib/error.hpp"
#include "lib/handle.hpp"
#include "vault/mem/buffhandle.hpp"
#include "vault/out/timings.hpp"
#include "lib/iter-source.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <memory>


namespace vault {
namespace out  {
  
  using vault::mem::BuffHandle;
  using lib::time::FrameCnt;
  using std::unique_ptr;
  
  
  
  class DataSink;
  
  using FrameID = FrameCnt;
  
  
  
  /****************************************************************************//**
   * Interface: Generic output sink.
   * An OutputSlot represents the possibility to send data through multiple
   * channels to some kind of external sink (video in GUI window, video full screen,
   * sound, Jack, rendering to file). Clients are expected to retrieve a suitably
   * preconfigured implementation from some OutputManager instance. An OutputSlot
   * needs to be _claimed_ for output by invoking #allocate, which returns a
   * representation of the connection state. This operation is exclusive.
   * The actual [output sinks](\ref DataSink) can be retrieved
   * through the Allocation object returned from there.
   */
  class OutputSlot
    : util::NonCopyable
    {
      
    public:
      virtual ~OutputSlot();
      
      using OpenedSinks = lib::IterSource<DataSink>::iterator;
      
      class Allocation
        {
        public:
          virtual OpenedSinks getOpenedSinks() =0;
          virtual Timings timingConstraints()  =0;
          
          virtual bool isActive()  const       =0;
          
         ~Allocation(); ///< this is an interface
        };
      
      
      /** can this OutputSlot be allocated? */
      bool isFree()  const;
      
      /** claim this slot for exclusive use */
      Allocation& allocate();
      
      /** disconnect from this OutputSlot
       * @warning may block until DataSinks are gone */
      void disconnect();

      /** established output feed */
      class Connection;
      
      
    protected:
      /** active connections through this OutputSlot */
      template<class CON>
      class AllocState;
      
      unique_ptr<Allocation> state_;
      
      /** build the _connected state,_
       *  based on the existing configuration
       *  within this concrete OutputSlot */
      virtual unique_ptr<Allocation> buildState() =0;
    };
  
  
  
  /**
   * denotes an opened connection ready to receive media data for output.
   * Each DataSink (handle) corresponds to an OutputSlot::Connection entry.
   * Data is handed over frame wise in a two-phase protocol: first, the client
   * gets exclusive access to an output buffer, and then, when done, the buffer
   * is handed over by an #emit call.
   */
  class DataSink
    : public lib::Handle<OutputSlot::Connection>
    {
      
    public:
      BuffHandle lockBufferFor(FrameID);
      
      
      friend bool operator== (DataSink const& sink1, DataSink const& sink2)
      {
        return not (sink1 and sink2)
            or (sink1 and sink2
                and util::isSameObject (sink1.impl(), sink2.impl()));
      }
    };
  
  
  
}} // namespace vault::out
#endif /*VAULT_OUT_OUTPUT_SLOT_H*/
