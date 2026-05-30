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
 ** using this pattern allows to separate and abstract the data calculation and the organisation
 ** of playback and rendering from the specifics of the actual output sink. Actual output
 ** possibilities can be added and removed dynamically from various components (vault, stage),
 ** all using the same resolution and mapping mechanisms
 **
 ** Each OutputSlot is an unique and distinguishable entity. It corresponds explicitly to an
 ** external output, or a group of such outputs (e.g. left and right sound card output channels),
 ** or an output file or similar capability accepting media content. The setup for an OutputSlot
 ** is provided, configured and registered with an OutputManager, using a suitable implementation
 ** for the kind of media data to be sent (sound, video), and also suitable for the desired kind of
 ** output capability (render to file, display video in a GUI widget, send video to a full screen
 ** display, establish a Jack port or use any kind of "sound out"). Thus it is always limited to
 ** a single kind of media, and to a single connection unit, but this connection may still comprise
 ** several data feeds (e.g. stereoscopic video, multichannel sound). The number of such feeds
 ** and the expected data format depends on the _media stream type_ used for configuring or
 ** retrieving this OutputSlot. For example, a stereo sound output may expect only a single
 ** data feed, where data blocks for both channels are sent in interleaved format.
 ** 
 ** Once the client retrieves some OutputSlot, it was _allocated_ for exclusive use for that client.
 ** At any time, there may be only a single client using a given output capability this way. It should
 ** be noted thus that output slots don't provide any kind of inherent mixing capability; any adaptation,
 ** mixing, overlaying and sharing needs to be done within the Render Node Network that produces the
 ** data to be output. (As an aside, _some_ external output frameworks — e.g. the Jack audio connection
 ** system — may still provide additional mixing capabilities, yet these are beyond the scope of the
 ** Lumiera application)
 ** 
 ** The OutputSlot object is a (copyable) front-end handle, that should be stored by the client and
 ** discarded when further output is no longer required. From the OutputSlot, a set of actual DataSink
 ** handles can be retrieved (one for each possible data feed). Starting from these handles, a two-phase
 ** output protocol can be executed for each frame of data to send to the output. This includes obtaining
 ** a BuffHandle, similar to what the BufferProvider exposes. The suitability of the buffer and the data
 ** format is not validated in any way at that level, since the setup works under the assumption that
 ** the stream type and further metadata has been validated or configured suitably at a higher level.
 ** However, the OutputSlot exposes a definition of _timing constraints_ to describe when data must
 ** be provided, and what deadline to observe for each frame. The client is required to comply to these
 ** vault::out::Timings both when acquiring the BuffHandle for some frame and when _emitting_ data --
 ** any use outside these limits will cause that frame to be treated as a glitch. Furthermore,
 ** the client is required to invoke BuffHandle::release() as soon as no further access to
 ** the buffer is required (and after invoking `emit()`). Failure to do so will cause
 ** a blocked buffer and typically leads to abort of the output and play process.
 ** However, individual frames that miss the deadline are handled gracefully;
 ** the actual implementation is expected to provide some kind of de-click
 ** or de-flicker facility, which kicks in automatically whenever
 ** a timing failure is detected.
 ** 
 ** @see OutputSlotProtocol_test
 ** @see diagnostic-output-slot.hpp
 */


#ifndef VAULT_OUT_OUTPUT_SLOT_H
#define VAULT_OUT_OUTPUT_SLOT_H


#include "lib/error.hpp"
#include "vault/mem/buffhandle.hpp"
#include "vault/out/timings.hpp"
#include "lib/iter-source.hpp"
#include "lib/nocopy.hpp"
#include "lib/util.hpp"

#include <functional>
#include <memory>


namespace vault {
namespace out  {
  
