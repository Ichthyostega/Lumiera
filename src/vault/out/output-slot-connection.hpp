/*
  OUTPUT-SLOT-CONNECTION.hpp  -  implementation API for concrete output slots

   Copyright (C)
     2011,2026        Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file output-slot-connection.hpp
 ** Interface for concrete output implementations to talk to the OutputSlot frontend.
 ** This setup with an OutputSlot as façade helps to decouple the render engine implementation
 ** from the details of handling any kind of external output connection. The act of data output
 ** is reduced to the operations described by the OutputSlot::Connection interface.
 ** 
 ** # Interplay of parts involved into an actual implementation
 ** 
 ** Each distinct output capability must be registered with an OutputManager (and this is another
 ** topic beyond the scope of the interaction described here). When a client wants to output data,
 ** it obtains a suitable OutputSlot through the OutputDirector. This OutputSlot is created and
 ** initialised properly for this single use by the OutputManager in charge for that kind of
 ** output capability; notably it must be outfitted with an actual implementation of the
 ** OutputSlot::Allocation interface, and typically OutputSlot::AllocState<CON> will be
 ** used for that purpose.
 ** 
 ** This allocation state is linked at implementation level to some specific implementation of the
 ** OutputSlot::Connection interface, represented as template parameter \a CON. An instance will be
 ** created for each distinct data feed — which applies only if the media stream type in question
 ** comprises several data feeds running in parallel; here a data feed does not necessarily correspond
 ** to a _channel_, since it is quite common to _interleave_ several channels in a single feed. In many
 ** typical cases, only one data feed and thus only a single Connection instance is required. The client
 ** can then retrieve a DataSink handle for each data feed. Notably, each DataSink participates in the
 ** ref-count and thus keeps the OutputSlot and the Connection(s) alive.
 ** Furthermore, [timing constraints](\ref vault::out::Timings) must be defined, to instruct the client
 ** when precisely to deliver the data for each prospective frame (time window of delivery). Obviously,
 ** details depend on the underlying technology. In some cases, the client might prepare several buffers
 ** full of data in advance, while for another technology, that, for example, relies on _double buffering_,
 ** the client has to meet a single precisely limited time window immediately prior to each _buffer flip_.
 ** 
 ** The client code interacts with the output data buffer through a BuffHandle as front-end. This access
 ** scheme is similar to what is used by the generic vault::mem::BufferProvider within the Render Engine.
 ** To enable this setup, the OutputSlot::AllocState embeds a _proxy BufferProvider_ and uses a preconfigured
 ** [buffer (type) descriptor](\ref vault::mem::BuffDescr) for each connection, which also includes information
 ** about the size of the buffer, as obtained [from the connection](\ref OutputSlot::Connection::getBufferSize).
 ** To start work on a new frame, the client invokes DataSink, which actually is a functor. This call delegates
 ** through the proxy BufferProvider — see \ref OutputBufferProxy::provideBuffer() and thus ends up calling
 ** into Connection::claimBufferFor(frameNr).
 ** 
 ** ## Threadsafe production-grade implementation
 ** 
 ** OutputSlot operates as part of the BufferProvider framework within the Render Engine; it must thus
 ** be capable to handle concurrent access. The construction of a OutputSlot instance happens from a single thread,
 ** which is typically the session thread (since the output connection is created as part of starting a play process).
 ** Immediately at construction, an AllocState must be provided, which in turn creates all the connection objects.
 ** The activation of these connections and the access to output buffers (»lock a buffer) will then be initiated
 ** from the worker threads. It can be assumed that a single worker processes the job for some output frame, but
 ** other workers might start processing other output frames concurrently, assuming the underlying connection
 ** technology allows to produce output in advance this way (which is rather uncommon for real-time output,
 ** where typically a double-buffering scheme is employed, which implies that at any given time only one
 ** single output buffer can be exposed; yet for other kinds of output, like e.g. memory mapped file
 ** output, several frames can be delivered in parallel).
 ** 
 ** Notably this implies that the connection implementation must be prepared for receiving data concurrently.
 ** Other than that, the OutputBufferProxy will use a LocalSlice<BufferMetadata>, and thus a thread-local
 ** metadata table, in concert with the EngineBufferMetadata as central hub. The link to a connection object
 ** for a single channel is represented as a [»buffer type«](\ref BuffDescr). When a worker »locks« some
 ** frame, the BufferProvider implementation will access the thread-local metadata store to track the
 ** state transitions; on first usage it will have to sync-down the type descriptor from the central hub.
 ** 
 ** For the purpose of unit testing, a simple self-contained metadata table can be used instead, which has the
 ** advantage that all state is discarded when deleting the OutputSlot object. In fact, most unit tests rely on
 ** the \ref DiagnosticOutputSlot, which sets the `isTest` template parameter internally to use this variant.
 ** 
 ** @see OutputSlotProtocol_test
 ** @see OutputAllocState_test
 */


