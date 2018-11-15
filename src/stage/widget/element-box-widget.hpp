/*
  ELEMENT-BOX-WIDGET.hpp  -  fundamental UI building block to represent a placed element

  Copyright (C)         Lumiera.org
    2018,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file element-box-widget.hpp
 ** Widget to render an ID label with associated icon.
 ** This is a UI building block used at various places, e.g. to show
 ** the name of a clip in the media bins or in the timeline, to indicate
 ** the kind of an effect, or to render a marker label in the tracks or
 ** the timeline ruler. The common denominator of all those elements is
 ** that they combine some icon with a possibly abridged text and render
 ** them with a given indicator style, configurable via CSS. There is
 ** support for picking the icon and the indicator style based on some
 ** notion of _"type"._ 
 ** 
 ** @todo WIP-WIP-WIP as of 11/2018   ///////////////////////////////////////////////////////////////////////TICKET #1185
 ** 
 */


#ifndef STAGE_WIDGET_ELEMENT_BOX_WIDGET_H
#define STAGE_WIDGET_ELEMENT_BOX_WIDGET_H

#include "stage/gtk-base.hpp"

//#include "lib/util.hpp"

//#include <memory>
//#include <vector>



namespace stage  {
namespace widget {
  
  
  /**
   * @todo WIP-WIP as of 12/2016
   */
  class ElementBoxWidget
    {
    public:
      ElementBoxWidget ();
     ~ElementBoxWidget();
     
    private:/* ===== Internals ===== */
     
    };
  
  
}}// namespace stage::widget
#endif /*STAGE_WIDGET_ELEMENT_BOX_WIDGET_H*/
