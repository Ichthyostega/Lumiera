/*
  TimeGrid  -  reference scale for quantised time

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

* *****************************************************/


/** @file time-grid.cpp
 ** Implementation of access to time-quantised reference scales
 */


#include "steam/asset/meta/time-grid.hpp"
#include "steam/asset/struct-scheme.hpp"
#include "steam/assetmanager.hpp"
#include "lib/time/quantiser.hpp"
#include "lib/time/timevalue.hpp"
#include "lib/format-string.hpp"
#include "common/advice.hpp"
#include "lib/util.hpp"

#include <string>

using util::_Fmt;
using util::cStr;
using util::isnil;
using std::string;


namespace steam {
namespace asset {
namespace meta {
  
  namespace error = lumiera::error;
  
  
 
  /**
   * @internal to be invoked indirectly by means of the Builder<TimeGrid>.
   * @todo drafted in 2010 -- usage of the builder feels unsatisfactory as of 2018
   */
  TimeGrid::TimeGrid (GridID const& nameID)
    : Meta{nameID}
    { }
  
  
  using lib::time::Time;
  using lib::time::TimeValue;
  using lib::time::TimeSpan;
  using lib::time::Duration;
  using lib::time::Offset;
  using lib::time::FSecs;
  
  using lib::time::PQuant;
  using lib::time::Quantiser;
  using lib::time::FixedFrameQuantiser;
  using std::dynamic_pointer_cast;
  
  namespace advice = lumiera::advice;
  
  namespace {
    
    /** @internal helper to retrieve the smart-ptr
     * from the AssetManager, then attach a further
     * smart-ptr-to-Quantiser to that, which then can be
     * published via the [advice system](\ref advice.hpp)
     * @note this allows to use a time grid just "by name",
     *       without explicit dependence to the Session / Assets
     */
    inline PGrid
    publishWrapped (TimeGrid& newGrid)
    {
    PGrid gridImplementation = AssetManager::instance().wrap (newGrid);
    PQuant quantiser (dynamic_pointer_cast<const Quantiser>(gridImplementation));
    Literal bindingID (cStr(newGrid.ident.name));
    
    advice::Provision<PGrid>(bindingID).setAdvice(gridImplementation);
    advice::Provision<PQuant>(bindingID).setAdvice(quantiser);
    return gridImplementation;
    }
    

  }
  
  /** 
   * TimeGrid implementation: a trivial time grid,
   * starting at a given point in time and using a
   * constant grid spacing.
   * 
   * @note The actual implementation is mixed in,
   * together with the Quantiser API; the intended use
   * of this implementation is to publish it via the advice
   * framework, when building and registering the meta asset. 
   */
  class SimpleTimeGrid
    : public TimeGrid
    , public FixedFrameQuantiser
    {
      
    public:
      SimpleTimeGrid (Time start, Duration frameDuration, GridID const& name)
        : TimeGrid (name)
        , FixedFrameQuantiser(frameDuration,start)
        { }
      
      SimpleTimeGrid (Time start, FrameRate frames_per_second, GridID const& name)
        : TimeGrid (name)
        , FixedFrameQuantiser(frames_per_second,start)
        { }
    };
  
    
  
  /** Setup of a TimeGrid: validate the settings configured into this builder instance,
   *  then decide on the implementation strategy for the time grid. Convert the given
   *  frames per second into an appropriate gridSpacing time and build a suitable
   *  name-ID to denote the TimeGrid-meta-Asset to be built. 
   * @return shared_ptr holding onto the new asset::Meta, which has already been
   *         registered with the AssetManager.
   * @throw  error::Config in case of invalid frames-per-second. The AssetManager
   *         might raise further exception when asset registration fails.
   * @note the newly created grid is automatically published through the Advice System.
   *       This allows client code to pick up that grid definition just by using the
   *       Grid ID, without requiring an explicit link to the session or Asset subsystem.
   * @todo currently (12/2010) the AssetManager is unable to detect duplicate assets.
   *       Later on the intention is that in such cases, instead of creating a new grid
   *       we'll silently return the already registered existing and equivalent grid.
   */
  lib::P<TimeGrid>
  Builder<TimeGrid>::commit()
  {
    if (predecessor)
      throw error::Invalid("compound and variable time grids are a planned feature"
                          , error::LUMIERA_ERROR_UNIMPLEMENTED);
    ENSURE (fps, "infinite grid should have been detected by FrameRate ctor");
    
    if (isnil (id))
      {
        _Fmt gridIdFormat("grid(%f_%d)");
        id = string(gridIdFormat % fps % _raw(origin));
      }
    GridID nameID (id);
    
    return publishWrapped (*new SimpleTimeGrid(origin, fps, nameID));
  }
  
  
  /* === TimeGrid shortcut builder functions === */
  
  PGrid
  TimeGrid::build (Symbol gridID, FrameRate frames_per_second)
  {
    return build (gridID,frames_per_second, Time(0,0));
  }
  
  
  PGrid
  TimeGrid::build (Symbol gridID, FrameRate frames_per_second, Time origin)
  {
    string name(gridID);
    Builder<TimeGrid> spec(name);
    spec.fps = frames_per_second;
    spec.origin = origin;
    
    return spec.commit();
  }
  
  
}}} // namespace asset::meta
