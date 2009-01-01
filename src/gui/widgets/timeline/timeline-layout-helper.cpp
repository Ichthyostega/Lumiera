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
  totalHeight(0)
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

weak_ptr<timeline::Track>
TimelineLayoutHelper::header_from_point(const Gdk::Point &point)
{
  std::pair<weak_ptr<timeline::Track>, Gdk::Rectangle> pair; 
  BOOST_FOREACH( pair, headerBoxes )
    {
      // Hit test the rectangle
      const Gdk::Rectangle &rect = pair.second;
      
      if(point.get_x() >= rect.get_x() &&
        point.get_x() < rect.get_x() + rect.get_width() &&
        point.get_y() >= rect.get_y() &&
        point.get_y() < rect.get_y() + rect.get_height())
        return pair.first;
    }
    
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
  // Make sure the style are loaded
  //read_styles();
    
  // Clear previously cached layout
  headerBoxes.clear();

  // Start at minus-the-scroll offset
  int offset = 0;//-timelineWidget->get_y_scroll_offset();
    
  //const Allocation container_allocation = get_allocation();
  const int header_width = 150;//container_allocation.get_width();
  const int indent_width = 10;
  
  layout_headers_recursive(layoutTree.begin(),
    offset, header_width, indent_width, 0, true);
    
  totalHeight = offset;
}

void
TimelineLayoutHelper::layout_headers_recursive(
  TrackTree::iterator_base parent_iterator,
  int &offset, const int header_width, const int indent_width,
  const int depth, bool parent_expanded)
{
  REQUIRE(depth >= 0);
  
  TrackTree::sibling_iterator iterator;
  for(iterator = layoutTree.begin(parent_iterator);
    iterator != layoutTree.end(parent_iterator);
    iterator++)
    {
      const shared_ptr<model::Track> &model_track = *iterator;
      REQUIRE(model_track);
      
      shared_ptr<timeline::Track> timeline_track =
        lookup_timeline_track(model_track);   
            
      if(parent_expanded)
        {          
          // Calculate and store the box of the header
          const int track_height = timeline_track->get_height();
          const int indent = depth * indent_width;
          
          headerBoxes[timeline_track] = Gdk::Rectangle(
            indent,                                           // x
            offset,                                           // y
            max( header_width - indent, 0 ),                  // width
            track_height);                                    // height
          
          // Offset for the next header
          offset += track_height + TimelineWidget::TrackPadding;
        }
      
      layout_headers_recursive(iterator, offset, header_width,
        indent_width, depth + 1,
        timeline_track->get_expanded() && parent_expanded);
    }
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

}   // namespace timeline
}   // namespace widgets
}   // namespace gui
