/*
  timeline-track.cpp  -  Implementation of the timeline track object
 
  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
* *****************************************************/

// !!! This header must soon be removed when we drop Etch compatibility
#include <gtk/gtktoolbar.h>

#include "timeline-track.hpp"
#include "../timeline-widget.hpp"
#include "../../window-manager.hpp"
#include "../../dialogs/name-chooser.hpp"

using namespace boost;
using namespace Gtk;
using namespace sigc;

namespace gui {
namespace widgets {
namespace timeline {
  
const int Track::NoAnimationState = -1;
const int Track::MaxExpandAnimation = 10;
  
Track::Track(TimelineWidget &timeline_widget,
  shared_ptr<model::Track> track) :
  timelineWidget(timeline_widget),
  model_track(track),
  expanded(true),
  expandAnimationState(Track::NoAnimationState),
  enableButton(Gtk::StockID("track_enabled")),
  lockButton(Gtk::StockID("track_unlocked"))
{
  REQUIRE(model_track);
  
  titleMenuButton.set_relief(RELIEF_HALF);
  
  buttonBar.append(enableButton);
  buttonBar.append(lockButton);
  
  buttonBar.set_toolbar_style(TOOLBAR_ICONS);
  
#if 0
  buttonBar.set_icon_size(WindowManager::MenuIconSize);
#else
  TODO("This code soon be removed when we drop Etch compatibility");
  
  // Temporary bodge for etch compatibility - will be removed soon
  gtk_toolbar_set_icon_size (buttonBar.gobj(),
    (GtkIconSize)(int)WindowManager::MenuIconSize);
#endif

  headerWidget.pack_start(titleMenuButton, PACK_SHRINK);
  headerWidget.pack_start(buttonBar, PACK_SHRINK);
  
  // Setup the title menu button
  Menu::MenuList& title_list = titleMenuButton.get_menu().items();
  title_list.push_back( Menu_Helpers::MenuElem(_("_Name..."),
    mem_fun(this, &Track::on_set_name) ) );
  title_list.push_back( Menu_Helpers::SeparatorElem() );
  title_list.push_back( Menu_Helpers::MenuElem(_("_Remove"),
    mem_fun(this, &Track::on_remove_track) ) );
    
  update_name();
  
  // Setup the context menu
  Menu::MenuList& context_list = contextMenu.items();
  context_list.push_back( Menu_Helpers::MenuElem(_("_Add Track"),
    mem_fun(timelineWidget, &TimelineWidget::on_add_track_command) ) );
  context_list.push_back( Menu_Helpers::MenuElem(_("_Remove Track"),
    mem_fun(this, &Track::on_remove_track) ) );
}

Gtk::Widget&
Track::get_header_widget()
{
  return headerWidget;
}

shared_ptr<model::Track>
Track::get_model_track() const
{
  return model_track;
}

int
Track::get_height() const
{
  return 100;
}

bool
Track::get_expanded() const
{
  return expanded;
}

void
Track::expand_collapse(ExpandDirection direction)
{
  expandDirection = direction;
  if(direction == Expand)
    {
      expanded = true;
      expandAnimationState = 0;
    }
  else
    {
      expanded = false;
      expandAnimationState = MaxExpandAnimation;
    }
}

int
Track::get_expand_animation_state() const
{
  ENSURE((expandAnimationState >= 0 &&
    expandAnimationState <= MaxExpandAnimation) ||
    expandAnimationState == NoAnimationState);
  return expandAnimationState;
}
  
void
Track::tick_expand_animation()
{ 
  if(expandAnimationState <= NoAnimationState)
  {
    WARN(gui, "tick_expand_animation() was called when"
      " expandAnimationState was set to NoAnimationState");
    return;    
  }

  if(expandDirection == Expand)
    {
      expandAnimationState++;
      if(expandAnimationState >= MaxExpandAnimation)
        expandAnimationState = NoAnimationState;
    }
  else
    {
      expandAnimationState--;
      if(expandAnimationState <= 0)
        expandAnimationState = NoAnimationState;
    }  
  
  ENSURE((expandAnimationState >= 0 &&
    expandAnimationState <= MaxExpandAnimation) ||
    expandAnimationState == NoAnimationState);
}

void
Track::show_header_context_menu(guint button, guint32 time)
{
  contextMenu.popup(button, time);
}

void
Track::update_name()
{
  REQUIRE(model_track);
  titleMenuButton.set_label(model_track->get_name());
}

void
Track::on_set_name()
{
  REQUIRE(model_track);
  
  Gtk::Window *window = dynamic_cast<Window*>(
    timelineWidget.get_toplevel());
  REQUIRE(window != NULL); 
    
  dialogs::NameChooser dialog(*window,
    _("Set Track Name"), model_track->get_name());
    
  if(dialog.run() == RESPONSE_OK)
    {
      model_track->set_name(dialog.get_name());
      update_name();
    }
}

void
Track::on_remove_track()
{
  REQUIRE(model_track);
  REQUIRE(timelineWidget.sequence);
  
  timelineWidget.sequence->remove_child_track(model_track);
}

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

