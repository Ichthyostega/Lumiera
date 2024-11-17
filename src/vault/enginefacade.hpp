/*
  ENGINEFACADE.hpp  -  access point for communicating with the render engine

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file enginefacade.hpp
 ** Global control interface for the render engine subsystem.
 ** The render engine, as implemented in an combined effort by the
 ** Lumiera Vault-Layer and some parts of Steam-Layer, can be started and
 ** stopped as a [Subsystem](\ref subsys.hpp) of the whole application.
 ** @warning as of 4/2023 Render-Engine integration work is underway ////////////////////////////////////////TICKET #1280
 */


#ifndef VAULT_INTERFACE_ENGINEFACADE_H
#define VAULT_INTERFACE_ENGINEFACADE_H


#include "common/subsys.hpp"



namespace vault {
  
  
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
    
  
  
} // namespace vault
#endif
