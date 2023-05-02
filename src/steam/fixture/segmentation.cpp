/*
  Segmentation  -  Partitioning of a timeline for organising the render graph.

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


/** @file segmentation.cpp
 ** @todo stalled effort towards a session implementation from 2008
 ** @todo 2016 likely to stay, but expect some extensive rework
 */

#include "lib/error.hpp"
#include "steam/fixture/segmentation.hpp"
//#include "steam/mobject/builder/fixture-change-detector.hpp"  ///////////TODO



namespace steam {
namespace fixture {
  
  namespace error = lumiera::error;
  
  
//  typedef ModelPortRegistry::ModelPortDescriptor const& MPDescriptor;
  
  
  /** storage for the link to the global
      Registry instance currently in charge  */
//  lib::OptionalRef<ModelPortRegistry> ModelPortRegistry::theGlobalRegistry;
  
  
  Segmentation::~Segmentation() { }   // emit VTable here...
  
  /** access the globally valid registry instance.
   *  @throw error::State if this global registry is
   *         already closed or not yet initialised. */
//ModelPortRegistry&
//ModelPortRegistry::globalInstance()
//{
//  LockRegistry global_lock;
//  if (theGlobalRegistry.isValid())
//    return theGlobalRegistry();
//  
//  throw error::State ("global model port registry is not accessible"
//                     , LERR_(BUILDER_LIFECYCLE));
//}
  
  
  
  /**
   * @param start (optional) definition of the new Segment's start point (inclusive)
   * @param after (optional) definition of the end point (exclusive)
   * @param jobTicket specification of provided render functionality for the new Segment
   * @remarks missing definitions will be derived or interpolated according to context
   *   - if start point is omitted, the new Segment will start seamlessly after
   *     any preceding Segment's end, in case this preceding Segment ends earlier
   *   - otherwise the preceding Segment's start point will be used, thereby effectively
   *     replacing and expanding or trimming or inserting into the preceding Segment
   *   - similar for the end point: if the definition is omitted, the new Segment
   *     will cover the time range until the next Segmen's start
   *   - if upper/lower boundaries can not be established, the covered range will be
   *     expanded from Time::ANYTIME up to Time::ANYTIME in as fitting current context
   *   - after start and end point have been established by the above rules, the actual
   *     splicing operation will be determined; either an existing Segment is replaced
   *     altogether, or it is trimmed to fit, or the new Segment is inserted, thereby
   *     creating a second (copied) part of the encompassing old Segment.
   *   - in case the JobTicket is omitted, the new Segment will be marked as _passive_
   *     and any job created from such a Segment will then be a »NOP-job«
   */
  Segment const&
  Segmentation::splitSplice (OptTime start, OptTime after, const engine::JobTicket* jobTicket)
  {
    UNIMPLEMENTED ("determine predecessor, successor and orientation and perform del/trunc/split/swap/insert");
  }
  
  
  
//LUMIERA_ERROR_DEFINE (DUPLICATE_MODEL_PORT, "Attempt to define a new model port with an pipe-ID already denoting an existing port");
  
  
  
}} // namespace steam::fixture
