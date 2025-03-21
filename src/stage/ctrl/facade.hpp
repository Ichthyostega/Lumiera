/*
  FACADE.hpp  -  Manage the public facade interfaces of the UI

   Copyright (C)
     2017,            Hermann Vosseler <Ichthyostega@web.de>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

*/


/** @file facade.hpp
 ** Manage the lifecycle of the public UI facade interface(s).
 ** Startup of the UI is a tricky process, which requires to establish a main context and to wire
 ** the right components in the right order, prior to activating the GTK event loop. External interfaces
 ** of the UI-Layer should be activated only after everything is wired properly and after the event loop
 ** is actually operative.
 ** 
 ** @note GTK operates single threaded by design.
 **       For this reason, any call from other parts of the application need to be explicitly
 **       dispatched into the UI event loop. The external façade interfaces are constructed
 **       appropriately to ensure this constraint is regarded.
 ** 
 ** @see notification-service.hpp
 ** @see ui-manager.hpp
 ** @see ui-bus.hpp
 */


#ifndef STAGE_CTRL_FACADE_H
#define STAGE_CTRL_FACADE_H

#include "stage/notification-service.hpp"
#include "stage/display-service.hpp"
#include "lib/depend-inject.hpp"
#include "lib/nocopy.hpp"



namespace stage {
namespace ctrl {
  
  
  /**
   * A context to hold and manage the implementation of all UI facade interfaces.
   * The lifecycle follows RAII-style: whenever this object is constructed, all
   * facade interfaces are open and fully operative.
   * 
   * @remark the UiManager is responsible to activate and deactivate those interfaces
   */
  class Facade
    : util::NonCopyable
    {
      using Instance_Notification = lib::DependInject<NotificationService>::ServiceInstance<>;
      using Instance_DisplayService = lib::DependInject<DisplayService>::ServiceInstance<>;
      
      Instance_Notification notificationService_;
      Instance_DisplayService displayService_;        ///////////////////////////////////////////////////////TICKET #82 obsolete and will go away once we have a real OutputSlot offered by the UI
      
      
    public:
      /**
       * Activate all external UI facade interfaces.
       */
      Facade (UiBus& bus, UiManager& manager)
        : notificationService_{bus.getAccessPoint(), manager}   // opens the GuiNotificationService instance
        , displayService_{}                                     // opens the DisplayService instance ////////TICKET #82 obsolete
        {
          INFO (stage, "UI-Facade Interfaces activated.");
        }
      
    private:
    };
  
  
  
}}// namespace stage::ctrl
#endif /*STAGE_CTRL_FACADE_H*/
