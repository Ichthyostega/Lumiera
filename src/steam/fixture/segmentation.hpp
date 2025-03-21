/*
  SEGMENTATION.hpp  -  Partitioning of a timeline for organising the render graph.

   Copyright (C)
     2023,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file segmentation.hpp
 ** Part of the Fixture datastructure to manage time segments of constant structure.
 ** The Fixture is result of the build process and separation between high-level and
 ** low-level model. It's kind of an effective resulting timeline, and split into segments
 ** of constant wiring structure: whenever the processing nodes need to be wired differently
 ** for some timespan, we start a new segment of the timeline. This might be for the duration
 ** of a clip, or just for the duration of a transition, when the pipes of both clips need to
 ** be wired up in parallel.
 ** 
 ** Within the Fixture, a Segment of the timeline is used as attachment point for all the
 ** render nodes relevant for rendering this segment. Thus, the Segmentation defines the
 ** index and access datastructure to get at any point of the render node network.
 ** Moreover, the segments are used as foundation for render node memory management
 ** 
 ** @todo 5/2023 now actually started with the implementation for the »Playback Vertical Slice«
 ** 
 ** @see Fixture
 ** @see ModelPort
 */


#ifndef STEAM_FIXTURE_SEGMENTATION_H
#define STEAM_FIXTURE_SEGMENTATION_H


#include "steam/fixture/segment.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/iter-adapter-stl.hpp"
#include "lib/format-string.hpp"
#include "lib/nocopy.hpp"

#include <list>
#include <optional>
#include <functional>


namespace steam {
namespace fixture {
  
  namespace error = lumiera::error;
  
  using std::list;
  using lib::time::TimeValue;
  using util::_Fmt;
  
  using OptTime = std::optional<lib::time::Time>;
  
  /**
   * For the purpose of building and rendering, the fixture (for each timeline)
   * is partitioned such that each segment is _structurally constant._
   * The Segmentation defines and maintains this partitioning. Furthermore,
   * it is the general entry point for accessing the correct part of the engine
   * responsible for a given timeline time point.
   * @ingroup fixture
   * @see SegmentationTool actually calculating the Segmentation
   * 
   * @todo 1/2012 Just a Placeholder. The real thing is not yet implemented.
   * @todo 4/2023 now about to bootstrap into the implementation structure step by step (WIP)
   * @see https://lumiera.org/wiki/renderengine.html#Fixture
   */
  class Segmentation
    : util::NonCopyable
    {
      /** segments of the engine in ordered sequence. */
      list<Segment> segments_;
      
    protected:
      Segmentation()                ///< there is always a single cover-all Segment initially
        : segments_{1}
      { }
      
    public:
      virtual ~Segmentation();      ///< this is an interface
      
      size_t
      size()  const
        {
          return segments_.size();
        }
      
      Segment const&
      operator[] (TimeValue time)  const
        {
          for (auto& seg : segments_)
            if (seg.after() > time)
              return seg;
          throw error::State (_Fmt{"Fixture datastructure corrupted: Time %s not covered"} % time);
        }
      
      auto
      eachSeg()  const              ///< @return iterator to enumerate each segment in ascending time order
        {
          return lib::iter_stl::eachElm (segments_);
        }
      
      
      /** rework the existing Segmentation to include a new Segment as specified */
      Segment const&
      splitSplice (OptTime start, OptTime after, engine::ExitNodes&& modelLink  =ExitNodes{});
      
      
    protected:
      /** @internal rewrite the NodeGraphAttachment in each Segment */
      void
      adaptSpecification (std::function<NodeGraphAttachment(NodeGraphAttachment const&)> rewrite)
        {
          for (fixture::Segment& seg : segments_)
              seg.exitNode = move(rewrite (seg.exitNode));
        }
    };
  
  
  
}} // namespace steam::fixture
#endif /*STEAM_FIXTURE_SEGMENTATION_H*/
