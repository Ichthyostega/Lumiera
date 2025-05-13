/*
  DEMO-CONTROLLER.hpp  -  playback controller object

   Copyright (C)
     2009,            Joel Holdsworth <joel@airwebreathe.org.uk>
     2025,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file demo-controller.hpp
 ** This file contains the definition of the playback controller object.
 ** 
 ** @deprecated this represents an early design of playback and will be reworked
 ** @remarks what we actually need is a PlaybackController as a shell or proxy
 **     to maintain a flexible link to ongoing processes in the core. But note,
 **     this is also related to the Displayer service, which needs to be offered
 **     by the UI, so we create a mutual dependency here, and there is not much
 **     that can be done about this.
 ** @todo as a temporary solution, 1/2017 the playback controller was moved
 **     into the viewer panel. Of course it can not work that way....
 ** @todo 5/2025 now used for a research project for XV video display    ////////////////////////////////////TICKET #1403 : attempt to upgrade the XV displayer
 */


#ifndef DEMO_CONTROLLER_H
#define DEMO_CONTROLLER_H

#include "stage/gtk-base.hpp"
#include "include/display-handles.hpp"
#include "lib/nocopy.hpp"

#include <memory>
#include <functional>

namespace steam {
namespace node {
  class DummyTickService;
  class DummyImageGenerator;
}}

namespace stage {
namespace ctrl {
  
  using std::unique_ptr;
  
  
  /** @deprecated we need a durable design for the playback process */
  class DemoController
    : util::NonCopyable
    , public sigc::trackable
    {
      unique_ptr<steam::node::DummyImageGenerator> imageGen_;
      unique_ptr<steam::node::DummyTickService>    tick_;
      
    public:
     ~DemoController();
      DemoController();
      
      bool isPlaying() const { return playing_; }
      
      void play();
      void pause();
      void stop();
      
      void activate (lumiera::DisplayerInput);
      sigc::signal<void(void* const)> output_;
    private:
      bool playing_;
      void processFrame();
    };
  
  
}} // namespace stage::ctrl
#endif /*DEMO_CONTROLLER_H*/

