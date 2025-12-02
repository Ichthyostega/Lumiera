/*
  OperationPoint  -  abstraction representing the point where to apply a build instruction

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file operationpoint.cpp
 ** Implementation of primitive builder operations on at the _current point of operation_.
 ** @todo stalled design draft from 2008 -- basically still considered relevant as of 2016
 ** @todo confirmed 12/2024 : the ideas drafted here are considered relevant,
 **       yet it is clear meanwhile that the Build will work on meta-descriptors,
 **       and not the actual Render Nodes
 */


#include "steam/mobject/builder/operationpoint.hpp"
#include "steam/asset/media.hpp"
#include "steam/asset/proc.hpp"
#include "steam/engine/node-factory.hpp"
#include "steam/streamtype.hpp"
//#include "common/util.hpp"

//#include <boost/ptr_container/ptr_vector.hpp>

namespace steam {
namespace mobject {
namespace builder {
  
//  using util::isnil;
  using engine::NodeFactory;
  
  /** @todo placeholder introduced 12/2024 to abstract from the actual Render Nodes */
  struct PNode
    {
      ;
    };
  
  
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
