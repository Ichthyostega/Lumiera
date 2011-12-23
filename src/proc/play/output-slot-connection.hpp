/*
  OUTPUT-SLOT-CONNECTION.hpp  -  implementation API for concrete output slots

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


#ifndef PROC_PLAY_OUTPUT_SLOT_CONNECTION_H
#define PROC_PLAY_OUTPUT_SLOT_CONNECTION_H


#include "lib/error.hpp"
#include "proc/play/output-slot.hpp"
#include "lib/handle.hpp"
//#include "lib/time/timevalue.hpp"
//#include "proc/engine/buffer-provider.hpp"
//#include "proc/play/timings.hpp"
#include "lib/iter-source.hpp"
#include "lib/iter-adapter-stl.hpp"
//#include "lib/sync.hpp"

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
//#include <string>
//#include <vector>
//#include <tr1/memory>


namespace proc {
namespace play {

  using proc::engine::BuffHandle;
//using proc::engine::BufferProvider;
//using lib::time::Time;
//using std::string;
  using lib::transform;
  using lib::iter_stl::eachElm;

//using std::vector;
//using std::tr1::shared_ptr;
  using boost::scoped_ptr;
  
  
  
  
  /** @internal represents the \em active
   *   point in each of the per-channel connections
   *   used when this OutputSlot is operational.
   *   
   * \par OutputSlot Core API
   *   
   * Actually, this extension point towards the implementation
   * of the actual output handling carries the core API of OutputSlot.
   * Thus, the task of actually implementing an OutputSlot boils down
   * to implementing this interface and providing a ConnectionState.
   * - \c lock() announces this FrameID and the corresponding buffer
   *   to be in exclusive use by the client from now on
   * - \c transfer() ends the client sided processing and initiates
   *   the outputting of the data found in the corresponding buffer.
   * - \c pushout() actually pushes the denoted buffer to the output.
   *   Typically, \c pushout() is called from the \c transfer()
   *   implementation; yet it may as well be called from a separate
   *   service thread or some kind of callback.
   * @note the meaning of FrameID is implementation defined.
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
    , boost::noncopyable
    {
    public:
      virtual ~ConnectionState() { }
    };
  
  
  /** 
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
  class ConnectionStateManager
    : public OutputSlot::ConnectionState
    , public vector<CON>
    {
      
      typedef OutputSlot::OpenedSinks OpenedSinks;
      
      
      /* == Allocation Interface == */
      
      OpenedSinks
      getOpenedSinks()
        {
          REQUIRE (this->isActive());
          return transform (eachElm(*this), connectOutputSink);
        }
      
      bool
      isActive()
        {
          return 0 < vector<CON>::size();
        }
      
      
    public:
      ConnectionStateManager()
        { }
      
      virtual
     ~ConnectionStateManager()
        { }
      
      
      void
      init (uint numChannels)
        {
          for (uint i=0; i<numChannels; ++i)
            push_back(buildConnection());
        }
      
      
      /** factory function to build the actual
       *  connection handling objects per channel */
      virtual CON buildConnection()  =0;
      
      
    private: // Implementation details
      
      static DataSink
      connectOutputSink (CON& connection)
        {
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
  
  
  
  
}} // namespace proc::play
#endif