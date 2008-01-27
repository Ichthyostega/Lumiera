/*
  DefsManager  -  access to preconfigured default objects and definitions
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/


#include "proc/mobject/session/defsmanager.hpp"
#include "proc/asset/procpatt.hpp"
#include "proc/asset/port.hpp"

using asset::Query;
using asset::Port;
using asset::ProcPatt;

namespace mobject
  {
  namespace session
    {
    
    /** initialize the most basic internal defaults. */
    DefsManager::DefsManager ()  throw()
    {
      
    }


    /** create or retrieve a default-configured port asset.
     */
    template<>
    shared_ptr<Port> 
    DefsManager::operator() (const Query<Port>& capabilities)
    {
      UNIMPLEMENTED ("query for default port with capabilities");
    }


    /** create or retrieve a default-configured processing pattern.
     */
    template<>
    shared_ptr<ProcPatt> 
    DefsManager::operator() (const Query<ProcPatt>& capabilities)
    {
      UNIMPLEMENTED ("query for default processing pattern with capabilities");
    }


  } // namespace mobject::session

} // namespace mobject
