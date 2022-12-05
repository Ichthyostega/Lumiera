/*
  GRID.hpp  -  time scale grid abstraction

  Copyright (C)         Lumiera.org
    2011,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file grid.hpp
 ** definition of a time grid abstraction for time and timecode handling.
 ** This interface is the foundation to deal with _quantised_ (grid aligned)
 ** time values, which is essential for handling of timecode data.
 */


#ifndef LIB_TIME_GRID_H
#define LIB_TIME_GRID_H

#include "lib/time/timevalue.hpp"

#include <memory>


namespace lib {
namespace time {
  
  
  /**
   * Abstraction of a value alignment grid.
   * Such a grid has an underlying scale (origin and measurement)
   * and is comprised of consecutive grid intervals, joined at the
   * _grid points._ These can be addressed by an ordering number,
   * centred at origin with grid point number zero.
   * The classical example is a 25fps time frame grid, but in fact
   * the length of the intervals is not necessarily constant. An
   * entity defining such a grid provides functions to calculate
   * the grid coordinates and to convert back to plain values.
   * This includes a way of rounding to the next lower
   * grid point, usable for grid aligning values.
   * 
   * \par usage
   * First off, there is the lib::time::Quantiser, which directly
   * implements this interface and plays a central role when it comes
   * to converting continuous time into any kind of frame based timecode.
   * Besides that, the session stores asset::TimeGrid definitions, which
   * can be used to create a Quantiser suitable for some specific output
   * bus or rendering target format.
   * 
   * @todo WIP-WIP-WIP
   * @todo maybe abstract this from Time altogether?
   */
  class Grid
    {
    public:
      virtual ~Grid();   ///< this is an Interface
      
      
      virtual FrameCnt  gridPoint (TimeValue const& raw)  const   =0;
      virtual TimeValue gridLocal (TimeValue const& raw)  const   =0;
      virtual TimeValue timeOf    (FrameCnt gridPoint)    const   =0;
      virtual TimeValue timeOf    (FSecs gridTime, int=0) const   =0;
    };
  
  typedef std::shared_ptr<const Grid> PGrid;
  
  
  
}} // lib::time
#endif
