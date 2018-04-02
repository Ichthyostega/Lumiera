/*
  APPSTATE.hpp  -  application initialisation and behaviour

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

/** @file appstate.hpp
 ** Registering and managing primary application-global services.
 ** This can be considered the "main" object of the Lumiera application
 ** Besides encapsulating the logic to start up the fundamental parts of
 ** the application, there is a mechanism for registering \em subsystems
 ** to be brought up and shut down in order. AppState will issue the global
 ** application lifecycle events (where other parts may have registered
 ** callbacks) and provides the top-level catch-all error handling.
 **
 ** @see LifecycleHook
 ** @see BasicSetup
 ** @see Subsys
 ** @see main.cpp
 ** @see logging.h
 */


#ifndef LUMIERA_APPSTATE_H
#define LUMIERA_APPSTATE_H

#include "lib/symbol.hpp"
#include "lib/nocopy.hpp"
#include "lib/depend.hpp"
#include "common/option.hpp"
#include "common/subsys.hpp"
#include "common/basic-setup.hpp"

#include <memory>
#include <string>
#include <map>



namespace lumiera {
  
  using std::string;
  
  class SubsystemRunner;
  
  
  /**
   * The Lumiera Application state and basic initialisation.
   * Singleton to hold global flags directing the overall application behaviour,
   * responsible for triggering lifecycle events and performing early initialisation tasks.
   * AppState services are available already from static initialisation code.
   * @warning don't use AppState in destructors.
   */
  class AppState
    : util::NonCopyable
    {
    private:
      AppState ();
      
     ~AppState ();
      friend class lib::DependencyFactory<AppState>;
      
      
    public:
      /** get the (single) AppState instance.
       *  @warning don't use it after the end of main()! */
      static lib::Depend<AppState> instance;
      
      
      /** evaluate the result of option parsing and maybe additional configuration
       *  such as to be able to determine the further behaviour of the application.
       *  Set the internal state within this object accordingly. */
      void init (lumiera::Option& options);
      
      
      /** access basic application setup values (from \c setup.ini) */
      string fetchSetupValue (lib::Literal key);
      
      
      /** building on the state determined by #init, decide if the given Subsys
       *  needs to be pulled up and, if necessary, register the Subsys and its
       *  prerequisites to be maintained throughout the application's lifetime.
       */
      void maybeStart (lumiera::Subsys&);
      
      
      enum ExitCode {
        NORMAL_EXIT,
        CLEAN_EXIT_AFTER_ERROR,
        CLEAN_EMERGENCY_EXIT,
        FAILED_EMERGENCY_EXIT
      };
      
      /** put the main thread of the application into a wait state, as long as some
       *  subsystem(s) registered with #maybeStart still need to be maintained.
       *  On termination of one of those components, tear down the remaining
       *  components and initiate a normal or emergency shutdown of the
       *  application, depending on the triggering component's
       *  mode of termination (exit or exception).
       *  @return global application exit code
       */
      ExitCode maybeWait();
      
      
      /** initiate the controlled error shutdown sequence
       *  @param problem causing exception */
      ExitCode abort (lumiera::Error& problem);
      
      
      /** initiate an fatal emergency shutdown,
       *  caused by an unforeseen error condition */
      ExitCode abort ()  throw();
      
      
      
    private:
      using PSub = std::unique_ptr<SubsystemRunner>;
      
      BasicSetup setup_;
      
      PSub  subsystems_;
      
      bool emergency_;
      bool core_up_;
      
    };
  
  
} // namespace lumiera

#endif
