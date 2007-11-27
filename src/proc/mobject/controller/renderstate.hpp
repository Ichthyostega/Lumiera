/*
  RENDERSTATE.hpp  -  renderengine state manager
 
  Copyright (C)         CinelerraCV
    2007,               Hermann Vosseler <Ichthyostega@web.de>
 
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


#ifndef MOBJECT_CONTROLLER_RENDERSTATE_H
#define MOBJECT_CONTROLLER_RENDERSTATE_H

#include "proc/stateproxy.hpp"



namespace mobject
  {
  namespace controller
    {

    typedef proc_interface::StateProxy StateProxy;


    /**
     * Encapsulates the logic used to get a "current render process"
     * in accordance to the currently applicable controller settings.
     * The provided StateProxy serves to hold any mutalbe state used
     * in the render process, so the rest of the render engine 
     * can be stateless.
     */
    class RenderState
      {
      public:
        StateProxy& getStateProxy () ;
      };



  } // namespace mobject::controller

} // namespace mobject
#endif
