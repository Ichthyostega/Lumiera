/*
  CODECADAPTER.hpp  -  Processing Node for (de)coding media data
 
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


#ifndef ENGINE_CODECADAPTER_H
#define ENGINE_CODECADAPTER_H

#include "proc/engine/trafo.hpp"



namespace engine
  {


  /**
   * Adapter for wiring an externally loaded 
   * codec into the render pipeline. The decoding
   * process thus is integrated into the framework
   * of the Renderengine, while the actual implementation
   * can be delegated to an external library accessed
   * through Cinelerra's datat backend. 
   */
  class CodecAdapter : public Trafo
    {};



} // namespace engine
#endif
