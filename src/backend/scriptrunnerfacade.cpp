/*
  ScriptRunnerFacade  -  access point for running a script within Lumiera application context
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#include "backend/scriptrunnerfacade.hpp"
#include "common/singleton.hpp"


namespace backend {

  using lumiera::Subsys;
  
  class ScriptRunnerSubsysDescriptor
    : public Subsys
    {
      
      bool 
      shouldStart (lumiera::Option&)
        {
          UNIMPLEMENTED ("determine, if a script should be executed");
          return false;
        }
      
      bool
      start (lumiera::Option&, SigTerm termination)
        {
          UNIMPLEMENTED ("start the script as defined by the options and register script abort/exit hook");
          return false;
        }
      
      void
      triggerShutdown ()  throw()
        {
          UNIMPLEMENTED ("halt any running script");
        }
      
    };
  
  static lumiera::Singleton<ScriptRunnerSubsysDescriptor> theDescriptor;  //////////////////TODO: work out startup sequence. Don't use static init!
  
  
  
  
  /** @internal intended for use by main(). */
  Subsys&
  ScriptRunnerFacade::getDescriptor()
  {
    return theDescriptor();
  }



} // namespace backend
