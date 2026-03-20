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
 ** The OutputSlot concept helps to decouple the render engine implementation from the details
 ** of handling external output connections. For this to work, a concrete implementation of such
 ** an external output needs to integrate with the generic OutputSlot frontend, as used by the
 ** engine. This generic frontend uses a PImpl, pointing to a AllocState object, which embodies
 ** the actual implementation. Moreover, this actual implementation is free to use specifically crafted
 ** OutputSlot::Connection elements to handle the ongoing output for individual channels. The latter
 ** thus becomes the central implementation side API for providing actual output capabilities.
 **
 ** @see OutputSlotProtocol_test
 ** @see diagnostic-output-slot.hpp  ////TODO
 */


#ifndef VAULT_OUT_OUTPUT_SLOT_CONNECTION_H
#define VAULT_OUT_OUTPUT_SLOT_CONNECTION_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "vault/out/output-slot.hpp"
#include "vault/out/output-buffer-proxy.hpp"
#include "lib/scoped-collection.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/iter-source.hpp"
#include "lib/handle.hpp"
#include "lib/time/timevalue.hpp"
//#include "steam/engine/buffer-provider.hpp"
//#include "steam/play/timings.hpp"
//#include "lib/sync.hpp"
#include "vault/mem/buffer-provider.hpp"

//#include <string>
#include <functional>
#include <utility>
//#include <vector>
//#include <memory>


namespace vault {
namespace out   {

  using vault::mem::BuffHandle;
  using vault::mem::BufferProvider;
  using lib::time::TimeValue; /////////////////OOO Rly?
//using std::string;
  using lib::iter_stl::eachElm;
  
//using std::placeholders::_1;
//using std::bind;
  using std::vector;
//using std::shared_ptr;
  
  
  
  
  /** @internal represents the \em active
   *   point in each of the per-channel connections
   *   used when this OutputSlot is operational.
   * 
   * # OutputSlot Core API
   * 
   * Actually, this extension point towards the implementation
   * of the actual output handling carries the core API of OutputSlot.
   * Thus, the task of actually implementing an OutputSlot boils down
   * to implementing this interface and providing a AllocState.
   * - `lock()` announces this FrameID and the corresponding buffer
   *   to be in exclusive use by the client from now on
   * - `transfer()` ends the client sided processing and initiates
   *   the outputting of the data found in the corresponding buffer.
   * - `pushout()` actually pushes the denoted buffer to the output.
   *   Typically, `pushout()` is called from the `transfer()`
   *   implementation; yet it may as well be called from a separate
   *   service thread or some kind of callback.
   * @note the meaning of FrameID is implementation defined.
   * @note typically the concrete connection is non-copyable
   */
  class OutputSlot::Connection
    {
    public:
      virtual ~Connection();
      
      virtual BuffHandle claimBufferFor(FrameID)  =0;
      virtual bool isTimely (FrameID, TimeValue)  =0;
      virtual void transfer (BuffHandle &)        =0;
      virtual void pushout  (BuffHandle &)        =0;
      virtual void discard  (BuffHandle &)        =0;
      virtual void shutDown ()                    =0;
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
   * handle is retained. The deleter function #shutdownConnection is
   * is invoked for each abandoned connection; once all connections
   * are defunct, the AllocState itself can be deconfigured.
   */
  template<class CON>
  class OutputSlot::AllocState
    : public OutputSlot::Allocation
    {
      using Connections = lib::ScopedCollection<CON>;
      using OpenedSinks = OutputSlot::OpenedSinks;
      using BufferProxy = OutputBufferProxy<AllocState>;
      
      Connections connections_;
      BufferProxy bufferProxy_;
      
      
      /* == Allocation Interface == */
      
      void
      release()  override
        {
          TODO ("propagate closing of output");
        }
      
      Timings
      getTimings()  override
        { //////////////////TODO somehow configure the timings into this implementation object
          UNIMPLEMENTED ("a lot regarding the Timings is still not clear...");                   //////////////////////////TICKET #831
        }
      
      
      /* == Callback API for BufferProxy == */
      
      
      
    protected: /* == API for OutputSlot-Impl == */
      
      using ConnectionStorage = Connections::ElementHolder;
      
      /**
       * The actual implementation of OutputSlot::buildState() needs to build
       * an AllocState<CON> instance, and has to provide a _population functor_
       * to setup the actual \a CON (Connection implementation) instances.
       * @see ScopedCollection_test::building_RAII_Style()
       */
      template<class CTOR>
      AllocState(uint numChannels, CTOR&& populator)
        : connections_(numChannels, std::forward<CTOR>(populator))
        , bufferProxy_{*this}
        { }
      
      
    private: /* === Implementation details === */
      
      static DataSink
      connectOutputSink (CON& connection)
        {
          TRACE (test, "activating Con=%p", &connection );
          
//        DataSink newSink;
//        newSink.activate(&connection, shutdownConnection);
//        return newSink;
        }
      
      static void
      shutdownConnection (OutputSlot::Connection* toClose)
        {
          REQUIRE (toClose);
          toClose->shutDown();
        }
    };
  
  
  
  
}} // namespace vault::out
#endif /*VAULT_OUT_OUTPUT_SLOT_CONNECTION_H*/
