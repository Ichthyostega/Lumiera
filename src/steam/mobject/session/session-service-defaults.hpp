/*
  SESSION-SERVICE-DEFAULTS.hpp  -  session implementation service API: manage default objects

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


/** @file session-service-defaults.hpp
 ** Implementation level session API: to manage default configured objects.
 ** 
 ** @todo rework the existing DefsManager to fit into this scheme. TICKET #404
 ** 
 ** @see session-impl.hpp implementation of the service
 ** @see session-services.cpp implementation of access
 **
 */


#ifndef MOBJECT_SESSION_SESSION_SERVICE_DEFAULTS_H
#define MOBJECT_SESSION_SESSION_SERVICE_DEFAULTS_H

//#include "proc/mobject/session.hpp"
//#include "lib/meta/generator.hpp"




namespace proc {
namespace mobject {
namespace session {
  
//  using lib::meta::InstantiateChained;
//  using lib::meta::InheritFrom;
//  using lib::meta::NullType;
  
  
  class SessionServiceDefaults
    {
    };
  
  
  
}}} // namespace proc::mobject::session
#endif
