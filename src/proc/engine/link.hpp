/*
  PIPE.hpp  -  forwarding, adapting or connecting ProcNode
 
  Copyright (C)         CinelerraCV
    2007,               Christian Thaeter <ct@pipapo.org>
 
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


#ifndef PROC_ENGINE_PIPE_H
#define PROC_ENGINE_PIPE_H

#include "proc/engine/procnode.hpp"



namespace proc
  {
  namespace engine
    {
    
    
    /**
     * Abstraction of all sorts of connecting Elements with
     * special functionality. For example the coneections enabling
     * to mix OpenGL implemented Effects/Previews with normal CPU based
     * processing in one render pipeline.
     */
    class Link : public ProcNode
      {
        ///////TODO: flatten hierarchy if we don't have common functionallity for all Link-Subclasses
        ///////TODO:  can be decdiede when we know more how to handle OpenGL rendering
      };
      
      
      
  } // namespace proc::engine

} // namespace proc
#endif
