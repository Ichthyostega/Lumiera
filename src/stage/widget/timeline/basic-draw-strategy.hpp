/*
  basic-draw-strategy.hpp  -  Declaration of a basic draw strategy

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


/** @file basic-draw-strategy.hpp
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */



#ifndef STAGE_WIDGET_TIMELINE_BASIC_DRAW_STRATEGY_H
#define STAGE_WIDGET_TIMELINE_BASIC_DRAW_STRATEGY_H

#include "stage/widget/timeline/draw-strategy.hpp"


namespace stage {
namespace widget {
namespace timeline {
  
  class BasicDrawStrategy
    : public DrawStrategy
    {
    public:
      
      BasicDrawStrategy() { };
      
      void
      draw (Entity const& entity
           ,Cairo::RefPtr<Cairo::Context> cr
           ,TimelineViewWindow* const window)  const;
    };
  
}}}// namespace stage::widget::timeline
#endif /*STAGE_WIDGET_TIMELINE_BASIC_DRAW_STRATEGY_H*/
