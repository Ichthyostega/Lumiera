/*
  TimeGrid  -  reference scale for quantised time

   Copyright (C)
     2010,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


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
    Literal bindingID (cStr(newGrid.ident.name));                            //////////////TODO 2023 shouldn't that be a lib::Symbol?
    
    advice::Provision<PGrid>(bindingID).setAdvice(gridImplementation);
    advice::Provision<PQuant>(bindingID).setAdvice(quantiser);
    return gridImplementation;
    }
  }//(End)Impl helper.
  
  
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
    
    // build new Meta-Asset, registered with AssetManager, and publish into Advice-System
    return publishWrapped (*new SimpleTimeGrid(origin, fps, nameID));             ///////////////////////////TICKET #840 currently this replaces an existing Registration. What do we actually want here?
  }                                                                              /////////////////////////// Remark 1/2025 : Tests meanwhile rely on being able just to replace a Grid without much ado. Seems natural this way...
  
  
  /* === TimeGrid shortcut builder functions === */
  
  PGrid
  TimeGrid::build (FrameRate frames_per_second)                   ///< automatically generates a generic grid name
  {
    return build (Symbol::EMPTY,frames_per_second);
  }
  
  PGrid
  TimeGrid::build (Symbol gridID, FrameRate frames_per_second)    ///< grid origin is at Time::ZERO
  {
    return build (gridID,frames_per_second, Time::ZERO);
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
