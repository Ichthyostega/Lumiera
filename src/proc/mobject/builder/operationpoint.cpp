/*
  OperationPoint  -  abstraction representing the point where to apply a build instruction
 
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
 
* *****************************************************/


#include "proc/mobject/builder/operationpoint.hpp"
#include "proc/asset/media.hpp"
#include "proc/engine/nodefactory.hpp"
#include "lib/streamtype.hpp"
//#include "common/util.hpp"

//#include <boost/ptr_container/ptr_vector.hpp>

namespace mobject {
namespace builder {
  
//  using util::isnil;
  using engine::NodeFactory;
  
  using engine::PNode;
  using lumiera::StreamType;
  
  
  struct RefPoint
    {
      NodeFactory& alloc_;
      
      vector<PNode> sources_;
      const StreamType * sType_;  //////TODO: maybe use the ChannelDescriptor??
      
      
      /** just continue connection chaining */
      RefPoint (RefPoint const& pred)
        : alloc_(pred.alloc_),
          sources_(pred.sources_),
          sType_(pred.sType_)
        { }
      
      /** initiate a connection chain at a real source */
      RefPoint (NodeFactory& nFactory, asset::Media srcMedia)
        : alloc_(nFactory),
          sources_(establishMediaChannels (srcMedia)),
          sType_(0)
        {
          deriveSourceStreamType ();
        }
    };
  
  
  
  OperationPoint::OperationPoint (NodeFactory& nFact,, asset::Media const& srcMedia)
    : refPoint_(new RefPoint (nFact, srcMedia))
  { }
  
  
  OperationPoint::OperationPoint (RefPoint const& sourcePoint)
    : refPoint_(new RefPoint (sourcePoint))
  { }
  
  
  void
  OperationPoint::attach (asset::PProc const& mediaProc)
  {
    UNIMPLEMENTED ("Actually-really-finally create a node corresponding to this processing asset");
  }
  
  
  void
  OperationPoint::join (OperationPoint& target)
  {
    UNIMPLEMENTED ("Actually-really-finally do a mixing output connection");
  }
  
  
  
}} // namespace mobject::builder
