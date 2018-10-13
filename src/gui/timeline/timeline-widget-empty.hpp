/*
  TIMELINE-WIDGET-EMPTY.hpp  -  placeholder widget for a missing timeline

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file timeline-widget-empty.hpp
 ** Empty placeholder to be rendered when the UI starts without session.
 ** The TimelinePannel represents the [model root](\ref mobject::session::Root) and thus
 ** has the responsibility to manage the individual TimelineWidget instances. In case the
 ** UI is started with no usable session loaded in Proc-Layer, this empty placeholder widget
 ** can be inserted.
 ** 
 ** @todo as of 12/2016 a complete rework of the timeline display is underway
 ** 
 */


#ifndef GUI_TIMELINE_TIMELINE_WIDGET_EMPTY_H
#define GUI_TIMELINE_TIMELINE_WIDGET_EMPTY_H

#include "gui/gtk-base.hpp"

#include "gui/timeline/timeline-widget.hpp"
#include "lib/time/timevalue.hpp"

//#include <memory>
//#include <vector>



namespace gui  {
namespace timeline {
  
  
  /**
   * Core timeline display (custom widget).
   * @todo WIP-WIP-rewrite as of 12/2016
   * @remarks At top level, this widget is split into a header pane (left)
   *     and a scrollable timeline body (right). The layout of both parts is aligned.
   */
  class TimelineWidgetEmpty
    : public TimelinePage
    {
    public:
      /**
       * @param source_state state to be used used as the
       *    data source (model) for this timeline widget.
       */
      TimelineWidgetEmpty();
     ~TimelineWidgetEmpty();  
      
      
      
    public: /* ===== Control interface ===== */
      
    public: /* ===== Signals ===== */
      
    private:/* ===== Events ===== */
      
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace gui::timeline
#endif /*GUI_TIMELINE_TIMELINE_WIDGET_EMPTY_H*/
