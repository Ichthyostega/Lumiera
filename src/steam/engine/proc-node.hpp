/*
  PROC-NODE.hpp  -  Key abstraction of the Render Engine: a Processing Node

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/

/** @file proc-node.hpp
 ** Interface to the processing nodes and the Render Nodes network.
 ** The Lumiera Render Engine is based on a graph of interconnected Render Nodes.
 ** This »Low-level-Model« is pre-arranged _by a Builder_ as result of compiling
 ** and interpreting the arrangement created by the user in the Session, known as
 ** »High-level-Model«. All ways to possibly _perform_ (play, render) the current
 ** arrangement are thus encoded into the configuration and connectivity of
 ** ProcNode elements.
 ** 
 ** # Usage
 ** 
 ** Regarding access, there are three different interfaces to consider
 ** - the ProcNode#pull is the invocation interface. It is function-call style
 ** - the builder interface, comprising the NodeBuilder (TODO 2024 what else actually?).
 ** - the control of playback and rendering processes is accomplished by the Player.
 ** For actual processing, Lumiera relies on functionality provided by dedicated
 ** domain libraries (e.g. FFmpeg for video processing). A binding implemented
 ** as a Lumiera Plug-in will expose such a Library's resources as _Assets_
 ** and will set up _function bindings_ to be embedded into Render Nodes.
 **
 ** By using the NodeBuilder interface, concrete \ref ProcNode and \ref Port instances
 ** are created, interconnected and attached below the Fixture, which is the »backbone«
 ** of the low-level-Model. The coordination and the act of invoking this NodeBuilder
 ** is conducted by structures in the **Builder** subsystem of Lumiera, which works
 ** similar to a compiler for programming languages — with the difference that within
 ** this application an edit and media arrangement is compiled into „executable“ form,
 ** ready for rendering and performance. In this context _performance_ implies to »play«
 ** (render) part of a »Timeline«, which is accomplished through a PlayProcess, which
 ** in turn breaks down the work into individual »Render Jobs« organised through the
 ** [Scheduler](\ref scheduler.hpp). Through such a sequence of translations, the
 ** processing of a frame ends up as a [job to invoke](\ref render-invocation.hpp)
 ** some entrance point into the Render Node network.
 ** 
 ** # Arrangement of Render Nodes
 ** 
 ** The arrangement of ProcNode elements in the render graph exhibits a [DAG] topology.
 ** Each node _knows only its direct predecessors,_ designated as »Lead Nodes« or »Leads«.
 ** Conceptually, each Node represents a specific processing capability, delegating internally
 ** to some actual Library implementation of the desired processing algorithm. Yet in reality,
 ** several _flavours_ of this processing capability are typically required. For example, maybe
 ** sound processing will be expected in stereo format (channel interleaved blocks of audio samples),
 ** but in addition also the two individual mono channels will be required independently. Or a video
 ** processing pipeline might be required in full resolution, but also sampled down for display in
 ** a GUI viewer window or for thumbnail images. The existence of several _flavours of computation_
 ** might seem obvious or irrelevant — yet touches on a fundamental decision: in Lumiera,
 ** **no media processing happens beyond the Render Nodes.** Even for the down-sampled
 ** tiny preview images, a render pipeline is specifically preconfigured, and then
 ** exposed through a \ref Port on the Render Node.
 ** 
 ** The actual rendering thus proceeds through the successive activation of Ports. Internally, each
 ** Port is connected to _predecessor ports,_ which can be _»pulled«_ to generate the _input data_ for
 ** the current processing step. Conducting the invocation of a single processing step in a Port thus
 ** requires the interplay of several, intricately interwoven activities — forming a »Weaving Pattern«:
 ** Establishing a frame, pulling from predecessors, spawning out further memory buffers to hold computed
 ** result data and finally triggering the actual »weft«. The _Port on a Node_ is thus _an interface_,
 ** actually implemented by a \ref Turnout, which comprises a _Weaving Pattern Template._ The most
 ** common scheme for media processing is embodied by the \ref MediaWeavingPattern template, yet
 ** other Weaving Patterns may be configured to adapt to different processing needs (e.g. hardware
 ** accelerated computation).
 ** 
 ** Templates in C++ _must be instantiated,_ with arguments specific to the usage. Which would be
 ** the signature of the processing function, the types and number of input- and output buffers
 ** and an additional tuple of specific parameters to pass, like e.g. the frame number or the
 ** (possibly automated) parameter settings for an effect. The invocation of a Port thus calls
 ** through a (classical, function-virtual) interface into specific code instantiated within the
 ** Library-adapter Plug-in. At compile time, consistency of involved buffer types and function
 ** signatures and memory allocation schemes can be ensured, so that no further checks and dynamic
 ** adaptation and transformation is necessary at runtime. The engine implementation works on
 ** data tuples, typed buffer pointers and helpers checked for memory safety — and not on
 ** plain arrays and void pointers.
 ** @remark A future extension to this scheme is conceivable, where common processing pipelines
 **         are pre-compiled in entirety, possibly combined with hardware acceleration.
 ** 
 ** @todo WIP-WIP 12/2024 Node-Invocation is reworked from ground up for the »Playback Vertical Slice«
 ** 
 ** @see turnout.hpp
 ** @see turnout-system.hpp
 ** @see node-builder.hpp
 ** @see node-link.test.cpp
 ** [DAG]: https://en.wikipedia.org/wiki/Directed_acyclic_graph
 */

