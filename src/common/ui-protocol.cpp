/*
  UiProtocol  -  magic keys used for communication with the Lumiera UI

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

* *****************************************************/


/** @file ui-protocol.cpp
 ** Storage for key constants and basic definitions used for interfacing with the GUI.
 */

//#include "stage/gtk-base.hpp"    /////////////////////TODO necessary?
#include "include/ui-protocol.hpp"


namespace stage {
  
  const Symbol META_kind{"kind"};
  
  const Symbol TYPE_Fork{"Fork"};
  const Symbol TYPE_Clip{"Clip"};
  const Symbol TYPE_Ruler{"Ruler"};
  const Symbol TYPE_Marker{"Marker"};
  const Symbol TYPE_Channel{"Channel"};
  const Symbol TYPE_Effect{"Effect"};
  
  const Symbol ATTR_name{"name"};
  const Symbol ATTR_fork{"fork"};
  const Symbol ATTR_assets{"assets"};
  
  
  /* ======== UI-Element protocol ======== */
  
  const Symbol MARK_reset{"reset"};
  const Symbol MARK_clearErr{"clearErr"};
  const Symbol MARK_clearMsg{"clearMsg"};
  const Symbol MARK_expand{"expand"};
  const Symbol MARK_reveal{"reveal"};
  const Symbol MARK_Flash{"Flash"};
  const Symbol MARK_Error{"Error"};
  const Symbol MARK_Warning{"Warning"};
  const Symbol MARK_Message{"Message"};
  
  
  /**  */
  
  
  
}// namespace stage
