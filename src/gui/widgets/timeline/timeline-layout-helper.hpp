/*
  timeline-layout-helper.hpp  -  Declaration of the timeline
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
 
*/
/** @file timeline-layout-helper.cpp
 ** This file contains the definition of the layout helpeer class
 */

#ifndef TIMELINE_LAYOUT_HELPER_HPP
#define TIMELINE_LAYOUT_HELPER_HPP

#include "../../gtk-lumiera.hpp"
#include "../../../lib/tree.hpp"

namespace gui {

namespace model {
class Track;
}
  
namespace widgets {

class TimelineWidget;
  
namespace timeline {
  
class Track;

/**
 * A helper class for the TimelineWidget. TimelineLayoutHelper
 * is a class which calculates the layout of tracks in the timeline
 * track tree.
 * @see gui::widgets::TimelineWidget
 */
class TimelineLayoutHelper : public boost::noncopyable
{
public:
  /**
   * Definition of the layout track tree type.
   **/
  typedef lumiera::tree< boost::shared_ptr<model::Track> > TrackTree;
  
public:
  /**
   * Constructor.
   * @param owner The timeline widget which is the owner of this helper
   * class.
   **/
  TimelineLayoutHelper(TimelineWidget &owner);
  
  /**
   * Clones the timelineWidget sequence's track tree to create a layout
   * tree which will be identitcal to it.
   * @remarks The current layout tree will be deleted and replaced with
   * the clone.
   * @see add_branch
   **/
  void clone_tree_from_sequence();
  
  /**
   * Gets a reference to the helper's layout tree.
   * @return Returns a reference to the helper's layout tree.
   **/
  TrackTree& get_layout_tree();
  
  /**
   * Recalculates the track layout from layoutTree.
   * @see layout_headers_recursive
   **/
  void update_layout();
  
  /**
   * Get's the header rectangle of a given timeline track.
   * @param[in] track The track which will be looked up.
   * @return Returns the rectangle of the header offset by the y-scroll
   * offset, or if the track is hidden, or not present in the layout
   * tree, an empty optional will be returned.
   * @remarks This function is only usable after update_layout() has
   * been called on a valid tree of tracks.
   * @see update_layout()
   **/
  boost::optional<Gdk::Rectangle> get_track_header_rect(
    boost::weak_ptr<timeline::Track> track);
  
  /**
   * Searches for a header which has the specified point inside of it.
   * @param[in] point The point to search with.
   * @return Returns the header which has been found, or if no header is
   * found, an empty shared pointer is returned.
   * @remarks The point specified is relative to the scroll offset, so
   * y = 0 is the top edge of the scroll view. This function is only
   * usable after update_layout() has been called on a valid tree of
   * tracks.
   * @see update_layout()
   **/
  boost::shared_ptr<timeline::Track> header_from_point(
    Gdk::Point point);
  
  /**
   * Searches for a tack which has the specified y-offset inside of it.
   * @param[in] y The y-coordinate to search with.
   * @return Returns the track which has been found, or if no track is
   * found, an empty shared pointer is returned.
   * @remarks The point specified is relative to the scroll offset, so
   * y = 0 is the top edge of the scroll view. This function is only
   * usable after update_layout() has been called on a valid tree of
   * tracks.
   * @see update_layout()
   **/
  boost::shared_ptr<timeline::Track> track_from_y(int y);
  
  boost::shared_ptr<timeline::Track>
    begin_dragging_track(const Gdk::Point &mouse_point);
  
  void end_dragging_track();
  
  boost::shared_ptr<timeline::Track> get_dragging_track() const;
  
  void drag_to_point(const Gdk::Point &point);
  
  /**
   * Returns the total height in pixels of the layout tree.
   * @remarks This function is only on returns a valid value fter
   * update_layout() has been called on a valid tree of tracks.
   * @see update_layout()
   **/
  int get_total_height() const;
  
  bool is_animating() const;

