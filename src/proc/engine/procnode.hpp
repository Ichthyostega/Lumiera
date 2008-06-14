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


#ifndef ENGINE_PROCNODE_H
#define ENGINE_PROCNODE_H

#include <vector>

#include "proc/state.hpp"
#include "proc/mobject/parameter.hpp"




namespace engine {

  using std::vector;
  using proc_interface::State;
  
  class ProcNode;
  class NodeFactory;
  
  typedef ProcNode* PNode;
  
  
  /**
   * Description of the input and output ports and the
   * predecessor nodes for a given ProcNode.
   */
  struct WiringDescriptor
    {
      
    };
    

  /**
   * Adapter to shield the ProcNode from the actual buffer management,
   * allowing the processing function within ProcNode to use logical
   * buffer IDs. StateAdapter is created on the stack for each pull()
   * call, using setup/wiring data preconfigured by the builder.
   * Its job is to provide the actual implementation of the Cache
   * push / fetch and recursive downcall to render the source frames.
   */
  class StateAdapter
    : public State
    {
      State& parent_;
      State& current_;
      
    protected:
      StateAdapter (State& callingProcess, WiringDescriptor const&) 
        : parent_ (callingProcess),
          current_(callingProcess.getCurrentImplementation())
        { }
      
      friend class ProcNode; // both are sharing implementation details...
      
      
      virtual State& getCurrentImplementation () { return current_; }
      
      /** contains the details of Cache query and recursive calls
       *  to the predecessor node(s), eventually followed by the 
       *  ProcNode::process() callback
       */
      void retrieve();
    };
  
  
  /**
   * Key abstraction of the Render Engine: A Data processing Node
   */
  class ProcNode
    {
      typedef mobject::Parameter<double> Param;   //////TODO: just a placeholder for automation as of 6/2008
      vector<Param> params;

    protected:
      ProcNode();
      virtual ~ProcNode() {};
      
      friend class NodeFactory;
      
      
      /** Callback doing the actual calculations
       *  after input / output buffers are ready
       *  @internal dispatch to implementation. 
       *            Client code should use #pull()
       */
      virtual void process(State&) = 0;
      
      friend void StateAdapter::retrieve();  // to issue the process() callback if necessary
      
      
      /** Extension point for subclasses to modify the input sources
       *  according to automation data and other state dependent properties. 
       */
      virtual WiringDescriptor const& getWiring (State&); 
      

    public:
      static NodeFactory create;
      
      /** Engine Core operation: render and pull output from this node.
       *  On return, currentProcess will hold onto output buffer(s)
       *  containing the calculated result frames.
       */
      void 
      pull (State& currentProcess) 
        {
          StateAdapter thisStep (currentProcess, getWiring (currentProcess));
          thisStep.retrieve(); // fetch or calculate results
        }

    };

} // namespace engine
#endif
