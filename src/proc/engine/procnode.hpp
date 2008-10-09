/*
  PROCNODE.hpp  -  Key abstraction of the Render Engine: a Processing Node
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
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
 
*/

/** @file procnode.hpp
 ** Interface to the processing nodes and the render nodes network.
 **
 ** Actually, there are three different interfaces to consider
 ** - the ProcNode#pull is the invocation interface. It is call-style (easily callable by C)
 ** - the builder interface, comprised by the NodeFactory and the WiringFactory. It's C++ (using templates)
 ** - the actual processing function is supposed to be a C function; it uses a set of C functions 
 **   for accessing the frame buffers with the data to be processed.
 **
 ** By using the builder interface, concrete node and wiring descriptor classes are created,
 ** based on some templates. These concrete classes form the "glue" to tie the node network
 ** together and contain much of the operation beahviour in a hard wired fashion.
 **
 ** @see nodefactory.hpp
 ** @see operationpoint.hpp
 */

#ifndef ENGINE_PROCNODE_H
#define ENGINE_PROCNODE_H

#include "pre.hpp"

#include "proc/common.hpp"
#include "proc/state.hpp"
#include "proc/mobject/parameter.hpp"
#include "lib/refarray.hpp"

#include <vector>



namespace engine {

  using std::vector;
  using proc_interface::State;
  
  class ProcNode;
  class NodeFactory;
  
  typedef ProcNode* PNode;
  
  
  /**
   * Interface: Description of the input and output ports,
   * processing function and predecessor nodes for a given ProcNode.
   */
  class WiringDescriptor
    {
    public:
      virtual ~WiringDescriptor() {}
      
      virtual uint getNrI()  const =0;           ///////////TODO: indeed need a virtual function??
      virtual uint getNrO()  const =0;
      
      lib::RefArray<ChannelDescriptor>& out;
      lib::RefArray<InChanDescriptor>&  in;
      
      typedef void (ProcFunc) (BuffHandle::PBuff, uint);
      
      ProcFunc* processFunction;
      
    protected:
      WiringDescriptor (lib::RefArray<ChannelDescriptor>& o, 
                        lib::RefArray<InChanDescriptor>& i,
                        ProcFunc pFunc)
        : out(o), in(i),
          processFunction(pFunc)
        { }  
      
      /** the wiring-dependent part of the node operation.
       *  Includes the creation of a one-way state object on the stack
       *  holding the actual buffer pointers and issuing the recursive pull() calls
       *  @see NodeWiring#callDown default implementation
       */
      virtual BuffHandle  callDown (State& currentProcess, uint requiredOutputNr)  const =0; 
      
      friend class ProcNode;
      
    };
  
  
  
  
  /**
   * Key abstraction of the Render Engine: A Data processing Node
   */
  class ProcNode
    {
      typedef mobject::Parameter<double> Param;   //////TODO: just a placeholder for automation as of 6/2008
      vector<Param> params;
      
      const WiringDescriptor& wiringConfig_;
      
    protected:
      ProcNode (WiringDescriptor const& wd)
        : wiringConfig_(wd)
        { }
        
      virtual ~ProcNode() {};
      
      friend class NodeFactory;
      
      
      
    public:
      static NodeFactory create; ///////TODO: really? probably we'll rather have a NodeFactory object in the builder...
      
      /** Engine Core operation: render and pull output from this node.
       *  On return, currentProcess will hold onto output buffer(s) 
       *  containing the calculated result frames. In case this node
       *  calculates a multi channel output, only one channel can be 
       *  retrieved by such a \c pull() call, but you can expect data
       *  of the other channels to be processed and fed to cache.
       *  @param currentProcess the current processing state for 
       *         managing buffers and accessing current parameter values
       *  @param requiredOutputNr the output channel requested (in case
       *         this node delivers more than one output channel)
       *  @return handle to the buffer containing the calculated result. 
       */
      BuffHandle
      pull (State& currentProcess, uint requiredOutputNr=0)  const
        {
          return this->wiringConfig_.callDown (currentProcess, requiredOutputNr);
        }
      
    };
  
} // namespace engine
#endif
