/*
  PIPE-WIDGET.hpp  -  display of pipe configuration with effect processing components

  Copyright (C)         Lumiera.org
    2016,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file pipe-widget.hpp
 ** A component to show and manipulate the processing pipe configuration.
 ** Such a (processing) pipe is a conceptual entity within the Lumiera session model.
 ** It is conceived as a sequence of processing steps applied to a data stream, where
 ** the processing steps are typically effects or codecs (media decoder or encoder).
 ** Whenever a clip is added somewhere into a sequence, the automatically triggered
 ** build process forms a pipe corresponding to the clip; this pipe can be tweaked
 ** to some extent within the UI, like toggling and re-ordering of processing steps
 ** or accessing the latter's parameter UI.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016
 ** 
 */


#ifndef GUI_TIMELINE_PIPE_WIDGET_H
#define GUI_TIMELINE_PIPE_WIDGET_H

#include "stage/gtk-base.hpp"

//#include "lib/util.hpp"

//#include <memory>
//#include <vector>



namespace gui  {
namespace timeline {
  
  
  /**
   * @todo WIP-WIP as of 12/2016
   */
  class PipeWidget
    {
    public:
      PipeWidget();
     ~PipeWidget();
     
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace gui::timeline
#endif /*GUI_TIMELINE_PIPE_WIDGET_H*/
