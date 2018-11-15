/*
  TIMELINE-CLIP.hpp  -  Declaration of the timeline clip object

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

/** @file timeline-clip.hpp
 ** Representation of a clip on the timeline
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */

#ifndef GUI_WIDGET_TIMELINE_CLIP_H
#define GUI_WIDGET_TIMELINE_CLIP_H

#include "gui/gtk-base.hpp"
#include "gui/model/clip.hpp"
#include "include/logging.h"
#include "lib/time/timevalue.hpp"

#include "gui/widget/timeline/draw-strategy.hpp"
#include "gui/widget/timeline/timeline-entity.hpp"

#include <string>


namespace gui {
namespace widget {
namespace timeline {
  
  using std::string;
  using lib::time::Time;
  
  
  class Clip
    : public Entity
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
      
      /** this clip is selected in the GUI */
      bool selected;
    };
  
  
}}}// namespace gui::widget::timeline
#endif /*GUI_WIDGET_TIMELINE_CLIP_H*/
