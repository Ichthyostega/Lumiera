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


#include "proc/mobject/builder/segmentation-tool.hpp"


using mobject::Buildable;
using mobject::session::Clip;
using mobject::session::Effect;


namespace proc {
namespace mobject {
namespace builder {
  
                                      /////////////////////////////////TICKET #414
  
  
  SegmentationTool::SegmentationTool(mobject::session::Fixture&)
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
  
  
}}} // namespace proc::mobject::builder
