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

#include "proc/mobject/parameter.hpp"




namespace engine {

  using std::vector;
  
  class ProcNode;
  class NodeFactory;
  
  typedef ProcNode* PNode;    ///< @todo handle ProcNode by pointer or by shared-ptr??


  /**
   * Key abstraction of the Render Engine: A Data processing Node
   */
  class ProcNode
    {
      typedef mobject::Parameter<double> Param;

      /** The predecessor in a processing pipeline.
       *  I.e. a source to get data to be processed
       */
      PNode datasrc;

      vector<Param> params;

    protected:
      ProcNode();
      virtual ~ProcNode() {};
      
      friend class NodeFactory;
      
      
      /** do the actual calculations.
       *  @internal dispatch to implementation. 
       *            Client code should use #render()
       *  @todo obviously we need a parameter!!!
       */
      virtual void process() = 0;

    public:
      static NodeFactory create;
      
      /** render and pull output from this node.
       *  @todo define the parameter!!!
       */
      void render() { this->process(); }

    };

} // namespace engine
#endif
