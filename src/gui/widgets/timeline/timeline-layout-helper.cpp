/*
  timeline-layout-helper.cpp  -  Implementation of the timeline
  layout helper class
 
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

#include <boost/foreach.hpp>

#include "timeline-layout-helper.hpp"
#include "../timeline-widget.hpp"
#include "../../model/sequence.hpp"

using namespace Gtk;
using namespace std;
using namespace boost;
using namespace lumiera;
using namespace util;

namespace gui {
namespace widgets {
namespace timeline {
  
const int TimelineLayoutHelper::AnimationTimeout = 20; // 20ms
  
TimelineLayoutHelper::TimelineLayoutHelper(TimelineWidget &owner) :
  timelineWidget(owner),
  totalHeight(0),
  animation_state(Track::NoAnimationState)
{
}

void
TimelineLayoutHelper::clone_tree_from_sequence()
{
  const shared_ptr<model::Sequence> &sequence = timelineWidget.sequence;
  REQUIRE(sequence);
  
  layoutTree.clear();
  TrackTree::iterator_base iterator = layoutTree.set_head(sequence);
  add_branch(iterator, sequence);
}

TimelineLayoutHelper::TrackTree&
TimelineLayoutHelper::get_layout_tree()
{
  return layoutTree;
}

void
TimelineLayoutHelper::add_branch(
  TrackTree::iterator_base parent_iterator, 
  shared_ptr<model::Track> parent)
{
  BOOST_FOREACH(shared_ptr<model::Track> child,
    parent->get_child_tracks())
    {
      TrackTree::iterator_base child_iterator = 
          layoutTree.append_child(parent_iterator, child);
      add_branch(child_iterator, child);
    }
}

optional<Gdk::Rectangle>
TimelineLayoutHelper::get_track_header_rect(
  boost::weak_ptr<timeline::Track> track)
{
  if(contains(headerBoxes, track))
  {
    Gdk::Rectangle rect(headerBoxes[track]);
    rect.set_y(rect.get_y() - timelineWidget.get_y_scroll_offset());
    return optional<Gdk::Rectangle>(rect);
  }
  return optional<Gdk::Rectangle>();
}

shared_ptr<timeline::Track>
TimelineLayoutHelper::header_from_point(Gdk::Point point)
{
  // Apply the scroll offset
  point.set_y(point.get_y() + timelineWidget.get_y_scroll_offset());
  
  // Search the headers
  std::pair<weak_ptr<timeline::Track>, Gdk::Rectangle> pair; 
  BOOST_FOREACH( pair, headerBoxes )
    {
      // Hit test the rectangle
      const Gdk::Rectangle &rect = pair.second;
      
      if(point.get_x() >= rect.get_x() &&
        point.get_x() < rect.get_x() + rect.get_width() &&
        point.get_y() >= rect.get_y() &&
        point.get_y() < rect.get_y() + rect.get_height())
        return shared_ptr<timeline::Track>(pair.first);
    }
  
  // No track was found - return an empty pointer
  return shared_ptr<timeline::Track>();
}

boost::shared_ptr<timeline::Track>
TimelineLayoutHelper::track_from_y(int y)
{
  // Apply the scroll offset
  y += timelineWidget.get_y_scroll_offset();
  
  // Search the tracks
  std::pair<weak_ptr<timeline::Track>, Gdk::Rectangle> pair; 
  BOOST_FOREACH( pair, headerBoxes )
    {
      // Hit test the rectangle
      const Gdk::Rectangle &rect = pair.second;
      if(y >= rect.get_y() && y < rect.get_y() + rect.get_height())
        return shared_ptr<timeline::Track>(pair.first);
    }

  // No track was found - return an empty pointer
  return shared_ptr<timeline::Track>();
}

int
TimelineLayoutHelper::get_total_height() const
{
  ENSURE(totalHeight >= 0);
  return totalHeight;
}

void
TimelineLayoutHelper::update_layout()
{
  // Reset the animation state value, before it gets recalculated
  animation_state = Track::NoAnimationState;
    
  // Clear previously cached layout
  headerBoxes.clear();
  
  // Do the layout
  const int header_width = TimelineWidget::HeaderWidth;
  const int indent_width = TimelineWidget::HeaderIndentWidth;
  totalHeight = layout_headers_recursive(layoutTree.begin(),
    0, header_width, indent_width, 0, true);
  
  // Signal that the layout has changed
  timelineWidget.on_layout_changed();
  
  // Begin animating as necessary
  if(animation_state != Track::NoAnimationState && !animationTimer)
    begin_animation();
}

int
TimelineLayoutHelper::layout_headers_recursive(
  TrackTree::iterator_base parent_iterator,
  const int branch_offset, const int header_width,
  const int indent_width, const int depth, const bool parent_expanded)
{
  REQUIRE(depth >= 0);
  
  int child_offset = 0;
  
  TrackTree::sibling_iterator iterator;
  for(iterator = layoutTree.begin(parent_iterator);
    iterator != layoutTree.end(parent_iterator);
    iterator++)
    {
      const shared_ptr<model::Track> &model_track = *iterator;
      REQUIRE(model_track);
      
      shared_ptr<timeline::Track> timeline_track =
        lookup_timeline_track(model_track);
      
      // Is the track animating?
      const int track_animation_state =
        timeline_track->get_expand_animation_state();
      
      // Is the track going to be shown?
      if(parent_expanded)
        {          
          // Calculate and store the box of the header
          const int track_height = timeline_track->get_height();
          const int indent = depth * indent_width;
          
          headerBoxes[timeline_track] = Gdk::Rectangle(
            indent,                                           // x
            branch_offset + child_offset,                     // y
            max( header_width - indent, 0 ),                  // width
            track_height);                                    // height
          
          // Offset for the next header
          child_offset += track_height + TimelineWidget::TrackPadding;
        }
      
      // Recurse to children
      const bool expand_child =
        ((animation_state != Track::NoAnimationState) ||
          timeline_track->get_expanded())
          && parent_expanded;
           
      const int branch_height = layout_headers_recursive(
        iterator, branch_offset + child_offset,
        header_width, indent_width, depth + 1, expand_child);
      
      // Do collapse animation as necessary
      if(track_animation_state != Track::NoAnimationState)
      {
        // Tick the track expand animation
        timeline_track->tick_expand_animation();
                     
        // Calculate the height of te area which will be
        // shown as expanded
        const float a = ((float)track_animation_state /
          (float)Track::MaxExpandAnimation);
        child_offset += branch_height * a * a;
        const int y_limit = branch_offset + child_offset;

        // Obscure tracks according to the animation state
        TrackTree::pre_order_iterator descendant_iterator(iterator);
        descendant_iterator++;
        TrackTree::sibling_iterator end(iterator);
        end++;
        
        for(descendant_iterator = layoutTree.begin(parent_iterator);
          descendant_iterator != end;
          descendant_iterator++)
          {
            const Gdk::Rectangle &rect = headerBoxes[
              lookup_timeline_track(*descendant_iterator)];
            
            if(rect.get_y() + rect.get_height() > y_limit)
              headerBoxes.erase(track);
          }
        
        // Make sure the global animation state includes this branch's
        // animation state
        animation_state = max(
          animation_state, track_animation_state);
      }
      else // If no animation, just append the normal length
        child_offset += branch_height;
    }
    
  return child_offset;
}

shared_ptr<timeline::Track>
TimelineLayoutHelper::lookup_timeline_track(
  shared_ptr<model::Track> model_track)
{
  REQUIRE(model_track != NULL);
  shared_ptr<timeline::Track> timeline_track =
    timelineWidget.lookup_timeline_track(model_track);
  ENSURE(timeline_track);
  
  return timeline_track;
}

void
TimelineLayoutHelper::begin_animation()
{
  animationTimer = Glib::signal_timeout().connect(
    sigc::mem_fun(this, &TimelineLayoutHelper::on_animation_tick),
    AnimationTimeout);
}

bool
TimelineLayoutHelper::on_animation_tick()
{
  update_layout();
  return animation_state != Track::NoAnimationState;
}

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
