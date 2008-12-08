/*
  Facade  -  access point for communicating with the Proc-Interface
 
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


#include "proc/facade.hpp"
#include "common/singleton.hpp"

#include <string>


namespace proc {
  
  using std::string;
  using lumiera::Subsys;
  
  
  class BuilderSubsysDescriptor
    : public Subsys
    {
      operator string ()  const { return "Builder"; }
      
      bool 
      shouldStart (lumiera::Option&)
        {
          TODO ("determine, if we need a Builder Thread");
          return false;
        }
      
      bool
      start (lumiera::Option&, Subsys::SigTerm termination)
        {
          UNIMPLEMENTED ("fire up a Builder in a separate Thread, and register shutdown hook");
          return false;
        }
      
      void
      triggerShutdown ()  throw()
        {
          UNIMPLEMENTED ("halt the Builder and cancel any build process");  /////TODO really cancel??
        }
      
      bool 
      checkRunningState ()  throw()
        {
          //Lock guard (*this);
          TODO ("implement detecting running state");
          return false;
        }
    };
  
  
  
  class SessionSubsysDescriptor
    : public Subsys
    {
      operator string ()  const { return "Session"; }
      
      bool 
      shouldStart (lumiera::Option&)
        {
          TODO ("determine, if an existing Session schould be loaded");
          return false;
        }
      
      bool
      start (lumiera::Option&, Subsys::SigTerm termination)
        {
          UNIMPLEMENTED ("load an existing session as denoted by the options and register shutdown hook");
          return false;
        }
      
      void
      triggerShutdown ()  throw()
        {
          UNIMPLEMENTED ("initiate closing this Session");
        }
      
      bool 
      checkRunningState ()  throw()
        {
          //Lock guard (*this);
          TODO ("implement detecting running state");
          return false;
        }
    };
  
  namespace {
    lumiera::Singleton<BuilderSubsysDescriptor> theBuilderDescriptor;   
    lumiera::Singleton<BuilderSubsysDescriptor> theSessionDescriptor;
  }
  
  
  
  
  /** @internal intended for use by main(). */
  Subsys&
  Facade::getBuilderDescriptor()
  {
    return theBuilderDescriptor();
  }
  
  
  /** @internal intended for use by main(). */
  Subsys&
  Facade::getSessionDescriptor()
  {
    return theSessionDescriptor();
  }



} // namespace proc
