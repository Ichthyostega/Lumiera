/*
  GUINOTIFICATIONFACADE.hpp  -  access point for pushing informations into the GUI
 
  Copyright (C)         Lumiera.org
    2008,               Hermann Vosseler <Ichthyostega@web.de>
 
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/

/** @file guifacade.hpp
 ** Interface for the GUI loader and for accessing the GUI interface from the
 ** lower layers of Lumiera. While part of the public interface of the Lumiera GUI,
 ** the implementation of this facility is part of the core application (and not
 ** contained within the GUI dynamic module), because it's job is to load and
 ** activate this module and to startup the GUI.
 **
 ** @see lumiera::AppState
 ** @see lumiera::Option
 ** @see guifacade.cpp
 ** @see main.cpp
 */


#ifndef GUI_GUINOTIFICATION_H
#define GUI_GUINOTIFICATION_H


#ifdef __cplusplus  /* ============== C++ Interface ================= */

#include "common/singletonsubclass.hpp"

#include <string>


namespace gui {
  
  using std::string;
  
  
  /*********************************************************************
   * Global access point to push state update and notification of events
   * from the lower layers into the Lumiera GUI. Typically, this happens
   * asynchronously and triggered by events within the lower layers.
   * 
   * This is a layer separation facade interface. Clients should use
   * the embedded #facade factory, which yields a proxy routing any 
   * calls through the lumieraorg_GuiNotification interface
   * @throws lumiera::error::State when interface is not opened
   */
  class GuiNotification
    {
    public:
      static lumiera::SingletonSub<GuiNotification> facade;
      
      /** push a user visible notification text */
      virtual void displayInfo (string const& text)          =0;
      
      /** causes the GUI to shut down unconditionally
       *  @param cause user visible explanation of the
       *         reason causing this shutdown      */
      virtual void triggerGuiShutdown (string const& cause)  =0;
      
    protected:
      virtual ~GuiNotification() {}
    };
    
  
  
} // namespace gui


extern "C" {
#endif /* =========================== CLI Interface ================= */ 

  
#include "lib/interface.h"

LUMIERA_INTERFACE_DECLARE (lumieraorg_GuiNotification, 1,
                           LUMIERA_INTERFACE_SLOT (void, displayInfo,        (const char*)),
                           LUMIERA_INTERFACE_SLOT (void, triggerGuiShutdown, (const char*)),
);


#ifdef __cplusplus
}
#endif
#endif
