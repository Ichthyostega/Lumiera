/*
  NODEFACTORY.hpp  -  Service for creating processing nodes of various kinds

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


/** @file nodefactory.hpp
 ** A factory to build processing nodes
 ** @todo draft from the early days of the Lumiera project, reworked in 2009
 **       Development activity in that area as stalled ever since. In the end,
 **       it is highly likely we'll _indeed need_ a node factory though...
 */


#ifndef ENGINE_NODEFACTORY_H
#define ENGINE_NODEFACTORY_H

#include "steam/engine/procnode.hpp"
#include "steam/mobject/placement.hpp"
#include "steam/engine/nodewiring.hpp"


  ////////////////////////////////////TODO: make inclusions / forward definitions a bit more orthogonal....

namespace lib { class AllocationCluster; }

namespace proc {
namespace mobject {
namespace session {
  
  class Clip;
  class Effect;
  typedef Placement<Effect> PEffect;
  // TODO: class Transition;
  
}} // namespace mobject::session


namespace engine {

  using std::vector;
  using lib::AllocationCluster;
  
  ///////////////////////////// TODO: as of 9/09, it seems no longer necessary to use any subclasses of ProcNode
  class Trafo;
  typedef Trafo* PTrafo;    ///< @note ProcNode is handled by pointer and bulk allocated/deallocated
  
  class WiringSituation;


  /**
   * Create processing nodes based on given objects of the high-level model.
   */
  class NodeFactory
    {
      AllocationCluster& alloc_;
      WiringFactory wiringFac_;
      
    public:
      NodeFactory (AllocationCluster& a)
        : alloc_(a)
        , wiringFac_(alloc_)
        { }
      
      
      PNode operator() (mobject::session::PEffect const&, WiringSituation&);
        
    };

}} // namespace proc::engine
#endif
