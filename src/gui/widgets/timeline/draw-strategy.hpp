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

#ifndef TIMELINE_DRAW_STRATEGY_HPP
#define TIMELINE_DRAW_STRATEGY_HPP

#include "timeline-entity.hpp"
#include "timeline-view-window.hpp"

namespace gui {
namespace widgets {
namespace timeline {

  /**
   * An interface for drawing strategies for timeline entities.
   */
  class DrawStrategy
  {
  protected:

    /**
     * Constructor.
     */
    DrawStrategy()
    {  }

  public:

    /**
     * Draw the entity.
     * @param entity draw the 
     */
    virtual void draw(const Entity &entity,
      Cairo::RefPtr<Cairo::Context> cr,
      TimelineViewWindow* const window) const = 0;
  };

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

#endif // TIMELINE_DRAW_STRATEGY_HPP
