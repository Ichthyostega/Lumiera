/*
  ZOOM-WINDOW.hpp  -  generic translation from domain to screen coordinates

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


/** @file zoom-window.hpp
 ** Abstraction: a multi-dimensional extract from model space into screen coordinates.
 ** This is a generic component to represent and handle the zooming and positioning of
 ** views within an underlying model space. This model space is conceived to be two fold:
 ** - it is a place or excerpt within the model topology (e.g. the n-th track in the fork)
 ** - it has a temporal extension within a larger temporal frame (e.g. some seconds within the timeline)
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
 ** @todo WIP-WIP as of 10/2022 the usage is just emerging, while the actual meaning
 **               of this interface still remains somewhat nebulous...
 ** @see \ref ZoomWindow_test
 ** 
 */


#ifndef STAGE_MODEL_ZOOM_WINDOW_H
#define STAGE_MODEL_ZOOM_WINDOW_H


#include "lib/error.hpp"
#include "lib/time/timevalue.hpp"
//#include "lib/nocopy.hpp"
//#include "lib/idi/entry-id.hpp"
//#include "lib/symbol.hpp"

//#include <utility>
//#include <string>


namespace stage {
namespace model {
  
//  using std::string;
//  using lib::Symbol;
  using lib::time::TimeValue;
  using lib::time::TimeSpan;
  using lib::time::TimeVar;
  using lib::time::FSecs;
  using lib::time::Time;
  
  
  /**
   */
  struct ZoomWindow
    {
      TimeSpan overallSpan{Time::ZERO, FSecs{23}}; ////////////////Lalala Lalü
      TimeSpan visible  = overallSpan;
      
      uint px_per_sec = 25;
      
      ZoomWindow()
        { }
      
      void reset();
      
    protected:
      
    private:
    };
  
  
  
  
  /** */

  
  
  
}} // namespace stage::model
#endif /*STAGE_MODEL_ZOOM_WINDOW_H*/
