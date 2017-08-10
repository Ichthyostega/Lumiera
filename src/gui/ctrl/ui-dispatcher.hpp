/*
  UI-DISPATCHER.hpp  -  dispatch invocations into the UI event thread

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file ui-dispatcher.hpp
 ** Allow dispatch of self-contained code blocks (lambdas) into the main UI event thread.
 ** GTK is _not threadsafe by design_ -- thus it is mandatory to dispatch any asynchronous invocations
 ** from external facilities in a controlled way into the main event loop. Unfortunately, this becomes
 ** a tricky undertaking when these external invocations need to pass argument data. This helper serves
 ** to accommodate such problems, relying on the automatic (heap based) argument storage of std::function.
 ** Client code provides the actual invocation in the form of _completely closed lambdas._
 ** 
 ** @warning these lambdas will be stored in a synchronised queue and invoked out of the original
 **          call stack. It is the client's responsibility to ensure that all bindings in the closure
 **          are either *by value*, or *by smart-ptr*, or alternatively to ensure the lifecycle of any
 **          referred entity exceeds the lifespan of the UI-Loop. Since the shutdown-order of Lumiera's
 **          subsystems is not deterministic, this rules out passing references to anything tied to some
 **          subsystem lifecycle. Referring to a static singleton is acceptable though.
 ** 
 ** @see NotificationService
 ** @see CallQueue_test
 */


#ifndef GUI_CTRL_UI_DISPATCHER_H
#define GUI_CTRL_UI_DISPATCHER_H

#include "gui/gtk-base.hpp"
#include "lib/call-queue.hpp"

#include <boost/noncopyable.hpp>
//#include <string>


namespace gui {
namespace ctrl {
  
//  using std::string;
//  class GlobalCtx;
  
  
  
  /**
   * Helper to dispatch code blocks into the UI event thread for execution.
   * Implementation of the actual dispatch is based on `Glib::Dispatcher`
   */
  class UiDispatcher
    : boost::noncopyable
    {
      
    public:
      UiDispatcher()
      { }
      
    private:
    };
  
  
  
}}// namespace gui::ctrl
#endif /*GUI_CTRL_UI_DISPATCHER_H*/
