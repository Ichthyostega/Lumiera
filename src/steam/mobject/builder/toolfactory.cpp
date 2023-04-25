/*
  ToolFactory  -  supply of Tool implementations for the Builder

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


/** @file toolfactory.cpp
 ** Implementation of factory functions for the tools used within the Builder
 ** @todo stalled design draft from 2008 -- basically still considered relevant as of 2016
 */


#include "steam/mobject/builder/toolfactory.hpp"
#include "lib/util.hpp"

#include <memory>

namespace steam {
namespace mobject {
namespace builder {
  
  using util::isnil;
  using std::unique_ptr;
  
  
  struct BuildProcessState
    {
      
      fixture::Fixture & fixedTimeline_;
      unique_ptr<engine::RenderGraph> procSegment_;
      
      unique_ptr<SegmentationTool> segmentation_;
      unique_ptr<NodeCreatorTool> fabrication_;
      
      
      BuildProcessState (fixture::Fixture& theTimeline)
        : fixedTimeline_(theTimeline),
          procSegment_(new engine::RenderGraph())
        { }
      
    };
  
  ToolFactory::ToolFactory (fixture::Fixture& theFixture)
    : state_(new BuildProcessState (theFixture))
  {
    ENSURE (state_->fixedTimeline_.isValid());
    ENSURE (state_->procSegment_.get());
  }
  
  
  SegmentationTool &
  ToolFactory::configureSegmentation ()
  {
    REQUIRE (state_->fixedTimeline_.isValid());
    REQUIRE (state_->procSegment_.get());
    
    state_->segmentation_.reset (new SegmentationTool (state_->fixedTimeline_));
    return *(state_->segmentation_);
  }
  
  
  NodeCreatorTool &
  ToolFactory::configureFabrication () //////////////////////TODO: should iterate in some way!
  {
    REQUIRE (state_->procSegment_.get());
    REQUIRE (!isnil (*(state_->segmentation_)));
    
    state_->fabrication_.reset (new NodeCreatorTool(*this, *state_->procSegment_));
    return *(state_->fabrication_);
  }
  
  
  engine::RenderGraph&
  ToolFactory::getProduct ()
  {
    state_->segmentation_.reset(0);
    state_->fabrication_.reset(0);
    UNIMPLEMENTED ("anything regarding the fixture and build process....");
    return *state_->procSegment_;
  }
  
  
  
}}} // namespace steam::mobject::builder
