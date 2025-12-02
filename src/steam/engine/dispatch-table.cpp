/*
  DispatchTable  -  implementation of frame job creation

   Copyright (C)
     2012,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file dispatch-table.cpp
 ** Implementation details of render job generation.
 ** @todo draft from 2011, stalled, relevance not yet clear
 ** @todo 6/2023 still unimplemented, as is the Fixture, which needs to back the Dispatcher
 */


#include "steam/engine/dispatch-table.hpp"



namespace steam {
namespace engine {
  
  JobTicket&
  DispatchTable::getJobTicketFor (size_t, TimeValue nominalTime)
  {
    UNIMPLEMENTED ("hook into the real implementation of the model backbone / segmentation");
  }
  
  size_t
  DispatchTable::resolveModelPort (ModelPort modelPort)
  {
    UNIMPLEMENTED ("some Map lookup in a prepared table to find out the actual slot number");
  }

  
  
  
  
}} // namespace engine
