/*
  MODEL-PORT.hpp  -  point to pull output data from the model
 
  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>
 
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

/** @file model-port.hpp
 ** Organising the output data calculation possibilities.
 ** Model ports are conceptual entities, denoting the points where output might
 ** possibly be produced. There is an actual representation, a collection of small
 ** descriptor objects managed by the Fixture and organised within the ModelPortTable
 ** datastructure. Thus, while the actual ModelPort descriptor entities are located
 ** within and managed by the Fixture -- model port as a concept spans the high-level
 ** and low-level view. A model port can be associated both to a pipe within a timeline
 ** in the HighLevelModel, as well as to denote a set of corresponding exit nodes within
 ** the segments of the render nodes network. Model ports are keyed by Pipe-ID and thus
 ** are unique within the application.
 ** 
 ** A model port is rather derived than configured; it emerges during the build process
 ** when a pipe claims an OutputDesignation and some other entity actually uses this
 ** designation as a target, either directly or indirectly. This match of provision
 ** and usage is detected by the Builder and produces an entry in the fixture's
 ** ModelPortTable. Because of the 1:1 association with a pipe, each model port
 ** has an associated StreamType.
 ** 
 ** Because model ports are discovered this way, dynamically during the build process,
 ** at some point there is a <i>transactional switch</i> to promote the new configuration
 ** to be come the valid current model port configuration. After that switch, model ports
 ** are immutable.
 ** 
 ** Model ports are to be accessed, enumerated and grouped in various ways, because each
 ** port belongs to a specific timeline and is used for producing data of a single
 ** StreamType solely. But all those referrals, searching and grouping happens only
 ** after the build process has discovered all model ports currently available.
 ** Thus we provide a MPortRef smart-pointer to ease handling of those access
 ** operations. The actual model port descriptors are owned and managed by
 ** the fixture; they are bulk allocated in a similar manner than the
 ** ProcNode and WiringDescriptor objects.
 ** 
 ** TODO fill in more details?
 ** TODO where to put the ModelPortTable
 ** 
 ** @see OutputDesignation
 ** @see OutputMapping
 ** @see Timeline
 */


#ifndef PROC_MOBJECT_MODEL_PORT_H
#define PROC_MOBJECT_MODEL_PORT_H

#include "proc/asset/pipe.hpp"
//#include "lib/opaque-holder.hpp"
//#include "lib/meta/typelist-util.hpp"

//extern "C" {
//#include "lib/luid.h"
//}

namespace mobject {
  
  /**
   * TODO type comment
   */
  class ModelPort
    : boost::noncopyable
    {
      
    public:
    };
  
  
  
} // namespace mobject
#endif
