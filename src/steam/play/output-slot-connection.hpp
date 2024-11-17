/*
  OUTPUT-SLOT-CONNECTION.hpp  -  implementation API for concrete output slots

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

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
 ** engine. This generic frontend uses a PImpl, pointing to a ConnectionState object, which embodies
 ** the actual implementation. Moreover, this actual implementation is free to use specifically crafted
 ** OutputSlot::Connection elements to handle the ongoing output for individual channels. The latter
 ** thus becomes the central implementation side API for providing actual output capabilities.
 **
 ** @see OutputSlotProtocol_test
 ** @see diagnostic-output-slot.hpp  ////TODO
 */


#ifndef STEAM_PLAY_OUTPUT_SLOT_CONNECTION_H
#define STEAM_PLAY_OUTPUT_SLOT_CONNECTION_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "steam/play/output-slot.hpp"
#include "lib/scoped-collection.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/iter-source.hpp"
#include "lib/handle.hpp"
//#include "lib/time/timevalue.hpp"
//#include "steam/engine/buffer-provider.hpp"
//#include "steam/play/timings.hpp"
//#include "lib/sync.hpp"

//#include <string>
#include <functional>
#include <vector>
//#include <memory>


namespace steam {
namespace play {

  using steam::engine::BuffHandle;
//using steam::engine::BufferProvider;
//using lib::time::Time;
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
   * to implementing this interface and providing a ConnectionState.
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
      virtual void transfer (BuffHandle const&)   =0;
      virtual void pushout  (BuffHandle const&)   =0;
      virtual void discard  (BuffHandle const&)   =0;
      virtual void shutDown ()                    =0;
    };
  
  
  
  /** 
   * Extension point for Implementation.
   * The ConnectionState is where the concrete output
   * handling implementation is expected to reside.
   * OutputSlot is a frontend and accesses
   * ConnectionState in the way of a PImpl.
   */
  class OutputSlot::ConnectionState
    : public OutputSlot::Allocation
    , util::NonCopyable
    {
    public:
      virtual ~ConnectionState() { }
      
      virtual Connection& access (uint)  const    =0;
    };
  
  
  
  
  /* ===== Building blocks for OutputSlot implementation ===== */
  
  /** Base for OutputSlot standard implementation */
  class OutputSlotImplBase
    : public OutputSlot
    {
    protected:
      template<class CON>
      class ConnectionManager;
    };
  
  
  /**
   * Maintaining a list of active connections.
   * Base class for the typical implementation approach.
   * Using this class is \em not mandatory. But obviously,
   * we'd get to manage a selection of Connection objects
   * representing the "active points" in several media channels
   * connected through this OutputSlot. These Connection subclasses
   * are what is referenced by the DataSink smart-ptrs handed out
   * to the client code. As ConnectionState implements the Allocation
   * API, it has the liability to create these DataSink smart-ptrs,
   * which means to wire them appropriately and also provide an
   * deleter function (here #shutdownConnection) to be invoked
   * when the last copy of the smart-handle goes out of scope.
   * 
   * The typical standard/base implementation provided here
   * manages a collection of active Connection subclass objects.
   */
  template<class CON>
  class OutputSlotImplBase::ConnectionManager
    : public OutputSlot::ConnectionState
    {
      typedef lib::ScopedCollection<CON> Connections;
      typedef OutputSlot::OpenedSinks OpenedSinks;
      
      Connections connections_;
      
      
      /* == Allocation Interface == */
      
      OpenedSinks
      getOpenedSinks()
        {
                                                                                 //////////////////////////TICKET #878  not re-entrant, lifecycle isn't clear
          REQUIRE (this->isActive());
          return lib::iter_source::transform (eachElm(connections_), connectOutputSink);
        }
      
      Timings
      getTimingConstraints()
        {
          UNIMPLEMENTED ("find out about timing constraints");                   //////////////////////////TICKET #831
        }
      
      bool
      isActive()  const
        {
          return 0 < connections_.size();
        }
      
      CON&
      access (uint chanNr)  const
        {
          return connections_[chanNr];
        }
      
      
    protected: /* == API for OutputSlot-Impl == */
      void
      init() ///< derived classes need to invoke this to build the actual connections
        {
                                                                                 //////////////////////////TICKET #878  really build all at once? or on demand?
          connections_.populate_by (&ConnectionManager::buildConnection, this);
        }
      
      typedef typename Connections::ElementHolder& ConnectionStorage;
      
      /** factory function to build the actual
       *  connection handling objects per channel */
      virtual void buildConnection(ConnectionStorage)  =0;
      
      
      ConnectionManager(uint numChannels)
        : connections_(numChannels)
        { }
      
    public:
      virtual
     ~ConnectionManager()
        { }
      
      
      
    private: // Implementation details
      
      static DataSink
      connectOutputSink (CON& connection)
        {
          TRACE (test, "activating Con=%p", &connection );
          
          DataSink newSink;
          newSink.activate(&connection, shutdownConnection);
          return newSink;
        }
      
      static void
      shutdownConnection (OutputSlot::Connection* toClose)
        {
          REQUIRE (toClose);
          toClose->shutDown();
        }
    };
  
  
  
  
}} // namespace steam::play
#endif
