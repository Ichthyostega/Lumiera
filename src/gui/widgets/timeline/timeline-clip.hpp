/*
  timeline-clip.hpp  -  Declaration of the timeline clip object

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>

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
/** @file widgets/timeline/timeline-clip.hpp
 ** This file contains the definition of timeline clip object
 */

#include <cairomm/pattern.h>

#include "gui/gtk-lumiera.hpp"
#include "gui/model/clip.hpp"
#include "timeline-view-window.hpp"
#include "include/logging.h"

#ifndef TIMELINE_CLIP_HPP
#define TIMELINE_CLIP_HPP

namespace gui {
namespace widgets {
namespace timeline {

  class Clip : public model::Clip
  {
  public:
    Clip(boost::shared_ptr<model::Clip> clip);

    void draw_clip(Cairo::RefPtr<Cairo::Context> cairo,
                   TimelineViewWindow* const window) const;

    /**
     * Sets the selected status of the clip.
     **/
    void
    setSelected (bool state);

  private:

    boost::shared_ptr<model::Clip> modelClip;

    /**
     * True when this clip is selected in the GUI.
     */
    bool selected;
  };

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

#endif // TIMELINE_CLIP_HPP
