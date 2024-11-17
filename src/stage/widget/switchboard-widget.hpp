/*
  SWITCHBOARD-WIDGET.hpp  -  display of a video viewer control switchboard

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file switchboard-widget.hpp
 ** Widget to render a switchboard for controlling the video display
 ** sources and overlays. The generic viewer widget can be _allocated_
 ** to several playback processes, and in fact multiple playback or
 ** render processes may be running at the same time. For any playback
 ** and display to become possible, a _viewer display connection_ needs
 ** to be established; in the typical usage situation with a single timeline
 ** this happens automatically. The result of such a connection is that some
 ** player widget is hooked up with the playback process in the application
 ** core, and the corresponding timeline gets a _playhead cursor._
 ** 
 ** While playback is running, the switchboard allows to connect to several
 ** playback processes dynamically, and it especially allows to tap into
 ** any _probe points_ inserted into the processing network. Moreover,
 ** the viewer allows to show several running displays in _overlay,_ so to
 ** compare or relate them in partial view. An example would be to place
 ** a probe point before some effect processing chain, hook it up to the
 ** switchboard and then to compare the processed and unprocessed video
 ** side by side by moving a slider 
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016 this is a plan (yes, we most definitively
 **       want that!), but we do not even remotely know how to implement such
 **       a feature. It is only clear by now that we'll build a output processing
 **       graph on top of the core render nodes network, and this output processing
 **       will care e.g. for scaling the output to fit into the viewer window.
 **       Activating the switchboard needs to extend this output processing graph.
 ** 
 */


#ifndef STAGE_WIDGET_SWITCHBOARD_WIDGET_H
#define STAGE_WIDGET_SWITCHBOARD_WIDGET_H

#include "stage/gtk-base.hpp"

//#include "lib/util.hpp"

//#include <memory>
//#include <vector>



namespace stage  {
namespace widget {
  
  
  /**
   * @todo WIP-WIP as of 12/2016
   */
  class SwitchboardWidget
    {
    public:
      SwitchboardWidget ();
     ~SwitchboardWidget();
     
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace stage::widget
#endif /*STAGE_WIDGET_SWITCHBOARD_WIDGET_H*/
