/*
  UI-PROTOCOL.h  -  magic keys used for communication with the Lumiera UI

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


/** @file ui-protocol.hpp
 ** Hard wired key constants and basic definitions for communication with the GUI.
 ** The Lumiera UI is connected to the application core via message oriented interfaces,
 ** running an asynchronous communication protocol.
 ** - the [UI-Bus](\ref ui-bus.hpp) allows for some specific kinds of messages to be
 **   sent either "upstream" (towards the CoreService) or "downstream" to individual
 **   [UI-Elements](\ref stage::model::Tangible) known by ID.
 ** - these ["tangible interface elements"](\ref tangible.hpp) themselves define a
 **   basic set of actions and "state mark" messages, known as the "element protocol"
 ** - and changes to the structure of tangible elements exposed through the UI are
 **   conducted by pushing up [»diff messages«](\ref mutation-message.hpp) into the
 **   UI via UI-Bus. These messages need to comply to an underlying structural
 **   model of elements to be edited within the session and exposed for GUI
 **   manipulation. This is referred to as "Session Model Scheme"
 ** 
 ** These protocols are basically _implementation defined_ conventions and not
 ** rigorously formalised. To a large extent, they are guided by the types and
 ** corresponding API interfaces. However, since the lumiera session allows for
 ** quite flexible structures, these protocols are kept open for local extensions.
 ** At various levels, magic ID-Values are used to initiate specific interactions.
 ** This header supplies the hard wired key constants and IDs thereby employed.
 ** 
 ** @see common/ui-protocol.cpp (translation unit backing those constants)  
 */


#ifndef STAGE_INTERFACE_UI_PROTOCOL_H
#define STAGE_INTERFACE_UI_PROTOCOL_H


#include "lib/error.hpp"
#include "lib/symbol.hpp"


namespace lib {
namespace diff{
  LUMIERA_ERROR_DECLARE(DIFF_STRUCTURE); ///< Invalid diff structure: implicit rules and assumptions violated.
}}
namespace Glib {
//class ustring;  /////////////////////////////TODO needed?
}
namespace stage {
  
//using lib::Literal;
  using lib::Symbol;
//using cuString = const Glib::ustring;
  using lib::diff::LERR_(DIFF_STRUCTURE);
  
  
  extern const Symbol META_kind;
  
  extern const Symbol TYPE_Fork;
  extern const Symbol TYPE_Clip;
  extern const Symbol TYPE_Ruler;
  extern const Symbol TYPE_Marker;
  extern const Symbol TYPE_Channel;
  extern const Symbol TYPE_Effect;
  
  extern const Symbol ATTR_name;
  extern const Symbol ATTR_fork;
  extern const Symbol ATTR_assets;
  
  
  /* ======== UI-Element protocol ======== */
  
  extern const Symbol MARK_reset;
  extern const Symbol MARK_clearErr;
  extern const Symbol MARK_clearMsg;
  extern const Symbol MARK_expand;
  extern const Symbol MARK_reveal;
  extern const Symbol MARK_Flash;
  extern const Symbol MARK_Error;
  extern const Symbol MARK_Warning;
  extern const Symbol MARK_Message;
  
  
}// namespace stage
#endif /*STAGE_INTERFACE_UI_PROTOCOL_H*/
