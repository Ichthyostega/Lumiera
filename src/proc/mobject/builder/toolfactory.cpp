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


#include "proc/mobject/builder/toolfactory.hpp"
#include "lib/util.hpp"

#include <boost/noncopyable.hpp>
#include <memory>

namespace proc {
namespace mobject {
namespace builder {
  
  using util::isnil;
  using std::auto_ptr;
  using std::unique_ptr;
  
  
  struct BuildProcessState
    {
      
      session::Fixture & fixedTimeline_;
      auto_ptr<engine::RenderGraph> procSegment_;
      
      unique_ptr<SegmentationTool> segmentation_;
      unique_ptr<NodeCreatorTool> fabrication_;
      
      
      BuildProcessState (session::Fixture& theTimeline)
        : fixedTimeline_(theTimeline),
          procSegment_(new engine::RenderGraph())
        { }
      
    };
  
  ToolFactory::ToolFactory (session::Fixture& theTimeline)
    : state_(new BuildProcessState (theTimeline))
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
  
  
                   //////////////////////////////////////////TODO: a better idea than using auto_ptr?
  auto_ptr<engine::RenderGraph>
  ToolFactory::getProduct ()
  {
    state_->segmentation_.reset(0);
    state_->fabrication_.reset(0);
    return state_->procSegment_;
  }
  
  
  
}}} // namespace proc::mobject::builder
