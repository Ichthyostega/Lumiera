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
 ** # implementation considerations
 ** 
 ** Basically the implementation relies on the [standard mechanism][Gtkmm-tutorial] for multithreaded
 ** UI applications. But on top we use our own [dispatcher queue](\ref lib::CallQueue) to allow passing
 ** arbitrary argument data, based on the argument storage of `std::function`. Which in the end effectively
 ** involves two disjoint thread collaboration mechanisms:
 ** - the caller creates a closure of the operation to be invoked, binding all arguments by value
 ** - this closure is wrapped into a `std::function` instance
 ** - which in turn is added into the dispatcher queue. Depending on the implementation,
 **   this might incur explicit locking
 ** - after successfully enqueuing the closure, the GTK event thread is signalled through
 **   the [Glib-Dispatcher], which actually messages through an OS-pipe (kernel based IO)
 ** - the Dispatcher need to be created within the UI event thread (which is the case, since
 **   all of Lumiera's UI top-level context is created in the thread dedicated to run GTK)
 ** - relying on internal GLib / GIO ``magic'', the dispatcher hooks into the respective GLib
 **   ``main context'' to ensure this signalling is picked up from the event thread, which...
 ** - ...finally leads to invocation of the Dispatcher's signal from within the event loop
 ** 
 ** This hybrid approach is rather simple to establish, but creates additional complexities at runtime.
 ** More specifically, we have to pay the penalty of chaining the overhead and the inherent limitations
 ** of two thread signalling mechanisms (in our dispatcher queue and within kernel based IO).
 ** It would be conceivable to implement all of the hand-over mechanism solely within our framework,
 ** yet unfortunately there seems to be no easily accessible and thus ``official'' way to hook into
 ** the event processing, at least without digging deep into GLib internals.
 ** 
 ** @note as detailed in the documentation for [Glib-Dispatcher], all instances built within a
 **       given receiver thread (here the UI event loop thread) will _share a single pipe for signalling._
 **       Under heavy load this queue might fill up and block the sender on dispatch.
 ** 
 ** [Glib-Dispatcher]: https://developer.gnome.org/glibmm/2.42/classGlib_1_1Dispatcher.html
 ** [GTKmm-tutorial]:  https://developer.gnome.org/gtkmm-tutorial/3.12/sec-using-glib-dispatcher.html.en
 ** @see NotificationService
 ** @see CallQueue_test
 */


#ifndef GUI_CTRL_UI_DISPATCHER_H
#define GUI_CTRL_UI_DISPATCHER_H

#include "gui/gtk-base.hpp"
#include "lib/call-queue.hpp"
#include "lib/format-string.hpp"

#include <boost/noncopyable.hpp>
#include <utility>


namespace gui {
namespace ctrl {
  
  using std::move;
  using ::util::_Fmt;
  
  namespace {
    /** @note reads and clears the lumiera error flag */
    inline string
    generateErrorResponse (const char* problem = "unexpected problem")
    {
      static _Fmt messageTemplate{"asynchronous UI response failed: %s (error flag was: %s)"};
      string response{messageTemplate % problem % lumiera_error()};
      WARN (gui, "%s", response.c_str());
      return response;
    }
  }
  
  
  /**
   * Helper to dispatch code blocks into the UI event thread for execution.
   * Implementation of the actual dispatch is based on `Glib::Dispatcher`
   * @warning any UiDispatcher instance must be created such as to ensure
   *          it outlives the GTK event loop
   */
  class UiDispatcher
    : boost::noncopyable
    {
      lib::CallQueue queue_;
      Glib::Dispatcher dispatcher_;
      
      using Operation = lib::CallQueue::Operation;
      
    public:
      template<class FUN>
      UiDispatcher(FUN notification)
        : queue_{}
        , dispatcher_{}
        {
          dispatcher_.connect(
                         [=]() {try {
                                      queue_.invoke();
                                    }
                                  catch (std::exception& problem)
                                    {
                                      notification (generateErrorResponse(problem.what()));
                                    }
                                  catch (...)
                                    {
                                      notification (generateErrorResponse());
                                    }
                               });
        }
      
      /**
       * move the given operation into our private dispatcher queue and
       * then schedule dequeuing and invocation into the UI event thread.
       * @param op a completely closed lambda or functor
       * @warning closure need to be by value or equivalent, since
       *        the operation will be executed in another call stack
       */
      void
      event (Operation&& op)
        {
          queue_.feed (move(op));
          dispatcher_.emit();
        }
    };
  
  
  
}}// namespace gui::ctrl
#endif /*GUI_CTRL_UI_DISPATCHER_H*/
