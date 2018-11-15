/*
  ENGINEFACADE.hpp  -  access point for communicating with the render engine

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


/** @file enginefacade.hpp
 ** Global control interface for the render engine subsystem.
 ** The render engine, as implemented in an combined effort by the
 ** Lumiera Vault-Layer and some parts of Steam-Layer, can be started and
 ** stopped as a [Subsystem](\ref subsys.hpp) of the whole application.
 */


#ifndef VAULT_INTERFACE_ENGINEFACADE_H
#define VAULT_INTERFACE_ENGINEFACADE_H


#include "common/subsys.hpp"



namespace backend {
  
  
  /**************************************************************//**
   * Interface to the vault layer (render engine subsystem):
   * Global access point for starting the render engine subsystem and
   * for defining the public interface(s) for talking with the engine.
   * 
   * While the engine is partially implemented relying on steam-Layer 
   * operations, the general access point and the playback/render
   * controller is considered part of the vault. This results in
   * a "W"-shaped control flow: from stage to vault to steam to
   * vault, feeding resulting data to output.
   * 
   */
  struct EngineFacade
    {
      /** provide a descriptor for lumiera::AppState,
       *  wired accordingly to allow main to pull up and
       *  shut down the renderengine. */
      static lumiera::Subsys& getDescriptor();
      
      
      //////////////////TODO: define the global access interface for the engine
      //////////////////TODO: provide a function for accessing this interface
      
    };
    
  
  
} // namespace backend
#endif
