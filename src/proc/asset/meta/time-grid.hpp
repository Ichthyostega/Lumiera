/*
  TIME-GRID.hpp  -  reference scale for quantised time

  Copyright (C)         Lumiera.org
    2010,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file time-grid.hpp
 ** To establish a reference scale for quantised time values.
 ** Contrary to the continuous Time values (Lumiera internal time scale),
 ** quantised time can only take on some discrete values corresponding
 ** to blocks or frames, according to the respective \em timecode format.
 ** The act of quantising continuous time values requires the definition
 ** of a time grid of such allowed time values. At least, the specification
 ** of a time origin and a frame spacing is required, but there might be
 ** way more elaborate specifications, e.g. a grid varying over time.
 ** 
 ** Time grid specifications are integrated into Lumiera's framework
 ** for meta assets, automation, configuration and similar metadata.
 ** 
 ** ## using time grids
 ** 
 ** TimeGrid is an interface (ABC), but provides some actual factory
 ** functions, which can be used as a convenience shortcut to fabricate
 ** the kind of simple time grid used most often.
 ** 
 ** @see MetaFactory creating concrete asset::Meta instances
 ** @see time::Quantiser
 **
 */

#ifndef ASSET_META_TIME_GRID_H
#define ASSET_META_TIME_GRID_H

#include "proc/asset/meta.hpp"
#include "lib/idi/entry-id.hpp"
#include "lib/time/grid.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/symbol.hpp"



namespace proc {
namespace asset {
namespace meta {
  
  using lib::Symbol;
  using lib::time::Time;
  using lib::time::TimeVar;
  using lib::time::TimeValue;
  using lib::time::FrameRate;
  using lib::time::FSecs;
  
  
  class TimeGrid;
  using PGrid = lib::P<TimeGrid>;
  using GridID = lib::idi::EntryID<TimeGrid>;
  
  
  /**
   * Interface: a grid and scale definition for time quantisation.
   * This meta-Asset describes a coordinate system or reference scale
   * for quantised time values. Especially it allows to define an actual
   * implementation, which can then implicitly be used by lib::time::QuTime
   * and for conversions into timecode.
   * @note for this to work, the actual implementation classes returned
   *       by the builder or the static #build function additionally expose
   *       an implementation of the lib::time::Quantiser API
   */ 
  class TimeGrid
    : public Meta
    , public virtual lib::time::Grid
    {
      
    public:
      /* === shortcut builder functions === */
      static PGrid build (Symbol gridID, FrameRate frames_per_second);
      static PGrid build (Symbol gridID, FrameRate frames_per_second, Time origin);
      
    protected:
      TimeGrid (GridID const&);
    };
    
  
  
  
  
  template<>
  struct Builder<TimeGrid>
    {
      string id;
      
      FrameRate fps;
      TimeVar origin;
      
      /** when building a compound or variable grid,
       *  the predecessor is the grid active _before_
       *  the origin of this (local) grid.
       * @todo currently not supported (as of 12/2010)
       */
      lib::P<TimeGrid> predecessor;
      
      /** 
       * initialise to blank (zero).
       * You need at least to set the framerate,
       * in order to create a usable TimeGrid
       */
      Builder(string const& nameID  ="")
        : id {nameID}
        , fps{1}
        , origin{TimeValue(0)}
        , predecessor{}
        { }
      
      /** create a time grid
       *  based on settings within this builder
       */
      lib::P<TimeGrid> commit();
      
    };
  
}}} // namespace proc::asset::meta
#endif
