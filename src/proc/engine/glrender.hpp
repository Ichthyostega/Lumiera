/*
  GLRENDER.hpp  -  Representation of a OpenGL accellerated Video render process
 
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


#ifndef ENGINE_GLRENDER_H
#define ENGINE_GLRENDER_H

#include "proc/stateproxy.hpp"



namespace engine
  {


  /**
   * Representation of a OpenGL accelerated Video render process
   */
  class GLRender : public proc_interface::StateProxy
    {};

} // namespace engine
#endif
