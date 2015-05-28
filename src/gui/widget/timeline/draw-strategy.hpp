/*
  draw-strategy.hpp  -  Definition the timeline draw strategy interface

  Copyright (C)         Lumiera.org
    2010,               Stefan Kangas <skangas@skangas.se

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

/** @file draw-strategy.hpp
 ** Declares the timeline entity drawing strategy interface.
 */

#ifndef WIDGETS_TIMELINE_DRAW_STRATEGY_HPP
#define WIDGETS_TIMELINE_DRAW_STRATEGY_HPP

#include "gui/gtk-base.hpp"
#include "gui/widgets/timeline/timeline-entity.hpp"
#include "gui/widgets/timeline/timeline-view-window.hpp"

namespace gui {
namespace widgets {
namespace timeline {
  
  /////////TODO some questions:
  /////////     1) who is allowed to destroy DrawStrategy objects
  /////////     2) shouldn't DrawStragegy be boost::noncopyable?
  
  
  /**
   * An interface for drawing strategies for timeline entities.
   */
  class DrawStrategy
    {
    protected:
  
      DrawStrategy() { }
  
      virtual ~DrawStrategy();
  
    public:
  
      virtual void
      draw (const Entity &entity,
            Cairo::RefPtr<Cairo::Context> cr,
            TimelineViewWindow* const window) const  = 0;
    };
  
  
}}}    // namespace gui::widgets::timeline
#endif // TIMELINE_DRAW_STRATEGY_HPP
