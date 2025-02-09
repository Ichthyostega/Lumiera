/*
  ENGINE-CTX.hpp  -  Services and parameters used globally within the render engine

   Copyright (C)
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file engine-ctx.hpp
 ** Dependency context to hold global parameters and services for the render engine.
 ** Notably the services to provide access to working buffers are linked directly into the
 ** render node connectivity, where they are used for each invocation of a render job.
 ** 
 ** The EngineCtx itself is meant to be [dependency injected](\ref lib::Depend), so that
 ** tests can use suitably adapted variants for verifications. The default instantiation
 ** provides a naive self-contained implementation suitable for demonstration and test.
 ** @todo who is responsible for setup of the services for the actual render engine?
 **       Might be closely related to brining up façade interfaces.
 ** @todo WIP-WIP 2/2025 provide actual service implementation and find a way
 **       how to populate the Facilities with these actual services...
 ** 
 ** @see engine-ctx-facilities.hpp implementation
 ** @see buffer-provider.hpp
 ** @see buffhandle.hpp
 */

#ifndef STEAM_ENGINE_ENGINE_CTX_H
#define STEAM_ENGINE_ENGINE_CTX_H


#include "lib/depend.hpp"
#include "steam/engine/buffhandle.hpp"
#include "lib/nocopy.hpp"

//#include <utility>
#include <memory>


namespace steam {
namespace engine {
  
//  using lib::Literal;
//  using std::unique_ptr;
//  using std::forward;
  
  class EngineCtx
    : util::NonCopyable
    {
      class Facilities;
      std::unique_ptr<Facilities> services_;
      
    public:
      BufferProvider& mem;
      BufferProvider& cache;
//      BufferProvider& output;  /////////////////////////OOO presumably no longer necessary
      
      static lib::Depend<EngineCtx> access;
      
    private:
     ~EngineCtx();
      EngineCtx();
      
      friend class lib::DependencyFactory<EngineCtx>;
    };
  
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_ENGINE_CTX_H*/
