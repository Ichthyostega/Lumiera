/*
  timeline-entity.hpp  -  Declaration of the timeline entity class

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

* *****************************************************/
/** @file timeline-entity.hpp
 ** Declares the Timeline Entity class.
 */

#ifndef TIMELINE_ENTITY_HPP
#define TIMELINE_ENTITY_HPP

#include <string>

// TODO: Remove once we get better measure of duration.
extern "C" {
#include <stdint.h>
#include <gavl/gavltime.h>
}

#include <boost/shared_ptr.hpp>
#include <cairomm/cairomm.h>

namespace gui {
namespace widgets {
namespace timeline {

  class DrawStrategy;
  class TimelineViewWindow;

  /**
   * Base class for timeline entities.
   * Everything that can be placed on the timeline is a timeline Entity.
   */
  class Entity {
  protected:

    Entity(boost::shared_ptr<timeline::DrawStrategy> drawStrategy);

    virtual ~Entity();

  public:

    virtual gavl_time_t
    getBegin () const = 0;
    
    virtual void
    draw(Cairo::RefPtr<Cairo::Context> cairo,
      TimelineViewWindow* const window) const;

    bool
    getEnabled () const;

    virtual gavl_time_t
    getEnd () const = 0;

    virtual std::string
    getName () const = 0;

    void
    setEnabled(bool selected);

  private:

    bool enabled;

    boost::shared_ptr<timeline::DrawStrategy> drawStrategy;
  };

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

#endif // TIMELINE_ENTITY_HPP
