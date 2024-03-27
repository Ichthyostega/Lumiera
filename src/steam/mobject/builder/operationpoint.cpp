/*
  OperationPoint  -  abstraction representing the point where to apply a build instruction

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

* *****************************************************/


/** @file operationpoint.cpp
 ** Implementation of primitive builder operations on at the _current point of operation_.
 ** @todo stalled design draft from 2008 -- basically still considered relevant as of 2016
 */


#include "steam/mobject/builder/operationpoint.hpp"
#include "steam/engine/proc-node.hpp"
#include "steam/asset/media.hpp"
#include "steam/asset/proc.hpp"
#include "steam/engine/nodefactory.hpp"
#include "steam/streamtype.hpp"
//#include "common/util.hpp"

//#include <boost/ptr_container/ptr_vector.hpp>

namespace steam {
namespace mobject {
namespace builder {
  
//  using util::isnil;
  using engine::NodeFactory;
  
  using engine::PNode;
  
  
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
      RefPoint (NodeFactory& nFactory, asset::Media const& srcMedia)
        : alloc_(nFactory),
          sType_(0)
        {
          establishMediaChannels (srcMedia);
          deriveSourceStreamType ();
        }
      
      
    private:
      void
      establishMediaChannels (asset::Media const& srcMedia)
        {
          UNIMPLEMENTED ("find out about the actual channels for this stream to connect");
        }
      
      void
      deriveSourceStreamType ()
        {
          UNIMPLEMENTED ("calculate / decide on the effective stream type used within this pipe");
        }
      
    };
  
  
  
  OperationPoint::OperationPoint (NodeFactory& nFact, asset::Media const& srcMedia)
    : refPoint_(new RefPoint (nFact, srcMedia))
  { }
  
  
  OperationPoint::OperationPoint (RefPoint const& sourcePoint)
    : refPoint_(new RefPoint (sourcePoint))
  { }
  
  
  void
  OperationPoint::attach (asset::Proc const& mediaProc)
  {
    UNIMPLEMENTED ("Actually-really-finally create a node corresponding to this processing asset");
  }
  
  
  void
  OperationPoint::join (OperationPoint& target)
  {
    UNIMPLEMENTED ("Actually-really-finally do a mixing output connection");
  }
  
  
  
}}} // namespace steam::mobject::builder