#ifndef STEAM_ENGINE_PROC_NODE_H
#define STEAM_ENGINE_PROC_NODE_H

#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/hash-value.h"
//#include "steam/asset/proc.hpp"
//#include "steam/mobject/parameter.hpp"
#include "steam/engine/buffhandle.hpp"
#include "steam/engine/turnout-system.hpp"
#include "lib/ref-array.hpp" /////////////////////OOO phase out
#include "lib/format-string.hpp"
#include "lib/several.hpp"

#include <string>
#include <optional>



namespace steam {
namespace engine {
  namespace err = lumiera::error;

  using std::move;
  using std::string;
  using lib::HashVal;
  using util::_Fmt;
  
  class ProcID;
  class ProcNode;
  class ProcNodeDiagnostic;
  
  using ProcNodeRef = std::reference_wrapper<ProcNode>;
  using OptionalBuff = std::optional<BuffHandle>;
  
  
  /** arbitrary safety limit on fain-in / fan-out
   * @note expect lower limits in practice caused by AllocationCluster */
  const size_t MAX_NODE_ARG = LUMIERA_MAX_ORDINAL_NUMBER / 2;
  
  
  
  class Port
    : util::NonCopyable
    {
    public:
      virtual ~Port();  ///< this is an interface
      Port (ProcID& id) : procID{id} { }
      
      virtual BuffHandle weave (TurnoutSystem&, OptionalBuff =std::nullopt)   =0;
      
      ProcID& procID;
    };
  
  using PortRef = std::reference_wrapper<Port>;
  
  /**
   * Interface: Description of the input and output ports,
   * processing function and predecessor nodes for a given ProcNode.
   * 
   * @todo the design of this part is messy in several respects.
   *       Basically, its left-over from a first prototypical implementation from 2008
   *       As of 1/2012, we're re-shaping that engine interface and invocation with a top-down approach,
   *       starting from the player. Anyhow, you can expect the basic setup to remain as-is: there will
   *       be a ProcNode and a Connectivity descriptor, telling how it's connected to its predecessors,
   *       and defining how the Node is supposed to operate
   * 
   * @todo WIP-WIP-WIP 2024 Node-Invocation is reworked from ground up for the »Playback Vertical Slice«
   */
  class Connectivity
    {
    public: /* === public information record describing the node graph === */
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
      using Ports = lib::Several<Port>;
      using Leads = lib::Several<ProcNodeRef>;
      
      Ports ports;
      Leads leads;
      
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
      
//  protected:
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
      Connectivity (Ports&& pr, Leads&& lr)
        : ports(move(pr))
        , leads(move(lr))
        { }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
      
      
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
      /* ==== strategy API for configuring the node operation ==== */
      
      friend class ProcNode;                               /////////////////////////////////OOO who needs friendship?
      
