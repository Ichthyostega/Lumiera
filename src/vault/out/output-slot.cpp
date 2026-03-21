/*
  OutputSlot  -  capability to transfer data to a physical output

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file output-slot.cpp
 ** Generic implementation functionality common to all output connections.
 */


#include "lib/error.hpp"
#include "vault/out/output-slot.hpp"
#include "vault/out/output-slot-connection.hpp"

#include <vector>


namespace vault {
namespace out   {
  namespace err = lumiera::error;
  
  using std::vector;
  
  
  
  
  
  // emit VTables here....
  OutputSlot::Allocation::~Allocation() { }
  OutputSlot::Connection::~Connection() { }
  
  
  
  /**
   * @internal setup of the OutputSlot and the PImpl (Allocation) for active connected state.
   *   Some wiring must be carried out here, so that the ref-count based lifecycle management
   *   works as desired. All customisation for some specific kind of output mechanism and
   *   the corresponding OutputSlot::Connection implementation is embedded into the actual
   *   type of the PImpl, i.e. the Allocation subclass passed in as argument. The OutputSlot,
   *   together with the DataSink handles, will take shared ownership of the connection setup.
   */
  OutputSlot::PAlloc
  OutputSlot::connect (unique_ptr<Allocation> allocation)
  {
    REQUIRE (allocation);
    return {allocation.release()
           ,[](Allocation* allo) {// disconnection call-back
                                   REQUIRE(allo);
                                   allo->release();
                                   delete allo;
                                 }};
  }

  
  
  /**
   * Entrance point: retrieve a sequence of actual DataSink handles, one for each data feed.
   * Typically, these _sink handles_ will be propagated to some calculation stream and further
   * embedded into render job definitions, so that the render invocation can use the sink handle
   * to get a buffer for each frame.
   * @return an iterator that produces DataSink objects; these are ref-counting handles
   *         and also functor objects with the signature `BuffHandle(FrameID)`, where the
   *         FrameID is defined in relation to the _frame grid_ defined by the Timings.
   */
  OutputSlot::OpenedSinks
  OutputSlot::getOpenedSinks()
  {
    REQUIRE (alloc_);
    return alloc_->connect (alloc_);
  }
  
  
  Timings
  OutputSlot::timingConstraints()
  {
    REQUIRE (alloc_);
    return alloc_->getTimings();
  }
  
  
  
  /* === DataSink frontend === */
  
  
  
  
}} // namespace vault::out
