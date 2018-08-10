/*
  NOTIFICATION-HUB.hpp  -  receive and reroute notification messages

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


/** @file notification-hub.hpp
 ** Controller to receive and handle notification messages for the user.
 ** This component is addressed in order to show error, warning and notification messages
 ** in the UI. It is responsible for maintaining a local log of these messages, and to
 ** allocate an appropriate display widgets, to show such notifications asynchronously.
 ** On first use, an InfoboxPanel is allocated to hold an ErrorlogWidget for presentation
 ** of those messages
 ** 
 ** @see error-log-widget.hpp
 ** @see notification-service.hpp
 */


#ifndef GUI_CTRL_NOTIFICATION_HUB_H
#define GUI_CTRL_NOTIFICATION_HUB_H

#include "gui/widget/error-log-widget.hpp"
#include "gui/model/controller.hpp"
#include "lib/diff/tree-mutator.hpp"

//#include <memory>
//#include <list>


namespace gui {
namespace ctrl {
  
  using lib::diff::TreeMutator;
  
  
  
  
  /**
   * Service to receive and display error, warning and notification messages.
   * These are sent over the UI-Bus through the NotificationService; after receiving
   * such a message, this controller ensures to display the message and alert the
   * user, while not blocking the overall UI. 
   */
  class NotificationHub
    : public model::Controller
    {
      
      /** content population and manipulation via UI-Bus */
      void
      buildMutator (lib::diff::TreeMutator::Handle buffer)  override
        {
      //  using Attrib = std::pair<const string,string>;
      //  using lib::diff::collection;
          
          buffer.create (
            TreeMutator::build()
          );
          UNIMPLEMENTED ("create a sensible binding between AssetManager in the session and AssetController in the UI");
        }
    
    public:
      
    public:
      NotificationHub (ID identity, ctrl::BusTerm& nexus)////////////////////////////////////TODO #1099 : define the ID hard-wired at an appropriate location, so it can be used from the NotificationService impl as well
        : model::Controller{identity, nexus}
        { }
        
     ~NotificationHub() { };
      
    private:
      
    };
  
  
  
  /** */
  
  
  
}}// namespace gui::ctrl
#endif /*GUI_CTRL_NOTIFICATION_HUB_H*/
