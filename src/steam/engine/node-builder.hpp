/*
  NODE-BUILDER.hpp  -  Setup of render nodes connectivity

  Copyright (C)         Lumiera.org
    2009,               Hermann Vosseler <Ichthyostega@web.de>
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

/** @file node-builder.hpp
 ** Specialised shorthand notation for building the Render Node network.
 ** During the Builder run, the render nodes network will be constructed by gradually
 ** refining the connectivity structure derived from interpreting the »high-level model«
 ** from the current Session. At some point, it is essentially clear what data streams
 ** must be produced and what media processing functionality from external libraries
 ** will be utilised to achieve this goal. This is when the fluent builder notation
 ** defined in this header comes into play, allowing to package the fine grained and
 ** in part quite confusing details of parameter wiring and invocation preparation into
 ** some goal oriented building blocks, that can be combined and directed with greater
 ** clarity by the control structure to govern the build process.
 ** 
 ** # Levels of connectivity building
 ** 
 ** The actual node connectivity is established by a process of gradual refinement,
 ** operating over several levels of abstraction. Each of these levels uses its associated
 ** builder and descriptor records to collect information, which is then emitted by a
 ** _terminal invocation_ to produce the result; the higher levels thereby rely on the
 ** lower levels to fill in and elaborate the details.
 ** - Level-1 is the preparation of an actual frame processing operation; the Level-1-builder
 **   is in fact the implementation class sitting behind a Render Node's _Port._ It is called
 **   a _Turnout_ and contains a preconfigured »blue print« for the data structure layout
 **   used for the invocation; its purpose is to generate the actual data structure on the
 **   stack, holding all the necessary buffers and parameters ready for invoking the external
 **   library functions. Since the actual data processing is achieved by a _pull processing,_
 **   originating at the top level exit nodes and propagating down towards the data sources,
 **   all the data feeds at all levels gradually link together, forming a _TurnoutSystem._
 ** - Level-2 generates the actual network of Render Nodes, which in turn will have the
 **   Turnout instances for Level-1 embedded into its internal ports. Conceptually, a
 **   _Port_ is where data production can be requested, and the processing will then
 **   retrieve its prerequisite data from the ports of the _Leads,_ which are the
 **   prerequisite nodes situated one level below or one step closer to the source.
 ** - Level-3 establishes the processing steps and data retrieval links between them;
 **   at this level, thus the outline of possible processing pathways is established.
 **   After spelling out the desired connectivity at a high level, the so called »Level-3 build
 **   walk« is triggered by invoking the [terminal builder operation](\ref ProcBuilder::build()
 **   on the [processing builder](\ref ProcBuilder) corresponding to the topmost node. This
 **   build walk will traverse the connectivity graph depth-first, and then start invoking the
 **   Level-2 builder operations bottom-up to generate and wire up the corresponding Render Nodes.
 ** 
 ** @todo WIP-WIP-WIP 7/2024 Node-Invocation is reworked from ground up -- some parts can not be
 **       spelled out completely yet, since we have to build this tightly interlocked system of
 **       code moving bottom up, and then filling in further details later working top-down.
 ** 
 ** @see steam::engine::NodeFactory
 ** @see nodewiring.hpp
 ** @see node-basic-test.cpp
 ** 
 */


#ifndef ENGINE_NODE_BUILDER_H
#define ENGINE_NODE_BUILDER_H


#include "steam/engine/proc-node.hpp"
#include "lib/several-builder.hpp"
#include "lib/nocopy.hpp"

#include <utility>
#include <vector>


namespace steam {
namespace engine {
  
  using std::move;
  using std::forward;
  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
  namespace {
    
    template<template<typename> class ALO =std::void_t, typename...INIT>
    struct AlloPolicySelector
      {
        template<class I, class E=I>
        static auto
        setupBuilder (INIT&& ...alloInit)
        {
          return lib::makeSeveral<I,E>()
                     .template withAllocator<ALO> (forward<INIT> (alloInit)...);
        }
        
        template<class I, class E=I>
        using BuilderType = decltype(setupBuilder<I,E> (std::declval<INIT>()...));
      };
    
