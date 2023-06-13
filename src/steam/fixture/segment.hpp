/*
  SEGMENT.hpp  -  Segment of the timeline for rendering.

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

*/


/** @file segment.hpp
 ** Building block of the backbone of the low-level (render node) model
 ** @todo WIP 6/2023 now actually in implementation due to »Playback Vertical Slice«
 ** @todo Datastructure as such is settled; memory allocation and remoulding of an
 **       existing Segmentation remains future work as of 6/2023
 */


#ifndef STEAM_FIXTURE_SEGMENT_H
#define STEAM_FIXTURE_SEGMENT_H


#include "steam/common.hpp"
#include "steam/fixture/node-graph-attachment.hpp"
#include "steam/mobject/explicitplacement.hpp"
#include "steam/engine/job-ticket.hpp"
#include "lib/allocator-handle.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/itertools.hpp"
#include "lib/util.hpp"

#include <utility>
#include <deque>
#include <tuple>


namespace steam {
namespace fixture {
  
  using mobject::ExplicitPlacement;
  using lib::time::TimeSpan;
  using lib::time::Time;
  using util::unConst;
  using std::move;
  
  /**
   * For the purpose of building and rendering, the fixture (for each timeline)
   * is partitioned such that each segment is _structurally constant._
   * For each segment there is a RenderGraph (unit of the render engine)
   * which is able to render all ExitNode(s) for this segment.
   * 
   * @ingroup fixture
   * @todo WIP-WIP as of 6/2023 -- about to establish the engine backbone
   * @see http://lumiera.org/wiki/renderengine.html#Fixture
   */
  class Segment
    {
      using JobTicket   = engine::JobTicket;
      using TicketAlloc = lib::AllocatorHandle<JobTicket>;
      using PortTable   = std::deque<std::reference_wrapper<JobTicket>>;
      
    protected:
      
      /** begin of this timeline segment. */
      TimeSpan span_;
      
      /** manage JobTicket: render plan / blueprint to use for this segment */
      TicketAlloc ticketAlloc_;
      PortTable   portTable_;
      
      ///////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #725 : placeholder code
      /** relevant MObjects comprising this segment. */
      std::deque<ExplicitPlacement> elements;
      // TODO: actually necessary??
      // TODO: ownership??
      ///////////////////////////////////////////////////////////////////////////////////////////////////////TICKET #725 : placeholder code
    public:
      explicit
      Segment (TimeSpan span =TimeSpan::ALL) /// create empty Segment
        : Segment{span, NodeGraphAttachment{}}
      { }
      
      /**
       * create a new Segment to cover the given TimeSpan
       * and to offer the rendering capabilities exposed by \a modelLink
       */
      Segment (TimeSpan covered
              ,NodeGraphAttachment&& modelLink)
        : span_{covered}
        , ticketAlloc_{}
        , portTable_{}
        , exitNode{move (modelLink)}
      { }
      
      /** copy-and-remould an existing Segment to sit at another time span
       * @see Segmentation::splitSplice
       */
      Segment (Segment const& original, TimeSpan changed)
        : span_{changed}
        , ticketAlloc_{} // Note: not cloning tickets owned by Segment
        , portTable_{}
        , exitNode{original.exitNode}   /////////////////////////////////////////////////////////////////////OOO really? cloning ExitNodes?
      { }
      
      // default copy acceptable
      
      Time start() const { return span_.start(); }
      Time after() const { return span_.end(); }
      
      /** connection to the render nodes network */
      NodeGraphAttachment exitNode;
      
      
      /**
       * Access the JobTicket for this segment and the given \a portNr
       * @remark will be created on-demand and remain stable thereafter.
       */
      engine::JobTicket&
      jobTicket (size_t portNr)  const
        {
          if (portNr >= portTable_.size())
            unConst(this)->generateTickets_onDemand (portNr);
          ASSERT (portNr < portTable_.size());
          return portTable_[portNr];
        }
      
      bool
      empty() const
        {
          return exitNode.empty();
        }
      
      
    private:
      void
      generateTickets_onDemand (size_t portNr)
        {
          for (size_t i = portTable_.size(); i <= portNr; ++i)
            if (isnil (exitNode[portNr]))                      // ‣ disable this slot
                portTable_.emplace_back (engine::JobTicket::NOP);
            else
              {// Ticket was not generated yet...
                JobTicket& newTicket = ticketAlloc_(exitNode[portNr], ticketAlloc_);
                portTable_.emplace_back (newTicket);      // ref to new ticket ‣ slot
              }
        }
    };
  
  
  
}} // namespace steam::fixture
#endif /*STEAM_FIXTURE_SEGMENT_H*/
