/*
  DRAW-STRATEGY.hpp  -  Definition the timeline draw strategy interface

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
 ** Strategy interface for drawing timeline entities. This is part of the
 ** first implementation draft of the timeline display.
 ** @deprecated very likely to be completely rewritten.
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */



#ifndef GUI_WIDGET_TIMELINE_DRAW_STRATEGY_HPP
#define GUI_WIDGET_TIMELINE_DRAW_STRATEGY_HPP

#include "gui/gtk-base.hpp"
#include "gui/widget/timeline/timeline-entity.hpp"
#include "gui/widget/timeline/timeline-view-window.hpp"

namespace gui {
namespace widget {
namespace timeline {
  
  /////////TODO some questions:
  /////////     1) who is allowed to destroy DrawStrategy objects
  /////////     2) shouldn't DrawStragegy be util::NonCopyable?
  
  
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
  
  
}}}// namespace gui::widget::timeline
#endif /*GUI_WIDGET_TIMELINE_DRAW_STRATEGY_HPP*/
