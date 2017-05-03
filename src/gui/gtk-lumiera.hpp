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




/* ======= Namespace Definitions ======= */

/** Lumiera GTK UI implementation root. */
namespace gui {
  

  /** Backbone of the Lumiera GTK UI.
   *  All interface elements and controllers of global relevance
   *  are connected to a communication structure known as UI-Bus.
   *  This structure allows to address interface elements by ID
   *  and to process command invocations with asynchronous feedback
   *  in a uniform way.
   */
  namespace ctrl {}
  
  /** UI interaction control.
   *  Any overarching concerns of interaction patterns,
   *  selecting the subject, forming and binding of commands,
   *  management of focus and perspective, keybindings and gestures.
   */
  namespace interact {}
  
  /** The Lumiera GTK-GUI uses a thin proxy layer data model
   *  on top of the actual "high-level-model", which lives in the
   *  Proc-Layer below. GUI operations interact with these proxy model
   *  entities, which in turn forward the calls to the actual objects
   *  in the Proc-Layer, through the Command system (which enables UNDO).
   *  
   *  @todo: as of 1/2011 this connection between the GUI proxy model and
   *         the Proc-Layer model needs to be set up. Currently, the GUI model
   *         entities are just created standalone and thus dysfunctional.
   *  @todo: as of 11/2015 this connection between UI and Proc-Layer is actually
   *         in the works, and it is clear by now that we won't use proxy objects,
   *         but rather a generic bus-like connection and symbolic IDs to designate
   *         the model elements
   *  @todo: and as of 2/2017 the new structure is basically set, but we need actually
   *         to implement a new timeline display in accordance to this reworked design.
   *         After this is done, most of the model elements in this packare are obsolete.
   */
  namespace model {}
  
  /** Video output implementation. */
  namespace output {}

  /** Dialog box classes. */
  namespace dialog {}
  
  /** Docking panel classes. */
  namespace panel {}
  
  /** The asset management, configuration and project settings. */
  namespace setting {}
  
  /** The timeline display and editing operations. */
  namespace timeline {}
  
  /** The global workspace with top-level windows. */
  namespace workspace {}
  
  /** Lumiera custom widgets. */
  namespace widget {}
  
  /** GUI helpers, utility functions and classes. */
  namespace util {}
  
  
  
  
  
  
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
