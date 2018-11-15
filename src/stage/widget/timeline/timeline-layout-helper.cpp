/*
  TimelineLayoutHelper  -  helper to coordinate the timeline presentation

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


/** @file timeline-layout-helper.cpp
 ** Preliminary implementation of Timeline presentation, layout helper.
 ** @deprecated as of 2016 this class will become obsolete and replaced by a different implementation approach
 */


#include "stage/widget/timeline/timeline-layout-helper.hpp"
#include "stage/widget/timeline-widget.hpp"
#include "stage/model/sequence.hpp"
#include "stage/draw/rectangle.hpp"

#include <boost/foreach.hpp>
#include <memory>
#include <list>

using std::pair;
using std::list;
using std::shared_ptr;
using std::weak_ptr;
using std::dynamic_pointer_cast;

using namespace Gtk;           ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace lumiera;       ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace util;          ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!
using namespace stage::draw;     ///////////////////////////////////////////////////////////////////////////////TICKET #1071 no wildcard includes please!

namespace stage {
namespace widget {
namespace timeline {
  
  
  TimelineLayoutHelper::TimelineLayoutHelper (TimelineWidget &owner)
    : timelineWidget(owner)
    , totalHeight(0)
    , dragBranchHeight(0)
    , animating(false)
    {
      // Init draggingTrackIter into a non-dragging state
      draggingTrackIter.node = NULL;
    }
  
  
  void
  TimelineLayoutHelper::clone_tree_from_sequence()
  {
    const shared_ptr<model::Sequence> sequence = get_sequence();
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
  TimelineLayoutHelper::add_branch (TrackTree::iterator_base parent_iterator
                                   ,shared_ptr<model::Track> parent)
  {
    BOOST_FOREACH(shared_ptr<model::Track> child,
      parent->get_child_tracks())
      {
        TrackTree::iterator_base child_iterator = 
            layoutTree.append_child(parent_iterator, child);
        add_branch(child_iterator, child);
      }
  }
  
  boost::optional<Gdk::Rectangle>
  TimelineLayoutHelper::get_track_header_rect (std::weak_ptr<timeline::Track> track)
  {
    if (contains(headerBoxes, track))
    {
      Gdk::Rectangle rect(headerBoxes[track]);
      rect.set_y(rect.get_y() - timelineWidget.get_y_scroll_offset());
      return boost::optional<Gdk::Rectangle>(rect);
    }
    return boost::optional<Gdk::Rectangle>();
  }
  
  
  shared_ptr<timeline::Track>
  TimelineLayoutHelper::header_from_point (Gdk::Point point)
  {
    // Apply the scroll offset
    point.set_y(point.get_y() + timelineWidget.get_y_scroll_offset());
    
    // Search the headers
    std::pair<weak_ptr<timeline::Track>, Gdk::Rectangle> pair; 
    BOOST_FOREACH( pair, headerBoxes )
      {
        // Hit test the rectangle      
        if (stage::draw::pt_in_rect (point, pair.second))
          return shared_ptr<timeline::Track>(pair.first);
      }
    
    // No track was found - return an empty pointer
    return shared_ptr<timeline::Track>();
  }
  
  
  shared_ptr<timeline::Track>
  TimelineLayoutHelper::track_from_y (int y)
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
  
  
  shared_ptr<timeline::Track>
  TimelineLayoutHelper::begin_dragging_track(Gdk::Point const& mouse_point)
  {
    shared_ptr<timeline::Track> dragging_track =
      header_from_point(mouse_point);
    if(!dragging_track)
      return shared_ptr<timeline::Track>();
    
    dragPoint = Gdk::Point(mouse_point.get_x(),
                           mouse_point.get_y() + timelineWidget.get_y_scroll_offset());
    
    const Gdk::Rectangle &rect = headerBoxes[dragging_track];
    dragStartOffset = Gdk::Point(
      dragPoint.get_x() - rect.get_x(),
      dragPoint.get_y() - rect.get_y());
    
    const shared_ptr<model::Track> modelTrack =
      dragging_track->getModelTrack();
    draggingTrackIter = iterator_from_track(modelTrack);
    dragBranchHeight = measure_branch_height(draggingTrackIter);
    
    dropPoint.relation = None;
    
    return dragging_track;
  }
  
  
  void
  TimelineLayoutHelper::end_dragging_track(bool apply)
  {
    if(apply)
      apply_drop_to_modelTree(dropPoint);
    
    draggingTrackIter.node = NULL;
    clone_tree_from_sequence();
    update_layout();
  }
  
  
  bool
  TimelineLayoutHelper::is_dragging_track()  const
  {
    return draggingTrackIter.node != NULL;
  }
  
  
  TimelineLayoutHelper::TrackTree::pre_order_iterator
  TimelineLayoutHelper::get_dragging_track_iter()  const
  {
    return draggingTrackIter;
  }
  
  
  void
  TimelineLayoutHelper::drag_to_point (Gdk::Point const& mouse_point)
  {
    
    DropPoint drop;
    drop.relation = None;
    
    // begin_dragging_track must have been called before
    REQUIRE(is_dragging_track());
    
    // Apply the scroll offset
    const Gdk::Point last_point(dragPoint);
    dragPoint = Gdk::Point(mouse_point.get_x(),
      mouse_point.get_y() + timelineWidget.get_y_scroll_offset());
    
    // Get a test-point
    // We probe on the bottom edge of the dragging branch if the track is
    // being dragged downward, and on the top edge if it's being dragged
    // upward.
    Gdk::Point test_point(dragPoint.get_x(),
      dragPoint.get_y() - dragStartOffset.get_y());
    if(last_point.get_y() > dragPoint.get_y())
      test_point.set_y(test_point.get_y());
    else
      test_point.set_y(test_point.get_y() + dragBranchHeight);
    
    TrackTree::pre_order_iterator iterator;
    TrackTree::pre_order_iterator begin = ++layoutTree.begin();
    
    if(test_point.get_y() < 0)
      {
        // Find the first header that's not being dragged
        for(iterator = begin;
          iterator != layoutTree.end();
          iterator++)
          {
            if(iterator == draggingTrackIter)
              iterator.skip_children();
            else
              {
                drop.relation = Before;
                drop.target = iterator;
                break;
              }
          }
      }
    else if(test_point.get_y() > totalHeight)
      {
        // Find the last header that's not being dragged
        for(iterator = --layoutTree.end();
          iterator != begin;
          iterator--)
          {
            if(iterator.is_descendant_of(draggingTrackIter))
              iterator.skip_children();
            else
              {
                drop.relation = After;
                drop.target = iterator;
                break;
              }
          }
      }
    else
      {
        // Search the headers
        for(iterator = ++layoutTree.begin();
          iterator != layoutTree.end();
          iterator++)
          { 
            // Skip the dragging branch
            if(iterator == draggingTrackIter)
              iterator.skip_children();
            else
              {
                // Do hit test       
                drop = attempt_drop(iterator, test_point);
                if(drop.relation != None)
                  break;
              }
          }
      }
    
    // Did we get a drop point?
    if (drop.relation != None)
      {
        REQUIRE(*drop.target);
        shared_ptr<timeline::Track> target_timeline_track =
          lookup_timeline_track(*drop.target);
        
        apply_drop_to_layout_tree(drop);
        dropPoint = drop;
        
        // Expand the branch if the user is hovering to add the track
        // as a child
        if((drop.relation == FirstChild || drop.relation == LastChild) &&
          !target_timeline_track->get_expanded())
            target_timeline_track->expand_collapse(Track::Expand);
      }
    
    update_layout();
  }
  
  
  int
  TimelineLayoutHelper::get_total_height()  const
  {
    ENSURE(totalHeight >= 0);
    return totalHeight;
  }
  
  
  bool
  TimelineLayoutHelper::is_animating()  const
  {
    return animating;
  }
  
  
  TimelineLayoutHelper::TrackTree::pre_order_iterator
  TimelineLayoutHelper::iterator_from_track (shared_ptr<model::Track> modelTrack)
  {
    REQUIRE(modelTrack);
    
    TrackTree::pre_order_iterator iter;
    for(iter = layoutTree.begin(); iter != layoutTree.end(); iter++)
      {
        if(*iter == modelTrack)
          break;
      }
      
    return iter;
  }
  
  
  int
  TimelineLayoutHelper::measure_branch_height(TrackTree::iterator_base parent_iterator)
  {
    shared_ptr<timeline::Track> parent_track =
      lookup_timeline_track(*parent_iterator);
    
    int branch_height = parent_track->get_height() +
      TimelineWidget::TrackPadding;
    
    // Add the heights of child tracks if this parent is expanded
    if(parent_track->get_expanded())
      {
        TrackTree::sibling_iterator iterator;
        for(iterator = layoutTree.begin(parent_iterator);
          iterator != layoutTree.end(parent_iterator);
          iterator++)
          {           
            shared_ptr<timeline::Track> child_track =
              lookup_timeline_track(*iterator);
            
            branch_height += measure_branch_height(iterator);
          }
      }
    
    return branch_height;
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
    if (animating && !animationTimer)
      begin_animation();
  }
  
  
  int
  TimelineLayoutHelper::layout_headers_recursive (TrackTree::iterator_base parent_iterator
                                                 ,const int branch_offset
                                                 ,const int header_width
                                                 ,const int indent_width
                                                 ,const int depth
                                                 ,const bool parent_expanded)
  {
    REQUIRE(depth >= 0);
    
    const bool dragging = is_dragging_track();
    int child_offset = 0;
    
    TrackTree::sibling_iterator iterator;
    for (iterator = layoutTree.begin(parent_iterator);
         iterator != layoutTree.end(parent_iterator);
         iterator++)
      {
        Gdk::Rectangle rect;
        int track_height = 0;
        
        const shared_ptr<model::Track> &modelTrack = *iterator;
        REQUIRE(modelTrack);
        
        shared_ptr<timeline::Track> timeline_track = lookup_timeline_track(modelTrack);
        
        // Is this the root track of a dragging branch?
        bool being_dragged = false;
        if (dragging)
          being_dragged = (modelTrack == *draggingTrackIter);
        
        // Is the track going to be shown?
        if (parent_expanded)
          {
            // Calculate and store the box of the header
            track_height = timeline_track->get_height()
                           + TimelineWidget::TrackPadding;
            const int indent = depth * indent_width;
            
            rect = Gdk::Rectangle(indent                             // x
                                 ,branch_offset + child_offset       // y
                                 ,max( header_width - indent, 0 )    // width
                                 ,track_height);                     // height
            
            // Offset for the next header
            child_offset += track_height;
            
            // Is this header being dragged?
            if (being_dragged)
              rect.set_y(dragPoint.get_y() - dragStartOffset.get_y());
            
            headerBoxes[timeline_track] = rect;
          }
        
        // Is the track animating?
        const bool is_track_animating = timeline_track->is_expand_animating();
        animating |= is_track_animating;
        
        // Recurse to children?
        const bool expand_child = (animating || timeline_track->get_expanded())
                                  && parent_expanded;
        
        int child_branch_height =
            layout_headers_recursive (iterator
                                     ,rect.get_y() + track_height
                                     ,header_width
                                     ,indent_width
                                     ,depth + 1
                                     ,expand_child);
        
        // Do collapse animation as necessary
        if (is_track_animating)
          {
            // Calculate the height of the area which will be
            // shown as expanded
            const float a = timeline_track->get_expand_animation_state();
            child_branch_height *= a * a;
            const int y_limit = branch_offset + child_offset + child_branch_height;
            
            // Obscureed tracks according to the animation state
            TrackTree::pre_order_iterator descendant_iterator(iterator);
            descendant_iterator++;
            TrackTree::sibling_iterator end(iterator);
            end++;
            
            for (descendant_iterator = layoutTree.begin(parent_iterator);
                 descendant_iterator != end;
                 descendant_iterator++)
              {
                const weak_ptr<timeline::Track> track = lookup_timeline_track(*descendant_iterator);
                const Gdk::Rectangle &rect = headerBoxes[track];
                
                if (rect.get_y() + rect.get_height() > y_limit)
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
  TimelineLayoutHelper::lookup_timeline_track(shared_ptr<model::Track> modelTrack)
  {
    REQUIRE(modelTrack != NULL);
    shared_ptr<timeline::Track> timeline_track =
      timelineWidget.lookup_timeline_track(modelTrack);
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
  
  
  TimelineLayoutHelper::DropPoint
  TimelineLayoutHelper::attempt_drop (TrackTree::pre_order_iterator target
                                     ,Gdk::Point const& point)
  {
    // Lookup the tracks
    const shared_ptr<model::Track> modelTrack(*target);
    REQUIRE(modelTrack);
    const weak_ptr<timeline::Track> timeline_track =
      lookup_timeline_track(modelTrack);
    
    // Calculate coordinates
    const Gdk::Rectangle &rect = headerBoxes[timeline_track];
    const int half_height = rect.get_height() / 2;
    const int y = rect.get_y();
    const int y_mid = y + half_height;
    const int full_width = rect.get_x() + rect.get_width();
    const int x_mid = rect.get_x() + rect.get_width() / 2;
    
    // Initialise the drop
    // By specifying relation = None, the default return value will signal
    // no drop-point was found at point
    DropPoint drop = {target, None};
    
    if (pt_in_rect (point, Gdk::Rectangle(0, y, full_width, half_height)))
      {
        // We're hovering over the upper half of the header
        drop.relation = Before;
      }
    else
    if (pt_in_rect(point, Gdk::Rectangle(0, y_mid, full_width, half_height)))
      {
        // We're hovering over the lower half of the header
        if(modelTrack->can_host_children())
          {
            if(modelTrack->get_child_tracks().empty())
              {
                // Is our track being dragged after this header?
                if(dragPoint.get_x() < x_mid)
                  drop.relation = After;
                else
                  drop.relation = FirstChild;
              }
            else
              drop.relation = FirstChild;
          }
        else
          {
            // When this track cannot be a parent,
            // the dragging track is simply dropped after
            drop.relation = After;
          }
      }
    
    return drop;
  }
  
  
  void
  TimelineLayoutHelper::apply_drop_to_layout_tree (TimelineLayoutHelper::DropPoint const& drop)
  {
    switch(drop.relation)
      {
      case None:
        break;
        
      case Before:
        draggingTrackIter = layoutTree.move_before(
          drop.target, draggingTrackIter);
        break;
        
      case After:
        draggingTrackIter = layoutTree.move_after(
          drop.target, draggingTrackIter);
        break;
        
      case FirstChild:
        if(draggingTrackIter.node->parent != drop.target.node)
          {
            draggingTrackIter = layoutTree.move_ontop(
              layoutTree.prepend_child(drop.target), draggingTrackIter);
          }
        break;
        
      case LastChild:
        if(draggingTrackIter.node->parent != drop.target.node)
          {
            draggingTrackIter = layoutTree.move_ontop(
              layoutTree.append_child(drop.target), draggingTrackIter);
          }
        break;
        
      default:
        ASSERT(0);  // Unexpected value of relation
        break;
      }
  }
  
  
  void
  TimelineLayoutHelper::apply_drop_to_modelTree (TimelineLayoutHelper::DropPoint const& drop)
  {
    if(drop.relation == None) return;
    
    
    const shared_ptr<model::Sequence> sequence = get_sequence();
    
    // Freeze the timeline widget - it must be done manually later
    timelineWidget.freeze_update_tracks();
    
    // Get the tracks
    shared_ptr<model::Track> &dragging_track = *draggingTrackIter;
    REQUIRE(dragging_track);
    REQUIRE(dragging_track != sequence);
    
    shared_ptr<model::Track> &target_track = *drop.target;
    REQUIRE(target_track);
    REQUIRE(target_track != sequence);
    
    // Detach the track from the old parent
    shared_ptr<model::ParentTrack> old_parent =
       sequence->find_descendant_track_parent(dragging_track);
    REQUIRE(old_parent);  // The track must have a parent
    old_parent->get_child_track_list().remove(dragging_track);
    
    if (drop.relation == Before || drop.relation == After)
      {
        // Find the new parent track
        shared_ptr<model::ParentTrack> new_parent =
          sequence->find_descendant_track_parent(target_track);
        REQUIRE(new_parent);  // The track must have a parent
        
        // Find the destination point
        observable_list< shared_ptr<model::Track> > &dest =
          new_parent->get_child_track_list();
        list< shared_ptr<model::Track> >::iterator iter;
        for (iter = dest.begin(); iter != dest.end(); iter++)
          {
            if(*iter == target_track)
              break;
          }
        REQUIRE(iter != dest.end());  // The target must be
                                      // in the destination
        
        // We have to jump on 1 if we want to insert after
        if(drop.relation == After)
          iter++;
        
        // Insert at this point
        dest.insert(iter, dragging_track);
      }
    else
    if (drop.relation == FirstChild || drop.relation == LastChild)
      {
        shared_ptr<model::ParentTrack> new_parent =
          dynamic_pointer_cast<model::ParentTrack, model::Track>(
            target_track);
        REQUIRE(new_parent);  // The track must have a parent
        
        if (drop.relation == FirstChild)
          new_parent->get_child_track_list().push_front(dragging_track);
        else
        if (drop.relation == LastChild)
          new_parent->get_child_track_list().push_back(dragging_track);
      }
    else ASSERT(0); // Unexpected value of relation
    
    // Thaw the timeline widget
    timelineWidget.thaw_update_tracks();
  }
  
  shared_ptr<model::Sequence>
  TimelineLayoutHelper::get_sequence()  const
  {
    REQUIRE(timelineWidget.state);
    shared_ptr<model::Sequence> sequence(
      timelineWidget.state->getSequence());
    ENSURE(sequence);
    return sequence;
  }
  
  
}}}// namespace stage::widget::timeline
