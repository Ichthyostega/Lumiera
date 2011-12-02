/*
  Facade  -  access point for communicating with the Proc-Interface

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

* *****************************************************/


#include "proc/facade.hpp"
#include "lib/singleton.hpp"
#include "proc/play/output-director.hpp"

#include <string>


namespace proc {
  
  using std::string;
  using lumiera::Subsys;
  using lumiera::Option;
  
  
  class BuilderSubsysDescriptor
    : public Subsys
    {
      operator string ()  const { return "Builder"; }
      
      bool 
      shouldStart (Option&)
        {
          TODO ("determine, if we need a Builder Thread");
          return false;
        }
      
      bool
      start (Option&, Subsys::SigTerm termination)
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
      shouldStart (Option&)
        {
          TODO ("determine, if an existing Session should be loaded");
          return false;
        }
      
      bool
      start (Option&, Subsys::SigTerm termination)
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
  
  
  
  class PlayOutSubsysDescriptor
    : public Subsys
    {
      operator string ()  const { return "PlayOut"; }
      
      /** determine, if any output system is required to start up explicitly.
       *  Moreover, extract configuration variations for specific kinds of output
       * @return true if any output system is required to start stand-alone.
       *         otherwise, the player and a default configured output connection
       *         is pulled up only when required by another subsystem (e.g. GUI)
       * @todo   actually define cmdline options and parse/decide here! 
       */
      bool 
      shouldStart (Option&)
        {
          TODO ("extract options about specific output systems to be brought up");
          return false;
        }
      
      bool
      start (Option&, Subsys::SigTerm termination)
        {
          this->completedSignal_ = termination;
          return play::OutputDirector::instance().connectUp();
        }
      
      SigTerm completedSignal_;
      
      
      void
      triggerShutdown ()  throw()
        {
          play::OutputDirector::instance().triggerDisconnect (completedSignal_);
        }
      
      
      bool 
      checkRunningState ()  throw()
        {
          return play::OutputDirector::instance().isOperational();
        }
    };
  
  namespace {
    lib::Singleton<BuilderSubsysDescriptor> theBuilderDescriptor;   
    lib::Singleton<SessionSubsysDescriptor> theSessionDescriptor;
    lib::Singleton<PlayOutSubsysDescriptor> thePlayOutDescriptor;
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
  
  
  /** @internal intended for use by main(). */
  Subsys&
  Facade::getPlayOutDescriptor()
  {
    return thePlayOutDescriptor();
  }



} // namespace proc
