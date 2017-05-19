/*
  UI-MANAGER.hpp  -  Global UI Manager

  Copyright (C)         Lumiera.org
    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
    2017,               Hermann Vosseler <Ichthyostega@web.de>

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


/** @file ui-manager.hpp
 ** Manager for global user interface concerns and global state.
 ** The central UiManager instance is owned by the GtkLumiera object and initialised in GTK-main.
 ** It establishes and wires the top-level entities of the UI-Layer and thus, indirectly offers
 ** services to provide Icons and other resources, to open and manage workspace windows, to
 ** form and issue (global) actions and to delve into the UI representation of top-level parts
 ** of the session model. Notable connections established herein:
 ** - connection to the [UI-Bus](\ref ui-bus.hpp)
 ** - the global Actions available though the menu
 ** - the InteractionDirector (top-level controller)
 ** - the StyleManager
 ** - the WindowList
 ** 
 ** @see gtk-lumiera.hpp
 ** @see ui-bus.hpp
 */


#ifndef GUI_CTRL_UI_MANAGER_H
#define GUI_CTRL_UI_MANAGER_H

#include "gui/gtk-base.hpp"

#include <boost/noncopyable.hpp>
#include <string>
#include <memory>


namespace gui {
  
  class UiBus;
  
namespace workspace { class StyleManager; }
namespace ctrl {
  
  using std::unique_ptr;
  using std::string;
  
  class GlobalCtx;
  class Actions;
  
  
  
  /** Framework initialisation base */
  class ApplicationBase
    : public Gtk::UIManager
    , boost::noncopyable
    {
    protected:
      Gtk::Main gtkMain_;
      ApplicationBase();
    };
  
  
  /**
   * Manage global concerns regarding a coherent user interface.
   * Offers access to some global UI resources, and establishes
   * further global services to create workspace windows, to bind
   * menu / command actions and to enter the top-level model parts.
   */
  class UiManager
    : public ApplicationBase
    {
      unique_ptr<GlobalCtx> globals_;
      unique_ptr<Actions>   actions_;
      
      unique_ptr<workspace::StyleManager> styleManager_;
      
    public:
      /**
       * There is one global UiManager instance,
       * which is created by [the Application](\ref GtkLumiera)
       * and allows access to the UI-Bus backbone. The UiManager itself
       * is _not a model::Controller,_ and thus not directly connected to the Bus.
       * Rather, supports the top-level windows for creating a consistent interface.
       */
      UiManager (UiBus& bus);
     ~UiManager ();
      
      /**
       * Set up the first top-level application window.
       * This triggers the build-up of the user interface widgets.
       */
      void createApplicationWindow();
      
      /**
       * start the GTK Main loop and thus activate the UI.
       * @note this function does not return until UI shutdown.
       */
      void performMainLoop();
      
      /**
       * Cause the main event loop to terminate, so the application as a whole unwinds.
       */
      void terminateUI();
      
      /** @todo find a solution how to enable/disable menu entries according to focus
       *                                               /////////////////////////////////////////////////TICKET #1076  find out how to handle this properly
       */
      void updateWindowFocusRelatedActions();
      
      void allowCloseWindow (bool yes);
      
    private:
      
    };
  
  
  
}}// namespace gui::workspace
#endif /*GUI_CTRL_UI_MANAGER_H*/
