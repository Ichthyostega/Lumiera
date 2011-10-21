/*
  timeline-entity.cpp  -  Implementation of the timeline entity object

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


#include "gui/gtk-lumiera.hpp"
#include "gui/widgets/timeline/timeline-entity.hpp"
#include "gui/widgets/timeline/draw-strategy.hpp"

namespace gui {
namespace widgets {
namespace timeline {

  Entity::Entity(std::tr1::shared_ptr<timeline::DrawStrategy> drawStrategy)
    : enabled(true),
      drawStrategy(drawStrategy)
  {  }

  Entity::~Entity()
  {  }
  
  void
  Entity::draw(Cairo::RefPtr<Cairo::Context> cr,
    TimelineViewWindow* const window) const
  {
    REQUIRE (cr);
    REQUIRE (window);

    drawStrategy->draw(*this, cr, window);
  }

  bool
  Entity::getEnabled () const
  {
    return enabled;
  }

  void
  Entity::setEnabled (bool enabled)
  {
    this->enabled = enabled;
  }
  
}   // namespace timeline
}   // namespace widgets
}   // namespace gui

