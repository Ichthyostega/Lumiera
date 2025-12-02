/*
  Dispatcher  -  translating calculation streams into frame jobs

   Copyright (C)
     2011,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file dispatcher.cpp
 ** Implementation parts of job generation within an ongoing render process
 ** @todo 2023 do we actually need a separate translation unit for this?
 */


#include "steam/engine/dispatcher.hpp"



namespace steam {
namespace engine {

  Dispatcher::~Dispatcher() { }  // emit VTables and Typeinfo here....
  
  
  
  
  
}} // namespace engine
