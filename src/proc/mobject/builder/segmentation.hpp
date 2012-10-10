/*
    SEGMENTATION.hpp  -  partitioning the effective timeline

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file segmentation.hpp
 ** Part of the Fixture datastructure to manage time segments of constant structure.
 ** The Fixture is result of the build process and separation between high-level and
 ** low-level model. It's kind of an effective resulting timeline, and split into segments
 ** of constant wiring structure: whenever the processing nodes need to be wired differently
 ** for some timespan, we start a new segment of the timeline. This might be for the duration
 ** of a clip, or just for the duration of a transition, when the pipes of both clips need to
 ** be wired up in parallel.
 ** 
 ** @see Fixture
 ** @see ModelPort
 */


#ifndef PROC_MOBJECT_BUILDER_SEGMENTATION_H
#define PROC_MOBJECT_BUILDER_SEGMENTATION_H

#include "lib/error.hpp"
//#include "lib/optional-ref.hpp"
#include "proc/asset/pipe.hpp"
//#include "proc/asset/struct.hpp"
//#include "proc/mobject/model-port.hpp"

//#include <map>

namespace proc    {
namespace mobject {
namespace builder {
  
  using asset::ID;
  using asset::Pipe;
//using asset::Struct;
  
//LUMIERA_ERROR_DECLARE (DUPLICATE_MODEL_PORT); ///< Attempt to define a new model port with an pipe-ID already denoting an existing port
  
  
  /**
   * TODO type comment
   */
  class Segment
    : boost::noncopyable
    {
      
      /////////////////////////////////////////////////TODO: placeholder code
      /////////////////////////////////////////////////TODO: see the planned structure at http://lumiera.org/wiki/renderengine.html#Fixture
      typedef ID<Pipe>   PID;
      
    public:
      
    };
  
  
  
  
  /** 
   * TODO type comment
   */
  class Segmentation
    {
      
    public:
    };
  
  
  
}}} // namespace proc::mobject::builder
#endif
