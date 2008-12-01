/*
  GUIFACADE.hpp  -  access point for communicating with the Lumiera GTK GUI
 
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


#ifndef GUI_FACADE_H
#define GUI_FACADE_H


#include "lumiera/subsys.hpp"



namespace gui {
  
  
  /*********************************************************************
   * Global access point for loading and starting up the Lumiera GTK GUI
   * and for defining the public interface(s) for addressing the GUI
   * from Backend or Proc-Layer.
   * 
   * If running Lumiera with a GUI is required (the default case),
   * it is loaded as dynamic module, thus defining the interface(s) 
   * for any further access. After successfully loading and starting
   * the GUI, this gui::Facade is wired internally with this interface
   * such as to allow transparent access from within the core. This
   * startup sequence includes providing the GUI with similar facade
   * access via interface handles for communication with Backend and
   * Proc-Layer.
   * 
   */
  struct GuiFacade
    {
      /** provide a descriptor for lumiera::AppState,
       *  wired accordingly to allow main to load, 
       *  start and stop the Lumiera GTK GUI. */
      static lumiera::Subsys& getDescriptor();
      
      
      //////////////////TODO: define the global access interface for the GUI
      //////////////////TODO: provide a function for accessing this interface
      //////////////////TODO: register similar proxy/facade interfaces for Proc/Backend
      
    };
    
  
  
} // namespace gui
#endif
