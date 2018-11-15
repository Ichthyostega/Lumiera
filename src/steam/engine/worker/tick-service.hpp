/*
  TICK-SERVICE.hpp  -  issuing timed callbacks

  Copyright (C)         Lumiera.org
    2009,               Joel Holdsworth <joel@airwebreathe.org.uk>,
                        Hermann Vosseler <Ichthyostega@web.de>

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

/** @file tick-service.hpp
 ** A timer service invoking a given callback periodically.
 ** This is a rough preliminary implementation as of 1/2009. We use it to
 ** drive the frame "creation" of a player dummy (the render engine is not 
 ** ready yet). The intention is to use this service as part of a mock engine
 ** setup, used to verify the construction of engine components. As an integration
 ** test, we build a "dummy player", delivering some test data frames to the Gui.
 ** 
 ** @see proc::play::DummyPlayerService
 **  
 */


#ifndef PROC_PLAY_TICKSERVICE_H
#define PROC_PLAY_TICKSERVICE_H


#include "lib/error.hpp"
#include "vault/thread-wrapper.hpp"

#include <unistd.h>   // for usleep()

#include <functional>
#include <limits>


namespace proc {
namespace node {

  using std::function;
  using std::bind;
  
  
  
  /********************************************************//**
   * Tick generating service for a periodic callback,
   * with adjustable frequency. Quick'n dirty implementation!
   */
  class TickService
    : backend::ThreadJoinable
    {
      typedef function<void(void)> Tick;
      volatile uint timespan_;
      
      /** poll interval for new settings in wait state */
      static const uint POLL_TIMEOUT = 1000;
      
    public:
      TickService (Tick callback)
        : ThreadJoinable("Tick generator (dummy)"
                        , bind (&TickService::timerLoop, this, callback)
                        )
        { 
          INFO (proc, "TickService started.");
        }
      
     ~TickService ()
        {
          timespan_ = 0;
          this->join();
          usleep (200000);    // additional delay allowing GTK to dispatch the last output
          
          INFO (proc, "TickService shutdown.");
        }
      
      
      /** set the periodic timer to run with a given frequency,
       *  starting \em now. Well, not actually now, but at the next
       *  opportunity. It should be \em now, but this implementation
       *  is sloppy! setting fps==0 halts (pauses) the timer.
       */
      void activate (uint fps)
        {
          REQUIRE (  0==fps 
                  ||(   1000000/fps < std::numeric_limits<uint>::max() 
                     && 1000000/fps > POLL_TIMEOUT));
          if (fps)
            timespan_ = 1000000/fps; // microseconds per tick
          else
            timespan_ = POLL_TIMEOUT;
        }
      
      
    private:
      void timerLoop(Tick periodicFun)
        {
          timespan_ = POLL_TIMEOUT;
          while (0 < timespan_)
            {
              if (timespan_ > POLL_TIMEOUT)
                periodicFun();
              
              usleep (timespan_);
            }
          TRACE (proc_dbg, "Tick Thread timer loop exiting..."); 
        }

    };
  
  
  
  
}} // namespace proc::node
#endif

