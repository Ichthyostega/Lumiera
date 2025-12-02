/*
  SegmentationTool  -  Tool for creating a partitioning of the current timeline

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


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
