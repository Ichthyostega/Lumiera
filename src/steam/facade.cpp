/*
  Facade  -  access point for communicating with the Steam-Interface

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


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
 ** - to operate on the session, use the steam::control::SessionCommand facade
 ** - playback and render operations can be operated by the lumiera::Play facade
 ** 
 ** @see SteamDispatcher
 ** @see OutputDirector
 ** @see subsys.hpp
 ** @see main.cpp
 **
 */


#include "steam/facade.hpp"
#include "lib/depend.hpp"
#include "steam/control/steam-dispatcher.hpp"
#include "steam/play/output-director.hpp"

#include <string>


namespace steam {
  
  using std::string;
  using std::unique_ptr;
  using lumiera::Subsys;
  using lumiera::Option;
  using steam::control::SteamDispatcher;
  
  
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
          return SteamDispatcher::instance().start (termNotification);
        }
      
      void
      triggerShutdown()  noexcept override
        {
          SteamDispatcher::instance().requestStop();
        }
      
      bool 
      checkRunningState()  noexcept override
        {
          return SteamDispatcher::instance().isRunning();
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



} // namespace steam
