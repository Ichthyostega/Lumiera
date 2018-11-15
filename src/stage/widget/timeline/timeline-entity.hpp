/*
  TIMELINE-ENTITY.hpp  -  Declaration of the timeline entity class

  Copyright (C)         Lumiera.org
    2010,               Stefan Kangas <skangas@skangas.se>

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


/** @file timeline-entity.hpp
 ** Any UI element that can be placed on the timeline.
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */

#ifndef GUI_WIDGET_TIMELINE_ENTITY_H
#define GUI_WIDGET_TIMELINE_ENTITY_H

#include "gui/gtk-base.hpp"
#include "lib/time/timevalue.hpp"

#include <string>
#include <memory>
#include <cairomm/cairomm.h>


namespace gui {
namespace widget {
namespace timeline {
  
  using lib::time::Time;
  using std::shared_ptr;
  
  
  class DrawStrategy;
  class TimelineViewWindow;
  
  /**
   * Base class for timeline entities.
   * Everything that can be placed on the timeline is a timeline Entity.
   */
  class Entity
    {
      bool enabled_;
      shared_ptr<timeline::DrawStrategy> drawStrategy_;
      
    protected:
      
      Entity (shared_ptr<timeline::DrawStrategy> drawStrategy);
      
      virtual ~Entity();
      
    public:
      
      
      virtual void
      draw(Cairo::RefPtr<Cairo::Context> cairo,
        TimelineViewWindow* const window) const;
      
      bool
      getEnabled () const;
      
      virtual Time
      getBegin () const = 0;
      
      virtual Time
      getEnd () const = 0;
      
      virtual std::string
      getName () const = 0;
      
      void
      setEnabled(bool selected);
      
    };
  
  
}}}// namespace gui::widget::timeline
#endif /*GUI_WIDGET_TIMELINE_ENTITY_H*/
