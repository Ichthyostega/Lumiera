/*
  FACADE.hpp  -  access point for communicating with the Steam-Interface

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

/** @file facade.hpp
 ** Top level entrance point and facade for the Steam-Layer.
 ** The middle layer of the application holds a session with the high-level model,
 ** to be translated by the Builder into a node network, which can be _performed_
 ** by the Engine to render output.
 ** 
 ** @see common.hpp
 ** @see main.cpp
 ** 
 */



#ifndef STEAM_INTERFACE_FACADE_H
#define STEAM_INTERFACE_FACADE_H


#include "common/subsys.hpp"



namespace proc {
  
  
  /******************************************************************//**
   * Global access point for the services implemented by the Steam-Layer.
   * 
   * @todo this is a dummy placeholder as of 1/2009. Currently, there
   *       is only implementation-level code within the Steam-Layer
   *       and the interfaces need to be worked out.
   * @todo at least the Play/Output subsystem slowly turns into
   *       something real, as of 6/2011
   * @todo and as of 1/2017, the Session subsystem is basically
   *       implemented and it became clear that there won't be
   *       a "Builder subsystem". So this interface might now
   *       be considered roughly complete...
   * 
   */
  struct Facade
    {
      /** provide a descriptor for lumiera::AppState,
       *  wired accordingly to allow `main()` to boot
       *  the support infrastructure for loading of
       *  and working on the editing session. */
      static lumiera::Subsys& getSessionDescriptor();
      
      
      /** provide a descriptor for lumiera::AppState,
       *  wired accordingly to allow `main()` to bring up
       *  the render / playback coordination and 
       *  output management subsystem. */
      static lumiera::Subsys& getPlayOutDescriptor();
      
      
      //////////////////TODO: define the global access interfaces for the Steam-Layer
      //////////////////TODO: provide a function for accessing this interface
      //////////////////TODO: register similar proxy/facade interfaces for the GUI
      
    };
    
  
  
} // namespace proc
#endif
