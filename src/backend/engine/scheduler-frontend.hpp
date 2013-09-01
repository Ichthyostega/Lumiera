/*
  SCHEDULER-FRONTEND.hpp  -  access point to the scheduler within the renderengine

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

*/


#ifndef BACKEND_ENGINE_SCHEDULER_FRONTEND_H
#define BACKEND_ENGINE_SCHEDULER_FRONTEND_H



//using std::list;
#include "lib/singleton.hpp"


namespace backend{
namespace engine {
  
  
  /**
   * Access point to the scheduler service provided by the back-end.
   * Proc-Layer uses this service as the primary means of instructing
   * the backend; suitably prepared and wired frame render jobs are
   * handed over to the scheduler for time-bound or bandwidth-controlled
   * execution
   * 
   * @todo this is planned to become the frontend
   * to the render node network, which can be considered
   * at the lower end of the middle layer; the actual 
   * render operations are mostly implemented by the backend
   * @todo define the low-level scheduler interface and hook in
   *       the necessary calls to implement this frontend.
   * ////////TODO WIP as of 9/2013
   */
  class SchedulerFrontend
    {
    public:
      /** access point to the Engine Interface.
       * @internal this is an facade interface for internal use
       *           by the player. Client code should use the Player.
       */
      static lib::Singleton<SchedulerFrontend> instance;

      
       ///// TODO: find out about the public operations
       // note: the play controller lives in the proc-layer,
       //       but is a subsystem separate of the session.
      
    protected:
      void activateTracing();
      void disableTracing(); ///< EX_FREE
      
      friend class SchedulerDiagnostics;
      
    private:
      
    };

}} // namespace backend::engine
#endif
