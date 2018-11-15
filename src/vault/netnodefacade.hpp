/*
  NETNODEFACADE.hpp  -  access point for maintaining a renderfarm node

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


/** @file netnodefacade.hpp
 ** Global control interface for running Lumiera as a renderfarm node.
 ** 
 ** @todo this just documents a vague plan. We won't be able to implement
 **       anything of that kind for the foreseeable future as of 1/2017
 */


#ifndef BACKEND_INTERFACE_NETNODEFACADE_H
#define BACKEND_INTERFACE_NETNODEFACADE_H


#include "common/subsys.hpp"



namespace backend {
  
  
  /**************************************************************//**
   * Interface to the vault layer (renderfarm node):
   * Global access point for starting a server listening on a TCP port
   * and accepting render tasks. Possibly such a server could also
   * use the backend file/media access functions to provide a media
   * data access service. 
   * 
   * @todo define the services provided by such a node. 
   * 
   */
  struct NetNodeFacade
    {
      /** provide a descriptor for lumiera::AppState,
       *  wired accordingly to allow main to start and stop
       *  a node server accepting render / file jobs via network. */
      static lumiera::Subsys& getDescriptor();
      
      
      //////////////////TODO: define the global access interface for a renderfarm node server
      //////////////////TODO: provide a function for accessing this interface
      
    };
    
  
  
} // namespace backend
#endif
