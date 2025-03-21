/*
  MIXER-WIDGET.hpp  -  display of a channel mixer control

   Copyright (C)
     2016,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file mixer-widget.hpp
 ** Widget to render a channel mixer within the Lumiera GUI.
 ** A typical mixing desk combines several _mixer strips_ with
 ** faders, where some are used to control the master level, while
 ** others are used to control _sub groups_ and possibly even
 ** individual channels. In addition, this UI allows to access the
 ** configuration and parameters of processing entities wired into
 ** the processing chain _before_ ("pre fader") or _after_ ("post fader")
 ** the main level control fader.
 ** 
 ** In Lumiera, the High-Level-Model (session model) allows for several
 ** top level session::Timeline elements, and the semi-automatic output
 ** management calculates the appropriate output sinks as indicated by
 ** those timeline's contents. In the typical standard situation, this
 ** will be a video master and a audio master output. These are termed
 ** "the global pipes". This mixing desk display defined here shall be
 ** used to expose a control UI for these global sinks; moreover it is
 ** possible to create additional _sub group_ collecting busses, together
 ** with additional processing steps. In a similar way as the video viewer
 ** widgets are _allocated_ for display, also a mixing desk display will
 ** be _allocated_ and thus associated with a given timeline. 
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** 
 */


#ifndef STAGE_WIDGET_MIXER_WIDGET_H
#define STAGE_WIDGET_MIXER_WIDGET_H

#include "stage/gtk-base.hpp"

//#include "lib/util.hpp"

//#include <memory>
//#include <vector>



namespace stage  {
namespace widget {
  
  
  /**
   * @todo WIP-WIP as of 12/2016
   */
  class MixerWidget
    {
    public:
      MixerWidget();
     ~MixerWidget();
     
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace stage::widget
#endif /*STAGE_WIDGET_MIXER_WIDGET_H*/
