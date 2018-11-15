/*
  LIFECYCLE-ADVISOR.hpp  -  outline of the session lifecycle

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file lifecycle-advisor.hpp
 ** Implementation facility providing an operation skeleton of session lifecycle.
 ** This header is intended to be included into the session manager implementation;
 ** it should not be used by client code otherwise. The purpose of the LifecycleAdvisor
 ** is to get a consolidated view on the whole lifecycle. Reading this source file
 ** should convey a complete picture about what is going on with respect to the
 ** session lifecycle. Besides that, no actual implementation code is to be found
 ** here; any implementation is delegated to the relevant session facilities.
 ** 
 ** The idea of a LifecycleAdvisor is inspired by GUI frameworks, especially 
 ** Spring RichClient. Typically, such frameworks provide a means for flexible
 ** configuration of the application lifecycle. Configurability isn't the primary
 ** goal here, as there is only one Lumiera application and the session lifecycle
 ** can be considered fixed, with the exception of some extension points, which are
 ** implemented as "lifecycle events".
 ** 
 ** @see SessManager
 ** @see LifecycleHook
 ** @see lumiera::AppState
 ** @see session.hpp
 ** @see sess-manager-impl.cpp concrete definition of Lifecycle
 ** 
 */


#ifndef STEAM_MOBJECT_SESSION_LIFECYCLE_ADVISOR_H
#define STEAM_MOBJECT_SESSION_LIFECYCLE_ADVISOR_H

#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "include/lifecycle.h"
#include "steam/mobject/session.hpp"
#include "lib/symbol.hpp"


namespace proc {
namespace mobject {
namespace session {
  
  using lib::Symbol;
  
  
  /**
   * Skeleton of operations conducting the session lifecycle sequences.
   * Any details of the operations are delegated to the current session
   * and associated services.
   * @warning this object is assumed to be used as a single instance
   *          in a controlled and safe (single threaded) environment
   */
  class LifecycleAdvisor
    : util::NonCopyable
    {
      
      
    public:
      /** operation sequence to pull up the session.
       *  After building the session implementation with all associated
       *  sub services in a default configured state, the new session is
       *  switched in to become the \em current session. Session content
       *  is loaded, either from default configuration or by de-serialising
       *  an existing session (loading from persistent storage). When
       *  everything is wired and ready, the new session is "armed"
       *  and the public session API is allowed to accept commands.
       */
      void
      pullUp()
        {
          createSessionFacilities();      // includes switch of the "current" Session
          emitEvent (ON_SESSION_START);
          injectSessionContent();
          emitEvent (ON_SESSION_INIT);
          getSessionReady();
          openSessionInterface();
          emitEvent (ON_SESSION_READY);
        }
      
      
      /** operation sequence for cleanly shutting down the session.
       *  To initiate shutdown, command processing is halted and the
       *  external session interface is closed. Any ongoing render processes
       *  are disconnected and asked to terminate. After maybe performing
       *  cleanup and consolidation routines, the command framework is
       *  disconnected from the log, discarding any pending commands.
       *  This brings the session subsystem back into \em de-configured
       *  state, all asset and content objects pending eviction,
       *  and the internal knowledge-base rolled back to zero.
       */
      void
      shutDown()
        {
          emitEvent (ON_SESSION_CLOSE);
          closeSessionInterface();
          disconnectRenderProcesses();
          emitEvent (ON_SESSION_END);
          commandLogCheckpoint();
          deconfigure();
        }
      
      
      virtual ~LifecycleAdvisor() { }   ///< is ABC
      
      
    protected: /* === Lifecycle building blocks === */
      
      void
      emitEvent (Symbol eventLabel)
        {
          lumiera::LifecycleHook::trigger (eventLabel);
        }
      
      
      /**
       * Build and wire all the sub components together forming the session implementation.
       * All these components are created to be operational in principle, but not initialised
       * or outfitted with actual boilerplate state. After leaving this function, all of the
       * technical / implementation level invariants are fulfilled. As a final step,
       * the "current" session pointer is switched.
       */
      virtual void createSessionFacilities()   =0;
      
      
      /**
       * 
       */
      virtual void injectSessionContent()      =0;
      
      
      /**
       * 
       */
      virtual void getSessionReady()           =0;
      
      
      /**
       * 
       */
      virtual void openSessionInterface()      =0;
      
      
      /**
       * 
       */
      virtual void closeSessionInterface()     =0;
      
      
      /**
       * 
       */
      virtual void disconnectRenderProcesses() =0;
      
      
      /**
       * 
       */
      virtual void commandLogCheckpoint()      =0;
      
      
      /**
       * This final stage of the session lifecycle terminates the operational state of
       * all parts of the current session. When entering this phase, it can be assumed
       * that no entity from outside the session will access any of these parts anymore.
       * Now, all the internal indices and knowledge registries are advised to purge,
       * thereby rendering any session content officially non-existent.
       * @note even after leaving this phase, all session components remain valid
       *       and basically operational. Already disconnected render processes
       *       might still access implementation facilities or session content.
       *       The actual unwinding and destruction is controlled by memory
       *       management and thus by reference count.
       */
      virtual void deconfigure()               =0;
    };
  
  
  
}}} // namespace mobject::session
#endif