      /** the wiring-dependent part of the node operation.
       *  Includes the creation of a one-way state object on the stack
       *  holding the actual buffer pointers and issuing the recursive pull() calls
       *  @see NodeWiring#callDown default implementation
       */
#if false /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
      virtual BuffHandle
      callDown (StateClosure_OBSOLETE& currentProcess, uint requiredOutputNr)  const =0;
#endif    /////////////////////////////////////////////////////////////////////////////////////////////////////////////UNIMPLEMENTED :: TICKET #1367 : Rebuild the Node Invocation
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
      
    };
  
  
  
  
  /**
   * Key abstraction of the Render Engine: A Data processing Node.
   * 
   * @todo WIP 2025 Node-Invocation is reworked from ground up for the »Playback Vertical Slice«
   */
  class ProcNode
    : util::NonCopyable
    {
      
      Connectivity wiring_;
      
    public:
      ProcNode (Connectivity&& con)
        : wiring_(move(con))
        { }
      
      
      Port&
      getPort (uint portIdx)
        {
          if (portIdx >= wiring_.ports.size())
            throw err::Logic{_Fmt{"Accessing node-port #%d, while only %d ports are defined."}
                     % portIdx % wiring_.ports.size()
                ,LERR_(INDEX_BOUNDS)
                };
          return wiring_.ports[portIdx];
        }
      
      
      /*************************************************************//**
       * Engine Core operation: render and pull output from this node.
       * Activate the indicated Port, which may recursively pull from
       * »Lead Nodes« (predecessors) and invoke the operation embodied
       * by this node. The stack frame of this call acts as anchor
       * to hold the TurnoutSystem for parameter access and coordination.
       * @param portIdx the port (≙flavour) to activate
       * @param output  a BuffHandle configured suitably to hold results
       * @param nomTime _absolute nominal Time_ to key this invocation
       * @param procKey additional process key for context parameters
       * @return handle to the buffer containing the calculated result.
       */
      BuffHandle
      pull (uint portIdx, BuffHandle output, Time nomTime, ProcessKey procKey)
        {
          TurnoutSystem turnoutSystem{nomTime, procKey};
          return getPort(portIdx).weave (turnoutSystem, output);
        }
      
      /// „backdoor“ to watch internals from tests
      friend class ProcNodeDiagnostic;
    };
  
  
  /* ========== Diagnostic and Testing ========== */
  
  class ProcNodeDiagnostic
    : util::MoveOnly
    {
      ProcNode& n_;
      
    public:
      ProcNodeDiagnostic (ProcNode& theNode)
        : n_{theNode}
        { }
      
      auto& leads() { return n_.wiring_.leads; }
      auto& ports() { return n_.wiring_.ports; }
      
      bool isSrc()  { return n_.wiring_.leads.empty(); }
      
      bool
      isValid()
        {
          return 0 < ports().size();
          ///////////////////////////////////////////////////TODO 10/2024 more to verify here
        }
      
      string getNodeSpec();   ///< generate a descriptive Spec of this ProcNode for diagnostics
      HashVal getNodeHash();  ///< calculate an unique hash-key to designate this node
      
      string getPortSpec  (uint portIdx);  ///< generate a descriptive diagnostic Spec for the designated Turnout
      HashVal getPortHash (uint portIdx);  ///< calculate an unique, stable and reproducible hash-key to identify the Turnout
    };
  
  inline ProcNodeDiagnostic
  watch (ProcNode& theNode)
  {
    return ProcNodeDiagnostic{theNode};
  }
  
  
  
  class PortDiagnostic
    : util::MoveOnly
    {
      Port& p_;
      
    public:
      PortDiagnostic (Port& thePort)
        : p_{thePort}
        { }
      
      lib::Several<PortRef> const& srcPorts();
      
      bool isSrc()  { return srcPorts().empty(); }
      
      string getProcSpec();   ///< generate a descriptive diagnostic Spec for the Turnout sitting behind this Port
      HashVal getProcHash();  ///< calculate an unique, stable and reproducible hash-key to identify the associated operation
    };
  
  inline PortDiagnostic
  watch (Port& thePort)
  {
    return PortDiagnostic{thePort};
  }
  
  
}} // namespace steam::engine
#endif /*STEAM_ENGINE_PROC_NODE_H*/
