/*
  OPERATIONPOINT.hpp  -  abstraction representing the point where to apply a build instruction

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


/** @file operationpoint.hpp
 ** Abstraction to represent the current point of operation within the builder.
 ** The Builder walks the tree of the high-level-model (session model) to generate
 ** a (low-level) render node network. The current point of operation within this tree walk
 ** is abstracted, to allow the use of some _building primitives._ A meta description of
 ** *processing patterns* can be used then to control the actual build process, without
 ** the need of hard wiring the individual builder steps.
 ** 
 ** @todo stalled design draft from 2008 -- basically still considered relevant as of 2016
 */


#ifndef MOBJECT_BUILDER_OPERATIONPOINT_H
#define MOBJECT_BUILDER_OPERATIONPOINT_H

//#include "steam/engine/proc-node.hpp"         /////TODO can we get rid of this header here?
//#include "common/query.hpp"
#include "lib/hash-standard.hpp"

#include <memory>
#include <vector>
#include <string>


namespace steam {

namespace asset { class Proc; }
namespace asset { class Media; }
namespace engine { class NodeFactory; }

namespace mobject {
namespace builder {
  
  using std::vector;
  using std::string;
  
  struct RefPoint;
  
  /**
   * A point in the render node network under construction.
   * By means of this unspecific reference, a ProcPatt is able
   * to deal with this location and to execute a single elementary
   * building operation denoted by a BuildInstruct at this point.
   * Usually, the actual point is retrieved from a Mould
   */
  class OperationPoint
    {
      std::unique_ptr<RefPoint> refPoint_;
      
    public:
      OperationPoint (engine::NodeFactory&, asset::Media const& srcMedia);
      OperationPoint (RefPoint const& sourcePoint);
      
      
      
      /** create node(s) corresponding to the given Processor-Asset
       *  and wire them as a successor to this OperationPoint; then
       *  move this point to refer to the resulting new exit node(s)
       */
      void attach (asset::Proc const&);
      
      /** connect the output this OperationPoint refers such as to
       *  connect or combine with the input of the already existing
       *  nodes accessible via the target OperationPoint.
       */
      void join (OperationPoint& target);
    };
  
  
  
}}} // namespace steam::mobject::builder
#endif /*MOBJECT_BUILDER_OPERATIONPOINT_H*/
