/*
  MODEL-PORT.hpp  -  point to pull output data from the model

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

/** @file model-port.hpp
 ** Organising the output data calculation possibilities.
 ** Model ports are conceptual entities, denoting the points where output might
 ** possibly be produced. There is an actual representation, a collection of small
 ** descriptor objects managed by the Fixture and organised within the registry
 ** datastructure. Thus, while the actual ModelPort descriptor entities are located
 ** within and managed by the Fixture -- model port as a concept spans the high-level
 ** and low-level view. A model port can be associated both to a pipe within a timeline
 ** in the HighLevelModel, as well as to denote a set of corresponding exit nodes within
 ** the segments of the render nodes network. Model ports are keyed by Pipe-ID and thus
 ** are bound to be unique within the application.
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
 ** to become the valid current model port configuration. After that switch, model ports
 ** are immutable.
 ** 
 ** Model ports are to be accessed, enumerated and grouped in various ways, because
 ** each port belongs to a specific timeline and is used to produce data of a single
 ** StreamType solely. But all those referrals, searching and grouping happens only
 ** after the build process has discovered all model ports currently available.
 ** Thus actually the ModelPort elements handed out to client code are just
 ** smart-handles, accessing a global ModelPortRegistry behind the scenes.
 ** Validity of these handles will be checked on each access. The actual
 ** model port descriptors are owned and managed by the fixture;
 ** @todo they might bulk allocated in a similar manner than the
 **       ProcNode and WiringDescriptor objects are.
 ** 
 ** @see ModelPortRegistry_test abstract usage example
 ** @see ModelPortRegistry management interface 
 ** @see OutputDesignation
 ** @see OutputMapping
 ** @see Timeline
 */


#ifndef STEAM_MOBJECT_MODEL_PORT_H
#define STEAM_MOBJECT_MODEL_PORT_H

#include "steam/asset/pipe.hpp"
#include "steam/streamtype.hpp"

namespace steam {
namespace mobject {
  
  LUMIERA_ERROR_DECLARE (INVALID_MODEL_PORT);     ///< Referral to unknown model port
  LUMIERA_ERROR_DECLARE (UNCONNECTED_MODEL_PORT); ///< Attempt to operate on an existing but unconnected model port
  
  
  using asset::ID;
  
  
  /**
   * Handle designating a point within the model,
   * where actually output data can be pulled.
   * ModelPort is a frontend to be used by clients.
   * These ModelPort handle objects may be copied and stored
   * at will, but their validity will be verified on each access.
   * Actually, the Builder will discover any model ports and
   * maintain a ModelPortRegistry behind the scenes.
   * 
   * Each model port corresponds to a (global) pipe within a
   * specific Timeline ("holder"); consequently each such port is also
   * bound to produce data of a specific StreamType (as defined by
   * the corresponding pipe). A model port may be in \em unconnected
   * state, which can be checked through \c bool conversion. While
   * the ModelPort handles are value objects, the identity of the
   * underlying model port (descriptor) is given by the
   * corresponding pipe-ID, thus effectively resulting
   * in a global namespace for model ports.
   * 
   * @see builder::ModelPortRegistry management interface
   * @see ModelPortRegistry_test abstract usage example
   */
  class ModelPort
    {
      ID<asset::Pipe> id_;
      
    public:
      ModelPort()                         ///< \em unconnected model port
        : id_(ID<asset::Pipe>::INVALID)
        { }
      
      ModelPort (ID<asset::Pipe> refID);  ///< @note conversion from pipe-ID
      
      // using default copy operations
      
      
      static bool exists (ID<asset::Pipe>);
      
      ID<asset::Pipe>   pipe()    const;
      ID<asset::Struct> holder()  const;
      StreamType::ID streamType() const;
      
      bool
      isValid()  const
        {
          return exists (this->id_);
        }
      
      explicit
      operator bool()  const
        {
          return isValid();
        }
      
      
      friend bool
      operator== (ModelPort const& mp1, ModelPort const& mp2)
      {
        return mp1.id_ == mp2.id_;
      }
      
      friend bool
      operator!= (ModelPort const& mp1, ModelPort const& mp2)
      {
        return mp1.id_ != mp2.id_;
      }
      
    private:
      
    };
  
  
  
}} // namespace steam::mobject
#endif
