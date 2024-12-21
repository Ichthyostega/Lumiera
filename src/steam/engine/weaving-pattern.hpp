/*
  WEAVING-PATTERN.hpp  -  Base patterns to organise data connections and render processing invocation

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>
     2024,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file weaving-pattern.hpp
 ** Construction set to assemble and operate a data processing scheme within a Render Node.
 ** Together with turnout.hpp, this header provides the "glue" which holds together the
 ** typical setup of a Render Node network for processing media data. A MediaWeavingPattern
 ** implements the sequence of steps — as driven by the Turnout — to combine the invocation
 ** of media processing operations from external Libraries with the buffer- and parameter
 ** management provided by the Lumiera Render Engine. Since these operations are conducted
 ** concurrently, all invocation state has to be maintained in local storage on the stack.
 ** 
 ** # Integration with media handling Libraries
 ** 
 ** A Render invocation originates from a [Render Job](\ref render-invocation.hpp), which first
 ** establishes a TurnoutSystem and then enters into the recursive Render Node activation by
 ** invoking Port::weave() for the »Exit Node«, as defined by the job's invocation parameters.
 ** The first step in the processing cycle, as established by the Port implementation (\ref Turnout),
 ** is to build a »Feed instance«, from the invocation of `mount(TurnoutSystem&)`.
 ** 
 ** Generally speaking, a `Feed` fulfils the role of an _Invocation Adapter_ and a _Manifold_ of
 ** data connections. The standard implementation, as given by MediaWeavingPattern, relies on a
 ** combination of both into a \ref FeedManifold. This is a flexibly configured data adapter,
 ** directly combined with an embedded _adapter functor_ to wrap the invocation of processing
 ** operations provided by an external library.
 ** 
 ** Usually some kind of internal systematics is assumed and applied within such a library.
 ** Operations can be exposed as plain function to invoke, or through some configuration and
 ** builder notion. Function arguments tend to follow a common arrangement and naming scheme,
 ** also assuming specific arrangement and data layout for input and output data. This kind of
 ** schematism is rooted in something deeper: exposing useful operations as a library collection
 ** requires a common ground, an understanding about the _order of things_ to be treated — at least
 ** for those kind of things, which fall into a specific _domain,_ when tasks related to such a
 ** domain shall be supported by the Library. Such an (implicit or explicit) framework of structuring
 ** is usually designated as a **Domain Ontology** (in contrast to the questions pertaining Ontology
 ** in general, which are the subject of philosophy proper). Even seemingly practical matters like
 ** processing media data do rely on fundamental assumptions and basic premises regarding what is
 ** at stake and what shall be subject to treatment, what fundamental entities and relationships
 ** to consider within the domain. Incidentally, many of these assumptions are positive in nature
 ** and not necessarily a given — which is the root of essential incompatibilities between Libraries
 ** targeting a similar domain: due to such fundamental differences, they just can not totally agree
 ** upon what kinds of things to expect and where to draw the line of distinction.
 ** 
 ** The Lumiera Render Engine and media handling framework is built in a way _fundamentally agnostic_
 ** to the specific presuppositions of this or that media handling library. By and large, decisions,
 ** distinctions and qualifications are redirected back into the scope of the respective library, by
 ** means of a media-library adapter plug-in. Assuming that the user _in fact understands_ the meaning
 ** of and reasoning behind employing a given library, the _mere handling_ of the related processing
 ** can be reduced to a small set of organisational traits. For sake of consistency, you may label
 ** these as a »Render Engine Ontology«. In all brevity,
 ** - We assume that the library provides distinguishable processing operations
 **   that can be structured and classified and managed as _processing assets,_
 ** - we assume that processing is applied to sources or »media« and that
 **   the result of processing is again a source that can be processed further;
 ** - specific operations can thus be conceptualised as processing-stages or _Nodes,_
 **   interconnected by _media streams,_ which can be tagged with a _stream type._
 ** - At implementation level, such streams can be represented in entirety as data buffers
 **   of a specific layout, filled with some »frame« or chunk of data
 ** - and the single processing step or operation can be completely encapsulated
 **   as a pure function (referentially transparent, without side effects);
 ** - all state and parametrisation can be represented either as some further data stream in/out,
 **   or as parameters-of-processing, which can be passed as a set of values to the function
 **   prior of invocation, thereby completely determining the observable behaviour.
 ** 
 ** # composition of the Invocation State
 ** 
 ** By means of this positing, the attachment point to an external library can be reduced into a small
 ** number of connector links. Handling the capabilities of a library within the Session and high-level Model
 ** will require some kind of _registration,_ which is beyond the scope of this discussion here. As far as
 ** the Render Engine and the low-level-Model is concerned, any usage of the external libraries capabilities
 ** can be reduced into...
 ** - Preparing an adapter functor, designated as **processing-functor**. This functor takes three kinds of
 **   arguments, each packaged as a single function call argument, which may either be a single item, or
 **   possibly be structured as a tuple of heterogeneous elements, or an array of homogeneous items.
 **   + an output data buffer or several such buffers are always required
 **   + (optionally) an input buffer or several such buffers need to be supplied
 **   + (optionally) also a parameter value, tuple or array can be specified
 ** - Supplying actual parameter values (if necessary); these are drawn from the invocation
 **   of a further functor, designated as **parameter-functor**, and provided from within
 **   the internal framework of the Lumiera application, either to deliver fixed parameter
 **   settings configured by the user in the Session, or by evaluating _Parameter Automation,_
 **   or simply to supply some technically necessary context information, most notably the
 **   frame number of a source to retrieve.
 ** - Preparing buffers filled with input data, in a suitable format, one for each distinct
 **   item expected in the input data section of the processing-functor; filling these
 **   input buffers requires the _recursive invocation_ of further Render Nodes...
 ** - Allocating buffers for output data, sized and typed accordingly, likewise one for
 **   each distinct item detailed in the output data argument of the processing-functor.
 ** 
 ** The FeedManifold template, which (as mentioned above) is used by this standard implementation
 ** of media processing in the form of the MediaWeavingPattern, is configured specifically for each
 ** distinct signature of a processing-functor to match the implied structural requirements. If a
 ** functor is output-only, no input buffer section is present; if it expects processing parameters,
 ** storage for an appropriate data tuple is provided and a parameter-functor can be configured.
 ** A clone-copy of the processing-functor itself is also stored as clone-copy alongside within
 ** the FeedManifold, and thus placed into stack memory, where it is safe even during deeply nested
 ** recursive invocation sequences, while rendering in general is performed massively in parallel.
 ** 
 ** In the end, the actual implementation code of the weaving pattern has to perform the connection
 ** and integration between the »recursive weaving scheme« and the invocation structure implied by
 ** the FeedManifold. It has to set off the recursive pull-invocation of predecessor ports, retrieve
 ** the result data buffers from these and configure the FeedManifold with the \ref BuffHandle entries
 ** retrieved from these recursive calls. Buffer handling in general is abstracted and codified thorough
 ** the [Buffer Provider framework](\ref buffer-provider.hpp), which offers the means to allocate further
 ** buffers and configure them into the FeedManifold for the output data. The »buffer handling protocol«
 ** also requires to invoke BuffHandle::emit() at the point when result data can be assumed to be placed
 ** into the buffer, and to release buffers not further required through a BuffHandle::release() call;
 ** Notably this applies to the input buffers are completion of the processing-functor invocation, and
 ** is required also for secondary (and in a way, superfluous) result buffers, which are sometimes
 ** generated as a by-product of the processing function invocation, but are actually not passed
 ** as output up the node invocation chain.
 ** 
 ** @see feed-manifold.hpp
 ** @see weaving-pattern-builder.hpp
 ** @see \ref proc-node.hpp "Overview of Render Node structures"
 ** 
 ** @warning WIP as of 12/2024 first complete integration round of the Render engine ////////////////////////////TICKET #1367
 ** 
 */


