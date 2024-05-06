/*
  PROC-NODE.hpp  -  Key abstraction of the Render Engine: a Processing Node

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

*/

/** @file proc-node.hpp
 ** Interface to the processing nodes and the render nodes network.
 **
 ** Actually, there are three different interfaces to consider
 ** - the ProcNode#pull is the invocation interface. It is function-call style
 ** - the builder interface, comprised by the NodeFactory and the WiringFactory.
 ** - the actual processing function is supposed to be a C function and will be
 **   hooked up within a thin wrapper.
 **
 ** By using the builder interface, concrete node and wiring descriptor classes are created,
 ** based on some templates. These concrete classes form the "glue" to tie the node network
 ** together and contain much of the operation behaviour in a hard wired fashion.
 **
 ** @see nodefactory.hpp
 ** @see operationpoint.hpp
 */

#ifndef ENGINE_PROC_NODE_H
#define ENGINE_PROC_NODE_H

#include "lib/error.hpp"
#include "steam/common.hpp"
#include "steam/asset/proc.hpp"
#include "steam/mobject/parameter.hpp"
#include "steam/engine/state-closure.hpp"
#include "steam/engine/channel-descriptor.hpp"
#include "lib/frameid.hpp"
#include "lib/ref-array.hpp"

#include <vector>



namespace steam {
namespace engine {

  using std::vector;
  using lumiera::NodeID;
  
  class ProcNode;
  typedef ProcNode* PNode;
  
  
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
   */
  class Connectivity
    {
    public: /* === public information record describing the node graph === */
      uint nrO;
      uint nrI;
      
      lib::RefArray<ChannelDescriptor>& out;
      lib::RefArray<InChanDescriptor>&  in;
      
      typedef asset::Proc::ProcFunc ProcFunc;
      
      ProcFunc* procFunction;
      
      NodeID const& nodeID;
      
      virtual ~Connectivity() {}
      
    protected:
      Connectivity (lib::RefArray<ChannelDescriptor>& o,
                        lib::RefArray<InChanDescriptor>& i,
                        ProcFunc pFunc, NodeID const& nID)
        : out(o), in(i),
          procFunction(pFunc),
          nodeID(nID)
        {
          nrO = out.size();
          nrI = in.size();
        }
      
      
      /* ==== strategy API for configuring the node operation ==== */
      
      friend class ProcNode;                               /////////////////////////////////TODO 1/12 : wouldn't it be better to extract that API into a distinct strategy?
      
      /** the wiring-dependent part of the node operation.
       *  Includes the creation of a one-way state object on the stack
       *  holding the actual buffer pointers and issuing the recursive pull() calls
       *  @see NodeWiring#callDown default implementation
       */
      virtual BuffHandle
      callDown (StateClosure& currentProcess, uint requiredOutputNr)  const =0;
      
    };
  
  
  
  
  /**
   * Key abstraction of the Render Engine: A Data processing Node
   * 
   * @todo it's not clear as of 9/09 if ProcNode shall be an ABC/Interface
   *       It might be used as ABC (as was the original intention) when implementing
   *       several query/information functions. In that case, the ctor will become protected.
   *       The alternative would be to push down these information-retrieval part into a
   *       configurable element within Connectivity, in which case we even might drop
   *       ProcNode as a frontend entirely.
   */
  class ProcNode
    : util::NonCopyable
    {
      typedef mobject::Parameter<double> Param;   //////TODO: just a placeholder for automation as of 6/2008
      vector<Param> params;
      
      const Connectivity& wiringConfig_;
      
    public:
      ProcNode (Connectivity const& wd)
        : wiringConfig_(wd)
        { }
      
      virtual ~ProcNode() {};  /////////////////////////TODO: do we still intend to build a hierarchy below ProcNode???
      
      
    public:
      bool isValid()  const;
      
      /** output channel count */
      uint nrO() { return wiringConfig_.nrO; }
      
      
      /** Engine Core operation: render and pull output from this node.
       *  On return, currentProcess will hold onto output buffer(s)
       *  containing the calculated result frames. In case this node
       *  calculates a multichannel output, only one channel can be
       *  retrieved by such a \c pull() call, but you can expect data
       *  of the other channels to be processed and fed to cache.
       *  @param currentProcess the current processing state for
       *         managing buffers and accessing current parameter values
       *  @param requestedOutputNr the output channel requested
       *         (in case this node delivers more than one output channel)
       *  @return handle to the buffer containing the calculated result.
       */
      BuffHandle
      pull (StateClosure& currentProcess, uint requestedOutputNr=0)  const
        {
          return this->wiringConfig_.callDown (currentProcess, requestedOutputNr);
        }
      
    };
  
  
  inline bool
  ProcNode::isValid()  const
  {
    UNIMPLEMENTED ("ProcNode validity self-check");
    return false; //////////////////////////TODO
  }
  
  
}} // namespace steam::engine
#endif /*ENGINE_PROC_NODE_H*/
