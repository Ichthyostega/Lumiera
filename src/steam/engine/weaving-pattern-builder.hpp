/*
  WEAVING-PATTERN-BUILDER.hpp  -  build an invocation pattern for media calculations

  Copyright (C)         Lumiera.org
    2024,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file weaving-pattern-builder.hpp
 ** Construction kit for establishing an invocation scheme for media calculations.
 ** 
 ** @see turnout.hpp
 ** @see node-builder.hpp
 ** @see NodeLinkage_test
 ** 
 ** @todo WIP-WIP-WIP as of 7/2024 prototyping how to build and invoke render nodes /////////////////////////TICKET #1367
 ** 
 */


#ifndef STEAM_ENGINE_WEAVING_PATTERN_BUILDER_H
#define STEAM_ENGINE_WEAVING_PATTERN_BUILDER_H

//#include "steam/common.hpp"
//#include "steam/engine/channel-descriptor.hpp"
//#include "vault/gear/job.h"
#include "lib/several-builder.hpp"
#include "steam/engine/turnout.hpp"
#include "steam/engine/buffer-provider.hpp"
//#include "lib/util-foreach.hpp"
//#include "lib/iter-adapter.hpp"
//#include "lib/meta/function.hpp"
//#include "lib/itertools.hpp"
//#include "lib/util.hpp"

//#include <utility>
#include <functional>
//#include <array>
#include <vector>


namespace steam {
namespace engine {
  
  using std::forward;
  using lib::Several;
  
  template<class POL, class I, class E=I>
  using DataBuilder = lib::SeveralBuilder<I,E, POL::template Policy>;
  

/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Prototyping: how to assemble a Turnout
  template<uint N, class FUN>
  using SimpleDirectInvoke = SimpleWeavingPattern<Conf_DirectFunctionInvocation<N,FUN>>;
  
  template<class POL, uint N, class FUN>
  struct SimpleWeavingBuilder
    : util::MoveOnly
    {
      DataBuilder<POL, PortRef>   leadPort;
      DataBuilder<POL, BuffDescr> outTypes;
      
      using TypeMarker = std::function<BuffDescr(BufferProvider&)>;
      using ProviderRef = std::reference_wrapper<BufferProvider>;
      
      std::vector<TypeMarker> buffTypes;
      std::vector<ProviderRef> providers;
      
      struct ServiceCtx
        {
          ProviderRef mem;
          ProviderRef cache;
          ProviderRef output;
        };
      ServiceCtx ctx; //////////////////////////////////////////OOO need to wire that top-down through all builders!
      
      SimpleWeavingBuilder
      attachToLeadPort(ProcNode& lead, uint portNr)
        {
          PortRef portRef; /////////////////////////////////////OOO TODO need Accessor on ProcNode!!!!!
          leadPort.emplace (portRef); 
          ENSURE (leadPort.size() < N);
          return move(*this);
        }
      
      template<class BU>
      SimpleWeavingBuilder
      appendBufferTypes(size_t cnt)
        {
          while (cnt--)
            buffTypes.emplace_back([](BufferProvider& provider)
                                    { return provider.getDescriptor<BU>(); });
          ENSURE (buffTypes.size() < N);
          return move(*this);
        }
      
      SimpleWeavingBuilder
      selectOutputSlot(size_t i)
        {
          maybeFillDefaultProviders (i+1);
          ENSURE (providers.size() > i);
          providers[i] = ctx.output;
        }
      
      
      auto
      build()
        {
          maybeFillDefaultProviders (buffTypes.size());
          uint i=0;
          for (auto& typeCtor : buffTypes)
            outTypes.emplace (typeCtor(providers[i]));
          
          ENSURE (leadPort.size() < N);
          ENSURE (outTypes.size() < N);
          
          using Product = Turnout<SimpleDirectInvoke<N,FUN>>;
          ///////////////////////////////OOO need a way to prepare SeveralBuilder-instances for leadPort and outDescr --> see NodeBuilder
          return Product{leadPort.build(), outTypes.build};
        }
      
      private:
        void
        maybeFillDefaultProviders (size_t maxSlots)
          {
            for (uint i=providers.size(); i < maxSlots; ++i)
              providers.emplace_back (ctx.mem);
          }
    };
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : (End)Prototyping: how to assemble a Turnout
  
  
  
}}// namespace steam::engine
#endif /*STEAM_ENGINE_WEAVING_PATTERN_BUILDER_H*/
