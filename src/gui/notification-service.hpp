/*
  NOTIFICATION-SERVICE.hpp  -  public service allowing to push information into the GUI

  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>

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

/** @file notification-service.hpp
 ** A public service provided by the GUI, implementing the gui::GuiNotification facade interface.
 ** The purpose of this service is to push state update and notification of events from the lower
 ** layers into the Lumiera GUI. Typically, this happens asynchronously and triggered either by
 ** events within the lower layers, or as result of invoking commands on the session.
 ** 
 ** This service is the implementation of a layer separation facade interface. Clients should use
 ** gui::GuiNotification#facade to access this service. This header defines the interface used
 ** to \em provide this service, not to access it.
 **
 ** @see gui::GuiFacade
 ** @see core-sevice.hpp starting this service
 */


#ifndef GUI_NOTIFICATION_SERVICE_H
#define GUI_NOTIFICATION_SERVICE_H


#include "include/gui-notification-facade.h"
#include "common/instancehandle.hpp"
#include "gui/ctrl/bus-term.hpp"
#include "lib/singleton-ref.hpp"

#include <memory>



namespace lib {
  namespace diff { class GenNode; }
}
namespace gui {
  
  namespace ctrl {
    class UiManager;
    class UiDispatcher;
  }
  
  
  
  /**************************************************//**
   * Actual implementation of the GuiNotification service
   * within the Lumiera GTK GUI. Creating an instance of
   * this class automatically registers the interface
   * with the Lumiera Interface/Plugin system and creates
   * a forwarding proxy within the application core to
   * route calls through this interface.
   * 
   * @note the ctor of this class establishes an "up-link"
   *       connection to the [UI-Bus](ui-bus.hpp), which
   *       enables the service implementation to talk to
   *       other facilities within the UI.
   */
  class NotificationService
    : public GuiNotification
    , public ctrl::BusTerm
    {
      
      /* === Implementation of the Facade Interface === */
      
      void displayInfo (NotifyLevel,string const& text) override;
      void markError (ID uiElement, string const& text) override;
      void markNote  (ID uiElement, string const& text) override;
      void mutate (ID uiElement, DiffMessage&)          override;      ////////////////////////////////////////TICKET #1066 : how to pass a diff message
      void triggerGuiShutdown (string const& cause)     override;
      
    private:
      std::unique_ptr<ctrl::UiDispatcher> dispatch_;
      ctrl::UiManager& uiManager_;
      
      void dispatchMsg(ID, lib::diff::GenNode&&);
      
      
      /* === Interface Lifecycle === */
      
      using ServiceInstanceHandle = lumiera::InstanceHandle< LUMIERA_INTERFACE_INAME(lumieraorg_GuiNotification, 0)
                                                           , GuiNotification
                                                           > ;
      lib::SingletonRef<GuiNotification> implInstance_;
      ServiceInstanceHandle serviceInstance_;
      
      
      
    public:
     ~NotificationService();
      NotificationService (ctrl::BusTerm& upLink, ctrl::UiManager& uiManager);
      
    };
    
  
  
} // namespace gui
#endif