#ifndef VAULT_OUT_OUTPUT_SLOT_CONNECTION_H
#define VAULT_OUT_OUTPUT_SLOT_CONNECTION_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "vault/mem/buffhandle.hpp"
#include "vault/out/output-slot.hpp"
#include "vault/out/output-buffer-proxy.hpp"
#include "lib/scoped-collection.hpp"
#include "lib/iter-explorer.hpp"
#include "lib/util.hpp"

#include <utility>


namespace vault {
namespace out   {
  
  using vault::mem::Buff;
  using vault::mem::BuffHandle;
  using util::unConst;
  
  
  /**
   * # OutputSlot core implementation API
   * 
   * This interface describes the operations used by OutputSlot to interact
   * with the actual output technology. Essentially, the task of providing
   * a specific OutputSlot consists primarily of implementing and integrating
   * this Connection interface: for each individual data feed, an instance will
   * be established as part of the [allocation state](\ref OutputSlot::AllocState).
   * The general assumption is that some _data buffer_ will be handed-over in a
   * timely fashion for each data frame. The interface methods thus reflect the
   * stages of the »Output Slot Protocol« and will be used in sequence for each frame:
   * - `claimBufferFor(FrameID)` announces a FrameID and exposes a data buffer of suitable
   *   size for exclusive use by the client.
   * - `publish(buffer)` marks the end of client sided processing and passes the
   *   data in the buffer to the output mechanism for publication.
   * - `release(buffer)` indicates that the client will not touch this buffer any more,
   *   so that it can be repurposed for outputting another frame.
   * @remark _by convention_ the client is required to invoke `release()` reliably,
   *   even in cases where `publish()` is skipped (and thus no actual data was provided.
   *   However, invoking `publish()` also transfers ownership, and thus it can be assumed
   *   that the client will not touch the buffer after this point, so the invocation of
   *   `release()` can be ignored after `publish()`. In addition to this sequence,
   *   the client is also bound by the [timing constraints](\ref Timings), yet timing
   *   glitches and missed deadlines can not be precluded; the Connection is expected
   *   to check current time and handle a frame as failure transparently, when
   *   `publish()` is invoked too late, but must keep the buffer in locked / exclusive
   *   state, since the client might still be writing data, even after the deadline.
   * @note the meaning of FrameID is implementation defined.
   * @warning the Connection implementation must be threadsafe; while there will be never
   *   any contention regarding a single frame number, consecutive frames can and will
   *   be delivered by different threads, concurrently.
   */
  class OutputSlot::Connection
    : util::NonCopyable
    {
    public:
      virtual ~Connection();
      
      virtual size_t getBufferSize()  const =0;
      virtual Buff* claimBufferFor(FrameID) =0;
      virtual void publish (Buff*)          =0;
      virtual void release (Buff*)          =0;
      virtual void shutDown ()              =0;
    };
  
  
  
  /* ===== Building blocks for OutputSlot implementation ===== */
  