  using vault::mem::BuffHandle;
  using lib::time::FrameCnt;
  using std::unique_ptr;
  
  
  using FrameID = FrameCnt;
  
  
  /**
   * Handle to represent an opened connection ready to receive media data for output.
   * Each DataSink (handle) corresponds to an OutputSlot::Connection entry. Data is
   * published frame wise in a two-phase protocol: in the first stage, the client
   * gets exclusive access to an output buffer. Once the data is ready for output,
   * the client signals `emit()` and the buffer enters the second stage, where the
   * intern output mechanism of that specific connection gets exclusive access.
   * 
   * The DataSink handle is a _functor_ and can be invoked with a frame number,
   * to start such a two-phase transaction for _this specific frame._ The result
   * is a BuffHandle, to be used in the usual way as with a BufferProvider:
   * - the client can access the buffer memory, either raw or with a forced cast
   * - the client invokes BuffHandle::emit() to indicate that data is ready for output
   * - once the client is done with that handle, it **must** invoke BuffHandle::release()
   * Each output mechanism defines specific constraints regarding the time window when
   * to get such a BuffHandle and when `emit()` must have been called. These constraints
   * are indicated by the vault::out::Timings, that can be retrieved from the OutputSlot.
   * 
   * @note DataSink embeds a ref-counting handle to detect automatically when some
   *       connection can be released; the _allocation of an OutputSlot_ is released
   *       once all connections were discarded.
   */
  class DataSink
    : public std::function<BuffHandle(FrameID)>
    {
    public:
                                                                /////////////////////////////////////////////TICKET #1377 : presumably we want some stream-type hash ID here -- to allow building a processing-ID for each Render Node invocation
    };
  
  
  
  /****************************************************************************//**
   * Interface: Generic output sink.
   * An OutputSlot represents the possibility to send data through multiple
   * channels to some kind of external sink (video in GUI window, video full screen,
   * sound, Jack, rendering to file). Clients are expected to retrieve a suitably
   * preconfigured instance from some OutputManager. At that point, the given
   * OutputSlot is already activated and reserved for exclusive use for the client.
   * The further lifecycle is managed by ref-count, which implies that OutputSlot
   * is a (copyable) front-end handle. For the actual output, the client has to
   * [retrieve the DataSink handles](\ref getOpenedSinks()), one for each independent
   * data feed associated with this OutputSlot. How many feeds this are, depends on
   * the _media stream type_ for which this OutputSlot was configured. The retrieved
   * DataSink handles also participate in the ref-count based connection state; once
   * the OutputSlot and all DataSink handles are discarded, the output connection
   * and all associated resources are released.
   * @see DataSink for the next steps to perform on the handles for output
   *      in compliance with the »Output Slot protocol«.
   */
  class OutputSlot
    {
      
    public:
      using OpenedSinks = lib::IterSource<DataSink>::iterator;
      
      OpenedSinks getOpenedSinks();
      Timings timingConstraints();
      
      /** is this OutputSlot allocated/activated? */
      bool
      isActive()  const
        {
          return bool(alloc_);
        }

      /** established output feed */
      class Connection;
      
      
    protected:
      
      /** @internal interface for the allocated state */
      class Allocation;
      using PAlloc = std::shared_ptr<Allocation>;
      
      PAlloc alloc_;
      
      class Allocation
        : util::NonCopyable
        {
        public:
          virtual ~Allocation(); ///< this is an interface
          
          virtual void release()                =0;
          virtual Timings getTimings()          =0;
          virtual OpenedSinks connect (PAlloc&) =0;
        };
      
      PAlloc
      connect (unique_ptr<Allocation>);
      
      /**
       * @internal Implementation subclass:
       *   manage active connections through this OutputSlot
       */
      template<class CON, bool isTest=false>
      class AllocState;
      
      
    public:
      /** by default marked as inactive/defunct */
      OutputSlot() = default;
      
      /**
       * Build an OutputSlot that is in  _connected state_.
       * @param allocation fully configured for a specific kind of Connection.
       */
      OutputSlot (unique_ptr<Allocation> allocation)
        : alloc_{connect (move(allocation))}
        { }
      
      // standard copy operations acceptable
      
      
      /**
       * Builder: allocate a set of connections of the given type
       * @note usually there is no need to subclass OutputSlot or AllocState,
       *       rather this factory should be used to establish an activated
       *       OutputSlot configured to use a specific OutputSlot::Connection.
       * @see output-slot-connection.hpp
       */
      template<class CON, typename...ARGS>
        requires std::is_constructible_v<CON, ARGS...>
      static OutputSlot allocate (size_t cnt, ARGS&&... args);
      
      template<class CON, class FUN>
      static OutputSlot allocate (size_t cnt, FUN&& populator);
    };
  
  
}}// namespace vault::out
#endif /*VAULT_OUT_OUTPUT_SLOT_H*/
