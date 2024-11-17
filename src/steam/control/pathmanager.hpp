/*
  PATHMANAGER.hpp  -  Manager for deciding the actual render strategy

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file pathmanager.hpp
 ** Facility to decide upon the actual render strategy.
 ** When wiring the various processing facilities to form a render nodes network,
 ** at times there are multiple possibilities to reach a given goal. This opens
 ** the opportunity for optimisation and to configure specific variants or flavours.
 ** @todo this is a design draft and just defines a placeholder to indicate our plans.
 */


#ifndef CONTROL_PATHMANAGER_H
#define CONTROL_PATHMANAGER_H




namespace steam {
namespace control {
  
  
  /**
   * While building a render engine, this Strategy class 
   * decides on the actual render strategy in accordance
   * to the current controller settings (system state)
   */
  class PathManager
    {
    public:
      // TODO: find out about public interface
      // TODO: allocation, GC??
    };



}} // namespace steam::control
#endif
