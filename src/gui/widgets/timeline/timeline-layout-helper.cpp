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

TimelineLayoutHelper::TimelineLayoutHelper(TimelineWidget &owner) :
  timelineWidget(owner),
  totalHeight(0),
  animating(false)
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

void
TimelineLayoutHelper::begin_dragging_track(
  boost::shared_ptr<timeline::Track> track)
{
  REQUIRE(track);
  draggingTrack = track;
  g_message("begin_dragging_track");
  
  // Find the track in the tree
  const shared_ptr<model::Track> model_track = track->get_model_track();
  REQUIRE(model_track);
  for(draggingTrackIter = layoutTree.begin();
    draggingTrackIter != layoutTree.end();
    draggingTrackIter++)
    {
      if(*draggingTrackIter == model_track)
        break;
    }
}

void
TimelineLayoutHelper::end_dragging_track()
{
  draggingTrack.reset();
  clone_tree_from_sequence();
  update_layout();
}

boost::shared_ptr<timeline::Track>
TimelineLayoutHelper::get_dragging_track() const
{
  return draggingTrack;
}

void
TimelineLayoutHelper::drag_to_point(Gdk::Point point)
{
  // Apply the scroll offset
  point.set_y(point.get_y() + timelineWidget.get_y_scroll_offset());
  
  // Search the headers
  TrackTree::pre_order_iterator iterator;
  for(iterator = ++layoutTree.begin(); // ++ so we miss out the root sequence
    iterator != layoutTree.end();
    iterator++)
    {
      // Hit test the rectangle      
      const weak_ptr<timeline::Track> track = 
        lookup_timeline_track(*iterator);
      const Gdk::Rectangle &rect = headerBoxes[track];
      
      if(point.get_x() >= rect.get_x() &&
        point.get_x() < rect.get_x() + rect.get_width() &&
        point.get_y() >= rect.get_y() &&
        point.get_y() < rect.get_y() + rect.get_height())
        {
          // Relocate the header
          draggingTrackIter = layoutTree.move_after(
            iterator, draggingTrackIter);
          update_layout();
          return;
        }
    }
}

int
TimelineLayoutHelper::get_total_height() const
{
  ENSURE(totalHeight >= 0);
  return totalHeight;
}

bool
TimelineLayoutHelper::is_animating() const
{
  return animating;
}

void
TimelineLayoutHelper::update_layout()
{
  // Reset the animation state value, before it gets recalculated
  animating = false;
    
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
  if(animating && !animationTimer)
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
        
      // Is the track animating?
      const bool is_track_animating =
        timeline_track->is_expand_animating();
      animating |= is_track_animating;
      
      // Recurse to children
      const bool expand_child =
        (animating || timeline_track->get_expanded())
          && parent_expanded;
           
      int child_branch_height = layout_headers_recursive(
        iterator, branch_offset + child_offset,
        header_width, indent_width, depth + 1, expand_child);
      
      // Do collapse animation as necessary
      if(is_track_animating)
      {                     
        // Calculate the height of te area which will be
        // shown as expanded
        const float a = timeline_track->get_expand_animation_state();
        child_branch_height *= a * a;
        const int y_limit =
          branch_offset + child_offset + child_branch_height;
        
        // Obscure tracks according to the animation state
        TrackTree::pre_order_iterator descendant_iterator(iterator);
        descendant_iterator++;
        TrackTree::sibling_iterator end(iterator);
        end++;
        
        for(descendant_iterator = layoutTree.begin(parent_iterator);
          descendant_iterator != end;
          descendant_iterator++)
          {
            const weak_ptr<timeline::Track> track = 
              lookup_timeline_track(*descendant_iterator);
            const Gdk::Rectangle &rect = headerBoxes[track];
            
            if(rect.get_y() + rect.get_height() > y_limit)
              headerBoxes.erase(track);
          }
          
        // Tick the track expand animation
        timeline_track->tick_expand_animation();
      }
      
      child_offset += child_branch_height;
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
  animationTimer = Glib::signal_idle().connect(
    sigc::mem_fun(this, &TimelineLayoutHelper::on_animation_tick),
    Glib::PRIORITY_DEFAULT);
}

bool
TimelineLayoutHelper::on_animation_tick()
{
  update_layout();
  return animating;
}

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
