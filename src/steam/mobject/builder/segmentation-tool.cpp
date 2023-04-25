/*
  SegmentationTool  -  Tool for creating a partitioning of the current timeline

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


/** @file segmentation-tool.cpp
 ** Implementation of builder primitives to create a timeline segmentation.
 ** @todo stalled design draft from 2008 -- basically still considered relevant as of 2016
 */


#include "steam/mobject/builder/segmentation-tool.hpp"


namespace steam {
namespace mobject {
namespace builder {
  
  using mobject::Buildable;
  using session::Clip;
  using session::Effect;
  using fixture::Fixture;
  
                                      /////////////////////////////////TICKET #414
  
  
  SegmentationTool::SegmentationTool(Fixture&)
  {
    UNIMPLEMENTED ("create new SegmentationTool");
  }
  
  
  void
  SegmentationTool::treat (Buildable& something)
  {
    UNIMPLEMENTED ("??? when partitioning timeline");
  }
  
  
  void
  SegmentationTool::treat (Clip& clip)
  {
    UNIMPLEMENTED ("consider clip when partitioning timeline");
  }
  
  
  void
  SegmentationTool::treat (Effect& effect)
  {
    UNIMPLEMENTED ("note applied effect when partitioning timeline");
  }
  
  
  void
  SegmentationTool::onUnknown (Buildable& target)
  {
    UNIMPLEMENTED ("catch-all when partitioning timeline");
  }
  
  
  bool
  SegmentationTool::empty()  const
  {
    UNIMPLEMENTED ("detect an empty segmentation");
  }
  
  
}}} // namespace steam::mobject::builder