    struct UseHeapAlloc
      {
        template<class I, class E=I>
        static auto
        setupBuilder()
        {
          return lib::makeSeveral<I,E>();
        }
        
        template<class I, class E=I>
        using BuilderType = lib::SeveralBuilder<I,E>;
      };
    
    template<class POL, class I, class E=I>
    using DataBuilder = typename POL::template BuilderType<I,E>;
  }
  
  template<class POL>
  class PortBuilder;
  
  template<class POL>
  class NodeBuilder
    : util::MoveOnly
    {
      
      using PortData = DataBuilder<POL, Port>;
      
      PortData ports_;
      std::vector<ProcNodeRef>  leads_{};
    public:
      template<typename...INIT>
      NodeBuilder (INIT&& ...alloInit)
        : ports_{POL::template setupBuilder<Port> (forward<INIT> (alloInit)...)}
        { }
      
      NodeBuilder
      addLead (ProcNode const& lead)
        {
          UNIMPLEMENTED ("append the given predecessor node to the sequence of leads");
          return move(*this);
        }
      
      
      void //////////////////////////////////////////////////////////OOO return type
      preparePort ()
        {
          UNIMPLEMENTED ("recursively enter detailed setup of a single processing port");
//        return move(*this);
        }
      
      /****************************************************//**
       * Terminal: complete the Connectivity defined thus far.
       */
      Connectivity
      build()
        {
          /////////////////////////////////////////////////////////////////////OOO actually use the collected data to build
          return Connectivity{ports_.build()
                             ,lib::makeSeveral<ProcNodeRef>().build()
                             ,NodeID{}};
        }
    };
  
  template<class POL>
  class PortBuilder
    : protected NodeBuilder<POL>
    , util::MoveOnly
    {
    public:
      
      PortBuilder
      inSlots (uint s)
        {
          UNIMPLEMENTED ("define number of predecessor-source slots");
          return move(*this);
        }
      
      PortBuilder
      outSlots (uint r)
        {
          UNIMPLEMENTED ("define number of result slots");
          return move(*this);
        }
      
      template<class ILA, typename...ARGS>
      PortBuilder
      createBuffers (ARGS&& ...args)
        {
          UNIMPLEMENTED ("define builder for all buffers to use");
          return move(*this);
        }
      
      /****************************************************//**
       * Terminal: complete the Port wiring and return to the node level.
       */
      void //////////////////////////////////////////////////////////OOO return type
      completePort()
        {
          UNIMPLEMENTED("finish and link-in port definition");
        }
    };
  
  
  /**
   * Entrance point for building actual Render Node Connectivity (Level-2)
   */
  inline auto
  prepareNode()
  {
    return NodeBuilder<UseHeapAlloc>{};
  }
    
  
  class ProcBuilder
    : util::MoveOnly
    {
    public:
      
      void //////////////////////////////////////////////////////////OOO return type
      requiredSources ()
        {
          UNIMPLEMENTED ("enumerate all source feeds required");
//        return move(*this);
        }
      
      void //////////////////////////////////////////////////////////OOO return type
      retrieve (void* streamType)
        {
          UNIMPLEMENTED ("recursively define a predecessor feed");
//        return move(*this);
        }
      
      /****************************************************//**
       * Terminal: trigger the Level-3 build walk to produce a ProcNode network.
       */
      void //////////////////////////////////////////////////////////OOO return type
      build()
        {
          UNIMPLEMENTED("Level-3 build-walk");
        }
    };
  
  
  class LinkBuilder
    : util::MoveOnly
    {
    public:
      
      void //////////////////////////////////////////////////////////OOO return type
      from (void* procAsset)
        {
          UNIMPLEMENTED ("recursively enter definition of processor node to produce this feed link");
//        return move(*this);
        }
    };
  
  
  
  /**
   * Entrance point for defining data flows and processing steps.
   */
  inline auto
  retrieve(void* streamType)
  {
    UNIMPLEMENTED("start a connectivity definition at Level-3");
    return LinkBuilder{};  ///////////////////////////////////////////////////////////////////OOO this is placeholder code; should at least open a ticket
  }
  
  
}} // namespace steam::engine
#endif /*ENGINE_NODE_BUILDER_H*/
