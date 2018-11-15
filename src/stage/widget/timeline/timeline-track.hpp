/*
  TIMELINE-TRACK.hpp  -  representation of a group track in the timeline display

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


/** @file timeline-track.hpp
 ** A group track in the first draft version of the timeline display.
 ** @todo as of 2016 this is about to be replaced by a new implementation approach
 ** @warning as of 2016 the entire timeline display is planned to be reworked
 */



#ifndef GUI_WIDGET_TIMELINE_TRACK_H
#define GUI_WIDGET_TIMELINE_TRACK_H


#include "stage/gtk-base.hpp"
#include "stage/model/track.hpp"
#include "stage/widget/menu-button.hpp"
#include "stage/widget/mini-button.hpp"
#include "stage/widget/button-bar.hpp"
#include "stage/widget/timeline/timeline-clip.hpp"
#include "stage/widget/timeline/timeline-header-container.hpp"
#include "stage/widget/timeline/timeline-header-widget.hpp"
#include "lib/time/timevalue.hpp"

#include <memory>


namespace gui {
namespace widget { 
namespace timeline {
  
  using lib::time::Time;
  
  class TimelineViewWindow;
  
  
  
  /**
   * Timeline tracks are created by the timeline widget to correspond to
   * model tracks. Timeline tracks are used to store UI specific state
   * data.
   */
  class Track
    : public sigc::trackable
    {
    public:
      /**
       * An enum used by the branch expand/collapse animation.
       * ExpandDirection represents whether the branch us being expanded or
       * collapsed, or neither.
       */
      enum ExpandDirection {
        None,
        Expand,
        Collapse
      };
      
      
      Track(TimelineWidget& owner,
            shared_ptr<model::Track> track);
      
      
      virtual ~Track();   ///< this is an abstract base class
      
      
      Gtk::Widget& get_header_widget();
      
      
      shared_ptr<model::Track>
      getModelTrack() const;
      
      
      /**
       * Return the visual height of the track in pixels.
       * @return The visual height of the track in pixels.
       */
      int get_height()  const;
      
      /**
       * Gets whether the descendant tracks are expanded or collapsed.
       * @return Returns true if the branch is expanded, false if it's
       * collapsed.
       * @see expand_collapse
       */
      bool get_expanded()  const;
      
      /**
       * Expands or collapses this branch.
       * @param direction Specifies whether this branch should be expanded
       * or collapse. direction must not equal None
       */
      void expand_collapse (ExpandDirection direction);
      
      /**
       * The current expand state.
       * @return Returns the expand state value, this value is a number
       * between 0 and 1.0, and is recalculated by tick_expand_animation().
       * @remarks A value of 1.0 is given when the branch is fully expanded
       * (and animating), 0.0 is given when the branch is fully collapsed
       * (and animating). When the branch is not animating this value has
       * an indeterminate value.
       * @see tick_expand_animation
       */
      float get_expand_animation_state()  const;
      
      /**
       * Gets whether the branch is animation.
       * @return Returns true if the branch is animating, false if not.
       */
      bool is_expand_animating()  const;
      
      /**
       * When this track is being animated, tick_expand_animation must be
       * called repeatedly to cause the animation to progress.
       */
      void tick_expand_animation();
      
      /**
       * Calculates the expander style, given the animation state.
       */
      Gtk::ExpanderStyle get_expander_style()  const;
      
      /**
       *
       */
      void show_header_context_menu (guint button, guint32 time);
      
      /**
       * Draw the track
       */
      virtual void draw_track(Cairo::RefPtr<Cairo::Context> cairo,
                              TimelineViewWindow* const window)    const = 0;
      
      /**
       * Gets the clip that is occupying the given time.
       * The default implementation simply returns an empty pointer.
       * @param the given time
       */
      virtual shared_ptr<timeline::Clip>
      getClipAt (Time position) const;
      
    private:
      
      /**
       * Specifies the period of the expand animation in seconds.
       */
      static const float ExpandAnimationPeriod;
      
      /**
       * Event handler for when the enabled status changes.
       */
      void onEnabledChanged(bool);
      
      /**
       * Event handler for when the track name changes.
       */
      void onNameChanged(std::string);
      
      /**
       * Event handler for when the user requested to remove the track.
       */
      void on_remove_track();
      
      /**
       * Event handler for when the locked status changes.
       */
      void onLockedChanged(bool);
      
      /**
       * Event handler for when the user requested a name change.
       */
      void on_set_name();
      
      /**
       * Event handler for when the user pressed the Enable button.
       */
      void onToggleEnabled();
      
      /**
       * Event handler for when the user pressed the Lock button.
       */
      void onToggleLocked();
      
      void updateEnableButton();
      
      void updateLockButton();
      
      void updateName();
      
    protected:
      
      TimelineWidget &timelineWidget;
      shared_ptr<model::Track> modelTrack;
      
    private:
      /**
       * This bool is true if this branch is expanded. false if it is
       * collapsed.
       */
      bool expanded;
      
      /**
       * This enum specifies which direction the expand/collapse animation
       * is moving - if any.
       * @remarks If no animation is occuring, expandDirection is set to
       * None.
       */
      ExpandDirection expandDirection;
      
      /**
       * The current expand state.
       * @remarks This value is a number between 0 and 1.0,
       * and is recalculated by tick_expand_animation(). This variable is
       * set to 1.0 when the branch is fully expanded (and animating) and
       * 0.0 when the branch is fully collapsed (and animating). When the
       * branch is not animating this value has an indeterminate value.
       * @see tick_expand_animation
       */
      double expandAnimationState;
      
      /**
       * An internal timer used for the expand/collapse animation.
       */
      std::unique_ptr<Glib::Timer> expand_timer;
      
      //----- Header Widgets ------//
      
      timeline::TimelineHeaderWidget headerWidget;
      
      Gtk::VBox headerBox;
      
      MenuButton titleMenuButton;
      
      MiniButton enableButton;
      MiniButton lockButton;
      
      Gtk::Entry titleBox;
      ButtonBar buttonBar;
      
      Gtk::Menu contextMenu;
      
      friend class TimelineHeaderWidget;
    };
  
  
}}}// namespace gui::widget::timeline
#endif /*GUI_WIDGET_TIMELINE_TRACK_H*/
