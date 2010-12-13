/*
  Segmentation  -  partitioning the effective timeline
 
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
 
* *****************************************************/


/** @file segmentation.cpp
 ** Implementation details of fixture data structures.
 ** 
 ** /////TODO file comment necessary?
 ** 
 ** @see mobject::session::Fixture
 **
 */


#include "lib/error.hpp"
//#include "include/logging.h"
//#include "lib/sync-classlock.hpp"
//#include "proc/mobject/builderfacade.hpp"
#include "proc/mobject/builder/segmentation.hpp"
#include "proc/mobject/builder/fixture-change-detector.hpp"

namespace mobject {
namespace builder {
  
  namespace error = lumiera::error;
  
  
//  typedef ModelPortRegistry::ModelPortDescriptor const& MPDescriptor;
  
  
  /** storage for the link to the global
      Registry instance currently in charge  */
//  lib::OptionalRef<ModelPortRegistry> ModelPortRegistry::theGlobalRegistry;
  
  
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
//                     , LUMIERA_ERROR_BUILDER_LIFECYCLE); 
//}
  
  
  
  /** */
//  bool
//  ModelPortRegistry::contains (ID<Pipe> key)  const
//  {
//    return bool(key)
//        && util::contains (transaction_, key); 
//  }
  
  
  
//LUMIERA_ERROR_DEFINE (DUPLICATE_MODEL_PORT, "Attempt to define a new model port with an pipe-ID already denoting an existing port");
  
  
  
}}// namespace mobject::builder
