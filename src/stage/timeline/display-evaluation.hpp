/*
  DISPLAY-EVALUATION.hpp  -  recursive interwoven timeline layout allocation

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

*/


/** @file display-evaluation.hpp
 ** A collaboration to establish a globally coherent timeline display layout.
 ** Whenever the layout of timeline contents has to be (re)established, a recursive
 ** evaluation pass is triggered, which in fact is a tree walk. As a foundation, we assume
 ** that GTK has provided each of the actual widgets with a screen space allocation sufficient
 ** to render the content known thus far. At this point, we need to traverse the whole timeline
 ** structure, collect size and allocation information and create additional screen space requests
 ** if necessary. Generally speaking, this recursive process can not be entirely successful at first
 ** pass; typically additional space requirements are discovered and propagated _as message_ to GTK,
 ** and so the DisplayEvaluation can be expected to be re-triggered soon thereafter.
 ** 
 ** ## Tasks to perform
 ** 
 ** The basic goal is to establish a coherent vertical space allocation for all tracks within the
 ** timeline (while, to the contrary, the horizontal extension is a fixed requirement and can be
 ** derived from the covered time span as translated by a zoom factor). This can be achieved by
 ** - collecting the vertical content extension within each track individually
 ** - buildup of a timeline::TrackProfile to accommodate those requirements and all decorations
 ** - adjustment of the TrackHeadWidget extensions to match the allocated track body space precisely.
 ** 
 ** ## Evaluation structure
 ** 
 ** This is an intricate collaboration of closely related elements; however, each of the aforementioned
 ** tasks is defined such as to operate in a self-confined way on some part of the timeline. All the
 ** collaborating elements implement a common invocation interface timeline::LayoutElement, which allows
 ** to pass on the DisplayEvaluation itself by reference, recursively. To make the overall process work,
 ** moreover we establish a *Requirement* to pass on this invocation _strictly in layout order_ -- which
 ** implies a recursive depth-first invocation proceeding *top-down* and *from left to right*. It is
 ** each LayoutElement's liability to recurse appropriately to make this happen.
 ** 
 ** ## Evaluation state and phases
 ** 
 ** The DisplayEvaluation works both by direct (side)effect within the invoked elements, and by collecting
 ** [resizing requests](\ref ReSizeRequest) for asynchronous dispatch. Thus the data and state within the
 ** evaluation record itself is also used to control the successive phases of the evaluation. The evaluation
 ** is triggered repeatedly, until the layout is _globally balanced_ and no further resizing requests emerge.
 ** 
 ** @todo WIP-WIP-WIP as of 3/2020
 ** 
 */


#ifndef STAGE_TIMELINE_DISPLAY_EVALUATION_H
#define STAGE_TIMELINE_DISPLAY_EVALUATION_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"
//#include "lib/util.hpp"

//#include <sigc++/signal.h>



namespace stage  {
namespace timeline {
  
//using util::max;
  
//using model::ViewHooked;
//class TrackHeadWidget;
//class TrackBody;
  template<class WID>
  class DisplayEvaluation;
  
  
  /** @todo WIP-WIP as of 3/2020 */
  template<class WID>
  class LayoutElement
    {
    public:
      using SubKey = WID*;
          
      virtual ~LayoutElement();        ///< this is an interface
      
      virtual void establishLaylut (DisplayEvaluation<WID>&)  =0;
      virtual void doResize (SubKey elm, int newSize)         =0;
    };
  
  
  /**
   * Visitor and state holder for a collaborative layout adjustment pass.
   * @todo WIP-WIP as of 3/2020
   */
  template<class WID>
  class DisplayEvaluation
    : util::NonCopyable
    {
      
//    DisplayEvaluation();
      
    public:
      using SubKey = WID*;
      
      virtual ~DisplayEvaluation() { };    ///< this is an interface
      
      /**  */
      virtual void perform()                                  =0;
      
      /** schedule to resize the indicated sub-element */
      virtual void planResize (SubKey elm, int newSize)       =0;
      
      
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_DISPLAY_EVALUATION_H*/
