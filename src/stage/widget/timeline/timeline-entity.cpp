/*
  TimelineEntity  -  Implementation of the timeline entity object

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


/** @file timeline-entity.cpp
 ** Implementation of a generic element to be placed onto the timeline
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */


#include "stage/gtk-base.hpp"
#include "stage/widget/timeline/timeline-entity.hpp"
#include "stage/widget/timeline/draw-strategy.hpp"

namespace gui {
namespace widget {
namespace timeline {
  
  Entity::Entity (shared_ptr<timeline::DrawStrategy> drawStrategy)
    : enabled_(true)
    , drawStrategy_(drawStrategy)
    { }
  
  
  Entity::~Entity() {  }
  
  
  void
  Entity::draw (Cairo::RefPtr<Cairo::Context> cr,
                TimelineViewWindow* const window)  const
  {
    REQUIRE (cr);
    REQUIRE (window);
    
    drawStrategy_->draw(*this, cr, window);
  }
  
  
  bool
  Entity::getEnabled()  const
  {
    return enabled_;
  }
  
  void
  Entity::setEnabled (bool enabled)
  {
    this->enabled_ = enabled;
  }
  
  
}}}// namespace gui::widget::timeline
