/*
  timeline-arrow-tool.hpp  -  Declaration of the ArrowTool class

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

/** @file timeline-arrow-tool.hpp
 ** This file contains the definition of the arrow tool class
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */

#ifndef GUI_WIDGET_TIMELINE_ARROW_TOOL_H
#define GUI_WIDGET_TIMELINE_ARROW_TOOL_H


#include "gui/gtk-base.hpp"
#include "gui/widget/timeline-widget.hpp"
#include "gui/widget/timeline/timeline-tool.hpp"
#include "gui/widget/timeline/timeline-body.hpp"
#include "gui/widget/timeline/timeline-track.hpp"

#include "lib/time/timevalue.hpp"

namespace gui {
namespace widget {
namespace timeline {

  /**
   * A helper class to implement the timeline arrow tool
   */
  class ArrowTool : public Tool
  {
  public:
    /**
     * Constructor
     * @param timelineBody The owner timeline body object
     */
    ArrowTool(TimelineBody &timelineBody);

    /**
     * Gets the type of tool represented by this class
     */
    ToolType get_type() const;
  
  protected:

    /**
     * Gets the cursor to display for this tool at this moment.
     */
    Glib::RefPtr<Gdk::Cursor> get_cursor() const;
  
    /**
     * The event handler for button press events.
     */
    void on_button_press_event(GdkEventButton* event);
  
    /**
     * The event handler for button release events.
     */
    void on_button_release_event(GdkEventButton* event);
  
    /**
     * The event handler for mouse move events.
     */
    void on_motion_notify_event(GdkEventMotion *event);

  private:

    shared_ptr<timeline::Track>
    getHoveringTrack ();

    bool selectionRectangleActive;
  };
  
  
}}}// namespace gui::widget::timeline
#endif /*GUI_WIDGET_TIMELINE_ARROW_TOOL_H*/
