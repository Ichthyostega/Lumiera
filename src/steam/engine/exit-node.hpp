/*
  EXIT-NODE.hpp  -  top-level node of the render network to pull generated media

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

/** @file exit-node.hpp
 ** Effective top-level exit point to pull rendered data from the nodes network.
 ** 
 ** @todo 6/2023 WIP-WIP used as placeholder; relation to ProcNode not yet determined           /////////////TICKET #1306
 ** @see procnode.hpp
 */

#ifndef ENGINE_EXIT_NODE_H
#define ENGINE_EXIT_NODE_H

#include "lib/error.hpp"
#include "lib/nocopy.hpp"
#include "lib/hash-value.h"
#include "lib/iter-adapter-stl.hpp"

#include <deque>

using lib::HashVal;


namespace steam {
namespace engine {

  class ExitNode;
  using ExitNodes = std::deque<engine::ExitNode>;
  
  
  
  /**
   * A top-level point in the render node network where data generation can be driven.
   * 
   * @todo 6/2023 for the »Playback Vertical Slice« we need somehow to represent "the nodes",
   *       while the final solution how to hook up ProcNode and how to represent prerequisites
   *       remains still to be settled. So this is a placeholder to support mock testing for now.
   */
  class ExitNode
    : util::MoveOnly
    {
      HashVal   pipelineIdentity_;                 //////////////////////////////////////////////////////////TICKET #1293 : Hash-Chaining for invocation-ID... derive from ProcNode wiring
      ExitNodes prerequisites_;                   ///////////////////////////////////////////////////////////TICKET #1306 : actual access to low-level-Model (ProcNode)
      
    public:
      ExitNode()
        : pipelineIdentity_{0}
        , prerequisites_{}
      { }
      
      ExitNode (HashVal id, ExitNodes&& prereq =ExitNodes{})
        : pipelineIdentity_{id}
        , prerequisites_{std::move (prereq)}
      { }
      
      static ExitNode NIL;
      
      
      HashVal
      getPipelineIdentity()  const
        {
          return pipelineIdentity_;
        }
      
      auto
      getPrerequisites()  const
        {
          return lib::iter_stl::eachElm (prerequisites_);
        }
    };
  
  
}} // namespace steam::engine
#endif /*ENGINE_EXIT_NODE_H*/
