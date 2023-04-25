/*
  Segmentation  -  Partitioning of a timeline for organising the render graph.

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

* *****************************************************/


/** @file segmentation.cpp
 ** @todo stalled effort towards a session implementation from 2008
 ** @todo 2016 likely to stay, but expect some extensive rework
 */

#include "lib/error.hpp"
#include "steam/fixture/segmentation.hpp"
//#include "steam/mobject/builder/fixture-change-detector.hpp"  ///////////TODO



namespace steam {
namespace fixture {
  
  namespace error = lumiera::error;
  
  
//  typedef ModelPortRegistry::ModelPortDescriptor const& MPDescriptor;
  
  
  /** storage for the link to the global
      Registry instance currently in charge  */
//  lib::OptionalRef<ModelPortRegistry> ModelPortRegistry::theGlobalRegistry;
  
  
  Segmentation::~Segmentation() { }   // emit VTable here...
  
  /** access the globally valid registry instance.
   *  @throw error::State if this global registry is
   *         already closed or not yet initialised. */
//ModelPortRegistry&
//ModelPortRegistry::globalInstance()
//{
//  LockRegistry global_lock;
//  if (theGlobalRegistry.isValid())
//    return theGlobalRegistry();
//  
//  throw error::State ("global model port registry is not accessible"
//                     , LERR_(BUILDER_LIFECYCLE));
//}
  
  
  
  /** */
//  bool
//  ModelPortRegistry::contains (ID<Pipe> key)  const
//  {
//    return bool(key)
//        && util::contains (transaction_, key);
//  }
  
  
  
//LUMIERA_ERROR_DEFINE (DUPLICATE_MODEL_PORT, "Attempt to define a new model port with an pipe-ID already denoting an existing port");
  
  
  
}} // namespace steam::fixture
