/*
  SEGMENTATION-TOOL.hpp  -  Tool for creating a partitioning of the current timeline

   Copyright (C)
     2008,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file segmentation-tool.hpp
 ** Visiting tool to create a viable _segmentation_ of the timeline for rendering.
 ** Within each such segment, the _configuration and wiring_ of the render node network
 ** is considered stable. Any further variation within a segment is created solely by varying
 ** parameters (automation). The created Segmentation is considered the "backbone" of the
 ** low-level render node network
 ** @todo stalled design draft from 2008 -- basically still considered relevant as of 2016
 */


#ifndef MOBJECT_BUILDER_SEGMENTATION_TOOL_H
#define MOBJECT_BUILDER_SEGMENTATION_TOOL_H


#include "steam/mobject/builder/applicable-builder-target-types.hpp"

#include "steam/fixture/segmentation.hpp"
#include "steam/fixture/fixture.hpp"   //////TODO really on the header??


#include <list>
using std::list;



namespace steam {
namespace mobject {
namespace builder {
  
  
  /**
   * Tool implementation for deriving a partitioning of the current 
   * timeline, such that each Segment has a constant configuration. 
   * "Constant" means here, that any remaining changes over time
   * can be represented by automation solely, without the need 
   * to change the node connections.
   */
  class SegmentationTool 
    : public ApplicableBuilderTargetTypes<SegmentationTool>
    {
      
    public:
      SegmentationTool (fixture::Fixture &) ;
      
      void treat (mobject::session::Clip& clip) ;
      void treat (mobject::session::Effect& effect) ;
      
      void treat (mobject::Buildable& something) ;
      
      void onUnknown (Buildable& target) ;             /////////TODO why doesn't the treat(Buildable) function shadow this??
      
      bool empty()  const;
      
    private:
      using Segment = fixture::Segment;
      
      /** Partitioning of the Timeline to be created by this tool. */
      //session::Segmentation& segments_;
      ///////////////////////////////////////////TODO: either put it inline, or use a scopend_ptr!!!!!!!!!!
      
    };
  
  
  
}}} // namespace steam::mobject::builder
#endif /*MOBJECT_BUILDER_SEGMENTATION_TOOL_H*/
