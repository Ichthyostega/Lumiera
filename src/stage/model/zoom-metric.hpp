/*
  ZOOM-METRIC.hpp  -  mix-in to provide a DisplayMetric based on a ZoomWindow

  Copyright (C)         Lumiera.org
    2022,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file zoom-metric.hpp
 ** Implementation building block to get the \ref DisplayMetric as defined through a
 ** \ref ZoomWindow
 ** 
 ** # rationale
 ** 
 ** TBW
 ** 
 ** # Interactions
 ** 
 ** - *bla*: connect to blubb
 ** see [sigc-track] for an explanation.
 ** 
 ** [MVC-Pattern]: http://en.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93controller
 ** [sigc-track]: http://issues.lumiera.org/ticket/940#comment:3 "Ticket #940"
 ** 
 ** @see \ref ZoomWindow_test
 ** 
 */


#ifndef STAGE_MODEL_ZOOM_METRIC_H
#define STAGE_MODEL_ZOOM_METRIC_H


#include "lib/error.hpp"
//#include "lib/nocopy.hpp"
//#include "lib/idi/entry-id.hpp"
//#include "lib/symbol.hpp"
#include "stage/model/canvas-hook.hpp"

//#include <utility>
//#include <string>


namespace stage {
namespace model {
  
//  using std::string;
//  using lib::Symbol;
  
  
  /**
   */
  template<class I>
  class ZoomMetric
    : public I
    {
      
      ZoomMetric()
        { }
      
      
    protected:
      
    private:
    };
  
  
  
  
  /** */

  
  
  
}} // namespace stage::model
#endif /*STAGE_MODEL_ZOOM_METRIC_H*/
