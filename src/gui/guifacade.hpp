/*
  GUIFACADE.hpp  -  access point for communicating with the Lumiera GTK GUI

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

/** @file guifacade.hpp
 ** Interface for the GUI loader and for accessing the GUI interface from the
 ** lower layers of Lumiera. While part of the public interface of the Lumiera GUI,
 ** the implementation of this facility is part of the core application (and not
 ** contained within the GUI dynamic module), because it's job is to load and
 ** activate this module as plug-in and to startup the GUI.
 **
 ** @see lumiera::AppState
 ** @see lumiera::Option
 ** @see guifacade.cpp
 ** @see main.cpp
 */


#ifndef GUI_FACADE_H
#define GUI_FACADE_H


#include "common/subsys.hpp"

extern "C" {
#include "common/interface.h"
}

#include <boost/noncopyable.hpp>


namespace gui {
  
  
  
  /*****************************************************************//**
   * Global access point for loading and starting up the Lumiera GTK GUI
   * and for controlling the GUI lifecycle. The implementation part of
   * this class also is responsible for making the "business" interface
   * of the GUI available, i.e. gui::GuiNotification
   * 
   * When running Lumiera with a GUI is required (the default case),
   * it is loaded as dynamic module, thus defining the interface(s) 
   * for any further access. After successfully loading and starting
   * the GUI, the actual "business" interfaces of the GUI are opened
   * and wired internally such as to allow transparent access from
   * within the core.
   * 
   * \par implementation notes
   * This is an facade interface to the GUI subsystem, but it is setup
   * somewhat special, as its sole purpose is to expose the subsystem
   * descriptor, which, when started, loads the GUI as a plugin and
   * invokes `launchUI(term)` there. For the implementation see
   * gui::GuiRunner (guifacade.cpp) and gtk-lumiera.cpp (the plugin).
   * 
   * @note this facade is intended to be used by Lumiera main solely.
   *       client code should always use the "business" interface(s).
   * 
   */
  class GuiFacade
    : boost::noncopyable
    {
    public:
      
      /** provide a descriptor for lumiera::AppState,
       *  wired accordingly to allow main to load, 
       *  start and stop the Lumiera GTK GUI. */
      static lumiera::Subsys& getDescriptor();
      
      
      /** weather the GUI has been started and all interfaces are opened */
      static bool isUp();
      
      
      /*  ===== control interface for the GuiStarterPlugin ======= */
      
      /** start the actual GUI thread(s), after successfully loading
       *  the GuiStarterPlugin, that is. The implementation of this function
       *  must ensure to invoke the given termination signal reliably after
       *  shutting down the GUI, otherwise the application will hang on exit.
       *  @internal this function is invoked automatically during the GUI
       *            loading and startup process. Don't call it manually.
       */
      virtual bool launchUI (lumiera::Subsys::SigTerm&)  =0;
      
      
    protected:
      virtual ~GuiFacade() {}
    };
    
  /** interface of the GuiStarterPlugin */
  LUMIERA_INTERFACE_DECLARE (lumieraorg_Gui, 1,
                             LUMIERA_INTERFACE_SLOT (bool, launchUI, (void*))
  );
  
  
} // namespace gui
#endif
