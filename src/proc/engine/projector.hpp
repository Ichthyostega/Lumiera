/*
  PROJECTOR.hpp  -  video ProcNode for scaling and translating image data
 
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


#ifndef PROC_ENGINE_PROJECTOR_H
#define PROC_ENGINE_PROJECTOR_H

#include "proc/engine/trafo.hpp"



namespace proc
  {
  namespace engine
    {
    
    
    /**
     * Special video processing node used to scale and translate image data.
     */
    class Projector : public Trafo
      {
        ////////////
      };
      
      
      
  } // namespace proc::engine

} // namespace proc
#endif
