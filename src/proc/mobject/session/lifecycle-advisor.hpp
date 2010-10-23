/*
  LIFECYCLE-ADVISOR.hpp  -  outline of the session lifecycle
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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
 
*/


/** @file lifecycle-advisor.hpp
 ** Implementation facility providing an operation skeleton of session lifecycle.
 ** This header is intended to be included into the session manager implementation;
 ** it should not be used by client code otherwise. The purpose of the LifecycleAdvisor
 ** is to to get a consolidated view on the whole lifecycle. Reading this source file
 ** should convey a complete picture about what is going on with respect to the
 ** session lifecycle. Besides that, no actual implementation code is to be found
 ** here; any implementation is delegated to the relevant session facilities.
 ** 
 ** The idea of a LifecycleAdvisor is inspired by GUI frameworks, especially 
 ** Spring RichClient. Typically, such frameworks provides a means for flexible
 ** configuration of the application lifecycle. This isn't a goal here, as there
 ** is only one Lumiera application and the session lifecycle can be considered
 ** hard wired, with the exception of some extension points, which are implemented
 ** as "lifecycle events".
 **
 ** @see SessManager
 ** @see LifecycleHook
 ** @see lumiera::AppState
 ** @see session.hpp
 */


#ifndef MOBJECT_SESSION_LIFECYCLE_ADVISOR_H
#define MOBJECT_SESSION_LIFECYCLE_ADVISOR_H

#include "lib/error.hpp"
#include "include/lifecycle.h"
#include "proc/mobject/session.hpp"

#include <boost/noncopyable.hpp>


namespace mobject {
namespace session {
  
  using lumiera::LifecycleHook;
  
  
  /**
   * Skeleton operations conducting the session lifecycle sequences.
   * Any details of the operations are delegated to the current session
   * and associated services.
   * @warning this object is assumed to be used as a single instance
   *          in a controlled and safe (single threaded) environment
   */
  class LifecycleAdvisor
    : boost::noncopyable
    {
      
      
    public:
      /** operation sequence to pull up the session
       * 
       */
      void
      pullUp()
        {
          LifecycleHook::trigger (ON_SESSION_START);
          LifecycleHook::trigger (ON_SESSION_INIT);
          LifecycleHook::trigger (ON_SESSION_READY);
        }
      
      
      /** operation sequence for cleanly shutting down the session
       * 
       */
      void
      shutDown()
        {
          LifecycleHook::trigger (ON_SESSION_END);
        }
    };
  
  
  
}} // namespace mobject::session
#endif
