/*
  ASSEMBLER.hpp  -  building facility (implementation of the build process)

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


/** @file assembler.hpp
 ** @todo initially generated code of unclear relevance
 */


#ifndef MOBJECT_BUILDER_ASSEMBLER_H
#define MOBJECT_BUILDER_ASSEMBLER_H

#include "proc/engine/renderengine.hpp"


namespace proc    {
namespace mobject {
namespace builder {
  
  
  /**
   * This is the actual building facility:
   * provided the correct tools and associations, 
   * it serves to build and connect the individual ProcNode objects
   */
  class Assembler
    {
    public:
      engine::RenderEngine&
      build ();
      // TODO: allocation, GC???
    };
  
  
  
}}} // namespace proc::mobject::builder
#endif
