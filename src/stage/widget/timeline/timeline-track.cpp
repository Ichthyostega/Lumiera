/*
  TimelineTrack  -  representation of a group track in the timeline display

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

* *****************************************************/


/** @file timeline-track.cpp
 ** A group track in the first draft version of the timeline display.
 ** @todo as of 2016 this is about to be replaced by a new implementation approach
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */


#include "stage/widget/timeline/timeline-track.hpp"
#include "stage/widget/timeline-widget.hpp"
#include "stage/workspace/ui-style.hpp"
#include "stage/dialog/name-chooser.hpp"
#include "include/logging.h"

using namespace Gtk;          ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace sigc;         ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

using std::shared_ptr;

namespace gui {
namespace widget {
namespace timeline {
  
  const float Track::ExpandAnimationPeriod = 0.15;
  
  
  
  Track::Track (TimelineWidget& owner,
                shared_ptr<model::Track> track)
    : timelineWidget(owner)
    , modelTrack(track)
    , expanded(true)
    , expandDirection(None)
    , headerWidget(*this)
    , enableButton(Gtk::StockID("track_enabled"), workspace::UIStyle::MenuIconSize)
    , lockButton(Gtk::StockID("track_unlocked"), workspace::UIStyle::MenuIconSize)
    {
      REQUIRE(modelTrack);
      
      titleMenuButton.set_relief(RELIEF_HALF);
      //titleMenuButton.unset_flags(CAN_FOCUS);
      
      buttonBar.append(enableButton, mem_fun(this, &Track::onToggleEnabled));
      buttonBar.append(lockButton, mem_fun(this, &Track::onToggleLocked));
      
      headerWidget.set_child_widget(headerBox);
      
      headerBox.pack_start(titleMenuButton, PACK_SHRINK);
      headerBox.pack_start(buttonBar, PACK_SHRINK);
      
      headerWidget.show_all();
#if false                                                   ///////////////////////TODO unfinished code
      // Setup the title menu button
      Menu::MenuList& title_list = titleMenuButton.get_menu().items();
      title_list.push_back( Menu_Helpers::MenuElem(_("_Name..."),
        mem_fun(this, &Track::on_set_name) ) );
      title_list.push_back( Menu_Helpers::SeparatorElem() );
      title_list.push_back( Menu_Helpers::MenuElem(_("_Remove"),
        mem_fun(this, &Track::on_remove_track) ) );
#endif
      updateEnableButton();
      
      updateLockButton();
      
      updateName();
#if false                                                   ///////////////////////TODO unfinished code
      // Setup the context menu
      Menu::MenuList& context_list = contextMenu.items();
      //context_list.push_back( Menu_Helpers::MenuElem(_("_Add Track"),
      //  mem_fun(timelineWidget, &TimelineWidget::on_add_track_command) ) );
      context_list.push_back( Menu_Helpers::MenuElem(_("_Remove Track"),
        mem_fun(this, &Track::on_remove_track) ) );
#endif
      // Connect to the model
      modelTrack->signalEnabledChanged().connect(sigc::mem_fun(this,
        &Track::onEnabledChanged));
      modelTrack->signalLockedChanged().connect(sigc::mem_fun(this,
        &Track::onLockedChanged));
      modelTrack->signalNameChanged().connect(sigc::mem_fun(this,
        &Track::onNameChanged));
    }
  
  
  Track::~Track()
  {
    headerWidget.remove(headerBox);
  }
  
  
  Gtk::Widget&
  Track::get_header_widget()
  {
    return headerWidget;
  }
  
  
  shared_ptr<model::Track>
  Track::getModelTrack()  const
  {
    return modelTrack;
  }
  
  
  int
  Track::get_height()  const
  {
    return 100;
  }
  
  
  bool
  Track::get_expanded()  const
  {
    return expanded;
  }
  
  
  shared_ptr<timeline::Clip>
  Track::getClipAt (Time)  const
  {
    // Default implementation returns empty pointer
    return shared_ptr<timeline::Clip>();
  }
  
  
  void
  Track::expand_collapse (ExpandDirection direction)
  {
    REQUIRE(direction != None);
    
    expandDirection = direction;
    if(direction == Expand)
      {
        expanded = true;
        expandAnimationState = 0.0;
      }
    else
      {
        expanded = false;
        expandAnimationState = 1.0;
      }
    
    // Create a timer if we don't already have one
    if(!expand_timer)
      {
        expand_timer.reset(new Glib::Timer());
        expand_timer->start();
      }
    else  // Reset the timer if we do
      expand_timer->reset();
  }
  
  
  float
  Track::get_expand_animation_state()  const
  {
    ENSURE(expandAnimationState >= 0.0 &&
      expandAnimationState <= 1.0);
    return expandAnimationState;
  }
  
  
  bool
  Track::is_expand_animating()  const
  {
    return expandDirection != None;
  }
  
  
  void
  Track::tick_expand_animation()
  {
    REQUIRE(expandDirection != None); // tick_expand_animation should not
                                      // be unless is_expand_animating
                                      // returns true
    REQUIRE(expand_timer);
    const float delta = 
      (float)expand_timer->elapsed() / ExpandAnimationPeriod;
    expand_timer->reset(); // reset the timer to t=0
    
    if(expandDirection == Expand)
      {
        expandAnimationState += delta;
        if(expandAnimationState >= 1.0)
          expandDirection = None;
      }
    else
      {
        expandAnimationState -= delta;
        if(expandAnimationState <= 0.0)
          expandDirection = None;
      }
      
    if(expandDirection == None)
      expand_timer.reset();  // We've finished with the timer - delete it
  }
  
  
  Gtk::ExpanderStyle
  Track::get_expander_style()  const
  {
    if (expanded)
      {
        if(expandDirection == None)
          return EXPANDER_EXPANDED;
        else if(expandAnimationState >= 2.0 / 3.0)
          return EXPANDER_SEMI_EXPANDED;
        else if(expandAnimationState >= 1.0 / 3.0)
          return EXPANDER_SEMI_COLLAPSED;
        else
          return EXPANDER_COLLAPSED;
      }
    else
      {
        if(expandDirection == None)
          return EXPANDER_COLLAPSED;
        else if(expandAnimationState >= 2.0 / 3.0)
          return EXPANDER_EXPANDED;
        else if(expandAnimationState >= 1.0 / 3.0)
          return EXPANDER_SEMI_EXPANDED;
        else
          return EXPANDER_SEMI_COLLAPSED;
      }
    
    NOBUG_ERROR(gui, "Track::get_expander_style() final return reached");
    
    return EXPANDER_COLLAPSED;   // This should never happen
  }
  
  
  void
  Track::show_header_context_menu (guint button, guint32 time)
  {
    contextMenu.popup(button, time);
  }
  
  
  void
  Track::onEnabledChanged (bool)
  {
    updateEnableButton();
  }
  
  
  void
  Track::onLockedChanged (bool)
  {
    updateLockButton();
  }
  
  
  void
  Track::on_set_name()
  {
    REQUIRE(modelTrack);
    
    Gtk::Window *window = dynamic_cast<Window*>(timelineWidget.get_toplevel());
    REQUIRE(window != NULL); 
    
    dialog::NameChooser dialog(*window, _("Set Track Name"), modelTrack->get_name());
    
    if (RESPONSE_OK == dialog.run())
      modelTrack->set_name(dialog.getName());
  }
  
  
  void
  Track::onNameChanged (std::string)
  {
    updateName();
  }
  
  
  void
  Track::on_remove_track()
  {
    REQUIRE(modelTrack);
    shared_ptr<TimelineState> state = timelineWidget.get_state();
    REQUIRE(state);
    
    state->getSequence()->remove_descendant_track(modelTrack);
  }
  
  
  void
  Track::onToggleEnabled()
  {
    bool status = modelTrack->getEnabled();
    modelTrack->setEnabled(!status);
  }
  
  
  void
  Track::onToggleLocked()
  {
    bool status = modelTrack->getLocked();
    modelTrack->setLocked(!status);
  }
  
  
  void
  Track::updateEnableButton()
  {
    REQUIRE (modelTrack);
    
    if (modelTrack->getEnabled())
      {
        enableButton.setStockID (Gtk::StockID("track_enabled"), workspace::UIStyle::MenuIconSize);
        enableButton.set_tooltip_text(_("Disable track"));
      }
    else
      {
        enableButton.setStockID (Gtk::StockID("track_disabled"), workspace::UIStyle::MenuIconSize);
        enableButton.set_tooltip_text(_("Enable track"));
      }
  }
  
  
  void
  Track::updateLockButton()
  {
    REQUIRE (modelTrack);
    
    if (modelTrack->getLocked())
      {
        lockButton.setStockID (Gtk::StockID("track_locked"), workspace::UIStyle::MenuIconSize);
        lockButton.set_tooltip_text(_("Unlock track"));
      }
    else
      {
        lockButton.setStockID (Gtk::StockID("track_unlocked"), workspace::UIStyle::MenuIconSize);
        lockButton.set_tooltip_text(_("Lock track"));
      }
  }
  
  
  void
  Track::updateName()
  {
    REQUIRE(modelTrack);
    titleMenuButton.set_label(modelTrack->get_name());
  }
  
  
}}}// namespace gui::widget::timeline
