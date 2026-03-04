/*
  VOYAGE.hpp  -  application initialisation and behaviour

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file voyage.hpp
 ** Registering and managing primary application-global services.
 ** This can be considered the "main" object of the Lumiera application
 ** Besides encapsulating the logic to start up the fundamental parts of
 ** the application, there is a mechanism for registering \em subsystems
 ** to be brought up and shut down in order. The vessel::Voyage will issue
 ** global application lifecycle events (where other parts may have attached
 ** callbacks) and provides the top-level catch-all error handling.
 **
 ** @see LifecycleHook
 ** @see BasicSetup
 ** @see Subsys
 ** @see main.cpp
 ** @see logging.h
 */


#ifndef VESSEL_VOYAGE_H
#define VESSEL_VOYAGE_H

#include "lib/symbol.hpp"
#include "lib/nocopy.hpp"
#include "lib/depend.hpp"
#include "vessel/option.hpp"
#include "vessel/subsys.hpp"
#include "vessel/basic-setup.hpp"

#include <memory>
#include <string>
#include <map>



namespace vessel {
  
  using std::string;
  
  class SubsystemRunner;
  
  
  /**
   * The Lumiera Application state and basic initialisation.
   * Singleton to hold global flags directing the overall application behaviour,
   * responsible for triggering lifecycle events and performing early initialisation tasks.
   * @warning don't use vessel::Voyage in destructors.
   */
  class Voyage
    : util::NonCopyable
    {
    private:
      Voyage();
     ~Voyage();
      
      friend class lib::DependencyFactory<Voyage>;
      
      
    public:
      /** get the (single) vessel::Voyage instance.
       *  @warning don't use it after the end of main()! */
      static lib::Depend<Voyage> access;
      
      
      /** evaluate the result of option parsing and maybe additional configuration
       *  such as to be able to determine the further behaviour of the application.
       *  Set the internal state within this object accordingly. */
      void init (Option& options);
      
      
      /** access basic application setup values (from \c setup.ini) */
      string fetchSetupValue (lib::Literal key);
      
      
      /** building on the state determined by #init, decide if the given Subsys
       *  needs to be pulled up and, if necessary, register the Subsys and its
       *  prerequisites to be maintained throughout the application's lifetime.
       */
      void maybeStart (Subsys&);
      
      
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
      ExitCode abort ()  noexcept;
      
      
      
    private:
      using PSub = std::unique_ptr<SubsystemRunner>;
      
      BasicSetup setup_;
      
      PSub  subsystems_;
      
      bool emergency_;
      bool core_up_;
      
    };
  
  
} // namespace vessel
#endif /*VESSEL_VOYAGE_H*/
