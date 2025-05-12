/*
  DemoController  -  playback controller object

   Copyright (C)
     2008,            Joel Holdsworth <joel@airwebreathe.org.uk>
     2025,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file demo-controller.cpp
 ** Implementation parts of PlaybackController.
 ** @todo 5/2025 now used for a research project for XV video display    ////////////////////////////////////TICKET #1403 : attempt to upgrade the XV displayer
 */


#include "stage/ctrl/demo-controller.hpp"
#include "steam/engine/worker/tick-service.hpp"
#include "steam/engine/worker/dummy-image-generator.hpp"

#include <utility>


namespace stage {
namespace ctrl {
  namespace error = lumiera::error;

  namespace {
    const uint FPS = 4;
  }
  
  using std::make_unique;
  using steam::node::TickService;
  using steam::node::DummyImageGenerator;
  using lumiera::DisplayerInput;
  
  
  DemoController::DemoController()
    : imageGen_{make_unique<DummyImageGenerator>(FPS)}
    , tick_{}
    , output_{}
    , playing_{false}
    { }
  
  DemoController::~DemoController() { stop(); }
  
  /** Signal slot to be called after the output window was created
   *  and the actually usable video display technology has been determined.
   * @param displayFormat format for the frames expected in the passed image buffer.
   */
  void
  DemoController::activate (lumiera::DisplayerInput displayFormat)
  {
    REQUIRE (imageGen_);
    imageGen_->configure (displayFormat);
  }
  
  void
  DemoController::processFrame()
  {
    REQUIRE (tick_);
    REQUIRE (imageGen_);
    
    if (playing_)
      output_(imageGen_->next());
    else
      output_(imageGen_->current());
  }
  
  
  void
  DemoController::play()
  {
    if (not tick_)
        tick_.reset (new TickService{[this]{ processFrame(); }});
    ASSERT (tick_);
    tick_->activate (FPS);
    playing_ = true;
  }
  
  void
  DemoController::pause()
  {
    if (tick_)
      tick_->activate(0);
    playing_ = false;
  }
  
  void
  DemoController::stop()
  {
    tick_.reset(); // blocks for one cycle to join()
    playing_ = false;
  }
  
  
}} // namespace stage::ctrl

