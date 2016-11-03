/*
  PATHMANAGER.hpp  -  Manager for deciding the actual render strategy

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file §§§
 ** TODO §§§
 */


#ifndef CONTROL_PATHMANAGER_H
#define CONTROL_PATHMANAGER_H




namespace proc {
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



}} // namespace proc::control
#endif
