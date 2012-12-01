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


#ifndef MOBJECT_BUILDER_OPERATIONPOINT_H
#define MOBJECT_BUILDER_OPERATIONPOINT_H

//#include "proc/engine/procnode.hpp"         /////TODO can we get rid of this header here?
//#include "common/query.hpp"

#include <boost/scoped_ptr.hpp>
#include <vector>
#include <string>


namespace proc {

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
      boost::scoped_ptr<RefPoint> refPoint_;
      
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
  
  
  
}}} // namespace proc::mobject::builder
#endif
