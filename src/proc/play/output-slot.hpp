/*
  OUTPUT-SLOT.hpp  -  capability to transfer data to a physical output

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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
 ** data -- he's even required to provide a current time specification, alongside with the data.
 ** Based on this information, the output slot has the ability to handle timing failures gracefully;
 ** the concrete output slot implementation is expected to provide some kind of de-click or
 ** de-flicker facility, which kicks in automatically when a timing failure is detected.
 **
 ** @see OutputSlotProtocol_test
 ** @see diagnostic-output-slot.hpp
 */


#ifndef PROC_PLAY_OUTPUT_SLOT_H
#define PROC_PLAY_OUTPUT_SLOT_H


#include "lib/error.hpp"
#include "lib/handle.hpp"
#include "lib/time/timevalue.hpp"
#include "proc/engine/buffer-provider.hpp"
#include "proc/play/timings.hpp"
#include "lib/iter-source.hpp"
#include "lib/nocopy.hpp"

#include <memory>


namespace proc {
namespace play {
  
  using proc::engine::BuffHandle;
  using proc::engine::BufferProvider;
  using lib::time::FrameCnt;
  using lib::time::TimeValue;
  using lib::time::Time;
  
  using std::unique_ptr;
  
  
  
  class DataSink;
  
  typedef FrameCnt FrameID;
  
  
  
  /****************************************************************************//**
   * Interface: Generic output sink.
   * An OutputSlot represents the possibility to send data through multiple
   * channels to some kind of external sink (video in GUI window, video full screen,
   * sound, Jack, rendering to file). Clients are expected to retrieve a suitably
   * preconfigured implementation from some OutputManager instance. An OutputSlot
   * needs to be \em claimed for output by invoking #allocate, which returns a
   * representation of the connection state. This operation is exclusive.
   * The actual \link DataSink output sinks \endlink can be retrieved
   * through the Allocation object returned from there.
   */
  class OutputSlot
    : util::NonCopyable
    {
      
    protected:
      
      /** active connections through this OutputSlot */
      class ConnectionState;
      
      unique_ptr<ConnectionState> state_;
      
      /** build the \em connected state,
       *  based on the existing configuration
       *  within this concrete OutputSlot */
      virtual ConnectionState* buildState() =0;
      
      
    public:
      virtual ~OutputSlot();
      
      typedef lib::IterSource<DataSink>::iterator OpenedSinks;
      
      class Allocation
        {
        public:
          virtual OpenedSinks getOpenedSinks()   =0;
          virtual bool isActive()  const         =0;
          
          virtual Timings getTimingConstraints() =0;
          
        protected:
         ~Allocation(); ///< never to be managed by clients directly 
        };
      
      /** established output channel */
      class Connection;
      
      
      /** can this OutputSlot be allocated? */
      bool isFree()  const;
      
      /** claim this slot for exclusive use */
      Allocation& allocate();
      
      /** disconnect from this OutputSlot
       * @warning may block until DataSinks are gone */
      void disconnect();
      
    private:
      
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
      void emit(FrameID, BuffHandle const&, TimeValue currentTime = Time::ANYTIME); 
    };
  
  
  
  
  
}} // namespace proc::play
#endif