#ifndef STEAM_ENGINE_WEAVING_PATTERN_H
#define STEAM_ENGINE_WEAVING_PATTERN_H

#include "steam/common.hpp"
#include "steam/engine/turnout.hpp"
#include "steam/engine/turnout-system.hpp"
#include "steam/engine/feed-manifold.hpp"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #1367 : Rebuild the Node Invocation
//#include "vault/gear/job.h"
//#include "steam/engine/exit-node.hpp"
//#include "lib/time/timevalue.hpp"
//#include "lib/linked-elements.hpp"
#include "lib/several.hpp"
//#include "lib/util-foreach.hpp"
//#include "lib/iter-adapter.hpp"
//#include "lib/meta/function.hpp"
//#include "lib/itertools.hpp"
//#include "lib/util.hpp"      ////////OOO wegen manifoldSiz<FUN>()

//#include <stack>


namespace steam {
namespace engine {
  
  using lib::Several;
  
  
    /**
     * Standard implementation for a _Weaving Pattern_ to connect
     * the input and output data feeds (buffers) into a processing function.
     * @tparam INVO a configuration / policy base class to _adapt for invocation_
     * @note assumptions made regarding the overall structure
     *     - `INVO::Feed` defines an _invocation adapter_ for the processing function
     *     - `INVO::buildFeed()` is a functor to (repeatedly) build `Feed` instances
     *     - the _invocation adapter_ in turn embeds a `FeedManifold<FUN>` to hold
     *       + a setup of output buffer pointers (single, tuple or array)
     *       + (optionally) a similar setup for input buffer pointers
     *       + (optionally) a parameter or parameter tuple
     *       + storage to configure BuffHandle entries for each «slot»
     *       + storage to hold the actual processing functor
     */
  template<class INVO>
  struct MediaWeavingPattern
    : INVO
    {
      using Feed = typename INVO::Feed;
      
      static_assert (_verify_usable_as_InvocationAdapter<Feed>());
      
      Several<PortRef>   leadPort;
      Several<BuffDescr> outTypes;
      
      uint resultSlot{0};
      
      /** forwarding-ctor to provide the detailed input/output connections */
      template<typename...ARGS>
      MediaWeavingPattern (Several<PortRef>&&   pr
                          ,Several<BuffDescr>&& dr
                          ,uint resultIdx
                          ,ARGS&& ...args)
        : INVO{forward<ARGS>(args)...}
        , leadPort{move(pr)}
        , outTypes{move(dr)}
        , resultSlot{resultIdx}
        { }
      
      
      Feed
      mount (TurnoutSystem& turnoutSys)
        {
          ENSURE (leadPort.size() <= INVO::FAN_I);
          ENSURE (outTypes.size() <= INVO::FAN_O);
          return INVO::buildFeed (turnoutSys);
        }
      
      void
      pull (Feed& feed, TurnoutSystem& turnoutSys)
        {
          if constexpr (Feed::hasInput())
            for (uint i=0; i<leadPort.size(); ++i)
              {
                BuffHandle inputData = leadPort[i].get().weave (turnoutSys);
                feed.inBuff.createAt(i, move(inputData));
              }
        }
      
      void
      shed (Feed& feed, OptionalBuff outBuff)
        {
          for (uint i=0; i<outTypes.size(); ++i)
              {
                BuffHandle resultData =
                  i == resultSlot and outBuff? *outBuff
                                             : outTypes[i].lockBuffer();
                feed.outBuff.createAt(i, move(resultData));
              }
          feed.connect();
        }
      
      void
      weft (Feed& feed)
        {
          feed.invoke();                 // process data
        }
      
      BuffHandle
      fix (Feed& feed)
        {
          if constexpr (Feed::hasInput())
            for (uint i=0; i<leadPort.size(); ++i)
              {
                feed.inBuff[i].release();
              }
          for (uint i=0; i<outTypes.size(); ++i)
              {
                feed.outBuff[i].emit();    // state transition: data ready
                if (i != resultSlot)
                  feed.outBuff[i].release();
              }
          ENSURE (resultSlot < INVO::FAN_O, "invalid result buffer configured.");
          return feed.outBuff[resultSlot];
        }
      
    };
  
  
  
}}// namespace steam::engine
#endif /*STEAM_ENGINE_WEAVING_PATTERN_H*/
