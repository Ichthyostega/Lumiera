/*
  ASSEMBLER.hpp  -  building facility (implementation of the build process)

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file assembler.hpp
 ** @todo initially generated code of unclear relevance
 */


#ifndef MOBJECT_BUILDER_ASSEMBLER_H
#define MOBJECT_BUILDER_ASSEMBLER_H

#include "steam/engine/renderengine.hpp"


namespace steam    {
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
  
  
  
}}} // namespace steam::mobject::builder
#endif
