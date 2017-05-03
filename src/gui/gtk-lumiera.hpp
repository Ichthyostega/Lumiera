/*
  GTK-LUMIERA.hpp  -  The Lumiera GUI Application Object

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>

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


/** @file gtk-lumiera.hpp
 ** The main application object.
 ** Invoking the GtkLumiera::main() function brings up the GUI; this
 ** function will block in the GTK event thread until the Application gets
 ** closed by user interaction or by triggering a shutdown via the GuiNotificationFacade.
 ** GtkLumiera is a singleton and owns the central WindowList instance used for
 ** opening all windows and registering and loading icons and resources.
 ** 
 ** \par configuration and resource search
 ** The GUI object retrieves the necessary configuration values from lumiera::Config,
 ** the config facade in the application core. Currently as of 2/2011 these values are
 ** loaded from setup.ini, because the full-blown config system is not yet implemented.
 ** Amongst others, this configuration defines a <i>search path</i> for icons and a
 ** separate search path for resources. These path specs may use the token \c $ORIGIN
 ** to refer to the installation directory of the currently executing program.
 ** This allows for a relocatable Lumiera installation bundle.
 ** 
 ** @see guistart.cpp the plugin to pull up this GUI
 ** @see gui::GuiFacade access point for starting the GUI
 ** @see gui::GuiNotification interface for communication with the gui from the lower layers
 ** @see lumiera::Config
 ** @see lumiera::BasicSetup definition of the acceptable configuration values
 ** @see lumiera::AppState general Lumiera application main
 ** 
 */

#ifndef GUI_GTK_LUMIERA_H
#define GUI_GTK_LUMIERA_H


#include "gui/gtk-base.hpp"

#include <boost/noncopyable.hpp>


namespace gui {
  
  
  
  
  /* ====== The Application Class ====== */
  
  /**
   *  Top level entry point: The Lumiera GTK UI.
   *  @todo as of 12/2016 this is a singleton, which is sketchy.
   *        It should be instantiated in local scope of GuiLifecycle
   *        within guistart.cpp and discarded right after shutdown   ///////////////TICKET #1048 : rectify lifecycle
   */
  class GtkLumiera
    : boost::noncopyable
    {
      
    public:
      /** access the the global application object */
      static GtkLumiera& application();                 ////////////////////////////TICKET #1048 : this loophole needs to be closed
      
      
      
      /** start up the GUI and run the event thread */
      void run();
      
    };
  
}// namespace gui
#endif
