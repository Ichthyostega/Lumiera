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
#include "lib/time/timevalue.hpp"
#include "vault/engine/job.h"              //////////////////////////////////////////////////////////////////TICKET #1295 : rather need a way to retrieve a real JobFunctor building block from the ProcNode

#include <deque>

using lib::HashVal;
using lib::time::FSecs;        ////TODO using hard wired value
using lib::time::Duration;


namespace steam {
namespace engine {

  class ExitNode;
  using ExitNodes = std::deque<engine::ExitNode>;
  
  using vault::engine::JobFunctor;
  
  
  /**
   * A top-level point in the render node network where data generation can be driven.
   * 
   * @todo 6/2023 for the »Playback Vertical Slice« we need somehow to represent "the nodes",
   *       while the final solution how to hook up ProcNode and how to represent prerequisites
   *       remains still to be settled. So this is a placeholder to support mock testing for now.
   * @warning ExitNode should ideally be NonCopyable, since it is referred by the JobTicket
   *          However, we need to clone-and-remould Segments (Split-Splice-Algo), and this implies
   *          that the render nodes can be shared among multiple Segments. If all these assessments
   *          are correct an only be decided when the actual memory management is settled.
   */
  class ExitNode
    : util::Cloneable
    {
      HashVal   pipelineIdentity_;                 //////////////////////////////////////////////////////////TICKET #1293 : Hash-Chaining for invocation-ID... derive from ProcNode wiring
      ExitNodes prerequisites_;                   ///////////////////////////////////////////////////////////TICKET #1306 : actual access to low-level-Model (ProcNode)
      JobFunctor* action_{nullptr};              ////////////////////////////////////////////////////////////TICKET #1295 : link to actual implementation action in low-level-Model
      
    public:
      ExitNode()
        : pipelineIdentity_{0}
        , prerequisites_{}
      { }
      
      ExitNode (HashVal id
               ,ExitNodes&& prereq =ExitNodes{}
               ,JobFunctor* functor =nullptr)
        : pipelineIdentity_{id}
        , prerequisites_{std::move (prereq)}
        , action_{functor}
      { }
      
      static ExitNode NIL;
      
      
      bool
      empty()  const
        {
          return 0 == pipelineIdentity_
              or not action_;
        }
      
      bool
      isValid()  const
        {
          return true;                ///////////////////////////////////////////////////////////////////////TICKET #826 : ProcNode groundwork; integrate self-validation of render network
        }
      
      
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
      
      JobFunctor&
      getInvocationFunctor()  const
        {
          REQUIRE (action_);
          return *action_;            ///////////////////////////////////////////////////////////////////////TICKET #1295 : decision on actual JobFunctor and invocation parameters
        }
      
      Duration
      getUpperBoundRuntime()  const
        {
                                  ///////////////////////////////////////////////////////////////////////////TICKET #1283 : lay foundation how to observe timing behaviour for a render pipeline
          return Duration{FSecs{1,50}};  // Uh-Oh booo
        }
    };
  
  
}} // namespace steam::engine
#endif /*ENGINE_EXIT_NODE_H*/
