/*
  STYPEREGISTRY.hpp  -  implementation of the registry for stream type descriptors

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file styperegistry.hpp
 ** This is part of the _implementation_ of the stream type manager (include).
 ** Only used in stypemanager.cpp and accompanying unit tests.
 ** 
 ** @see control::STypeManager
 ** @see lumiera::StreamType
 **
 */




#ifndef CONTROL_STYPEREGISTRY_H
#define CONTROL_STYPEREGISTRY_H


#include "steam/control/stypemanager.hpp"
#include "lib/nocopy.hpp"
//#include "common/query.hpp"
//#include "lib/util.hpp"
//#include "lib/p.hpp"

//#include <set>
//#include <vector>
//#include <memory>


namespace steam {
namespace control {

  
//  using std::string;
  
  namespace impl {
  } // (End) impl namespace
  
  
  
  /**
   * @internal Helper for organising preconfigured default objects.
   * Maintains a collection of objects known or encountered as "default"
   * for a given type. This collection is ordered by "degree of constriction",
   * which is implemented by counting the number of predicates in the query
   * used to define or identify each object.
   * Accessing an object identified by a query causes the query to be resolved
   * (executed in prolog), which may result in some additional properties of
   * the object being bound or specified.
   * @todo as of 3/2008 the real query implementation is missing, and the
   * exact behaviour has to be defined.
   */
  class STypeManager::Registry
    : util::NonCopyable
    {
    public:
      
    };
  
  
  
}} // namespace steam::control
#endif
