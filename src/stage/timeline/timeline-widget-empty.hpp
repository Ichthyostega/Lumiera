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
 ** A TimelinePannel is able to expose several timelines in a tabbed notebook.
 ** However, the actual [model root](\ref mobject::session::Root) is represented by the
 ** InteractionDirector, which receives the timeline contents pushed up from the session
 ** as diff messages and injects a TimelineWidget for each of them. But when not timeline
 ** is present and thus no actual TimelineWidget was yet instantiated, the TimelinePanel
 ** will install this TimelineWidgetEmpty placeholder instead to fill the void.
 ** 
 */


#ifndef STAGE_TIMELINE_TIMELINE_WIDGET_EMPTY_H
#define STAGE_TIMELINE_TIMELINE_WIDGET_EMPTY_H

#include "stage/gtk-base.hpp"
#include "stage/timeline/timeline-widget.hpp"


namespace gui  {
namespace timeline {
  
  
  /**
   * Placeholder to show when no actual Timeline is present.
   * Typically this happens when the application starts up and no session has been loaded (yet).
   * @todo This widget could offer a list of last sessions to pick from etc...
   * @remarks We just display some quote to fill the void...
   */
  class TimelineWidgetEmpty
    : public TimelinePage
    {
      
      Gtk::Frame frame_;
      Gtk::Box   content_;
      Gtk::Label messageText_;
      Gtk::Label quoteText_;
      
      Gtk::Image image_;
      
      cuString
      getLabel()  const override
        {
          return _("[virgin]");
        }
      
    public:
      TimelineWidgetEmpty()
        : TimelinePage{}
        , content_{Gtk::ORIENTATION_VERTICAL}
        , image_{Gtk::StockID{"track_enabled"}, Gtk::ICON_SIZE_LARGE_TOOLBAR}
        {
          messageText_.set_markup("To be or not to be that is the question...");
          messageText_.set_max_width_chars(80);
          messageText_.set_line_wrap();
          messageText_.set_halign(Gtk::ALIGN_FILL);
          messageText_.set_valign(Gtk::ALIGN_FILL);
          quoteText_.set_markup ("<i>-- the great actor Joseph Tura</i>");
          quoteText_.set_halign(Gtk::ALIGN_END);
          quoteText_.set_valign(Gtk::ALIGN_START);
          content_.pack_start (messageText_, Gtk::PACK_SHRINK);
          content_.pack_start (quoteText_, Gtk::PACK_EXPAND_PADDING);
          frame_.add(content_);
          frame_.set_border_width(5);
          frame_.set_halign(Gtk::ALIGN_END);
          frame_.set_valign(Gtk::ALIGN_START);
          
          Gtk::Paned::add1 (image_);
          Gtk::Paned::add2 (frame_);
        }
    };
  
  
}}// namespace gui::timeline
#endif /*STAGE_TIMELINE_TIMELINE_WIDGET_EMPTY_H*/
