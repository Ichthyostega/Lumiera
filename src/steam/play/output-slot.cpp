/*
  OutputSlot  -  capability to transfer data to a physical output

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

* *****************************************************/


/** @file output-slot.cpp
 ** Generic implementation functionality common to all output connections.
 */


#include "lib/error.hpp"
#include "steam/play/output-slot.hpp"
#include "steam/play/output-slot-connection.hpp"

#include <vector>


namespace steam {
namespace play {
  
  using std::vector;
  
  namespace error = lumiera::error;
  
  
  
  
  OutputSlot::~OutputSlot() { }  // emit VTables here....
  
  OutputSlot::Allocation::~Allocation() { }
  
  OutputSlot::Connection::~Connection() { }
  
  
  
  
  
  /** whether this output slot is occupied
   * @return true if currently unconnected and
   *         able to connect and handle output data 
   */
  bool
  OutputSlot::isFree()  const
  {
    return not this->state_;
  }
  
  
  /** claim this OutputSlot for active use as output sink(s).
   *  At any point, a given slot can only be used for a single
   *  ongoing output process (which may serve several channels though).
   *  The assumption is for the OutputSlot to be picked through a query
   *  to some OutputManater, so the parameters (resolution, sample rate...)
   *  should be suited for the intended use. Thus no additional configuration
   *  is necessary.
   * @return Allocation representing the "connected state" from the client's POV.
   *         The client may retrieve the effectively required Timings from there,
   *         as well as the actual output sinks, ready for use.
   * @remarks calls back into #buildState, where the concrete OutputSlot
   *         is expected to provide a private Connection implementation,
   *         subclassing OutputSlot::Allocation 
   */
  OutputSlot::Allocation&
  OutputSlot::allocate()
  {
    if (not isFree())
      throw error::Logic ("Attempt to open/allocate an OutputSlot already in use.");
    
    state_.reset (this->buildState());
    return *state_;
  }
  
  
  void
  OutputSlot::disconnect()
  {
    if (not isFree())
      state_.reset(0);    
  }
  
  
  
  /* === DataSink frontend === */
  
  BuffHandle
  DataSink::lockBufferFor(FrameID frameNr)
  {
    return impl().claimBufferFor(frameNr);
  }
  
  
  void
  DataSink::emit (FrameID frameNr, BuffHandle const& data2emit, TimeValue currentTime)
  {
    OutputSlot::Connection& connection = impl();
    if (connection.isTimely(frameNr,currentTime))
      connection.transfer(data2emit);
    else
      connection.discard(data2emit);
  }
  
  
  
  
}} // namespace steam::play
