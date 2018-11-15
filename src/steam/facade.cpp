/*
  Facade  -  access point for communicating with the Steam-Interface

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


/** @file facade.cpp
 ** Implementation of subsystem lifecycle behaviour for the core parts of Steam-Layer.
 ** - The »session subsystem« is responsible for accepting operations to work on the
 **   session datastructure, and it will trigger the Builder to reflect those changes
 **   into a suitable render nodes network
 ** - The »play out subsystem« is able to _perform_ such a render nodes network
 **   for video playback and rendering.
 ** 
 ** lumiera::Subsys and lumiera::SubsystemRunner together define a protocol for some
 ** large scale building blocks of the whole application to be started and terminated.
 ** Typically this entails to create a dedicated thread to manage the top level concerns
 ** for the given subsystem, and to create registration and services for public access:
 ** - to operate on the session, use the proc::control::SessionCommand facade
 ** - playback and render operations can be operated by the lumiera::Play facade
 ** 
 ** @see ProcDispatcher
 ** @see OutputDirector
 ** @see subsys.hpp
 ** @see main.cpp
 **
 */


#include "steam/facade.hpp"
#include "lib/depend.hpp"
#include "steam/control/proc-dispatcher.hpp"
#include "steam/play/output-director.hpp"

#include <string>


namespace proc {
  
  using std::string;
  using std::unique_ptr;
  using lumiera::Subsys;
  using lumiera::Option;
  using proc::control::ProcDispatcher;
  
  
  class SessionSubsystem
    : public Subsys
    {
      operator string()  const { return "Session"; }
      
      /** @remarks there is no need explicitly to start the session,
       *   since it is passive, waiting for invocations and will be
       *   pulled up as prerequisite of other subsystems. */
      bool 
      shouldStart (Option&)  override
        {
          return false;
        }
      
      bool
      start (Option&, Subsys::SigTerm termNotification)  override
        {
          return ProcDispatcher::instance().start (termNotification);
        }
      
      void
      triggerShutdown()  noexcept override
        {
          ProcDispatcher::instance().requestStop();
        }
      
      bool 
      checkRunningState()  noexcept override
        {
          return ProcDispatcher::instance().isRunning();
        }
    };
  
  
  
  class PlayOutSubsysDescriptor
    : public Subsys
    {
      operator string()  const { return "PlayOut"; }
      
      /** determine, if any output system is required to start up explicitly.
       *  Moreover, extract configuration variations for specific kinds of output
       * @return true if any output system is required to start stand-alone.
       *         otherwise, the player and a default configured output connection
       *         is pulled up only when required by another subsystem (e.g. GUI)
       * @todo   actually define cmdline options and parse/decide here! 
       */
      bool 
      shouldStart (Option&)  override
        {
          TODO ("extract options about specific output systems to be brought up");
          return false;
        }
      
      bool
      start (Option&, Subsys::SigTerm termination)  override
        {
          this->completedSignal_ = termination;
          return play::OutputDirector::instance().connectUp();
        }
      
      SigTerm completedSignal_;
      
      
      void
      triggerShutdown()  noexcept override
        {
          play::OutputDirector::instance().triggerDisconnect (completedSignal_);
        }
      
      
      bool 
      checkRunningState()  noexcept override
        {
          return play::OutputDirector::instance().isOperational();
        }
    };
  
  namespace {
    lib::Depend<SessionSubsystem> theSessionSubsystemLifecycle;
    lib::Depend<PlayOutSubsysDescriptor> thePlayOutDescriptor;
  }
  
  
  
  
  /** @internal intended for use by main(). */
  Subsys&
  Facade::getSessionDescriptor()
  {
    return theSessionSubsystemLifecycle();
  }
  
  
  /** @internal intended for use by main(). */
  Subsys&
  Facade::getPlayOutDescriptor()
  {
    return thePlayOutDescriptor();
  }



} // namespace proc