  /**
   * A utility function which finds the iterator of a track in the
   * layout tree.
   * @param model_track The model track to look for.
   * @return Returns the model iterator of layoutTree.end() if no
   * iterator was found.
   **/
  TrackTree::pre_order_iterator iterator_from_track(
    boost::shared_ptr<model::Track> model_track);
  
protected:
  
  /**
   * A helper function for clone_tree_from_sequence(). This function
   * clones a branch within the model tree into the specified point in
   * that layout tree.
   * @param[in] parent_iterator The iterator of the node in the tree
   * which will become the parent of any tracks added.
   * @param[in] parent A pointer to the model track whose children
   * will be added to the layout tree branch.
   * @see clone_tree_from_sequence()
   **/
  void add_branch(TrackTree::iterator_base parent_iterator, 
    boost::shared_ptr<model::Track> parent);
  
  /**
   * Recursively calculates the boxes for a given branch in the timeline
   * tree.
   * @param[in] parent_iterator The iterator of the parent of the branch
   * whose boxes will be laid out.
   * @param[in] branch_offset The y-coordinate of the start of this
   * branch as measured in pixels from the origin.
   * @param[in] header_width The width of the header container widget in
   * pixels.
   * @param[in] header_width The width of indentation per branch in
   * pixels.
   * @param[in] depth The depth within the tree of tracks. depth = 0 for
   * root tracks.
   * @param[in] parent_expanded This value is set to true if all of the
   * ancestors of this track, up to the root are expanded and visible,
   * false if any of them are collapsed.
   * @return Returns the height of the branch in pixels.
   * @see update_layout()
   **/
  int layout_headers_recursive(
    TrackTree::iterator_base parent_iterator, const int branch_offset,
    const int header_width, const int indent_width, const int depth,
    const bool parent_expanded);
  
  /**
   * A helper function which calls lookup_timeline_track within the
   * parent timeline widget, but also applies lots of data consistency
   * checks in the process.
   * @param model_track The model track to look up in the parent widget.
   * @return Returns the track found, or returns NULL if no matching
   * track was found.
   * @remarks If the return value is going to be NULL, an ENSURE will
   * fail.
   **/
  boost::shared_ptr<timeline::Track> lookup_timeline_track(
    boost::shared_ptr<model::Track> model_track);
  
  /**
   * A helper function which kicks off the animation timer.
   **/
  void begin_animation();
  
  /**
   * The animation timer tick callback.
   **/
  bool on_animation_tick();
  
protected:

  /**
   * The owner timeline widget as provided to the constructor.
   **/
  TimelineWidget &timelineWidget;
  
  /**
   * The layout tree.
   **/
  TrackTree layoutTree;
  
  /**
   * A map of tracks to the rectangles of their headers.
   * @remarks This map is used as a cache, so that the rectangles don't
   * need to be perpetually recalculated. This cache is regenerated by
   * the update_layout method.
   * @see update_layout()
   **/
  std::map<boost::weak_ptr<timeline::Track>, Gdk::Rectangle>
    headerBoxes;
  
  /**
   * The total height of the track tree layout in pixels. This value
   * is only valid after layout_headers has been called.
   * @see update_layout()
   **/
  int totalHeight;
  
  TrackTree::pre_order_iterator draggingTrackIter;
  
  boost::shared_ptr<timeline::Track> draggingTrack;
  
  Gdk::Point dragStartOffset;
  
  Gdk::Point dragPoint;
  
  /**
   * The connection to the animation timer.
   * @see begin_animation()
   * @see on_animation_tick()
   **/
  sigc::connection animationTimer;
  
  /**
   * This value is true if the layout animation should continue.
   * @remarks This value is recalculated by update_layout()
   * @see update_layout()
   * @see on_animation_tick()
   **/
  bool animating;
};

}   // namespace timeline
}   // namespace widgets
}   // namespace gui

#endif // TIMELINE_LAYOUT_HELPER_HPP
