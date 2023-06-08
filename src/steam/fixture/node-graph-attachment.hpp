/*
  NODE-GRAPH-ATTACHMENT.hpp  -  Binding from a Fixture-Segment into the low-level-Model

  Copyright (C)         Lumiera.org
    2023,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file node-graph-attachment.hpp
 ** Link from the Fixture datastructure into the render node network.
 ** The Segmentation is a partitioning of the effective timeline into segments with
 ** uniform processing structure; however this structure itself is defined as a graph
 ** of [render nodes](\ref engine::ProcNode); any specifics and decisions how rendering
 ** actually happens are represented in the way some ModelPort is connected to the model,
 ** which is the purpose of this binding object, stored as part of each Segment.
 ** 
 ** @todo 2023 WIP-WIP-WIP »Playback Vertical Slice« need at least a drafted model
 */


#ifndef STEAM_FIXTURE_NODE_GRAPH_ATTACHMENT_H
#define STEAM_FIXTURE_NODE_GRAPH_ATTACHMENT_H

#include "steam/engine/exit-node.hpp"
//#include "lib/time/timevalue.hpp"

#include <utility>

namespace steam {
namespace fixture {
  
  using engine::ExitNode;
  using engine::ExitNodes;
  
  
  /**
   * Binding and access point from a given Segment to access the actual render nodes.
   * For each ModelPort, we can expect to get an ExitNode (the number of ports is fixed
   * for the complete Timeline). However, this ExitNode does not need to be active, since
   * parts of the timeline can be empty, or partially empty for some ModelPort.
   * @warning while this descriptor object is freely copyable, the referred ExitNode(s) are
   *          move-only, and will be referred by the JobTicket. Thus a Segment in the Fixture
   *          must remain fixed in memory as long as any derived render jobs are alive.
   * 
   * @todo WIP-WIP as of 4/2023 -- starting with a fake implementation                  /////////////////////TICKET #1306 : create an actual link to the low-level Model
   */
  class NodeGraphAttachment
    {
      ExitNodes exitNodes_;
      
    public:
      NodeGraphAttachment()
        : exitNodes_{}
      { }
      
      NodeGraphAttachment (ExitNodes&& exitNodes)
        : exitNodes_{std::move (exitNodes)}
      { }
      
      // default copy acceptable
      
      ExitNode const&
      operator[] (size_t idx)  const
        {
          return idx < exitNodes_.size()? exitNodes_[idx]
                                        : ExitNode::NIL;
        }
    };
  
  
  
}} // namespace steam::fixture
#endif /*STEAM_FIXTURE_NODE_GRAPH_ATTACHMENT_H*/
