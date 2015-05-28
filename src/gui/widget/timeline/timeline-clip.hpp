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

#ifndef WIDGETS_TIMELINE_CLIP_H
#define WIDGETS_TIMELINE_CLIP_H

#include "gui/gtk-lumiera.hpp"
#include "gui/model/clip.hpp"
#include "include/logging.h"
#include "lib/time/timevalue.hpp"

#include "gui/widgets/timeline/draw-strategy.hpp"
#include "gui/widgets/timeline/timeline-entity.hpp"

#include <string>


namespace gui {
namespace widgets {
namespace timeline {
  
  using std::string;
  using lib::time::Time;

  
  class Clip : public Entity
    {
    public:
      Clip (shared_ptr<model::Clip> clip,
            shared_ptr<timeline::DrawStrategy> drawStrategy);
      
      
      Time getBegin()  const;
      Time getEnd()    const;
      string getName() const;
      
      /** Sets the selected status of the clip. */
      void setSelected (bool);
  
    private:
  
      shared_ptr<model::Clip> modelClip;
  
      /**
       * True when this clip is selected in the GUI.
       */
      bool selected;
    };
  
  
}}}    // namespace gui::widgets::timeline
#endif // TIMELINE_CLIP_HPP