  /**
   * Implementation of active Allocation state: maintaining
   * a list of active connections. An instance is created whenever
   * a client _locks_ (and thereby »allocates«) the OutputSlot. The
   * actual implementation of OutputSlot::buildState() will employ a
   * custom Connection, where all the actual state handling and data
   * transport is implemented. Such a Connection subclass is also what
   * is referenced by the DataSink smart-hendle, that is handed out
   * to the client code and allows to mark the state transitions.
   * Theses smart-handles are ref counting and thus the individual
   * Connection object is kept alive as long as at least one DataSink
   * handle is retained.
   * @tparam isTest use a simple BufferMetadata table in OutputBufferProxy.
   */
  template<class CON, bool isTest=false>
  class OutputSlot::AllocState
    : public OutputSlot::Allocation
    {
      using Connections = lib::ScopedCollection<CON>;
      using OpenedSinks = OutputSlot::OpenedSinks;
      using BufferProxy = OutputBufferProxy<Connection, isTest>;
      
      Connections connections_;
      BufferProxy bufferProxy_;
      
      
      /* == Allocation Interface == */
      
      void
      release()  override
        {
          for (Connection& con : connections_)
            con.shutDown();
        }
      
      Timings
      getTimings()  override
        { //////////////////TODO somehow configure the timings into this implementation object
          UNIMPLEMENTED ("a lot regarding the Timings is still not clear...");                   //////////////////////////TICKET #831
        }
      
      /**
       * Construct an iterator pipeline over the #connections_,
       * where each element is a DataSink functor that uses this connection
       * to start an output cycle by locking a buffer by invoking Connection::claimBufferFor(FrameID)
       * @param lifecycleManager the ref-counting shared_ptr which controls the lifecycle of this Allocation
       * @note two statefull objects are embedded into the DataSink handle
       *     - a BuffDescr preconfigured to point into this connection, using the proper buffer size
       *     - the \a lifecycleManager, so that the DataSink participates in the ref-count
       */
      OpenedSinks
      connect (PAlloc& lifecycleManager)  override
        {
          auto buildDataSink = [&](CON& connection)
                                  {  // attach to ref-count preconfigure output connection to address
                                    return DataSink{lifecycleManager, bufferProxy_.getDescriptorFor (connection)};
                                  };
          
          return lib::explore (connections_)
                    .transform(buildDataSink)
                    .asIterSource();
        }
      
      
      
    public:
      /**
       * OutputSlot is built with an activated Allocation that engages a specific implementation
       * of the Connection interface. A _populator functor_ is passed, that will emplace instances
       * of this connection implementation into the provided storage, one for each expected data feed.
       * After the AllocState is created, the client will invoke OutputSlot::getOpenedSinks(), which
       * uses AllocState::connect() to configure a DataSink handle for each feed.
       * @see ScopedCollection_test::building_RAII_Style()
       * @see OutputBufferProxy::OutputBufferStore::provideBuffer()
       */
      template<class CTOR>
      AllocState(uint numDataFeeds, CTOR&& populator)
        : connections_(numDataFeeds, std::forward<CTOR>(populator))
        , bufferProxy_{}
        { }

      /** interface used by the populator functor to emplace connection objects */
      using ConnectionStorage = Connections::ElementHolder;
    };
  
  
  
  
  /* ===== OutputSlot factory functions ===== */
  
  /**
   * @remark this is the generic builder variant, using a _populator functor_
   *   that is passed directly to the constructor of lib::ScopedConnection
   */
  template<class CON, class FUN>
  OutputSlot
  OutputSlot::allocate (size_t cnt, FUN&& populator)
    {
      return OutputSlot{std::make_unique<AllocState<CON>> (cnt, std::forward<FUN>(populator))};
    }

  /**
   * @remark for most cases this variant is preferable,
   *   since it creates all connection objects from the same set of constructor arguments
   * @note args will be copied into a _populator functor_ that is then applied for each connection to create
   */
  template<class CON, typename...ARGS>
    requires std::is_constructible_v<CON, ARGS...>
  OutputSlot
  OutputSlot::allocate (size_t cnt, ARGS&&... args)
    {
      using Connections = lib::ScopedCollection<CON>;
      return OutputSlot{std::make_unique<AllocState<CON>> (cnt, Connections::fill(std::forward<ARGS>(args)...) )};
    }

  
}} // namespace vault::out
#endif /*VAULT_OUT_OUTPUT_SLOT_CONNECTION_H*/
