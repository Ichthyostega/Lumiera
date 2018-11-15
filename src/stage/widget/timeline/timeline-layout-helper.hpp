/*
  TIMELINE-LAYOUT-HELPER.hpp  -  helper to coordinate the timeline presentation

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
/** @file timeline-layout-helper.cpp
 ** Preliminary implementation of Timeline presentation, layout helper.
 ** @todo this class should be thoroughly reworked; it seems to be kind of a "god class" ////////TODO 5/2015
 ** @deprecated as of 2016 this class will become obsolete and replaced by a different implementation approach
 */


#ifndef STAGE_WIDGET_TIMELINE_LAYOUT_HELPER_H
#define STAGE_WIDGET_TIMELINE_LAYOUT_HELPER_H

#include "stage/gtk-base.hpp"
#include "lib/util-coll.hpp"
#include "lib/tree.hpp"

#include <memory>
#include <boost/optional.hpp>


namespace stage {
  
  namespace model {
    class Track;
    class Sequence;
  }
  namespace widget {
    class TimelineWidget;
  
  
  namespace timeline {
    
  using std::shared_ptr;
    
  class Track;
  
  
  /**
   * A helper class for the TimelineWidget. TimelineLayoutHelper
   * is a class which calculates the layout of tracks in the timeline
   * track tree.
   * @see stage::widget::TimelineWidget
   */
  class TimelineLayoutHelper
    : util::NonCopyable
    {
    public:
      /** Definition of the layout track tree type.*/
      using TrackTree = lib::Tree< std::shared_ptr<model::Track>>;
      
    public:
      TimelineLayoutHelper (TimelineWidget& owner);
      
      /**
       * Clones the timelineWidget sequence's track tree to create a layout
       * tree which will be identical to it.
       * @remarks The current layout tree will be deleted
       *          and replaced with the clone.
       * @see add_branch
       */
      void clone_tree_from_sequence();
      
      /**
       * Gets a reference to the helper's layout tree.
       * @return Returns a reference to the helper's layout tree.
       */
      TrackTree& get_layout_tree();
      
      /**
       * Recalculates the track layout from layoutTree.
       * @see layout_headers_recursive
       */
      void update_layout();
      
      
      /**
       * Get's the header rectangle of a given timeline track.
       * @param[in] track The track which will be looked up.
       * @return Returns the rectangle of the header offset by the y-scroll
       *         offset, or if the track is hidden, or not present in the layout
       *         tree, an empty optional will be returned.
       * @remarks This function is only usable after update_layout() has
       *         been called on a valid tree of tracks.
       * @see #update_layout()
       */
      boost::optional<Gdk::Rectangle>
      get_track_header_rect (std::weak_ptr<timeline::Track> track);
      
      
      /**
       * Searches for a header which has the specified point inside of it.
       * @param[in] point The point to search with.
       * @return the header which has been found, or if no header
       *         is found, an empty shared pointer is returned.
       * @remarks The point specified is relative to the scroll offset,
       *         so y = 0 is the top edge of the scroll view.
       *         This function is only usable after #update_layout()
       *         has been called on a valid tree of tracks.
       * @see #update_layout()
       */
      shared_ptr<timeline::Track> header_from_point (Gdk::Point point);
      
      /**
       * Searches for a tack which has the specified y-offset inside of it.
       * @param[in] y The y-coordinate to search with.
       * @return the track which has been found, or if no track
       *         is found, an empty shared pointer is returned.
       * @remarks The point specified is relative to the scroll offset,
       *         so y = 0 is the top edge of the scroll view.
       *         This function is only usable after #update_layout()
       *         has been called on a valid tree of tracks.
       * @see #update_layout()
       */
      shared_ptr<timeline::Track> track_from_y (int y);
      
      /**
       * Begins to drag the track under mouse_point, if there is one.
       * @param mouse_point The mouse point to begin dragging from, measured
       * in pixels from the top left of the header container widget.
       */
      shared_ptr<timeline::Track>
      begin_dragging_track (Gdk::Point const& mouse_point);
      
      /**
       * Drops the dragging track.
       * @param apply true if the model tree should be modified.
       */
      void end_dragging_track(bool apply);
      
      /**
       * Returns true if a track is being dragged.
       */
      bool is_dragging_track()  const;
      
      /**
       * Gets the iterator of the layout tree node that is being dragged.
       */
      TrackTree::pre_order_iterator get_dragging_track_iter()  const;
      
      
      /**
       * Drags the dragging branch to a new mouse point.
       * @param mouse_point point to drag the dragging track to.
       *        This point is in pixels relative to the top left
       *        of the header container.
       * @remarks drag_to_point may only be called after #begin_dragging_track
       *        and before #end_dragging_point have been called.
       */
      void drag_to_point (Gdk::Point const& mouse_point);
      
      
      /**
       * Returns the total height in pixels of the layout tree.
       * @remarks This function is only on returns a valid value after
       *        #update_layout() has been called on a valid tree of tracks.
       * @see #update_layout()
       */
      int get_total_height() const;
      
      /**
       * Returns true if the layout is currently animating.
       */
      bool is_animating() const;
      
      
      /**
       * A utility function which finds the iterator
       * of a track in the layout tree.
       * @param modelTrack The model track to look for.
       * @return the model iterator of layoutTree.end()
       *         if no iterator was found.
       */
      TrackTree::pre_order_iterator
      iterator_from_track (shared_ptr<model::Track> modelTrack);
      
      
      /**
       * A function that recursively calculates the visible height of a
       * branch taking into account branches that are expanded or collapsed.
       * @param parent_iterator parent of the branch to measure. This node
       *        and all the child nodes will be included in the measurement.
       * @return the height of the branch in pixels.
       */
      int measure_branch_height (TrackTree::iterator_base parent_iterator);
      
      
    protected:
      /**
       * An enum to specify the relationship between a tree node, and
       * another node which is going to be inserted adjacent.
       */
      enum TreeRelation
      {
        /**
         * No relation
         */
        None,
        
        /**
         * The node will be inserted immediately before this one.
         */
        Before,
        
        /**
         * The node will be inserted immediately after this one.
         */
        After,
        
        /**
         * The node will be inserted as the first child of this one.
         */
        FirstChild,
        
        /**
         * The node will be inserted as the last child of this one.
         */
        LastChild
      };
      
      /**
       * A structure used to specify where a track will be dropped when
       * dragging ends.
       */
      struct DropPoint
        {
          /**
           * Specifies the target node onto which the dragging track will be
           * dropped.
           */
          TrackTree::pre_order_iterator target;
          
          /**
           * The where to drop the dragging track in relation to target.
           */
          TreeRelation relation;
        };
      
      
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
       */
      void add_branch(TrackTree::iterator_base parent_iterator
                     ,shared_ptr<model::Track> parent);
      
      
      /**
       * Recursively calculates the boxes for a given branch in the timeline tree.
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
       */
      int
      layout_headers_recursive (TrackTree::iterator_base parent_iterator
                               ,const int branch_offset
                               ,const int header_width
                               ,const int indent_width
                               ,const int depth
                               ,const bool parent_expanded);
      
      /**
       * A helper function which calls lookup_timeline_track within the
       * parent timeline widget, but also applies lots of data consistency
       * checks in the process.
       * @param modelTrack The model track to look up in the parent widget.
       * @return Returns the track found, or returns NULL if no matching
       * track was found.
       * @remarks If the return value is going to be NULL, an ENSURE will
       * fail.
       */
      shared_ptr<timeline::Track>
      lookup_timeline_track (shared_ptr<model::Track> modelTrack);
      
      /**
       * A helper function which kicks off the animation timer.
       */
      void begin_animation();
      
      /**
       * The animation timer tick callback.
       */
      bool on_animation_tick();
      
      /**
       * Attempts to find a drop point on the target node at point.
       * @param[in] target The iterator of the target node on which to make
       * the attempt.
       * @param[in] point The point on which do to the test.
       * @remarks This function hit-tests a header looking to see if the
       * point is hovering over it, and if it is, it works out what part of
       * the header, and therefore what drop location the user us gesturally
       * pointing to.
       */
      TimelineLayoutHelper::DropPoint
      attempt_drop (TrackTree::pre_order_iterator target
                   ,Gdk::Point const& point);
      
      /**
       * Drops the dragging track to a new location in the layout tree
       * as specified by drop.
       * @param[in] drop The point in the tree to drop onto.
       */
      void apply_drop_to_layout_tree (DropPoint const& drop);
      
      /**
       * Drops the dragging track to a new location in the model tree
       * as specified by drop.
       * @param[in] drop The point in the tree to drop onto.
       */
      void apply_drop_to_modelTree(DropPoint const& drop);
      
      /**
       * Helper to get the sequence object from the state.
       * @return Returns a shared pointer to the sequence.
       */
      shared_ptr<model::Sequence>
      get_sequence() const;
      
      
    protected:
      /** owner timeline widget as provided to the constructor. */
      TimelineWidget &timelineWidget;
      
      TrackTree layoutTree;
      
      /**
       * A map of tracks to the rectangles of their headers.
       * @remarks This map is used as a cache, so that the rectangles don't
       *        need to be perpetually recalculated. This cache is regenerated by
       *        the update_layout method.
       * @see #update_layout()
       */
      std::map< std::weak_ptr<timeline::Track>
              , Gdk::Rectangle
              , ::util::WeakPtrComparator
              >
        headerBoxes;
      
      /**
       * The total height of the track tree layout in pixels. This value
       * is only valid after layout_headers has been called.
       * @see update_layout()
       */
      int totalHeight;
      
      /**
       * The iterator of the layoutTree node that is presently being
       * dragged.
       * @remarks draggingTrackIter.node is set to NULL when no drag is
       * taking place.
       */
      TrackTree::pre_order_iterator draggingTrackIter;
      
      /**
       * The offset of the mouse relative to the top-left corner of the
       * dragging track.
       */
      Gdk::Point dragStartOffset;
      
      /**
       * The coordinates of the dragging mouse in pixels, measured from the
       * top left of the whole layout.
       * @remarks This value is changed by begin_dragging_track and
       * drag_to_point
       */
      Gdk::Point dragPoint;
      
      /**
       * The total height of the dragging branch in pixels.
       * @remarks This value is updated by begin_dragging_track
       */
      int dragBranchHeight;
      
      /**
       * The tree point the the user is currently hovering on.
       * @remarks This value is updated by drag_to_point.
       */
      DropPoint dropPoint;
      
      /**
       * The connection to the animation timer.
       * @see begin_animation()
       * @see on_animation_tick()
       */
      sigc::connection animationTimer;
      
      /**
       * This value is true if the layout animation should continue.
       * @remarks This value is recalculated by update_layout()
       * @see update_layout()
       * @see on_animation_tick()
       */
      bool animating;
    };
  
  
}}}// namespace stage::widget::timeline
#endif /*STAGE_WIDGET_TIMELINE_LAYOUT_HELPER_H*/
