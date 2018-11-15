/*
  SCRIPTRUNNERFACADE.hpp  -  access point for running a script within Lumiera application context

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


/** @file scriptrunnerfacade.hpp
 ** Global control interface for script driven operation of Lumiera.
 ** 
 ** @todo this just documents a vague plan. We won't be able to implement
 **       anything of that kind for the foreseeable future as of 1/2017
 */


#ifndef VAULT_INTERFACE_SCRIPTRUNNERFACADE_H
#define VAULT_INTERFACE_SCRIPTRUNNERFACADE_H


#include "common/subsys.hpp"



namespace backend {
  
  
  /*******************************************************************//**
   * Interface to the vault layer (TODO 2018 really?) (script runner):
   * Global access point for starting a script within Lumiera application
   * context. 
   * 
   * @todo build the (LUA,C)-script runner. 
   * 
   */
  struct ScriptRunnerFacade
    {
      /** provide a descriptor for lumiera::AppState,
       *  wired accordingly to allow main to start a script and to
       *  (prematurely) abort a running script. */
      static lumiera::Subsys& getDescriptor();
      
      
      //////////////////TODO: define the access interface for starting a (LUA, C, ...)-script
      //////////////////TODO: provide a function for accessing this interface
      
    };
    
  
  
} // namespace backend
#endif
