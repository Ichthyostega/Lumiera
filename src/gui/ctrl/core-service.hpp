/*
  CORE-SERVICE.hpp  -  fake user interface backbone for test support

  Copyright (C)         Lumiera.org
    2015,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file core-service.hpp
 ** Dedicated service node within the UI-Bus to handle command invocation
 ** and presentation state. Mostly, the UI-Bus is just a star shaped network
 ** with one central [routing hub][ctrl::Nexus], and serves to distribute
 ** generic state and update messages. But there are some special messages
 ** which need central processing: The command preparation and invocation
 ** messages and the presentation state tracking messages (state marks).
 ** The Nexus is configured such as to forward these special messages
 ** to the [CoreService] terminal, which invokes the dedicated services.
 ** 
 ** @todo initial draft and WIP-WIP-WIP as of 12/2015
 ** 
 ** @see TODO_abstract-tangible-test.cpp
 ** 
 */


#ifndef GUI_CTRL_CORE_SERVICE_H
#define GUI_CTRL_CORE_SERVICE_H


#include "lib/error.hpp"
//#include "lib/idi/entry-id.hpp"
#include "gui/ctrl/bus-term.hpp"
//#include "lib/util.hpp"
//#include "gui/model/tangible.hpp"
//#include "lib/diff/record.hpp"
#include "lib/idi/entry-id.hpp"

#include <boost/noncopyable.hpp>
//#include <string>


namespace gui {
namespace ctrl{
  
//  using lib::HashVal;
//  using util::isnil;
//  using lib::idi::EntryID;
//  using lib::diff::Record;
//  using std::string;
  
  
  /**
   * Attachment point to "central services" within the UI-Bus.
   * This special implementation of the [BusTerm] interface receives and
   * handles those messages to be processed by centralised services:
   * - commands need to be sent down to Proc-Layer
   * - presentation state messages need to be recorded and acted upon.
   * 
   * @todo write type comment
   */
  class CoreService
    : public BusTerm
    , boost::noncopyable
    {
      
      
      virtual void
      act (GenNode const& command)
        {
          UNIMPLEMENTED("receive and handle command invocation");
        }
      
      
      virtual void
      note (ID subject, GenNode const& mark)  override
        {
          UNIMPLEMENTED ("receive and handle presentation state note messages.");
        }
      
      
    public:
      explicit
      CoreService (BusTerm& backlink_to_Nexus, ID identity =lib::idi::EntryID<CoreService>())
        : BusTerm(identity, backlink_to_Nexus)
      { }
    };
  
  
  
}} // namespace gui::ctrl
#endif /*GUI_CTRL_CORE_SERVICE_H*/
