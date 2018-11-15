/*
  FOCUS-TRACKER.hpp  -  track focus and activation changes to move the activity spot

  Copyright (C)         Lumiera.org
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file focus-tracker.hpp
 ** Helper to track focus/activation changes to move the activity "Spot" automatically.
 ** The Lumiera UI introduces a set of structures and abstractions right above the usual
 ** and well-known "mechanics" of a graphical user interface. We do this to allow for a
 ** more coherent organisation of user interactions, based on the metaphor of working
 ** within a work space environment. Yet all of these amendments are intentionally
 ** subtle and non-obstructive; thus we need a way to link them to the well-established
 ** mechanics users come to expect of any graphical user interface:
 ** - you may click on something to mark it
 ** - you may right-click on something to get a context menu
 ** - you may push some button you see
 ** - you may issue a menu or global key-bound command to work on an implicit "subject"
 ** - some widget always has the "keyboard focus", and you may shift this focus.
 ** Any of these interactions as such are desirable, since they serve an obvious expectation
 ** of the user, but they somehow counterfeit the goal of establishing a coherent spatial
 ** ordering of work sites, since these interactions are "point-and-shot", not "go somewhere
 ** and do something". To mitigate this inner contradiction, we need to introduce a binding
 ** rule to gently move the global spot of activity alongside, whenever the user just
 ** "jumps at something" in sight. Doing so obviously also incurs the necessity to
 ** distinguish between coherent local moves (_within one_ WorkSite) from cross-cutting
 ** jump moves. For the implementation this means we have to query the _current WorkSite_
 ** to decide if a given move is still within its realm.
 ** 
 ** @todo WIP 2/2017 early draft / foundations of "interaction control"
 ** 
 ** @see interaction-director.hpp
 ** @see ui-bus.hpp
 */


#ifndef STAGE_INTERACT_FOCUS_TRACKER_H
#define STAGE_INTERACT_FOCUS_TRACKER_H

#include "stage/gtk-base.hpp"
#include "lib/nocopy.hpp"

//#include <string>
//#include <memory>


namespace gui {
namespace interact {
  
//  using std::unique_ptr;
//  using std::string;
  
//  class GlobalCtx;
  class Navigator;
  
  
  
  /**
   * Helper to pick up typical focus/activity changes, for the purpose
   * of keeping a coherent system of WorkSite locations and a global
   * "Spot" of activity in sync with spontaneous jumps to some
   * arbitrary widget within the UI.
   * 
   * @todo initial draft as of 2/2017 -- actual implementation has to be filled in
   */
  class FocusTracker
    : util::NonCopyable
    {
      Navigator& navigator_;
      
    public:
      FocusTracker (Navigator&);
     ~FocusTracker ();
      
    private:
      
    };
  
  
  
}}// namespace gui::interact
#endif /*STAGE_INTERACT_FOCUS_TRACKER_H*/
