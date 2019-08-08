/*
  DISPLAY-MANAGER.hpp  -  abstraction to translate model structure and extension into screen layout

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file display-manager.hpp
 ** Abstraction: service for the widgets to translate themselves into screen layout.
 ** 
 ** # Architecture
 ** 
 ** We strive to break up the whole process of controlling the layout into several
 ** local concerns, each of which can be made self contained. The backbone is formed by
 ** a recursive collaboration between two abstractions (interfaces)
 ** - the building blocks of the timeline expose the interface timeline::Element
 ** - the global timeline widget implements a timeline::LayoutManager interface
 ** 
 ** ## Display evaluation pass
 ** 
 ** Whenever the layout of timeline contents has to be (re)established, we trigger a recursive
 ** evaluation pass, which in fact is a tree walk. The layout manager creates a DisplayEvaluation
 ** record, which is passed to the [Element's allocate function](\ref Element::allocate). The element
 ** in turn has the liability to walk its children and recursively initiate a nested evaluation
 ** by invoking DisplayEvaluation::evaluateChild(Element), which in turn calls back to
 ** LayoutManager::evaluate() to initiate a recursive evaluation pass. Within the recursively
 ** created DisplayEvaluation elements, we are able to transport and aggregate information
 ** necessary to give each element it' screen allocation. And this in turn allows us to
 ** decide upon a suitable display strategy for each individual element, within a local
 ** and self-contained context.
 ** 
 ** @todo WIP-WIP-WIP as of 11/2018
 ** @todo as of 10/2018 timeline display in the UI is rebuilt to match the architecture
 ** 
 */


#ifndef STAGE_TIMELINE_DISPLAY_MANAGER_H
#define STAGE_TIMELINE_DISPLAY_MANAGER_H


#include "lib/error.hpp"
#include "lib/nocopy.hpp"

#include "lib/util.hpp"

#include <sigc++/signal.h>

//#include <memory>
//#include <vector>



namespace stage  {
namespace timeline {
  
  using util::max;
  
//  class TrackHeadWidget;
  
  
  /** @todo quick-n-dirty hack. Should consider the Range TR (C++20) */
  struct PixSpan
    {
      int b = 0;
      int e = 0;
      
      PixSpan() { }
      PixSpan(int begin, int end)
        : b{begin}, e{max (begin,end)}
        { }
      
      bool
      empty()  const
        {
          return e <= b;
        }
      
      int
      delta()  const
        {
          return e - b;
        }
    };
  
  /**
   * Interface used by the widgets to translate themselves into screen layout.
   * @todo WIP-WIP as of 11/2018
   */
  class DisplayManager
    : util::NonCopyable
    {
      
//    TimelineLayout();
      
    public:
      virtual ~DisplayManager();    ///< this is an interface
      
      /** the overall horizontal pixel span to cover by this timeline */
      virtual PixSpan getPixSpan()         =0;
      
      /** cause a re-allocation of the complete layout */
      virtual void triggerDisplayEvaluation() =0;
      
      using SignalStructureChange = sigc::signal<void>;
      
      /**
       * signal to be invoked whenever the virtual structure of the
       * corresponding timeline changes, thus necessitating a new
       * arrangement of the timeline layout.
       */
      SignalStructureChange signalStructureChange_;
      
      
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace stage::timeline
#endif /*STAGE_TIMELINE_DISPLAY_MANAGER_H*/
