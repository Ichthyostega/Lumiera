/*
  DisplayEvaluation  -  recursive interwoven timeline layout allocation

  Copyright (C)         Lumiera.org
    2020,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file display-evaluation.cpp
 ** Implementation details of collaborative and global timeline display allocation.
 ** 
 ** @todo WIP-WIP-WIP as of 3/2020
 ** 
 */


#include "stage/gtk-base.hpp"
#include "stage/timeline/display-evaluation.hpp"

//#include "gui/ui-bus.hpp"
//#include "lib/format-string.hpp"
//#include "lib/format-cout.hpp"

//#include "lib/util.hpp"

//#include <algorithm>
//#include <vector>



//using util::_Fmt;
//using util::contains;
//using Gtk::Widget;
//using sigc::mem_fun;
//using sigc::ptr_fun;
//using std::cout;
//using std::endl;


namespace stage {
namespace timeline {
  
  
  
  
  LayoutElement::~LayoutElement() { }   // emit VTables here....
  
  
  
  /**
   * Conduct global passes over the wired layout elements, in order to adjust
   * and coordinate the overall timeline layout. Within these passes, each element
   * starts with the (vertical) extension as allocated by GTK; typically these values
   * need to be increased slightly, in order to align the track headers and the track
   * body display, and to accommodate the clips and effects placed into each track.
   * Such local adjustments will typically re-trigger the custom drawing code, and
   * thus re-invoke this function, until the overall layout is stable. In a similar
   * vein, the expanding or collapsing of clips and even whole sub-track scopes
   * will cause adjustments and a re-evaluation.
   * @warning care has to be taken to not "overshoot" each adjustment, since this
   *          might lead to never ending re-invocations and "layout oscillation".
   * @remark the Layout is established in two phases
   *         - first the necessary screen extension is determined
   *           and the global profile of tracks is reconstructed
   *         - the second pass verifies and possibly reflows
   *           to achieve consistency, possibly triggering
   *           recursively through additional size adjustments.
   */
  void
  DisplayEvaluation::perform()
  {
    this->reset();
    // Phase-1 : collect Layout information
    forkRoot_->establishLayout (*this);
    canvas_->establishLayout (*this);
    // Phase-2 : reflow and balance the Layout
    collectLayout_ = false;
    forkRoot_->establishLayout (*this);
  }
  
  /** pristine state for the next DisplayEvaluation pass */
  void
  DisplayEvaluation::reset()
  {
    collectLayout_ = true;
  }

  
  
  
  
}}// namespace stage::timeline
