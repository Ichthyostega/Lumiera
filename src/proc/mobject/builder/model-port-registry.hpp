/*
    MODEL-PORT-REGISTRY.hpp  -  creating and organising and accessing model ports
 
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

/** @file model-port-registry.hpp
 ** Mutation and management facility for model ports within the builder.
 ** Model ports denote the points where output might possibly be produced.
 ** While client code accesses model ports only as immutable descriptors handled
 ** through an (opaque) reference, the builder is in charge of detecting and organising
 ** any (new) model ports arising as the result of the build process. Changes to the set
 ** of current model ports are to be activated with an atomic <i>transactional switch.</i>
 ** 
 ** builder::ModelPortRegistry thus acts as management interface and factory for model ports.
 ** A given instance of this registry can be promoted to be "the" model port registry reflecting
 ** the current active model ports. Within the Lumiera application, the builder subsystem cares
 ** for setting up such a registry, while all other parts of the system just access the current
 ** model ports through the mobject::ModelPort frontend.
 ** 
 ** TODO fill in more details?
 ** 
 ** @see ModelPort
 ** @see OutputDesignation
 ** @see ModelPortRegistry_test
 */


#ifndef PROC_MOBJECT_BUILDER_MODEL_PORT_REGISTRY_H
#define PROC_MOBJECT_BUILDER_MODEL_PORT_REGISTRY_H

#include "proc/asset/pipe.hpp"
//#include "lib/opaque-holder.hpp"
//#include "lib/meta/typelist-util.hpp"

//extern "C" {
//#include "lib/luid.h"
//}

namespace mobject {
namespace builder {
  
  /**
   * TODO type comment
   */
  class ModelPortRegistry
    : boost::noncopyable
    {
      
    public:
    };
  
  
  
}} // namespace mobject::builder
#endif
