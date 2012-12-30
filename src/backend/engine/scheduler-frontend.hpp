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


namespace backend{
namespace engine {
  
  
  /**
   * @todo this is planned to become the frontend
   * to the render node network, which can be considered
   * at the lower end of the middle layer; the actual 
   * render operations are mostly implemented by the backend
   * ////////TODO WIP as of 12/2010
   */
  class SchedulerFrontend
    {
    public:
       ///// TODO: find out about the public operations
       // note: the play controller lives in the proc-layer,
       //       but is a subsystem separate of the sesison.
      
    private:
      
    };

}} // namespace backend::engine
#endif
