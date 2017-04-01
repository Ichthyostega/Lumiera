/*
  RENDERSTATE.hpp  -  renderengine state management

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


/** @file renderstate.hpp
 ** Build and prepare the render state abstraction.
 ** The intention was to rely later, during the actual rendering, on the
 ** setup prepared here
 ** @deprecated stalled design draft from 2008 -- especially the part regarding RenderSate
 **             will likely be implemented in a different way
 */


#ifndef MOBJECT_BUILDER_RENDERSTATE_H
#define MOBJECT_BUILDER_RENDERSTATE_H

#include "proc/state.hpp"



namespace proc    {
namespace mobject {
namespace builder {
  
  typedef proc_interface::State State;
  
  
  /**
   * Encapsulates the logic used to get a "render process".
   * The provided StateProxy serves to hold any mutable state used
   * in the render process, so the rest of the render engine 
   * can be stateless.
   * @todo probably the state management will work different (6/08)
   */
  class RenderState
    {
    public:
      State& getRenderProcess () ;
    };
  
  
  
}}} // namespace proc::mobject::session
#endif /*MOBJECT_BUILDER_RENDERSTATE_H*/
