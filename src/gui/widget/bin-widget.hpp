/*
  BIN-WIDGET.hpp  -  display of a media bin

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


/** @file bin-widget.hpp
 ** Widget to render a media bin display in the Lumiera GUI.
 ** A media bin is a grouping device, used in the asset management section
 ** to organise various "things", like clips, media, effect plugins, labels
 ** sequences, processing patterns and also other nested bins. In the Lumiera
 ** session model, media bins and tracks are visual renderings of the same entity,
 ** a [grouping fork](\ref proc::mobject::session::Fork). This wide array of
 ** possible usage gives rise to some complexity and several display styles, ranging
 ** from a simple list to a working area to arrange elements, with drag-n-drop
 ** support, searching and establishing of an element order. The generality
 ** of this element and the shared common implementation are intentional;
 ** we're aiming at a common workspace metaphor, and we want to support
 ** a fluent working style, where a rough arrangement of media elements
 ** can be turned into a preliminary cut.
 ** 
 ** @todo WIP-WIP-WIP as of 12/2016 we have only very vague preliminary plans
 **       regarding the asset management section; also the asset management
 **       within the session subsystem will be rewritten to fit our increased
 **       understanding of the editing process. So for now, this widget is a
 **       mere placeholder in a largely preliminary UI (need more weasel words?)
 ** 
 */


#ifndef GUI_WIDGET_BIN_WIDGET_H
#define GUI_WIDGET_BIN_WIDGET_H

#include "gui/gtk-base.hpp"

//#include "lib/util.hpp"

//#include <memory>
//#include <vector>



namespace gui  {
namespace widget {
  
  
  /**
   * @todo WIP-WIP as of 12/2016
   */
  class BinWidget
    {
    public:
      BinWidget ();
     ~BinWidget();
     
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace gui::widget
#endif /*GUI_WIDGET_BIN_WIDGET_H*/
