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
 ** The EngineCtx itself is meant to be [dependency injected](\ref lib::Depend), and acts
 ** as a front-end for the support facilities of the render engine — which are likewise
 ** dependency-injected, so that tests can use suitably adapted variants for verification.
 ** The default instantiation provides a naive self-contained implementation suitable for
 ** demonstration and test.
 ** 
 ** For productive use within the Lumiera Render Engine, a much more elaborate setup
 ** is necessary, including setup of a frame cache, and of timing strategies; these
 ** are configured as part of starting the steam::engine::RenderEnvironment.
 ** @todo who is responsible for setup of the services for the actual render engine?
 **       Might be closely related to brining up façade interfaces.
 ** @todo WIP-WIP 1/2026 provide a default implementation for test
 **       and find a way to populate the Facilities with the
 **       production variant of the services...
 ** 
 ** @see engine-ctx-test.cpp
 ** @see engine-ctx.cpp base implementation
 ** @see buffer-provider.hpp
 ** @see buffhandle.hpp
 */

#ifndef STEAM_ENGINE_ENGINE_CTX_H
#define STEAM_ENGINE_ENGINE_CTX_H


#include "lib/depend.hpp"
#include "vault/mem/buffhandle.hpp"
#include "lib/nocopy.hpp"


namespace steam {
namespace engine {
  
  using vault::mem::BufferProvider;
  
  
  class EngineCtx
    : util::NonCopyable
    {
      
    public:
      BufferProvider& mem;
      BufferProvider& cache;    /////////////////////////////////////////////////////////////////////////////TICKET #1223 : very likely a super interface for the CacheService
      
      static lib::Depend<EngineCtx> access;
      
    private:
      EngineCtx();
      friend class lib::DependencyFactory<EngineCtx>;
    };
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_ENGINE_CTX_H*/
