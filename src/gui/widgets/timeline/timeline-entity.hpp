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

namespace gui {
namespace widgets {
namespace timeline {

  /**
   * Base class for timeline entities.
   * Everything that can be placed on the timeline is a timeline Entity.
   */
  class Entity {
  protected:

    /**
     * Constructor
     */
    Entity();

  public:

    /**
     *
     */
    bool
    getEnabled();

    /**
     *
     */
    void
    setEnabled(bool selected);

  private:

    /**
     * True when this entity is enabled.
     */
    bool enabled;
  };

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

#endif // TIMELINE_ENTITY_HPP
