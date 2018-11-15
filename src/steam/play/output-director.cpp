/*
  OutputDirector  -  handling all the real external output connections

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file output-director.cpp
 ** Implementation of global output connection management
 */


#include "steam/play/play-service.hpp"
#include "steam/play/output-manager.hpp"
#include "steam/play/output-director.hpp"
#include "vault/thread-wrapper.hpp"

using backend::Thread;


namespace proc {
namespace play {
  
  
  
  namespace { // hidden local details of the service implementation....
    
  } // (End) hidden service impl details
  
  
  
  /** storage for the single application wide OutputDirector instance */
  lib::Depend<OutputDirector> OutputDirector::instance;
  
  
  /** bring up the framework for handling input/output connections.
   *  Creating this object happens on first access and shouldn't be
   *  confused with actually booting up / shutting down this subsystem.
   *  Rather, the purpose of the OutputDirector is actively to conduct
   *  the Lifecycle of booting, connecting, operating, disconnecting.
   */
  OutputDirector::OutputDirector()
    : player_{PlayServiceHandle::NOT_YET_STARTED}
    , shutdown_initiated_{false}
    { }
  
  OutputDirector::~OutputDirector() { }
  
  
  
  /** connect and bring up the external input/output connections,
   *  handlers and interface services and the render/playback service.
   * @return true if the output subsystem can be considered operational
   */
  bool
  OutputDirector::connectUp()
  {
    Lock sync(this);
    REQUIRE (not shutdown_initiated_);
    
    player_.createInstance();
    return this->isOperational();
  }
  
  
  /** @todo WIP-WIP-WIP 6/2011 */
  bool
  OutputDirector::isOperational()  const
  {
    return bool(player_);   ////////////TODO more to check here....
  }
  
  
  
  /** initiate shutdown of all ongoing render/playback processes
   *  and closing of all external input/output interfaces.
   *  Works as an asynchronous operation; the given callback signal
   *  will be invoked when the shutdown is complete.
   * @note starting a new thread, which might fail.
   *       When this happens, the raised exception will cause
   *       immediate unconditional termination of the application.
   */
  void
  OutputDirector::triggerDisconnect (SigTerm completedSignal)  noexcept
  {
    if (not shutdown_initiated_)
      {
        shutdown_initiated_ = true;
        Thread ("Output shutdown supervisor",
                [=]{
                     bringDown (completedSignal);
                   });
      }
  }
  
  
  /** @internal actually bring down any calculation processes
   *  and finally disconnect any external input/output interfaces.
   *  This shutdown and cleanup operation is executed in a separate
   *  "Output shutdown supervisor" thread and has the liability to
   *  bring down the relevant facilities within a certain timespan.
   *  When done, the last operation within this thread will be to
   *  invoke the callback signal given as parameter.
   *  @note locks the OutputDirector
   */
  void
  OutputDirector::bringDown (SigTerm completedSignal)
  {
    Lock sync(this);
    string problemLog;
    if (not isOperational())
      {
        WARN (play, "Attempt to OutputDirector::bringDown() -- "
                    "which it is not in running state. Invocation ignored. "
                    "This indicates an error in Lifecycle logic.");
        return;
      }
    try
      {
        TODO ("actually bring down the output generation");
        player_.shutdown();
        
        completedSignal(0);
      }
    
    catch (lumiera::Error& problem)
      {
        problemLog = problem.what();
        lumiera_error(); // reset error state
        completedSignal (&problemLog);
      }
    catch (...)
      {
        problemLog = "Unknown error while disconnecting output. "
                     "Lumiera error flag is = "+string(lumiera_error());
        completedSignal (&problemLog);
      }
  }

}} // namespace proc::play
